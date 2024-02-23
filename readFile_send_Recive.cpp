#include <iostream>
#include <fstream>
#include <vector>
#include <mpi.h>

using namespace std;

//functions declaration
template<class T1, class T2>
void read_into_array(string file_name_with_path,vector<T1> &array,T2 &array_size);
template<typename T>
int search_key_presence(const vector<T> &array,int task_start_index,int task_end_index, T  key_to_search);
// For testing
template <typename T>
void display_array(const vector<T> &array, int sizeofArray);

int main(int argc, char *argv[]) {
    int total_processes, current_process_rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &total_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &current_process_rank);

    vector<char> First_line_input_storage;
    int array_size = 0;
    char key_to_search = 'a';

    if (current_process_rank == 0) {
        
		read_into_array("data.txt",First_line_input_storage,array_size);
        // Displaying the array for testing
        display_array(First_line_input_storage, array_size);
    }
    /*
    one to all broadcast in O(n) time
    if (current_process_rank == 0) {
    	for(int i=1;i<total_processes;i++){
    		//sharing array_size with all processes by passing the refrence
    		MPI_Send(&array_size,1,MPI_INT,i,1212,MPI_COMM_WORLD);
		}	
	}
	else{
		//all other processes recieve the shared memory from processes rank=0
		MPI_Resv(&array_size,1,MPI_INT,0,1212,MPI_COMM_WORLD,MPI_STATUS.IGNORE);
	}
	*/
	
	//one to all broadcast in O(logn) time, through MPI_Bcast
    // Broadcast array_size to all processes
    MPI_Bcast(&array_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Resize the vector for all processes to match the broadcasted size
    First_line_input_storage.resize(array_size);

    // Broadcast the actual vector data to all processes from rank=0 process to others
    MPI_Bcast(First_line_input_storage.data(), array_size, MPI_CHAR, 0, MPI_COMM_WORLD);
	
	//dividing task on each process
    int all_process_tempcount = 0;
    int each_process_task_size = array_size / total_processes;
    int task_start_index = current_process_rank * each_process_task_size;
    int task_end_index = task_start_index + each_process_task_size - 1;

    // Adjust task_end_index for the last process to handle any remaining elements
    if (current_process_rank == total_processes - 1) {
        task_end_index = array_size - 1;
    }

//    cout << "rank: " << current_process_rank << " total processes: " << total_processes
//         << " each process task: " << each_process_task_size
//         << " start_work: " << task_start_index << " end task " << task_end_index << endl;
	all_process_tempcount = search_key_presence(First_line_input_storage, task_start_index,task_end_index, key_to_search);
   
    if (current_process_rank == 0) {
        int total_numberofTimes_key_found = all_process_tempcount;

        for (int i = 1; i < total_processes; i++) {
            MPI_Recv(&all_process_tempcount, 1, MPI_INT, i, 121518, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total_numberofTimes_key_found += all_process_tempcount;
        }

        cout << "\nTotal occurrences of key: " << total_numberofTimes_key_found << endl;
    } else {
        MPI_Send(&all_process_tempcount, 1, MPI_INT, 0, 121518, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
template<class T1,class T2>
void read_into_array(string file_name_with_path, vector<T1> &array,T2 &array_size){
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
            array.push_back(readchar);
            array_size++;
        }
        readFile.get(readchar);
    }
    readFile.close();
}
template<typename T>
int search_key_presence(const vector<T> &array,int task_start_index,int task_end_index, T  key_to_search){
	//counting number of times key found in an array and returing total count of key found
	/*
	__array = array on which key to be searched
	__task_start_index = the position from where key to be searched
	__task_end_index =  last position of array
	__key_to_Search = the key provided to be searched
	__return = count
	*/
	int count =0;
	for (int i = task_start_index; i <= task_end_index; i++) {
        if (array[i] == key_to_search) {
            count++;
        }
    }
    return count;
}
// For testing
template <typename T>
void display_array(const vector<T> &array, int sizeofArray) {
    cout << "Display created array: ";
    for (int i = 0; i < sizeofArray; i++) {
        cout << array[i] << ",";
    }
    cout << endl;
}

