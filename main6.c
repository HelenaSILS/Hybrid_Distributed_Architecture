# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# define MAX_BUFFER_CHAR 512
# define MAX_LINE 512


void insertVariableValue(char *, char *, char *);
void makeQueueOutOfCommandsAndSample(char **, int , char *, char** );

int main(int argc, char *argv[]){

FILE *conf;
FILE *comandos;
FILE *samples;

	if ((conf = fopen(argv[1], "r"))==NULL){
		printf("erro ao abrir arquivo conf\n");
		exit(1);
	 	}
	if ((comandos = fopen(argv[2], "r"))==NULL){
		printf("erro ao abrir arquivo comandos\n");
		exit(1);
		}
	// if ((samples = fopen(argv[3], "r"))==NULL){
	// 	printff("erro ao abrir arquivo samples\n");
	// 	exit(1);
	// 	}

    char string[MAX_LINE];
	char *nulo={"\0"};
    int countSamples;
    while (fscanf(conf, "%[^\n] ", string) != EOF) {
        //printf("%s\n", string);
        countSamples++;
    }
    rewind(conf);

    char samplesMatrix[countSamples][MAX_BUFFER_CHAR];

	for(int j=0; j<countSamples; j++){
        fscanf(conf, "%[^\n] ", string);
		//samplesMatrix[j] = (char*) malloc (MAX_BUFFER_CHAR*sizeof(char));
		strcpy(samplesMatrix[j], string);
		strcat(samplesMatrix[j], nulo);
        printf("%s\n", samplesMatrix[j]);
	}
///////////////////////////////////////////////
	int c2;
    while (fscanf(comandos, "%[^\n] ", string) != EOF) {
        //printf("%s\n", string);
        c2++;
    }
    rewind(comandos);

    char cm2[c2][MAX_BUFFER_CHAR];

	for(int j=0; j<c2; j++){
        fscanf(comandos, "%[^\n] ", string);
		//samplesMatrix[j] = (char*) malloc (MAX_BUFFER_CHAR*sizeof(char));
		strcpy(cm2[j], string);
		strcat(cm2[j], nulo);
        printf("%s\n", cm2[j]);
	}

	
    char toExecute[countSamples][MAX_BUFFER_CHAR];
	makeQueueOutOfCommandsAndSample (samplesMatrix, countSamples, cm2[1], toExecute);

return 0;
}

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
	//printf("result: %s\n", result);
}