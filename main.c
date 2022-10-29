# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# define MAX_BUFFER_CHAR 512
# define MAX_LINE 512

//Estruturas de dados
struct queueNode{
	char nome[MAX_BUFFER_CHAR];
	struct queueNode *next;
} queueNode;


//declaracoes de funcoes
char* readlinefile (FILE *, int, char *);
struct queueNode* insertElem (struct queueNode *, char *);
struct queueNode* trataSamples (FILE *, struct queueNode *);
void printQueue(struct queueNode *queue);
struct queueNode* retornaElemN(struct queueNode *, int );
char * insertVariableValue(char *, char *);
//struct queueNode * makeQueueOutOfCommandsAndSample (struct queueNode *, struct queueNode *);
char** makeQueueOutOfCommandsAndSample(char **, int , char* );

int main(int argc, char *argv[]){
FILE *conf;
FILE *comandos;
FILE *samples;

	if ((conf = fopen(argv[1], "r"))==NULL){
		printf("erro ao abrir arquivo conf\n");
		exit(1);
	 	}
	// if ((comandos = fopen(argv[2], "r"))==NULL){
	// 	printf("erro ao abrir arquivo comandos\n");
	// 	exit(1);
	// 	}
	// if ((samples = fopen(argv[3], "r"))==NULL){
	// 	printff("erro ao abrir arquivo samples\n");
	// 	exit(1);
	// 	}
	
	//char c;
	//int i=0;
	char *buffer;
	buffer = (char*)malloc(sizeof(char)*(MAX_BUFFER_CHAR));

	struct queueNode *queue = NULL;

	queue=trataSamples(conf, queue);
	struct queueNode *outro = queue;

	struct queueNode *outro2 = queue;
	printQueue(outro2);
	// struct queueNode *aux1, *aux2;
	// aux1=retornaElemN(queue, 9);
	// aux2=retornaElemN(queue, 11);
	// printf("aux: %s\n", aux1->nome);
	// buffer=insertVariableValue(aux1->nome, aux2->nome);
	// printf("buffer:-%s-\n", buffer);

	// struct queueNode *aux3;
	// aux3=makeQueueOutOfCommandsAndSample(aux1, aux2);
	// printf("=====================\n");
	// printQueue(aux3);

	// struct queueNode *aux4;
	// aux4=retornaElemN(queue, 110);
	// if(aux4==NULL)
	// 	printf("nulo\n");


	int countCommands=0;
    while(outro!=NULL){
        countCommands++;
		outro=outro->next;
    }

    char *commandsMatrix[countCommands];
	printf("antes, count: %d\n", countCommands);

	for(int j=0; j<countCommands; j++){
		//printf("dentro do for j:%d\n", j);
		commandsMatrix[j] = (char*) malloc (MAX_BUFFER_CHAR*sizeof(char));
		//printf("malocou, countCommands= %d \n", countCommands);
		//printf("queue-nome: %s\n", queue->nome);
		strcpy(commandsMatrix[j], queue->nome);
		queue=queue->next;
	}

	char *actualSample = "vjfvjkd";
    char **toExecute = (char**) malloc (sizeof(char*)*countCommands);
	toExecute = makeQueueOutOfCommandsAndSample (commandsMatrix, countCommands, actualSample);
	printf("a ser executado: %s\n",toExecute[1]);



	// fclose(conf);
	// fclose(comandos);
	// fclose(samples);


	return 0;
}


char* readlinefile (FILE *f, int length, char *c){
	//char c;
	char *string, *aux;
	int i=0;
	//buffer de tamanho excedente
	printf("antes do malloc de readlinefile\n");
	string = (char*)calloc(MAX_BUFFER_CHAR, sizeof(char));

	if(string==NULL)
		printf("readlinefile falhou no calloc\n");
	printf("depois do malloc de readlinefile\n");

	*c = fgetc(f);
	printf("depois do primeiro fget de readlinefile\n");
    while(*c!='\n' && *c!='\r' && *c!= EOF){
            string[i]=*c;
            i++;
			*c = fgetc(f);
			printf("em cada fgetc do while: %c\n", *c);
	}
	string[i]='\0';

	printf("q q tem depois do while: %s\n", string);
	//aux de tamanho apropriado para que nao haja lixo
	//aux = (char*)malloc(sizeof(char)*(length));
	//strncpy(aux, string, length);
	return string;
}

