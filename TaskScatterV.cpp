#include <mpi.h>
#include <iostream>
#include <cmath>
#include <cstdlib>   
#include <ctime>     

using namespace std;
int main(int argc, char *argv[])
{
    int No_Of_pes, My_rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &No_Of_pes);
    MPI_Comm_rank(MPI_COMM_WORLD, &My_rank);

    int range = atoi(argv[1]);
    int *data;
    int size = range / No_Of_pes;
    if(My_rank==0)
    {
        data = new int[range];
        cout<<"\nArray that I want to scatter: ";
        srand(static_cast<unsigned int>(time(0)));
        for (int i = 0; i < range; ++i) {
            data[i] = (rand() + time(0)) % 100;
            cout<<data[i]<<"\t";
        }
        
    }
    int data_for_each_pros[size];

    int sendcounts[size];
    int displacement[size];
    for (int i = 0; i < No_Of_pes; ++i) {
        sendcounts[i] = size;
        displacement[i] = i * size;
    }
    MPI_Scatterv(data, sendcounts, displacement, MPI_INT, data_for_each_pros, size, MPI_INT, 0, MPI_COMM_WORLD);
    

    cout << "\n\nP" << My_rank << ": ";
    for (int i = 0; i < size; i++)
    {
            cout << data_for_each_pros[i] << "\t";
    }
    cout << endl;

    MPI_Finalize();
    return 0;
}