# include "file_manager.h"
//definicoes das funcoes

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
// struct queueNode * insertElem (struct queueNode * queue, char * buffer){

// 	//create the new Element
// 	struct queueNode new;
// 	struct queueNode *aux;

// 	//new = (struct queueNode*) malloc (sizeof(struct queueNode));
// 	strcpy(new.nome, buffer);
// 	new.next=NULL;

// 	//not the first element ever
// 	if(queue!=NULL){
// 		aux=queue;
// 		while(aux->next!=NULL){
// 			aux=aux->next;
// 		}

// 		//update before the last one
// 		aux->next=new;

// 	//the first element ever
// 		return queue;
// 	}else{
// 		return &new;
// 	}
// }

// struct queueNode * trataSamples (FILE *f, struct queueNode *queue){

// 	char c;
// 	char *buffer;
// 	int i=0;
// 	printf("chegou em  trataSample\n");
// 	//buffer = (char*)malloc(sizeof(char)*(MAX_BUFFER_CHAR));
// 	buffer = (char*)malloc(MAX_LINE);
// 	if(buffer==NULL)
// 		printf("impossivel alocar\n");
// 	printf("malocou buffer de trataSample\n");

// 	while(c!=EOF){ 
// 		printf("antes do da chamada de readline de trataSamples\n");
// 		//buffer=readlinefile(f, MAX_BUFFER_CHAR, &c);
// 		fscanf(f, "%[^\b]", buffer);
// 		printf("depois do da chamada de readline de trataSamples\n");
// 		queue = insertElem (queue,buffer);
// 		printf("depois do da chamada de insertElem de trataSamples\n");
// 		for (int i = 0; i < MAX_BUFFER_CHAR; i++)
// 		{
// 			buffer='\0';
// 		}	

// 		printf("depois de limpar o buffer no trataSample\n");
// 	}
// 	return queue;
// }

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
// char * insertVariableValue(char *sentence, char *value){
// 	int sentenceSize = strlen(sentence);
// 	int valueSize = strlen(value);
// 	int i = 0;
// 	char j = 0;
// 	char c='-';
// 	char *buffer = (char*) malloc (sizeof(char)*MAX_BUFFER_CHAR);
// 	while(c!='\0'){
// 		c=sentence[i];
// 		if (c=='$'){
// 			strcat(buffer, value);
// 			j=j+valueSize;
// 			i++;
// 		}else{
// 			buffer[j]=c;
// 			i++;
// 			j++;
// 		}
// 		if (i>=MAX_BUFFER_CHAR){
// 			printf("extrapolou tamanho da string\n");
// 			return NULL;
// 		}		
// 	}
// 	return buffer;
// }

//insere a string value onde encontrar o simbolo $ na string sentence
void insertVariableValue(char *sentence, char *value, char *result){
	int sentenceSize = strlen(sentence);
	int valueSize = strlen(value);
	int i = 0;
	char j = 0;
	char c='-';
	char aux[MAX_BUFFER_CHAR];
	for(int k = 0; k<MAX_BUFFER_CHAR; k++){
		aux[k]='\0';
	}
	//printf("value: -%s-, valueSize: %d\n", value, valueSize);
	//strcpy(aux, sentence);
	while(c!='\0'){
		c=sentence[i];
		if (c=='$'){
			strcat(aux, value);
			//strncpy(aux, value, valueSize-1);
			j=j+valueSize-1;
			i++;
			//printf("aux: -%s-\n", aux);
		}else{
			aux[j]=c;
			i++;
			j++;
			//printf("aux: -%s-\n", aux);
		}
		if (i>=MAX_BUFFER_CHAR){
			printf("extrapolou tamanho da string\n");
		}		
	}
	strcpy(result, aux);
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

// char ** makeQueueOutOfCommandsAndSample(char **commandsMatrix, int rows, char* sampleElem){
// 	//char aux[rows][MAX_BUFFER_CHAR];
// 	char **aux;
// 	char *buffer = (char *) malloc (sizeof(char)*MAX_BUFFER_CHAR);
// 	int i = 0;
// 	aux=(char**) malloc (sizeof(char*)*rows);
// 	while(i<rows){
// 		buffer=insertVariableValue(commandsMatrix[i], sampleElem);
// 		aux[i] = (char*) malloc (sizeof(char)*MAX_BUFFER_CHAR);
// 		strcpy(aux[i], buffer);
// 		i++;
// 	}
// 	return aux;
// }

void makeQueueOutOfCommandsAndSample(char **commandsMatrix, int rows, char* sampleElem, char** result){
	//char aux[rows][MAX_BUFFER_CHAR];
	char **aux;
	int i = 0;
	char aux1[rows][MAX_BUFFER_CHAR];
	char aux2[rows][MAX_BUFFER_CHAR];
	memcpy(aux1, commandsMatrix, rows*MAX_BUFFER_CHAR);
	char bufCom[MAX_BUFFER_CHAR];
	char bufRes[MAX_BUFFER_CHAR];
	while(i<rows){
		memcpy(bufCom, aux1[i], MAX_BUFFER_CHAR);
		insertVariableValue(bufCom, sampleElem, bufRes);
		memcpy(aux2[i], result, MAX_BUFFER_CHAR);
		i++;
	}
	memcpy(aux2, result, rows*MAX_BUFFER_CHAR);
	
}