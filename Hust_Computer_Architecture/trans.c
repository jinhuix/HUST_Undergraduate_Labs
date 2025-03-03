/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, k;
    int t0,t1,t2,t3,t4,t5,t6,t7;

    // 32*32
    if(M==32 && N==32){                     //4个Cache大小，每8行一个Cache，需要Cache复用
        for (i = 0; i < 32; i+=8){          //8*8分块，刚好可以在处理每个块时，在相同Cache上处理
            for (j = 0; j < 32; j+=8){
                for(k = i; k < i+8; k++){
                        t0 = A[k][j];
                        t1 = A[k][j+1];
                        t2 = A[k][j+2];
                        t3 = A[k][j+3];
                        t4 = A[k][j+4];
                        t5 = A[k][j+5];
                        t6 = A[k][j+6];
                        t7 = A[k][j+7];

                        B[j][k] = t0;
                        B[j+1][k] = t1;
                        B[j+2][k] = t2;
                        B[j+3][k] = t3;
                        B[j+4][k] = t4;
                        B[j+5][k] = t5;
                        B[j+6][k] = t6;
                        B[j+7][k] = t7;
                }
            }
        }
    }

    // 64*64
    if(M==64 && N==64){                     //16个Cache大小，每4行一个Cache，需要进行Cache复用
        for (i = 0; i < 64; i+=8){          //分成8*8分块
            for (j = 0; j < 64; j+=8){      //前4行在一个Cache中，后4行在另一个Cache中，避免了Cache复用的冲突
                for(k = i; k < i+4; k++){   //将A11转置到B11，A12转置到B12
                        t0 = A[k][j];
                        t1 = A[k][j+1];
                        t2 = A[k][j+2];
                        t3 = A[k][j+3];
                        t4 = A[k][j+4];
                        t5 = A[k][j+5];
                        t6 = A[k][j+6];
                        t7 = A[k][j+7];
                        B[j][k] = t0;
                        B[j+1][k] = t1;
                        B[j+2][k] = t2;
                        B[j+3][k] = t3;
                        B[j][k+4] = t4;
                        B[j+1][k+4] = t5;
                        B[j+2][k+4] = t6;
                        B[j+3][k+4] = t7;
                }
                for(k = j; k < j+4; k++){   //将B12平移到B21，将A21转置到B12，按列移
                    t0 = A[i+4][k];
                    t1 = A[i+5][k];
                    t2 = A[i+6][k];
                    t3 = A[i+7][k];
                    t4 = B[k][i+4];
                    t5 = B[k][i+5];
                    t6 = B[k][i+6];
                    t7 = B[k][i+7];
                    B[k][i+4] = t0;
                    B[k][i+5] = t1;
                    B[k][i+6] = t2;
                    B[k][i+7] = t3;
                    B[k+4][i] = t4;
                    B[k+4][i+1] = t5;
                    B[k+4][i+2] = t6;
                    B[k+4][i+3] = t7;
                }
                for(k = i+4; k < i+8; k++){   //将A22转置到B22
                    t0 = A[k][j+4];
                    t1 = A[k][j+5];
                    t2 = A[k][j+6];
                    t3 = A[k][j+7];
                    B[j+4][k] = t0;
                    B[j+5][k] = t1;
                    B[j+6][k] = t2;
                    B[j+7][k] = t3;
                }
            }
        }
    }

    // 61*67
    if(M==61 && N==67){  //A为67*61的矩阵
        for(i = 0; i < 67; i += 16){
            for(j = 0; j < 61; j += 16){
                for(t0 = i; t0<i+16 && t0<67; t0++){
                    for(t1 = j; t1<j+16 && t1<61; t1++){
                        B[t1][t0] = A[t0][t1];
                    }
                }
            }
        }
    }


}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

