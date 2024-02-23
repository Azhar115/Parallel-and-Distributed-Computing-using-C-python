/*
  program working :each process with get block of array and will find number of times key found and result that result to process rank 0, where all will be sumed and return to user screen
  program technique : message passing using MPI
*/
#include<iostream>
#include<mpi.h>
using namespace std;

int total_times_key_found(int *array,int start_index,int end_index, int key);

int main(int argc,char *argv[]){
	int total_processes;
	int current_process_rank;
	int array[12]= {2,4,2,5,4,2,6,2,7,2,7,4};
	int key =2; // key to be searched
	
	//parallelism starts
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD, &total_processes); // total processes created information in total_processes
	MPI_Comm_rank(MPI_COMM_WORLD,&current_process_rank);
	int rank_work_start_position = current_process_rank * total_processes;
	int rank_work_end_poisition = rank_work_start_position + (total_processes -1); //as index starts with zero index
	int each_rank_work_result; 
	//first created process execution of rank=0
	if (current_process_rank==0){
		int total_times_key_found_in_array = total_times_key_found(array,rank_work_start_position,rank_work_end_poisition, key);
		//output for test
		cout<<"total times key found in rank "<<current_process_rank<< " is :"<<total_times_key_found_in_array <<endl;
		for(int i=1;i<total_processes;i++){
			//recieve work result of each process having tag 121518, and rank>0
			MPI_Recv(&each_rank_work_result,1,MPI_INT,i,121518,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			// add that work in sum
			total_times_key_found_in_array  += each_rank_work_result;
		
		}
		
		cout<<" -----------total times key found in program in rank 0 : "<<total_times_key_found_in_array <<endl;		
		
	}
	//other processes whose  0 < rank < total_processes
	else{
          
        // sending its work to process 0 with tag matching 121518
        each_rank_work_result =  total_times_key_found(array,rank_work_start_position,rank_work_end_poisition, key);
		MPI_Send(&each_rank_work_result,1,MPI_INT,0,121518,MPI_COMM_WORLD);
		//output for test
		cout<<"total times key found in rank "<<current_process_rank<< " is :"<<each_rank_work_result<<endl;
		
	}
	//parallelism end
	MPI_Finalize();
	return 0;
}

int total_times_key_found(int *array,int start_index,int end_index, int key){
	// finding number of times key found in array and retuen that
	int count =0;
	for(int i=start_index;i<=end_index;i++){
		if (array[i]==key){
			count++;
		}
	}
	return count;
}
