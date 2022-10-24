#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main (){ 
    char enviando[50];
    time_t initial_time, current_time;
    initial_time = time(NULL);
    sleep(1);
    double duration;
        current_time = time(NULL);
        duration = difftime(current_time, initial_time);
    snprintf(enviando, 50, "%.8f", duration);
    printf("duration: %s\n", enviando);

    int iam = 0;
    int rank = 1;
    int length = snprintf( NULL, 0, "%d", (iam+rank*100));
    char* iam_to_str = malloc( length + 1 );
    snprintf( iam_to_str, length + 1, "%d", (iam+rank*100) );
    printf("str: %s", iam_to_str);
}
