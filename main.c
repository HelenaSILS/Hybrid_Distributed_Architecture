# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# define MAX_BUFFER_CHAR 500

//Estruturas de dados
struct queueSamples{
	char nome[MAX_BUFFER_CHAR];
	struct queueSamples *next;
} queueSamples;


//declaracoes de funcoes
char* readlinefile (FILE *, int, char *);
struct queueSamples* insertElem (struct queueSamples *, char *);
struct queueSamples* trataSamples (FILE *, struct queueSamples *);
void printQueue(struct queueSamples *queue);
struct queueSamples* retornaElemN(struct queueSamples *, int );
char * insertVariableValue(char *, char *);

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

	struct queueSamples *queue = NULL;

	queue=trataSamples(conf, queue);
	printQueue(queue);
	struct queueSamples *aux1, *aux2;
	aux1=retornaElemN(queue, 12);
	aux2=retornaElemN(queue, 11);
	printf("aux: %s\n", aux1->nome);
	buffer=insertVariableValue(aux1->nome, aux2->nome);
	printf("buffer:-%s-\n", buffer);


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

//void insertElem (struct queueSamples *, char *);
struct queueSamples * insertElem (struct queueSamples * queue, char * buffer){

	struct queueSamples *aux, *new;

	//create the new Element
	new = (struct queueSamples*) malloc (sizeof(struct queueSamples));
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

struct queueSamples * trataSamples (FILE *f, struct queueSamples *queue){

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

void printQueue(struct queueSamples *queue){
	struct queueSamples *aux;
	aux=queue;
	while(aux!=NULL){
		printf("%s\n", aux->nome);
		aux=aux->next;
	}
}

struct queueSamples* retornaElemN(struct queueSamples *queue, int n){
	int i=0;
	struct queueSamples *aux = queue;
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
			printf("achou\n");
			j=j+valueSize;
			printf("buffer de dentro: -%s-\n", buffer);
			i++;
		}else{
			buffer[j]=c;
			i++;
			j++;
		}
		if (i>=MAX_BUFFER_CHAR){
			printf("extrapolou\n");
			return NULL;
		}
			
	}

	return buffer;
}