//definicoes das funcoes

char* readlinefile (FILE *f, int length, char *c){
	//char c;
	char *string;
	int i=0;
	string = (char*)malloc(sizeof(char)*(length));
	*c = fgetc(f);	
    while(*c!='\n' && *c!= EOF){
            string[i]=*c;
            i++;
			*c = fgetc(f);
	}
	return string;
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
		printf("%s\n", buffer);
		queue = insertElem (queue,buffer);
	}
	return queue;
}

void printQueue(struct queueSamples *queue){
	struct queueSamples *aux;
	aux=queue;
	printf("q: %s?\n", queue->nome);
	while(aux!=NULL){
		printf("%s\n", aux->nome);
		aux=aux->next;
	}
}