//void insertElem (struct queueNode *, char *);
struct queueNode * insertElem (struct queueNode * queue, char * buffer){

	//create the new Element
	struct queueNode new;
	struct queueNode *aux;

	//new = (struct queueNode*) malloc (sizeof(struct queueNode));
	strcpy(new.nome, buffer);
	new.next=NULL;

	//not the first element ever
	if(queue!=NULL){
		aux=queue;
		while(aux->next!=NULL){
			aux=aux->next;
		}

		//update before the last one
		aux->next=&new;

	//the first element ever
		return queue;
	}else{
		return &new;
	}
}

struct queueNode * trataSamples (FILE *f, struct queueNode *queue){

	char c;
	char *buffer;
	int i=0;
	printf("chegou em  trataSample\n");
	//buffer = (char*)malloc(sizeof(char)*(MAX_BUFFER_CHAR));
	buffer = (char*)malloc(MAX_LINE);
	if(buffer==NULL)
		printf("impossivel alocar\n");
	printf("malocou buffer de trataSample\n");

	while(c!=EOF){ 
		printf("antes do da chamada de readline de trataSamples\n");
		//buffer=readlinefile(f, MAX_BUFFER_CHAR, &c);
		fscanf(f, "%[^\b]", buffer);
		printf("depois do da chamada de readline de trataSamples\n");
		queue = insertElem (queue,buffer);
		printf("depois do da chamada de insertElem de trataSamples\n");
		for (int i = 0; i < MAX_BUFFER_CHAR; i++)
		{
			buffer='\0';
		}	

		printf("depois de limpar o buffer no trataSample\n");
	}
	return queue;
}

void printQueue(struct queueNode *queue){
	struct queueNode *aux;
	aux=queue;
	while(aux!=NULL){
		printf("%s\n", aux->nome);
		aux=aux->next;
	}
}

struct queueNode* retornaElemN(struct queueNode *queue, int n){
	int i=0;
	struct queueNode *aux = queue;
	while(i<n && aux!=NULL){
		aux=aux->next;
		i++;
	}
	return aux;
 }

//insere a string value onde encontrar o simbolo $ na string sentence
char * insertVariableValue(char *sentence, char *value){
	int sentenceSize = strlen(sentence);
	int valueSize = strlen(value);
	int i = 0;
	char j = 0;
	char c='-';
	char *buffer = (char*) malloc (sizeof(char)*MAX_BUFFER_CHAR);
	while(c!='\0'){
		c=sentence[i];
		if (c=='$'){
			strcat(buffer, value);
			j=j+valueSize;
			i++;
		}else{
			buffer[j]=c;
			i++;
			j++;
		}
		if (i>=MAX_BUFFER_CHAR){
			printf("extrapolou tamanho da string\n");
			return NULL;
		}		
	}
	return buffer;
}

// struct queueNode * makeQueueOutOfCommandsAndSample(struct queueNode *commandsQueue , struct queueNode * samplesQueue){
//     struct queueNode *commands = commandsQueue;
// 	struct queueNode *sample = samplesQueue;
// 	struct queueNode *aux = NULL;
// 	char *buffer = (char *) malloc (sizeof(char)*MAX_BUFFER_CHAR);

// 	while(commands!=NULL){
// 		buffer=insertVariableValue(commands->nome, sample->nome);
// 		aux= insertElem(aux, buffer);
// 		commands=commands->next;
// 	}
// 	return aux;
// }

char ** makeQueueOutOfCommandsAndSample(char **commandsMatrix, int rows, char* sampleElem){
	//char aux[rows][MAX_BUFFER_CHAR];
	char **aux;
	char *buffer = (char *) malloc (sizeof(char)*MAX_BUFFER_CHAR);
	int i = 0;
	aux=(char**) malloc (sizeof(char*)*rows);
	while(i<rows){
		buffer=insertVariableValue(commandsMatrix[i], sampleElem);
		aux[i] = (char*) malloc (sizeof(char)*MAX_BUFFER_CHAR);
		strcpy(aux[i], buffer);
		i++;
	}
	return aux;
}

