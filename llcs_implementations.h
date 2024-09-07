#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define LEN 51200
#define MIN(x,y) ((x) < (y) ? (x) : (y));
#define MAX(x,y) ((x) > (y) ? (x) : (y));

unsigned long long llcs_serial(const char* X, const char* Y, unsigned int** M)
{
    unsigned long long entries_visited = 0;

    int i = 0;
    while (i < LEN)
    {
        int j = 0;
        while (j < LEN)
        {
            if (X[i] == Y[j])
            {
                M[i + 1][j + 1] = M[i][j] + 1;
            }
            else if (M[i + 1][j] < M[i][j + 1])
            {
                M[i + 1][j + 1] = M[i][j + 1];
            }
            else
            {
                M[i + 1][j + 1] = M[i + 1][j];
            }

            entries_visited++;

            j++;
        }
        i++;
    }
    return entries_visited;
}

#if defined(_OPENMP)

unsigned long long llcs_parallel_tasks(const char *X, const char *Y, unsigned int **M) {
    unsigned long long entries_visited = 0;
    int blockSize = 256;
    int blockCount = LEN / blockSize;
    int row, col, antiDiagonalNum;
    int count;
    #pragma omp parallel
    #pragma omp single
    {
        //iterate antidiagonals
        for (antiDiagonalNum = 0; antiDiagonalNum < 2 * blockCount; antiDiagonalNum++)
        {
            //get the starting coordinates for the bottom and top blocks in the antidiagonal
            int bottomBlockRow = MIN(LEN - blockSize + 1, antiDiagonalNum * blockSize + 1);
            int topBlockRow = MAX(1, blockSize * (antiDiagonalNum - (blockCount - 1)) + 1);
            
            //traverse blocks
            for (row = bottomBlockRow; row >= topBlockRow; row -= blockSize)
            {
                col = antiDiagonalNum * blockSize + 2 - row;
               
                //create task for each block
                #pragma omp task firstprivate(row,col,count) depend(in:M[row+blockSize-1,col-1],M[row-1,col+blockSize-1]) depend(out:M[row + blockSize-1,col + blockSize-1])
                {
                    for (int i = row; i < row + blockSize; i++)
                    {
                        for (int j = col; j < col + blockSize; j++)
                        {
                            if (X[i - 1] == Y[j - 1])
                            {
                                M[i][j] = M[i - 1][j - 1] + 1;
                            }
                            else
                            {
                                M[i][j] = MAX(M[i - 1][j], M[i][j - 1]);
                            }
                            
                            count++;
                        }
                        
                    }
                    #pragma omp atomic
                    entries_visited+=count;
                }
               
            }
        
        }
    }

    //return LCS length
    return entries_visited;
}

unsigned long long llcs_parallel_taskloop(const char *X, const char *Y, unsigned int **M)
{
    // your explicit tasking code here
    unsigned long long entries_visited = 0;
    int blockSize = 256;
    int blockCount = LEN / blockSize;
    int row, col, antiDiagonalNum;
    #pragma omp parallel
    #pragma omp single
    {
        for (antiDiagonalNum = 0; antiDiagonalNum < 2 * blockCount; antiDiagonalNum++)
        {
            //get the starting coordinates for the bottom and top blocks' in the antidiagonal
            int bottomBlockRow = MIN(LEN - blockSize + 1, antiDiagonalNum * blockSize + 1);
            int topBlockRow = MAX(1, blockSize * (antiDiagonalNum - (blockCount - 1)) + 1);
            
            //create task for each block in the antidiagonal
            #pragma omp taskloop private(row,col) grainsize(1) reduction(+:entries_visited)
            for (row = bottomBlockRow; row >= topBlockRow; row -= blockSize)
            {
                col = antiDiagonalNum * blockSize + 2 - row;
                //traverse the block
                for (int i = row; i < row + blockSize; i++)
                {
                    for (int j = col; j < col + blockSize; j++)
                    {
                        if (X[i - 1] == Y[j - 1])
                        {
                            M[i][j] = M[i - 1][j - 1] + 1;
                        }
                        else
                        {
                            M[i][j] = MAX(M[i - 1][j], M[i][j - 1]);
                        }
                        entries_visited++;
                    }

                }
            }
        }
    }
    //return LCS length
    return entries_visited;
}

#endif