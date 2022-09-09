#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
# include <omp.h>

int main(int argc, char** argv) {

int rank, n_ranks;
int my_first, my_last;
int numbers = 10;

int provided, required=MPI_THREAD_MULTIPLE;


// First call MPI_Init
/// MPI_Init(&argc, &argv);
MPI_Init_thread(&argc, &argv, required, &provided);

// Check that there are at least two ranks
MPI_Comm_size(MPI_COMM_WORLD,&n_ranks);
if( n_ranks < 2 ){
    printf("This example requires at least two ranks");
    MPI_Finalize();
    return(1);
}
printf("total de nos: %d\n", n_ranks);

// Get my rank
MPI_Comm_rank(MPI_COMM_WORLD,&rank);



#pragma omp parallel private(rank)
{

    MPI_Request *request;
    MPI_Status *status;
    int *flag;

    request = (MPI_Request*) malloc (sizeof(MPI_Request)*n_ranks);
    status = (MPI_Status*) malloc (sizeof(MPI_Status)*n_ranks);
    flag = (int*) malloc (sizeof(int)*n_ranks);
    for(int i=0; i< n_ranks; i++)
    flag[i]=0;

    if(rank == 0 && omp_get_thread_num()==0 ){
    #pragma omp master
    { 
        char message0[50];
        char message1[50];
        int i=0, cont=0;
        //MPI_Wait( &request, &status );
        // while(cont<n_ranks){
        //     for(i=0; i<n_ranks;i++){
        //         MPI_Test(&request[i], &flag[i], &status[i]);
        //         if(flag[i]==0){
        //             printf("aguardando!, i: %d, cont: %d\n", i, cont);
        //             sleep(7);
        //         }else{
        //             if(i==0){ 
        //                 MPI_Irecv(message0, 50, MPI_CHAR, i, i, MPI_COMM_WORLD, &request[i]);
        //                 printf("%s who sent: %d, request: %d, status: %d\n",message0, i, request[i], status[i]);
        //             }
        //             else{
        //                 MPI_Irecv(message1, 50, MPI_CHAR, i, i, MPI_COMM_WORLD, &request[i]);
        //                 printf("%s who sent: %d, request: %d, status: %d\n",message1, i, request[i], status[i]);
        //             }
        //             cont++;
        //         }
        //     }
        // }

        // while(flag[0]==0){
        //     MPI_Test(&request[0], &flag[0], &status[0]);
        //     sleep(1);
        // } 
        // MPI_Irecv(message0, 50, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &request[0]);
        // printf("%s who sent: %d, request: %d, status: %d\n", message0, 0, request[0], status[0]);
        while(flag[1]==0){
            MPI_Test(&request[1], &flag[1], &status[1]);
            sleep(5);
        } 
        MPI_Irecv(message1, 50, MPI_CHAR, 1, 1, MPI_COMM_WORLD, &request[1]);
        printf("%s who sent: %d, request: %d, status: %d\n", message1, 1, request[1], status[1]);
    }
    }

    // if((rank == 0 && omp_get_thread_num()==1)){

    //     char *message = "Hello, world from Master Local, Master Node!\n";
    //     MPI_Isend(message, 50, MPI_CHAR, rank, rank, MPI_COMM_WORLD, &request[rank]);
    //     printf("rank %d, thread %d mandou msg\n", rank, omp_get_thread_num());
    // }
    
    if((rank != 0 && omp_get_thread_num()==0)){
#pragma omp single nowait
{ 
        char *message = "Hello, world from ML!\n";
        MPI_Isend(message, 50, MPI_CHAR, rank, rank, MPI_COMM_WORLD, &request[rank]);
        printf("rank %d, thread %d mandou msg\n", rank, omp_get_thread_num());
    }
}

 }
// Call finalize at the end
  return MPI_Finalize();
}
