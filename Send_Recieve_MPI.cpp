/*
  program working : process rank=0 will get the number recieved from each other processes and add them all
  program technique : message passing using MPI
*/
#include<iostream>
#include<mpi.h>
using namespace std;

int main(int argc,char *argv[]){
	int total_processes;
	int current_process_rank;
	//parallelism starts
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD, &total_processes); // total processes created information in total_processes
	MPI_Comm_rank(MPI_COMM_WORLD,&current_process_rank);
	int each_rank_work;
	//first created process execution of rank=0
	if (current_process_rank==0){
		int sum=0;
		for(int i=1;i<total_processes;i++){
			//recieve work result of each process having tag 121518, and rank>0
			MPI_Recv(&each_rank_work,1,MPI_INT,i,121518,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			// add that work in sum
			sum +=each_rank_work;
		
		}
		cout<<"rank 0  total : "<<sum<<endl;		
		
	}
	//other processes whose  0 < rank < total_processes
	else{
         each_rank_work =10;
        // sending its work to process 0 with tag matching 121518
		MPI_Send(&each_rank_work,1,MPI_INT,0,121518,MPI_COMM_WORLD);
		cout<<"total in rank "<<current_process_rank<< " is :"<<each_rank_work<<endl;
		
	}
	//parallelism end
	MPI_Finalize();
	
	return 0;
}
