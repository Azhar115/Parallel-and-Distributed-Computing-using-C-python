#include <iostream>
#include <fstream>
//#include <vector>
#include <mpi.h>

using namespace std;

//functions declaration
template<class T1, class T2>
void read_into_array(string file_name_with_path,T1 *array,T2 &array_size);
template<typename T>
int search_key_presence(const T *array,int total_size,T key_to_search);
// For testing
template <typename T>
void display_array(const T *array, int sizeofArray);

int main(int argc, char *argv[]) {
    int total_processes, current_process_rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &total_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &current_process_rank);
    
   // declaring varables without initializing gives error
    char *First_line_input_storage = new char();
    int data_distribution_size[total_processes];//recive the each process task size
    int *disp = new int[total_processes]; //each process start position in First_line_input_storage array for searching
    char *each_process_array;
    int array_size=0;
    char key_to_search = 'a';
	
    if (current_process_rank == 0) {
    	 
        
		read_into_array("data.txt",First_line_input_storage,array_size);
        // Displaying the array for testing
        display_array(First_line_input_storage, array_size);
        
      	//process zero finding the displacement(start position) of each rank task from global array for searching
    	for(int i=0;i<total_processes;i++){
    		int task_start_index =  current_process_rank * data_distribution_size[i]; //finding each process start position
        	disp[i]= task_start_index;
		}
    }
    
    int all_process_tempcount = 0;
    int each_process_task_size = array_size / total_processes;
    
    // Adjust task_end_index for the last process to handle any remaining elements
    if (current_process_rank == total_processes - 1) {
        each_process_task_size +=(array_size%total_processes); //only last process do the remaining tasks 
    }
    //each process sends its total task to process 0
    MPI_Gather(&each_process_task_size,1,MPI_INT,data_distribution_size,1,MPI_INT,0,MPI_COMM_WORLD);
    
   
    //each process resive its elements to search in
    MPI_Scatterv(First_line_input_storage,data_distribution_size,disp,MPI_INT,each_process_array,each_process_task_size,MPI_INT,0,MPI_COMM_WORLD);
    
    

//    cout << "rank: " << current_process_rank << " total processes: " << total_processes
//         << " each process task: " << each_process_task_size
//         << " start_work: " << task_start_index << " end task " << task_end_index << endl;
//disp[rank]: each process start position, and disp[rank+1]: is end position
	all_process_tempcount = search_key_presence(each_process_array,each_process_task_size, key_to_search);
	int total_count = 0;
   /*
    if (current_process_rank == 0) {
        int total_numberofTimes_key_found = all_process_tempcount;

        for (int i = 1; i < total_processes; i++) {
            MPI_Recv(&all_process_tempcount, 1, MPI_INT, i, 121518, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total_numberofTimes_key_found += all_process_tempcount;
        }

        cout << "\nTotal occurrences of key: " << total_numberofTimes_key_found << endl;
        delete []data_distribution_size;//clean up the pointer array
    } else {
        MPI_Send(&all_process_tempcount, 1, MPI_INT, 0, 121518, MPI_COMM_WORLD);
    }
    */
    MPI_Reduce(&all_process_tempcount,&total_count,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
    if (current_process_rank==0){
    	cout<<"total count is: "<<total_count<<endl;
	}

    MPI_Finalize();
    return 0;
}
template<class T1,class T2>
void read_into_array(string file_name_with_path, T1 *array,T2 &array_size){
	ifstream readFile(file_name_with_path);
	//if file does not exist or not open
    if (!readFile.is_open()) {
    cerr << "Error opening file!" << endl;
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
    char readchar;
    readFile.get(readchar);
    while (readchar != '\n') {
        if (readchar != ',') {
            array[array_size]=readchar;
            array_size++;
        }
        readFile.get(readchar);
    }
    readFile.close();
}
template<typename T>
int search_key_presence(const T *array,int total_size, T key_to_search){
	//counting number of times key found in an array and returing total count of key found
	/*
	__array = array on which key to be searched
	__task_start_index = the position from where key to be searched
	__task_end_index =  last position of array
	__key_to_Search = the key provided to be searched
	__return = count
	*/
	int count =0;
	for (int i =0; i <= total_size; i++) {
        if (array[i] == key_to_search) {
            count++;
        }
    }
    return count;
}
// For testing
template <typename T>
void display_array(const T *array, int sizeofArray) {
    cout << "Display created array: ";
    for (int i = 0; i < sizeofArray; i++) {
        cout << array[i] << ",";
    }
    cout << endl;
}

