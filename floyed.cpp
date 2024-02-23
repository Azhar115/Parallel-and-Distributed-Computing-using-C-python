
#include<iostream>
#include<mpi.h>
#include<fstream>
#include<string>
#include<sstream>
#include<algorithm>

using namespace std;
// function declaration

int find_total_rows(string file_name_with_path);
int find_total_columns(string file_name_with_path);
int** read_into_array(string file_name_with_path,int array_total_rows,int array_total_columns,int start_point_in_file);

int main(int argc,char*argv[]){
	int current_process,total_processes;
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&total_processes);
	MPI_Comm_rank(MPI_COMM_WORLD,&current_process);
	//row level task separation
    int process_row_level_total_tasks =0;
    //column level in each row 
    int process_each_task_size=0;
  //  int *shared_row;
    int **array; //staring each process self task storage
   
    if (current_process==0){
    	int array_total_rows =  find_total_rows("data.txt");
    	process_row_level_total_tasks = array_total_rows /total_processes;
         process_each_task_size= find_total_columns("data.txt");
        
	}
	
	
	//sharing the tasks to be performed by each processor
	MPI_Bcast(&process_row_level_total_tasks,1,MPI_INT,0,MPI_COMM_WORLD);
	//sharing the each task size in a process
	MPI_Bcast(&process_each_task_size,1,MPI_INT,0,MPI_COMM_WORLD);
    //	cout<<process_row_level_total_tasks<<"rows: and columns  :"<<process_each_task_size<<endl;
    
    //each process starting point to start reading the file
    int task_start_point = current_process * process_row_level_total_tasks;
    int task_end_point  = task_start_point + (process_row_level_total_tasks-1);
    
    // 1st) Each process storing its own block of data  from into its local array
    
    if (current_process!=0){
    	//each process performs its tasks of initializing its storage
		array = read_into_array("data.txt",process_row_level_total_tasks,process_each_task_size,task_start_point);
		//	testing purpose
		cout<<"\ncurrent_process : "<<current_process<<endl; 
		// test purpose
		for(int i=0;i<process_row_level_total_tasks;i++){
			for(int j=0;j<process_each_task_size;j++){
				cout<<array[i][j]<<",";
			}
			cout<<endl;
		}
		cout<<endl;
	}
	else{
		array = read_into_array("data.txt",process_row_level_total_tasks,process_each_task_size,task_start_point);
		cout<<"\ncurrent_process : "<<current_process<<endl; 
		//for test purpose
		for(int i=0;i<process_row_level_total_tasks;i++){
			for(int j=0;j<process_each_task_size;j++){
				cout<<array[i][j]<<",";
			}
			cout<<endl;
		}
		cout<<endl;
	}
	int current_process_row = 0; //used to select frame of each process row
    //2nd) performing floyed alogrithm finding min distance
    for(int frames=0;frames<process_each_task_size;frames++){
    	int *shared_row= new int[process_each_task_size];
    	cout<<task_start_point<<"end "<<task_end_point<<endl;
    	//any process whose full fill the below condtion will execute it
    	if(frames>=task_start_point && frames<=task_end_point){
    		cout<<"\n enter"<<endl;
    		for(int i=0;i<process_each_task_size;i++){ 
    				shared_row[i]= array[current_process_row][i];
    				
    				cout<<shared_row[i];
    				cout<<"\n middle "<<endl; 
				} 
				current_process_row++; //row changed for next time 
			cout<<"\n exit "<<endl; 
		}
		int process_identify = frames/process_row_level_total_tasks; //source sharing process finding
		MPI_Bcast(shared_row,process_each_task_size,MPI_INT,process_identify,MPI_COMM_WORLD);
				 
    	for(int rows=0;rows<process_row_level_total_tasks;rows++){
    		for(int columns=0;columns<process_each_task_size;columns++){
    			cout<<shared_row[columns];
    		array[rows][columns] =min(array[rows][columns], array[rows][frames]+shared_row[columns]);
			cout<<"run min: "<<array[rows][columns]<<endl;	
			}
			
			cout<<endl;
		}
		cout<<"\nstate"<<shared_row[1]<<endl;
		shared_row = NULL;
		delete[] shared_row;
	    
	}
		//deleting pointers
	for (int i = 0; i < process_row_level_total_tasks; i++) {
		array[i]=NULL;
   		delete[] array[i];
	}
	array =NULL;
	delete[] array;
	MPI_Finalize();
	
	return 1;
	  
}

int find_total_rows(string file_name_with_path){
	//finding total lines in a text file 
	int linecount=0;
	ifstream readFile(file_name_with_path);
	//if file does not exist or not open
	if(readFile.is_open()){
		string line;
		while(getline(readFile,line)){
			linecount++;		
		}
	}
	readFile.close();
	cout<<linecount<<"count";
   return linecount;  
}
int find_total_columns(string file_name_with_path){
	string str;
	char readchar;
	ifstream readFile(file_name_with_path);
	readFile.get(readchar);
	while (readchar != '\n' && !readFile.eof()) { //cheacking endfile and end of line
		if (readchar!=','){
			str +=readchar;  //storing characters that is part of one integer value
		}
		readFile.get(readchar);		
	}
	readFile.close();
	return str.size()-1;//each row size
}
int** read_into_array(string file_name_with_path,int array_total_rows,int array_total_columns,int start_point_in_file)
{

	/*
	file_name_with_path : string: file name to be open to read
	array_total_rows : int : required for array rows declaration
	array_total_columns : int : required for declaration of each row columns
	start_point_in_file : int : required to start position in a file for reading
	__return : the 2d dynamic array stored with desired elements from files 
	*/
	
	//array is filled with number of rows passed from file
	ifstream readFile(file_name_with_path);
	//if file does not exist or not open
    if (!readFile.is_open()) {
    	cerr << "Error opening file!" << endl;
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
    char readchar; // storing each char read
     
    // changing poisition to start reading from a file:
    int position = 0; //indicates cursor
    
    //iterate till we reach to desire position of cursor to start reading
	while (position !=start_point_in_file) { //cheacking endfile and end of line
		readFile.get(readchar);	
	    if(readchar == '\n' && !readFile.eof()){
	    	position++;	 //each time jump to new line 
		}			
	}
	
	int **array =new int*[array_total_rows]; 
 //	cout<<"1 :"<<array_size;
    int j=0; // use for variable indices
    //storing data in array, each row data is stored
    for(int i=0;i<array_total_rows;i++){
    	j=0; // total_columns
    	array[i]= new int[array_total_columns];//alocate memory for first element
    	readFile.get(readchar);
    	string str; //storing a single character
		while (readchar != '\n' && j<array_total_columns) { //cheacking endfile and end of line
			if (readchar!=','){
				str +=readchar;  //storing characters that is part of one integer value
			}
			else{
        	stringstream ss(str);
        	int value;
        	ss>>value;
        //	cout<<"value:"<<value<<",";
        	array[i][j] =value;
			j++;
        	str.clear();
        	}
        	readFile.get(readchar);
        }
        // to store last element of each row as that was not stored by while loop.
        stringstream ss(str);
        	int value;
        	ss>>value;
        	//cout<<"value:"<<value<<",";
        	array[i][j] =value;
       // cout<<endl;
	}
    readFile.close();
    return array;
}
