# include <stdio.h>
# include <stdlib.h>
//# include "mpi.h"
# include "com_nos.h"
# include "com_threads.h"
# include "utils.h"
# define SIZE 2
# define GLOBAL_MASTER 0
# define LOCAL_MASTER 0
# define FIM_DO_PROCESSAMENTO 1


//minhaDissertecao100%SS 
int main(int argc, char *argv[])
{

	/*
	* Variáveis de MPI
	*/
	int rank_MPI; 	/* rank_MPI DOS PROCESSOS */
	int numtasks_MPI;	/* NÚMERO DE PROCESSOS */
	int nameSize_MPI;	/* TAMANHO DO NOME */
	char computerName_MPI[MPI_MAX_PROCESSOR_NAME];
	//para usar no scatter:
	int sendcount_MPI, recvcount_MPI, source_MPI;
	//para usar no teste do scatter:
	float sendbuf_MPI[SIZE][SIZE] = {
		{1.0, 2.0},
		{5.0, 6.0} };
	float recvbuf_MPI[SIZE];
	//para usar no test do send e recv
	int *buf;
	//para usar no pooling do master
	MPI_Status status_pooling;
	int flog_pooling=0;
	/*
	*	Inicializa o grupo de comunicação de MPI
	*/
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks_MPI);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank_MPI);
	MPI_Get_processor_name(computerName_MPI, &nameSize_MPI);
	//para usar no scatter:
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks_MPI);
	//para usar nos send/recv de FIM_DE_PROCESSAMENTO
	MPI_Request reqs[2];
	//para usar no pooling
	MPI_Request request0, request1;
	int flag=0;


	fprintf(stderr, "nProcess %d (quantos nos existem).\n", numtasks_MPI);

	/*
	* rank_MPI = 0 is the global master (node)
	*/
	fprintf(stderr, "Ola gerado pelo processo %d, n maquina %s.\n", rank_MPI, computerName_MPI);


	//para usar no teste do isend e irecv
	buf = (int*)malloc(sizeof(int)*numtasks_MPI);
	for(int i = 0; i< numtasks_MPI; i++){
		buf[i]=0;
	}
	//usa scatter e manda pedaços do sendbuf_MPI para todos os nós
	if (numtasks_MPI == SIZE) {
		source_MPI = 1;
		sendcount_MPI = SIZE;
		recvcount_MPI = SIZE;
		MPI_Scatter(sendbuf_MPI,sendcount_MPI,MPI_FLOAT,recvbuf_MPI,recvcount_MPI,
					MPI_FLOAT,source_MPI,MPI_COMM_WORLD);

		printf("rank_MPI= %d  Results: %f %f %f %f\n",rank_MPI,recvbuf_MPI[0],
				recvbuf_MPI[1]);
		}
	else
  		printf("Must specify %d processors. Terminating.\n",SIZE);

	









	/*
	* Inicia local master do node 0
	*/

    //#pragma omp parallel num_threads(10)
	#pragma omp parallel
    {
	/*
	* if rank_MPI = 0, the local master thread is omp_get_thread_num = 1 for its rank_MPI
	* else, the local master thread is omp_get_thread_num = 0 for its rank_MPI
	*/




	/*
	* Se for rank_MPI 0, o global master, deve aguardar em pooling os resultados dos demais
	*/
	// int i = 0;
	// if (rank_MPI==0 && omp_get_thread_num()==0){
	// 	/*
	// 	* rank_MPI aguarda cada um dos nós
	// 	*/
	// 	while(i < numtasks_MPI){
	// 		sleep(1);
	// 		MPI_Irecv(&buf[i], 1, MPI_INT, i, FIM_DO_PROCESSAMENTO, MPI_COMM_WORLD, &reqs[i]);
	// 		//TODO: VERIFICAR SE TODOS OS NÓS ACABARAM SÓ QUANDO RECEBER MENSAGEM, OU SEJA, QUANDO buf[i]!=0
	// 		//se ele recebeu mensagem, ele imprime
	// 		if(buf[i]!=0){
	// 			printf ("Node %d received tag %d from node %d \n",GLOBAL_MASTER,FIM_DO_PROCESSAMENTO,rank_MPI);
	// 		}
	// 		printf("ta chegando aqui? %d\n", i);
	// 		i++;
	// 	}
	// }
	// if((rank_MPI==0 && omp_get_thread_num()==1) || (rank_MPI!=0 && omp_get_thread_num()==0)){
	// 	MPI_Isend(&rank_MPI, 1, MPI_INT, GLOBAL_MASTER, FIM_DO_PROCESSAMENTO, MPI_COMM_WORLD, &reqs[rank_MPI]);
	// }

	if(rank_MPI == 0 && omp_get_thread_num()==0 ){
		printf("master Global \n");
     	char message[16];
		MPI_Status status0, status1;
		MPI_Irecv(message, 16, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &request);
		//MPI_Wait( &request, &status );
		while(flag==0){
			printf("MG aguardando!\n");
			sleep(1);
			MPI_Test(&request1, &flag, &status1);
		}
		printf("recebido: %s, request: %d, status: %d\n",message, request, status);
	}

	if((rank_MPI == 0 && omp_get_thread_num()==1)){
		printf("master Local! \n");
		char *message = "Hello, world!\n";
     	MPI_Isend(message, 16, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &request0);
	}

	if((rank_MPI != 0 && omp_get_thread_num()==0)){
		printf("master Local! \n");
		char *message = "Hello, world!\n";
     	MPI_Isend(message, 16, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &request1);
	}
	
	//printf("buf[0] = %d, buf[1] = %d, rank = %d, thread = %d \n", buf[0], buf[1], rank_MPI, omp_get_thread_num());
	//printf("reqs[0] = %d, reqs[1] = %d, rank = %d, thread = %d \n", reqs[0], reqs[1], rank_MPI, omp_get_thread_num());



    	printf("omp_get_thread_num = %d == omp_get_num_procs = %d, processo %d, maquina %s\n", omp_get_thread_num(), omp_get_num_procs(), rank_MPI, computerName_MPI);
    #pragma omp single
        printf("=== > omp_get_num_procs = %d, processo %d, maquina %s\n", omp_get_num_procs(), rank_MPI, computerName_MPI);
    }

	MPI_Finalize();
	return 0;
}

