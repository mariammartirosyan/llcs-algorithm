#include<omp.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>

#include "llcs_implementations.h"
 
int main(){

    char *X = (char*) malloc(sizeof(char)*LEN);
    char *Y = (char*) malloc(sizeof(char)*LEN);
    
    FILE *file_X = fopen("X.in", "r");
    fscanf(file_X, "%s", X);
    fclose(file_X);

    FILE *file_Y = fopen("Y.in", "r");
    fscanf(file_Y, "%s", Y);
    fclose(file_Y);
    
    unsigned int **M = (unsigned int**) malloc(sizeof(unsigned int*) * (LEN + 1));
    for (int i=0; i < LEN+1; i++)
        M[i] = (unsigned int*) calloc(LEN+1, sizeof(unsigned int));

    unsigned long long entries_visited;
    
    double timer;

    #if !defined(_OPENMP)
        printf("Running serial version...\n");
        struct timeval begin, end;
        long seconds;
        long microseconds;
        gettimeofday(&begin, 0);

        entries_visited = llcs_serial(X, Y, M);

        gettimeofday(&end, 0);
        seconds = end.tv_sec - begin.tv_sec;
        microseconds = end.tv_usec - begin.tv_usec;
        timer = seconds + microseconds*1e-6;

    #else
        printf("Running task-parallel version...\n");
        timer = omp_get_wtime();
        #if defined(_TASKS)
            entries_visited = llcs_parallel_tasks(X, Y, M);
        #elif defined(_TASKLOOP)
            entries_visited = llcs_parallel_taskloop(X, Y, M);
        #endif
        timer = omp_get_wtime() - timer;
    #endif

    printf("Timer: %lf seconds | LLCS: %d (%s) | Entries Visited: %llu (%s)\n",
            timer,
            M[LEN][LEN], 
            (M[LEN][LEN] == 33492) ? "CORRECT": "WRONG",
            entries_visited, 
            (entries_visited == 2621440000) ? "CORRECT": "WRONG");
    

    for (int i=0; i < LEN+1; i++)
        free(M[i]);

    free(M);
    free(X);
    free(Y);

    return 0;
}