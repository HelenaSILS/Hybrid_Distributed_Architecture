# include "file_manager.h"
//definicoes das funcoes

char* readlinefile (FILE *f, int length, char *c){
	//char c;
	char *string, *aux;
	int i=0;
	//buffer de tamanho excedente
	string = (char*)malloc(sizeof(char)*(length+1));
	*c = fgetc(f);	
    while(*c!='\n' && *c!= EOF){
            string[i]=*c;
            i++;
			*c = fgetc(f);
	}
	string[i]='\0';
	//aux de tamanho apropriado para que nao haja lixo
	aux = (char*)malloc(sizeof(char)*(i+2));
	strncpy(aux, string, (i+2));
	return aux;
}

//void insertElem (struct queueNode *, char *);
struct queueNode * insertElem (struct queueNode * queue, char * buffer){

	struct queueNode *aux, *new;

	//create the new Element
	new = (struct queueNode*) malloc (sizeof(struct queueNode));
	strcpy(new->nome, buffer);
	new->next=NULL;

	//not the first element ever
	if(queue!=NULL){
		aux=queue;
		while(aux->next!=NULL){
			aux=aux->next;
		}

		//update before the last one
		aux->next=new;

	//the first element ever
		return queue;
	}else{
		return new;
	}
}

struct queueNode * trataSamples (FILE *f, struct queueNode *queue){

	char c;
	char *buffer;
	int i=0;
	buffer = (char*)malloc(sizeof(char)*(MAX_BUFFER_CHAR));

	while(c!=EOF){ 
		buffer=readlinefile(f, MAX_BUFFER_CHAR, &c);
		queue = insertElem (queue,buffer);
		free(buffer);
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

