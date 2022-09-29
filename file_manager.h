# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# define MAX_BUFFER_CHAR 200

//Estruturas de dados
struct queueSamples{
	char nome[50];
	struct queueSamples *next;
} queueSamples;


//declaracoes de funcoes
char* readlinefile (FILE *, int, char *);
struct queueSamples* insertElem (struct queueSamples *, char *);
struct queueSamples* trataSamples (FILE *, struct queueSamples *);
void printQueue(struct queueSamples *queue);
