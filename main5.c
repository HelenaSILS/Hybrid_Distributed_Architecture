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
//communication between workers and local master constants
#define DONE 0
#define ALMOST_DONE 1

#define PROCESSING 0
#define GO 1
#define PAUSE 2
#define END 3


//int runCommandOnSample (int, int, int, int, struct queueNode *, struct queueNode *);

int main(int argc, char *argv[]) {
    int numprocs, rank, namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int iam = 0, np = 1, init;
    int provided, required=MPI_THREAD_MULTIPLE;
    init=MPI_Init_thread(&argc, &argv, required, &provided);
    if(init!=MPI_SUCCESS){
        fprintf(stderr,"Erro ao inicializar o MPI_Init_thread\n");
        exit(1);
    }
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Get_processor_name(processor_name, &namelen);
    fprintf(stderr,"no MPI, rank: %d, MPI_COMM_WORLD: %d, processors name: %s\n", rank, MPI_COMM_WORLD, processor_name);


    /* Check that the MPI implementation supports MPI_THREAD_MULTIPLE */
    if (provided < MPI_THREAD_MULTIPLE) {
        fprintf(stderr,"MPI does not support MPI_THREAD_MULTIPLE\n");
        MPI_Abort(MPI_COMM_WORLD, -1);
        return 0;
    }

    /***********************************************************************************/

    /*
    * Input files
    */
    FILE *confFile;
    FILE *commandsFile;
    FILE *samplesFile;

    if ((commandsFile = fopen(argv[1], "r"))==NULL){
        fprintf(stderr,"erro ao abrir arquivo comandos\n");
        exit(1);
    }

    if ((samplesFile = fopen(argv[2], "r"))==NULL){
        fprintf(stderr,"erro ao abrir arquivo samples\n");
        exit(1);
    }

    int i;
    int j;
    int k;
    char buffer[MAX_BUFFER_CHAR];

    //commands input
	int countCommands=0;
    while (fscanf(commandsFile, "%[^\n] ", buffer) != EOF) {
        countCommands++;
    }
    fprintf(stderr,"numero de comandos = %d\n", countCommands);
    rewind(commandsFile);
    
    char commandsMatrix[countCommands][MAX_BUFFER_CHAR];

    for(j=0; j<countCommands; j++){
        for(k; k< MAX_BUFFER_CHAR; k++){
            buffer[k]='\0';
        }
		fscanf(commandsFile, "%[^\n] ", buffer);
		strcpy(commandsMatrix[j], buffer);
	}


    //samples input
	int countSamples=0;
    while (fscanf(samplesFile, "%[^\n] ", buffer) != EOF) {
        countSamples++;
    }
    fprintf(stderr,"numero de amostras .sra = %d\n", countSamples);
    rewind(samplesFile);

    char samplesMatrix[countSamples][MAX_BUFFER_CHAR];

	for(j=0; j<countSamples; j++){
        for(k; k< MAX_BUFFER_CHAR; k++){
            buffer[k]='\0';
        }
        fscanf(samplesFile, "%[^\n] ", buffer);
		strcpy(samplesMatrix[j], buffer);
	}

    char executeMatrix[countSamples][countCommands][MAX_BUFFER_CHAR];

    for(j=0; j<countSamples; j++){
        for(k=0; k< countCommands; k++){
                makeQueueOutOfCommandsAndSample (commandsMatrix, countCommands, samplesMatrix[j], executeMatrix[j]);
        }
    }

  /***********************************************************************************/


    //criar vetores de exemplo
    int *shared_pointer = (int*)malloc(sizeof(int)*NTHREADS);
    
    //exemplos
    for(i=0; i<NTHREADS; i++){
        shared_pointer[i]=i;
    }

    //pega o tempo atual
    time_t initial_time, current_time;
    initial_time = time(NULL);
    fprintf(stderr,"current time inicial: %d\n", initial_time);

    /***********************************************************************************/

    //vectors for communication between workers and local master
    int *local_master_order_array = (int*)malloc(sizeof(int)*NTHREADS);
    int *workers_status_array = (int*)malloc(sizeof(int)*NTHREADS);
    int *p_array = (int*)malloc(sizeof(int)*NTHREADS);
    int *indexFromLocalMaster = (int*)malloc(sizeof(int)*NTHREADS);

    //exemplos
    for(i=0; i<NTHREADS; i++){
        local_master_order_array[i]=PROCESSING;
    }

    for(i=0; i<NTHREADS; i++){
        workers_status_array[i]=DONE;
    }

    for(i=0; i<NTHREADS; i++){
        p_array[i]=0;
    }

    #pragma omp parallel default(shared) shared(shared_pointer, workers_status_array, local_master_order_array) private(iam, np, current_time) firstprivate(rank, numprocs, initial_time, executeMatrix, countSamples, countCommands) num_threads(NTHREADS)
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

    /***********************************************************************************/
        //workers
        if(!(iam<=1 && rank==0) && !(iam==0 && rank!=0)){ 
            int sysReturn;
            int i;
            double duration;
            char toExecute[countCommands][MAX_BUFFER_CHAR];
            char crntSample[MAX_BUFFER_CHAR];
            char nextSample[MAX_BUFFER_CHAR];


            int iterator=0;
            int offset = (rank==0) ? 2 : 1;
            //int position = (iam-offset)*numprocs + rank + (NTHREADS*numprocs-2*offset)*iterator;
            int position = (iam-offset)*numprocs + rank + (NTHREADS-offset)*numprocs*iterator;

            //#pragma omp flush(workers_status_array, local_master_order_array, indexFromLocalMaster)
            while (position < countSamples)
            {
                // #pragma omp flush(samplesMatrix)
                // strcpy(crntSample, samplesMatrix[position]);
                // printf("na thread %d do no %d, first crntSample= %s\n", iam, rank, crntSample);

                //execute until the penultimate one
                for (i=0; i<countCommands-1; i++){
                    //printf("dentro da thread %d, processo %d, execute[%d][%d]: %s\n", iam, rank, position, iterator, executeMatrix[position][i]);
                    sysReturn = system(executeMatrix[position][i]);
                    if(sysReturn!=0){
                         fprintf(stderr,"error in, thread %d, rank %d %s\n", iam, rank, executeMatrix[position][i]);
                    }
                }


                p_array[iam]=1;
                #pragma omp flush(workers_status_array)
                workers_status_array[iam]=ALMOST_DONE;

                //I am otimist that this while will never be true, but it is an important logical lock
                // while(local_master_order_array[iam]!=PROCESSING){
                //     sleep(1);
                // }

                //execute the last one
                sysReturn = system(executeMatrix[position][i]);
                if(sysReturn!=0){
                        fprintf(stderr,"error in, thread %d, rank %d %s\n", iam, rank, executeMatrix[position][i]);
                }
                //printf("(ultimo) dentro da thread %d, processo %d, execute[%d][%d]: %s\n", iam, rank, position, iterator, executeMatrix[position][i]);
                

                //next step happens only when nextSample is updated
                // while(local_master_order_array[iam]!=GO){
                //     nanosleep(200);
                // }
                workers_status_array[iam]=DONE;

                current_time = time(NULL);
                duration = difftime(current_time, initial_time);
                fprintf(stderr,"termino de thread %d, rank %d, duration:  %lf, %s\n", iam, rank, duration, executeMatrix[position][0]);

                
                iterator++;
                position = (iam-offset)*numprocs + rank + (NTHREADS*numprocs-2*offset)*iterator;
                
                
            }

        }

    /***********************************************************************************/

    /***********************************************************************************/

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
                sleep(1);
                if(flags[i] && status[i].MPI_TAG!=MSG_FROM_GLOBAL_MASTER){ 
                    erro1=MPI_Recv(recebendo, 50, MPI_CHAR, status[i].MPI_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status[i]);
                    fprintf(stderr,"status Recv Master Global: %d, message de %d recb.: %s\n", erro1, status[i].MPI_SOURCE, recebendo);
                    erro2=MPI_Send(devolta, 50, MPI_CHAR, status[i].MPI_SOURCE, MSG_FROM_GLOBAL_MASTER, MPI_COMM_WORLD);
                    fprintf(stderr,"status do Send do Master Global = %d, enviando para %d \n", erro2, status[i].MPI_SOURCE);
                    nlocal_master--;
                    probe = -1;
                }
                i=(i+1)%numprocs;
                //printf("proximo i: %d, nlocal_master: %d\n", i,nlocal_master);
            }
        } 
        }else{ 



    /***********************************************************************************/

    /***********************************************************************************/
 

            //Local Master
            if((iam==1 && rank==0) || (iam==0 && rank!=0)){
            #pragma omp single nowait
            {
                char enviando[50] = "bibi";
                MPI_Request *requestS;
                double duration;
                int i=0;
                int brk = 0;
                int erro1, erro2;
                struct queueNode *aux;

               //there are NTHREAD*numprocs-numprocs-1 workers threads, so, initially, it is the first value
                int iterator = 0;
                int indexSample = NTHREADS*numprocs-numprocs-1 + rank*iterator;
                fprintf(stderr,"valor do indice na fila de sample do rank %d, iteracao %d: %d\n", rank, iterator, indexSample);

                //rank 0 has two masters threads: local and global; the other ranks have only local thread
                int smallest_thread = (rank==0) ? 2 : 1;
                int greatest_thread = NTHREADS;
                int threadIterator = smallest_thread;

                int count=indexSample;

                sleep(1);
                current_time = time(NULL);
                duration = difftime(current_time, initial_time);
                snprintf(enviando, 50, "%lf", duration);

                erro1=MPI_Send(enviando, 50, MPI_CHAR, 0, (rank*100+iam), MPI_COMM_WORLD);
                fprintf(stderr,"status do Send Master local %d = %d\n", iam, erro1);

                erro2=MPI_Recv(enviando, 50, MPI_CHAR, 0, MSG_FROM_GLOBAL_MASTER, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                printf("status do Recv Master local %d = %d\n", iam, erro2);
                fprintf(stderr,"de volta, rank %d: %s\n", rank, enviando);
            }
            } 

    /***********************************************************************************/

        }

    }

  MPI_Finalize();
  return 0;
}



