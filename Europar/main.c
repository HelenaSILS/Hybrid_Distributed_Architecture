#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <wait.h>
#include <string.h>
#include <errno.h>
//#include <omp.h>
#include <sys/types.h>
#include "mpi.h"
#include <time.h>
#include "file_manager.h"
#define MSG_FROM_GLOBAL_MASTER 999
#define NTHREADS 4
#define DONE 0
#define ALMOST_DONE 1

#define PROCESSING 0
#define GO 1
#define PAUSE 2
#define END 3

extern int errno ;


int main(int argc, char *argv[]) {


    /***********************************************************************************/


    int numprocs, rank, namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int init;
    int world_size;
    int provided, required=MPI_THREAD_MULTIPLE;
    init=MPI_Init_thread(&argc, &argv, required, &provided);
    if(init!=MPI_SUCCESS){
        fprintf(stderr,"Erro ao inicializar o MPI_Init_thread: %d, rank: %d\n", init, rank);
        exit(1);
    }
    init=MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    if(init!=MPI_SUCCESS){
        fprintf(stderr,"Erro no Comm_size: %d, rank: %d\n", init, rank);
    }
    init=MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if(init!=MPI_SUCCESS){
        fprintf(stderr,"Erro no Comm_rank: %d, rank: %d\n", init, rank);
    }
    init=MPI_Get_processor_name(processor_name, &namelen);
    if(init!=MPI_SUCCESS){
        fprintf(stderr,"Erro no MPI_Get_processor_name: %d, rank: %d\n", init, rank);
    }
    fprintf(stderr,"no MPI, rank: %d, MPI_COMM_WORLD: %d, processors name: %s\n", rank, MPI_COMM_WORLD, processor_name);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);


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
    //FILE *confFile;

    int countSamples=0;
    int countCommands=0;

    char executeMatrix[200][200][MAX_BUFFER_CHAR];
    FILE *commandsFile;
    FILE *samplesFile;
    int errnum;

    if(rank==0){ 


        if ((commandsFile = fopen(argv[1], "r"))==NULL){
            errnum = errno;
            fprintf(stderr,"erro ao abrir arquivo comandos, erro: %s\n", strerror(errnum));
            exit(1);
        }

        if ((samplesFile = fopen(argv[2], "r"))==NULL){
            errnum = errno;
            fprintf(stderr,"erro ao abrir arquivo samples, erro: %s\n", strerror(errnum));
            exit(1);
        }


        int i;
        int j;
        int k;
        char buffer[MAX_BUFFER_CHAR];

        //commands input
        //int countCommands=0;
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
        //int countSamples=0;
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

        char executeMatrix_[countSamples][countCommands][MAX_BUFFER_CHAR];

        printf("aqui de boas\n");

        for(j=0; j<countSamples; j++){
            for(k=0; k< countCommands; k++){
                    makeQueueOutOfCommandsAndSample (commandsMatrix, countCommands, samplesMatrix[j], executeMatrix_[j]);
                    //printf("-> -%s-\n", executeMatrix[k][j]);
            }
        }

        printf("depois de preencher la executeMatrix_\n");
        /*
        * send to the others nodes
        */

        //int j, k;
        // for(j=0; j<countSamples; j++){
        //     for(k=0; k< countCommands; k++){
        //             printf("<--> -%s-\n", executeMatrix_[j][k]);
        //     }
        // }

        for(j=0; j<countSamples; j++){
            for(k=0; k< countCommands; k++){
                    memcpy(executeMatrix[j][k], executeMatrix_[j][k], (sizeof(char)*MAX_BUFFER_CHAR));
                    //printf("<===> -%s-\n", executeMatrix[j][k]);
            }
        }
       
        int l;
        for (l = 1; l < world_size; l++) {
            MPI_Send(&countSamples, 1, MPI_INT, l, 1, MPI_COMM_WORLD );
            MPI_Send(&countCommands, 1, MPI_INT, l, 2, MPI_COMM_WORLD );
            MPI_Send(&(executeMatrix_[0][0]), countCommands*countSamples*MAX_BUFFER_CHAR, MPI_CHAR, l, 3, MPI_COMM_WORLD );
        }
    }else{
        // If we are a receiver process, receive the data from the root
        MPI_Recv(&countSamples, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&countCommands, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        char executeMatrix_[countSamples][countCommands][MAX_BUFFER_CHAR];

        MPI_Recv(&(executeMatrix_[0][0]),countSamples*countCommands*MAX_BUFFER_CHAR, MPI_CHAR, 0, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
       
        // executeMatrix = (char***)malloc(sizeof(char)*countSamples*countCommands*MAX_BUFFER_CHAR);
        // //memcpy(executeMatrix, executeMatrix_, sizeof(char)*countSamples*countCommands*MAX_BUFFER_CHAR);
        // memcpy((void*)executeMatrix, executeMatrix_[0][0], sizeof(char)*countSamples*countCommands*MAX_BUFFER_CHAR);
        printf("recebi tudinho\n");

        int j, k;
        for(j=0; j<countSamples; j++){
            for(k=0; k< countCommands; k++){
                    memcpy(executeMatrix[j][k], executeMatrix_[j][k], (sizeof(char)*MAX_BUFFER_CHAR));
                    //printf("<===> -%s-, rank %d\n", executeMatrix[j][k], rank);
            }
        }

    }
    
    int j, k;
    printf("fora dos elses\n");
    char executeMatrix_[countSamples][countCommands][MAX_BUFFER_CHAR];
    for(j=0; j<countSamples; j++){
        for(k=0; k< countCommands; k++){
                memcpy(executeMatrix_[j][k], executeMatrix[j][k], (sizeof(char)*MAX_BUFFER_CHAR));
                //printf("rank: %d -%s-\n",rank,  executeMatrix[j][k]);
        }
    }


    // for(j=0; j<countSamples; j++){
    //     for(k=0; k< countCommands; k++){
    //             printf("rank: %d -%s-\n",rank,  executeMatrix_[j][k]);
    //     }
    // }


    /****************************************************************************************/

    FILE *rank_file;
    char rank_file_name[MAX_BUFFER_CHAR];
    sprintf(rank_file_name, "%d", rank);
    strcat(rank_file_name, "_file.txt");
    if ((rank_file = fopen(rank_file_name, "w"))==NULL){
        errnum = errno;
        fprintf(stderr,"erro ao abrir arquivo rank file, rank %d, erro: %s\n", rank, strerror(errnum));
        exit(2);
    }


    fprintf(rank_file, "rank %d tamanho da executeMatrix_: %d\n", rank, sizeof(executeMatrix_) );

    // if(rank==0){
    //     //print executeMatrix
    //     for(j=0; j<countSamples; j++){
    //         for(k=0; k< countCommands; k++){
    //                 printf("-> -%s-\n", executeMatrix[k][j]);
    //         }
    //     }
    // }

    //criar vetores de exemplo
    int *shared_pointer = (int*)malloc(sizeof(int)*NTHREADS);
    
    //exemplos
    int i;
    for(i=0; i<NTHREADS; i++){
        shared_pointer[i]=i;
    }

    //pega o tempo atual
    time_t initial_time, current_time;
	double duration_total;
    initial_time = time(NULL);
    fprintf(stderr,"current time inicial: %d\n", initial_time);

    /***********************************************************************************/

    //vectors for communication between workers and local master
    // int *local_master_order_array = (int*)malloc(sizeof(int)*NTHREADS);
    // int *workers_status_array = (int*)malloc(sizeof(int)*NTHREADS);
    // int *p_array = (int*)malloc(sizeof(int)*NTHREADS);
    // int *indexFromLocalMaster = (int*)malloc(sizeof(int)*NTHREADS);

    //exemplos
    // for(i=0; i<NTHREADS; i++){
    //     local_master_order_array[i]=PROCESSING;
    // }

    // for(i=0; i<NTHREADS; i++){
    //     workers_status_array[i]=DONE;
    // }

    // for(i=0; i<NTHREADS; i++){
    //     p_array[i]=0;
    // }

    #pragma omp parallel default(shared) private(current_time, errnum) firstprivate(rank, numprocs, initial_time, countSamples, countCommands, executeMatrix_) num_threads(NTHREADS)
    {
    //#pragma omp parallel default(shared) shared(shared_pointer, workers_status_array, local_master_order_array) private(current_time, errnum) firstprivate(rank, numprocs, initial_time, countSamples, countCommands, executeMatrix) num_threads(NTHREADS)
    
        int np = omp_get_num_threads();
        int iam = omp_get_thread_num();
        char recebendo[50];
        char devolta[] = "voltei\n";
        //#pragma omp critical
        //{
            fprintf(rank_file,"Hello from thread %d out of %d from process %d out of %d on %s\n", iam, np, rank, numprocs, processor_name);
        //}
        int i;
        char id[40];
        int length = snprintf( NULL, 0, "%d", (iam+rank*100));
        char* iam_to_str = malloc( length + 1 );
        int statusRun=0;
        int iteration = 0;

    /***********************************************************************************/

		int sysReturn=0;
		double duration;

        //to rewind fasterq
        // char rm_fasterq[] = "rm /scratch/inova-covd19/helena.silva/MeusTestes/experimento01/experimento2/";
        // char prefix1_fasterq[] = ".sra_1.fastq";
        // char prefix2_fasterq[] = ".sra_2.fastq";
        // char cmd_clt[MAX_BUFFER_CHAR] = "";

		int iterator=0;
		//int offset = (rank==0) ? 2 : 1;
		int offset = 0;
		//int position = (iam-offset)*numprocs + rank + (NTHREADS*numprocs-2*offset)*iterator;
		int position = (iam-offset)*numprocs + rank + (NTHREADS-offset)*numprocs*iterator;
		time_t current_time2;

        //for dummy only
        char cmd_complete[MAX_BUFFER_CHAR]="";
        char pos_str[4]="";
        char space_str[]=" ";
        char ul_str[]="_";
        char eM[countSamples][countCommands][MAX_BUFFER_CHAR];


    #pragma omp critical
    {
        for(j=0; j<countSamples; j++){
            for(k=0; k< countCommands; k++){
                    fprintf(rank_file, ", iam %d, rank: %d -%s-\n",iam, rank,  executeMatrix_[j][k]);
            }
        }
    }
        fprintf(rank_file, "em iam %d, rank %d tamanho da executeMatrix_: %d\n", iam, rank, sizeof(executeMatrix_) );

        fprintf(rank_file, "nas thread, antes do loop\n");
		while (position < countSamples)
		{
            fprintf(rank_file,"nas thread, dentro do loop while\n");

			/*
			*	Nem sempre ele consegue terminar o processo chamado pelo system quando são alocados mais de dois nodes no SDumont
			* 	Porém, quando são apenas dois nodes, esse problema nunca aconteceu nas inúmeras vezes que foi testado
			*/
			//execute until the penultimate one
			for (i=0; i<countCommands; i++){
                printf("nas thread, dentro do loop for\n");
				//printf("dentro da thread %d, processo %d, execute[%d][%d]: %s\n", iam, rank, position, iterator, executeMatrix[position][i]);
                //#pragma omp flush(executeMatrix)
				//fprintf(stderr,"dentro da thread %d, processo %d, cmd_complete: -%s-\n", iam, rank, executeMatrix[position][i]);
                fprintf(rank_file,"dentro da thread %d, processo %d, position: %d, i: %d cmd_complete: -%s-\n", iam, rank, position, i, executeMatrix_[position][i]);
				sysReturn = system(executeMatrix_[position][i]);
				if(sysReturn!=0){
				    errnum=errno;
				    //current_time2 = time(NULL);
				    //fprintf(stderr, "tempo inicial: %d, tempo final: %d, thread %d, rank %d\n", current_time, current_time2, iam, rank);
				    fprintf(rank_file,"error %s (%d) status system = %d, thread %d, rank %d\n=> %s\n", strerror(errnum), errnum, sysReturn, iam, rank, executeMatrix_[position][i]);

                    //se for o erro do fastq:
                    /*if(i==1){    
                        strcat(cmd_clt, rm_fasterq);
                        strcat(cmd_clt, samplesMatrix[position]);
                        strcat(cmd_clt, prefix1_fasterq);
                        sysReturn = system(cmd_clt);
                        memcpy(complete, "", sizeof(""));

                        strcat(cmd_clt, rm_fasterq);
                        strcat(cmd_clt, samplesMatrix[position]);
                        strcat(cmd_clt, prefix2_fasterq);
                        sysReturn = system(cmd_clt);
                        memcpy(complete, "", sizeof(""));
                    }*/
				}
			}


			//p_array[iam]=1;
			//#pragma omp flush(workers_status_array)
			//workers_status_array[iam]=ALMOST_DONE;
			
            //#pragma omp flush(executeMatrix_)
			//fprintf(rank_file,"ultimo, dentro da thread %d, processo %d, cmd_complete: -%s-\n", iam, rank, executeMatrix_[position][i]);
			//execute the last one
			// sysReturn = system(executeMatrix_[position][i]);
			// if(sysReturn!=0){
			//         fprintf(stderr,"error in, thread %d, rank %d %s\n", iam, rank, executeMatrix_[position][i]);
			// }
            //memcpy(cmd_complete, "", sizeof(""));

			//workers_status_array[iam]=DONE;
			current_time = time(NULL);
			duration = difftime(current_time, initial_time);
			fprintf(rank_file,"termino de thread %d, rank %d, duration:  %lf, %s\n", iam, rank, duration, executeMatrix_[position][0]);                
			iterator++;
			position = (iam-offset)*numprocs + rank + (NTHREADS*numprocs-2*offset)*iterator;
			

        }
    }

    // /***********************************************************************************/

    // /***********************************************************************************/


    // /***********************************************************************************/

    // /***********************************************************************************/
 


    // /***********************************************************************************/

    if(rank==0){
        current_time = time(NULL);
        duration_total = difftime(current_time, initial_time);
        fprintf(stderr,"rank %d, total duration:  %lf\n", rank, duration_total); 
    }
    MPI_Finalize();
    return 0;
}



