#include <stdio.h>
#include <omp.h>
#include "mpi.h"

int main(int argc, char *argv[]) {
    int numprocs, rank, namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int iam = 0, np = 1;
    int provided, required=MPI_THREAD_MULTIPLE;
    // First call MPI_Init
    /// MPI_Init(&argc, &argv);
    MPI_Init_thread(&argc, &argv, required, &provided);
    //MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Get_processor_name(processor_name, &namelen);
    printf("no MPI, rank: %d, MPI_COMM_WORLD: %d\n", rank, MPI_COMM_WORLD);
    
    /* Check that the MPI implementation supports MPI_THREAD_MULTIPLE */
    if (provided < MPI_THREAD_MULTIPLE) {
        printf("MPI does not support MPI_THREAD_MULTIPLE\n");
        MPI_Abort(MPI_COMM_WORLD, -1);
        return 0;
    }

    #pragma omp parallel default(shared) private(iam, np) firstprivate(rank)
    {
        np = omp_get_num_threads();
        iam = omp_get_thread_num();
        char recebendo[50];
        //printf("Hello from thread %d out of %d from process %d out of %d on %s\n", iam, np, rank, numprocs, processor_name);
        if(iam==0 && rank==0){ 
        #pragma omp single nowait
        { 
        printf("master, rank: %d, thread: %d\n", rank, iam);
        // while(flag==0){
        //     MPI_Test(request, &flag, status);
        //     sleep(1);
        // } 
        //MPI_Irecv(recebendo, 50, MPI_CHAR, 1, 0, MPI_COMM_WORLD, request);
        MPI_Recv(recebendo, 50, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("message: %s\n", recebendo);
        }
        }
        if(iam==1 && rank==1){
        #pragma opm single nowait
        {
            char enviando[50] = "serah que chega? ";
            MPI_Request *requestS;
            printf("single, rank: %d, thread: %d\n", rank, iam);
            //MPI_Isend(enviando, 50, MPI_CHAR, 0, 0, MPI_COMM_WORLD, requestS);
            MPI_Send(enviando, 50, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
            printf("enviando\n");
        }
        }
    }

  MPI_Finalize();
}