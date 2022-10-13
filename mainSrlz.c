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
#include "file_manager.h"
#define MSG_FROM_GLOBAL_MASTER 999
#define NTHREADS 4


int runCommandOnSample (int, int, int, int, struct queueNode *, struct queueNode *);

int main(int argc, char *argv[]) {
    int numprocs, rank, namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int iam = 0, np = 1, init;
    int provided, required=MPI_THREAD_SERIALIZED;
    init=MPI_Init_thread(&argc, &argv, required, &provided);
    if(init!=MPI_SUCCESS){
        printf("Erro ao inicializar o MPI_Init_thread\n");
        exit(1);
    }
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

    /***********************************************************************************/

    /*
    * Input files
    */
    //FILE *confFile;
    FILE *commandsFile;
    FILE *samplesFile;

    if ((commandsFile = fopen(argv[1], "r"))==NULL){
        printf("erro ao abrir arquivo comandos\n");
        exit(1);
    }

    if ((samplesFile = fopen(argv[2], "r"))==NULL){
        printf("erro ao abrir arquivo samples\n");
        exit(1);
    }
    
    char *buffer;
	buffer = (char*)malloc(sizeof(char)*(MAX_BUFFER_CHAR));

	struct queueNode *commandsQueue = NULL;
	commandsQueue=trataSamples(commandsFile, commandsQueue);
    printf("comandos: \n");
	printQueue(commandsQueue);

    struct queueNode *samplesQueue = NULL;
	samplesQueue=trataSamples(samplesFile, samplesQueue);
    printf("samples: \n");
	printQueue(samplesQueue);


    /***********************************************************************************/


    //criar vetores de exemplo
    int *shared_pointer = (int*)malloc(sizeof(int)*NTHREADS);
    int *workers_status_array = (int*)malloc(sizeof(int)*NTHREADS);
    
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

    #pragma omp parallel default(shared) shared(shared_pointer, workers_status_array, commandsQueue, samplesQueue) private(iam, np, current_time) firstprivate(rank, numprocs, initial_time) num_threads(NTHREADS)
    {
        np = omp_get_num_threads();
        iam = omp_get_thread_num();
        char recebendo[50];
        char devolta[] = "voltei\n";
        //printf("Hello from thread %d out of %d from process %d out of %d on %s\n", iam, np, rank, numprocs, processor_name);
        int i;
        char id[40];
        int length = snprintf( NULL, 0, "%d", (iam+rank*100));
        char* iam_to_str = malloc( length + 1 );
        int statusRun=0;
        int iteration = 0;

        if(!(iam<=1 && rank==0) && !(iam==0 && rank!=0)){ 

            //atualiza um vetor
            shared_pointer[iam]=shared_pointer[iam]*10;

            while(workers_status_array[iam]==0){ 
                if(rank==0)
                    sleep(5);
                statusRun = runCommandOnSample(iteration, iam, rank, numprocs, samplesQueue, commandsQueue);
                printf("status run de thread %d, rank %d: %d\n", iam, rank, statusRun);
                iteration++;
                workers_status_array[iam]=1;
            }

        }

        //master global
        if(iam==0 && rank==0){ 
        #pragma omp master
        { 
        int probe = -1;
        int nlocal_master = numprocs;
        int erro1, erro2;
        MPI_Status *status = (MPI_Status*)malloc(sizeof(MPI_Status)*numprocs);
        int *flags = (int*)malloc(sizeof(int)*numprocs);
        int i;
        for (i=0; i< numprocs; i++){
            flags[i]=0;
        }
        i=0;
        time_t duration;
            // pooling para saber se ele recebeu msg. Quando se recebe, é preciso renovar a variável probe para falso novamente
            while(nlocal_master>0){ 
                //int MPI_Iprobe(int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status)
                probe = MPI_Iprobe(i, MPI_ANY_TAG, MPI_COMM_WORLD, &flags[i], &status[i]);
                nanosleep(100);
                if(flags[i] && status[i].MPI_TAG!=MSG_FROM_GLOBAL_MASTER){ 
                    erro1=MPI_Recv(recebendo, 50, MPI_CHAR, status[i].MPI_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status[i]);
                    printf("status Recv Master Global: %d, message recb.: %s\n", erro1, recebendo);
                    erro2=MPI_Send(devolta, 50, MPI_CHAR, status[i].MPI_SOURCE, MSG_FROM_GLOBAL_MASTER, MPI_COMM_WORLD);
                    printf("status do Send do Master Global = %d\n", erro2);
                    nlocal_master--;
                    probe = -1;
                }
                i=(i+1)%numprocs;
                //printf("proximo i: %d, nlocal_master: %d\n", i,nlocal_master);
            }
        } 
        }else{ 

            //master local do no 0
            if((iam==1 && rank==0) || (iam==0 && rank==1)){
            #pragma omp single nowait
            //#pragma omp critical
            {
                char enviando[50] = "bibi";
                MPI_Request *requestS;
                double duration;
                int i=0;
                int brk = 0;
                int erro1, erro2;
                
                while(i < NTHREADS){
                    nanosleep(500);
                    #pragma omp flush(workers_status_array)
                    if(workers_status_array[i]==1){
                        current_time = time(NULL);
                        duration = difftime(current_time, initial_time);
                        printf("no master local %d: workers_status_array[%d]=%d\n",iam, i, workers_status_array[i]);
                        brk=1;
                        workers_status_array[i]=0;
                        break;
                    }
                    i++;
                    if(i==NTHREADS){
                        i=0;
                    }
                }

                printf("initial time master local: %.7f\n", initial_time);
                printf("current time master local: %.7f\n", current_time);
                printf("duration em %d: %.7f\n",iam, duration);
                snprintf(enviando, 50, "%lf", duration);

                erro1=MPI_Send(enviando, 50, MPI_CHAR, 0, (rank*100+iam), MPI_COMM_WORLD);
                printf("status do Send Master local %d = %d\n", iam, erro1);

                erro2=MPI_Recv(enviando, 50, MPI_CHAR, 0, MSG_FROM_GLOBAL_MASTER, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                printf("status do Recv Master local %d = %d\n", iam, erro2);
                printf("de volta, rank %d: %s\n", rank, enviando);
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

    }

  MPI_Finalize();
  return 0;
}

int runCommandOnSample (int iteration, int iam, int rank, int numprocs, struct queueNode *samplesQueue, struct queueNode *commandsQueue){

    int offset=iteration*numprocs;
    int pos = iam + rank + offset;

    struct queueNode *sample;
    sample = retornaElemN(samplesQueue, pos);
    if (sample==NULL){
        return 1;
    }

    int i=0;
    struct queueNode *command = retornaElemN(commandsQueue, i);
    char *buffer=(char*)malloc(sizeof(char)*(MAX_BUFFER_CHAR));
    while(command!=NULL){
        buffer = insertVariableValue(command->nome, sample->nome);
        //system(buffer);
        printf("comando: ->%s<-\n", buffer);
        i++;
        command=retornaElemN(commandsQueue,i);
    }
    return 0;

}