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
#include <time.h>
#define MSG_FROM_GLOBAL_MASTER 999
#define NTHREADS 8

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
    printf("no MPI, rank: %d, MPI_COMM_WORLD: %d\n, processors name: %s", rank, MPI_COMM_WORLD, processor_name);
    
    // /* Check that the MPI implementation supports MPI_THREAD_MULTIPLE */
    // if (provided < MPI_THREAD_MULTIPLE) {
    //     printf("MPI does not support MPI_THREAD_MULTIPLE\n");
    //     MPI_Abort(MPI_COMM_WORLD, -1);
    //     return 0;
    // }

    //criar vetores de exemplo
    int *shared_pointer = (int*)malloc(sizeof(int)*NTHREADS);
    int *workers_status_array = (int*)malloc(sizeof(int)*NTHREADS);
    char shared_char[NTHREADS] = {'q','w','e','r','t','y','u','i'};
    int shared_array[NTHREADS] = {2,4,6,8,10,12,14,16};
    
    //exemplos
    for(int i=0; i<NTHREADS; i++){
        shared_pointer[i]=i;
    }

    for(int i=0; i<NTHREADS; i++){
        workers_status_array[i]=0;
    }

    //pega o tempo atual
    time_t initial_time, current_time;
    initial_time = time(NULL);
    printf("current time inicial: %d\n", initial_time);

    #pragma omp parallel default(shared) shared(shared_array, shared_pointer, workers_status_array) private(iam, np, current_time) firstprivate(rank, numprocs, initial_time) num_threads(NTHREADS)
    {
        np = omp_get_num_threads();
        iam = omp_get_thread_num();
        char recebendo[50];
        char devolta[] = "voltei\n";
        printf("Hello from thread %d out of %d from process %d out of %d on %s\n", iam, np, rank, numprocs, processor_name);
        int i;
        char id[40];
        int length = snprintf( NULL, 0, "%d", (iam+rank*100));
        char* iam_to_str = malloc( length + 1 );

        if(!(iam<=1 && rank==0) && !(iam==0 && rank!=0)){ 
            // prepara variavel que será enviada para o processo
            snprintf( iam_to_str, length + 1, "%d", (iam+rank*100) );
            strcpy(id, "python testepy.py ");
            strcat(id, iam_to_str);
            //printf("para o system: %s\n",id);
            //envia o processo
            system(id);
            //atualiza um vetor
            shared_pointer[iam]=shared_pointer[iam]*10;
            //printf("shared_pointer[%d] = %d\n", iam, shared_pointer[iam]);
            workers_status_array[iam]=1;
            //printf("workers_status_array[%d]=%d\n", iam, workers_status_array[iam]);
            //sleep(3);
        }

        //master global
        if(iam==0 && rank==0){ 
        #pragma omp single nowait
        { 
        //printf("master, rank: %d, thread: %d\n", rank, iam);
        int probe = -1;
        int nlocal_master = numprocs;
        int erro1, erro2;
        MPI_Status status;
        time_t duration;
            // pooling para saber se ele recebeu msg. Quando se recebe, é preciso renovar a variável probe para falso novamente
            while(probe!=MPI_SUCCESS && nlocal_master>0){ 
                probe = MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                if(status.MPI_TAG!=MSG_FROM_GLOBAL_MASTER){ 
                    erro1=MPI_Recv(recebendo, 50, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                    printf("message recb.: %s\n", recebendo);
                    erro2=MPI_Send(devolta, 50, MPI_CHAR, status.MPI_SOURCE, MSG_FROM_GLOBAL_MASTER, MPI_COMM_WORLD);
                    printf("erro2 Send do Master Global = %d\n", erro2);
                    nlocal_master--;
                    probe = -1;
                }
            //}
        }
        } 
        }else{ 

            //master local do no 0
            if((iam==1 && rank==0) || (iam==0 && rank==1)){
            //#pragma omp single nowait
            #pragma omp critical
            {
                char enviando[50] = "bibi";
                MPI_Request *requestS;
                double duration;
                int i=0;
                int brk = 0;
                int erro1, erro2;
                //se
                while(i < NTHREADS){
                    nanosleep(500);
                    #pragma omp flush(shared_pointer)
                    //printf("no master local: shared_pointer[%d]=%d\n",i,shared_pointer[i]);
                    #pragma omp flush(workers_status_array)
                    if(workers_status_array[i]==1){
                        current_time = time(NULL);
                        duration = difftime(current_time, initial_time);
                        printf("no master local %d: workers_status_array[%d]=%d\n",iam, i, workers_status_array[i]);
                        brk=1;
                        break;
                    }
                    i++;
                    if(i==NTHREADS){
                        i=0;
                    }
                }

                //printf("initial time master local: %.7f\n", initial_time);
                //printf("current time master local: %.7f\n", current_time);
                printf("duration em %d: %.7f\n",iam, duration);
                snprintf(enviando, 50, "%lf", duration);
                //printf("single, rank: %d, thread: %d\n", rank, iam);
                MPI_Send(enviando, 50, MPI_CHAR, 0, (rank*100+iam), MPI_COMM_WORLD);
                //printf("enviando 0\n");
                erro2=MPI_Recv(enviando, 50, MPI_CHAR, 0, MSG_FROM_GLOBAL_MASTER, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                printf("erro2 Recv Master local %d = %d\n", iam, erro2);
                printf("de volta %d: %s\n", iam, enviando);
            }
            } //else{ 

                // //master local do no 1
                // if(iam==0 && rank==1){
                // //#pragma omp single nowait
                // #pragma omp critical (local_master_1)
                // {
                //     char enviando[50] = "serah que chega? 1";
                //     MPI_Request *requestS;
                //     printf("single, rank: %d, thread: %d\n", rank, iam);
                //     MPI_Send(enviando, 50, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
                //     printf("enviando 1\n");
                //     MPI_Recv(enviando, 50, MPI_CHAR, 0, MSG_FROM_GLOBAL_MASTER, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                //     printf("de volta 1: %s\n", enviando);
                // }
                // }
            //}
        }

        
        // if(!(iam==0 && rank==1) && !(iam<=1 && rank==0)){
        //     //printf("depois rank: %d, thread: %d %s\n",rank, iam, id);
        //     //printf("Hello from thread %d out of %d from process %d out of %d on %s\n", iam, np, rank, numprocs, processor_name);
        //     //system(id);
        
        // }else{
        //     i = 0+0;
        // }
    }

  MPI_Finalize();
}