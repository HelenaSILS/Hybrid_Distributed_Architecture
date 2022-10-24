#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <wait.h>
#include <string.h>
#include <omp.h>
#include <sys/types.h>
#include "mpi.h"

int main(int argc, char *argv[]) {
    int numprocs, rank, namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int iam = 0, np = 1;
    int provided, required=MPI_THREAD_SERIALIZED;
    // First call MPI_Init
    /// MPI_Init(&argc, &argv);
    MPI_Init_thread(&argc, &argv, required, &provided);
    //MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Get_processor_name(processor_name, &namelen);
    printf("no MPI, rank: %d, MPI_COMM_WORLD: %d\n", rank, MPI_COMM_WORLD);
    
    // /* Check that the MPI implementation supports MPI_THREAD_MULTIPLE */
    // if (provided < MPI_THREAD_MULTIPLE) {
    //     printf("MPI does not support MPI_THREAD_MULTIPLE\n");
    //     MPI_Abort(MPI_COMM_WORLD, -1);
    //     return 0;
    // }

    #pragma omp parallel  default(shared) private(iam, np) firstprivate(rank) num_threads(8)
    {
        np = omp_get_num_threads();
        iam = omp_get_thread_num();
        char recebendo[50];
        char devolta[] = "voltei\n";
        //printf("Hello from thread %d out of %d from process %d out of %d on %s\n", iam, np, rank, numprocs, processor_name);
        char id[40];
        int length = snprintf( NULL, 0, "%d", iam );
        char* iam_to_str = malloc( length + 1 );
        snprintf( iam_to_str, length + 1, "%d", iam );
        strcpy(id, "echo \"meu id eh: ");
        strcat(id, iam_to_str);
        strcat(id, "\" >> out.txt");
        printf("antes: %s\n",id);
        //#pragma critical
        { 
        system(id);
        }
        int i = 2 + 2;
        //master global
        //if(iam==0 && rank==0){ 
        #pragma omp sections firstprivate(rank,iam, np) 
        { 
        #pragma omp section 
        { 
        printf("master, rank: %d, thread: %d\n", rank, iam);
        // while(flag==0){
        //     MPI_Test(request, &flag, status);
        //     sleep(1);
        // } 
        //MPI_Irecv(recebendo, 50, MPI_CHAR, 1, 0, MPI_COMM_WORLD, request);
        MPI_Status status0, status1;
        MPI_Recv(recebendo, 50, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status0);
        printf("message de 0: %s\n", recebendo);
        MPI_Send(devolta, 50, MPI_CHAR, status0.MPI_SOURCE, status0.MPI_TAG, MPI_COMM_WORLD);
        MPI_Recv(recebendo, 50, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status1);
        printf("message de 1: %s\n", recebendo);
        MPI_Send(devolta, 50, MPI_CHAR, status1.MPI_SOURCE, status1.MPI_TAG, MPI_COMM_WORLD);
        }
        //}else{

            //master local do no 0
            //if(iam==1 && rank==0){
            //#pragma omp single nowait
            #pragma omp section 
            {
                char enviando[50] = "serah que chega? 0";
                MPI_Request *requestS;
                printf("single, rank: %d, thread: %d\n", rank, iam);
                //MPI_Isend(enviando, 50, MPI_CHAR, 0, 0, MPI_COMM_WORLD, requestS);
                MPI_Send(enviando, 50, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
                printf("enviando 0\n");
                MPI_Recv(enviando, 50, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                printf("de volta 0: %s\n", enviando);
            }
            

                //master local do no 1
                //if(iam==0 && rank==1){
                #pragma omp section 
                //#pragma omp critical
                {
                    char enviando[50] = "serah que chega? 1";
                    MPI_Request *requestS;
                    printf("single, rank: %d, thread: %d\n", rank, iam);
                    //MPI_Isend(enviando, 50, MPI_CHAR, 0, 0, MPI_COMM_WORLD, requestS);
                    MPI_Send(enviando, 50, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
                    printf("enviando 1\n");
                    MPI_Recv(enviando, 50, MPI_CHAR, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    printf("de volta 1: %s\n", enviando);
                }
                
            
        
        
        // if(!(iam==0 && rank==1) && !(iam==1 && rank==0)){
        //     printf("depois %s\n",id);
        //     system(id);
        
        // }
        }
    }

  MPI_Finalize();
}