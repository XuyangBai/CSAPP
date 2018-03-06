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



char transpose_submit_version1_desc[] = "Transpose submission version1";

void transpose_submit_version1(int M, int N, int A[N][M], int B[M][N]) {
    int i, j, ii, jj;
    for (i = 0; i < N; i += 8) {
        for (j = 0; j < M; j += 8) {
            for (ii = i; ii < i + 8; ii++) {
                for (jj = j; jj < j + 8; jj++) {
                    B[jj][ii] = A[ii][jj];
                }
            }
        }
    }
}

char transpose_submit_version2_desc[] = "Transpose submission version2";

void transpose_submit_version2(int M, int N, int A[N][M], int B[M][N]) {
    int i, j, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
    int n = N / 8 * 8;
    int m = M / 8 * 8;
    for (j = 0; j < m; j += 8) {
        for (i = 0; i < n; i++) {
            tmp0 = A[i][j];
            tmp1 = A[i][j + 1];
            tmp2 = A[i][j + 2];
            tmp3 = A[i][j + 3];
            tmp4 = A[i][j + 4];
            tmp5 = A[i][j + 5];
            tmp6 = A[i][j + 6];
            tmp7 = A[i][j + 7];

            B[j][i] = tmp0;
            B[j + 1][i] = tmp1;
            B[j + 2][i] = tmp2;
            B[j + 3][i] = tmp3;
            B[j + 4][i] = tmp4;
            B[j + 5][i] = tmp5;
            B[j + 6][i] = tmp6;
            B[j + 7][i] = tmp7;

        }
    }

}

char transpose_submit_version3_desc[] = "Transpose submission version3";

void transpose_submit_version3(int M, int N, int A[N][M], int B[M][N]) {
    int i, j, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
    int n = N / 8 * 8;
    int m = M / 8 * 8;
    for (j = 0; j < m; j += 8) {
        for (i = 0; i < n; i++) {
            tmp0 = A[i][j];
            tmp1 = A[i][j + 1];
            tmp2 = A[i][j + 2];
            tmp3 = A[i][j + 3];
            tmp4 = A[i][j + 4];
            tmp5 = A[i][j + 5];
            tmp6 = A[i][j + 6];
            tmp7 = A[i][j + 7];

            B[j][i] = tmp0;
            B[j + 1][i] = tmp1;
            B[j + 2][i] = tmp2;
            B[j + 3][i] = tmp3;
            B[j + 4][i] = tmp4;
            B[j + 5][i] = tmp5;
            B[j + 6][i] = tmp6;
            B[j + 7][i] = tmp7;

        }
    }
    for (i = n; i < N; i++)
        for (j = m; j < M; j++) {
            tmp0 = A[i][j];
            B[j][i] = tmp0;
        }
    for (i = 0; i < N; i++)
        for (j = m; j < M; j++) {
            tmp0 = A[i][j];
            B[j][i] = tmp0;
        }
    for (i = n; i < N; i++)
        for (j = 0; j < M; j++) {
            tmp0 = A[i][j];
            B[j][i] = tmp0;
        }
}

char transpose_submit_version4_desc[] = "Transpose submission version4";

void transpose_submit_version4(int M, int N, int A[N][M], int B[M][N]) {
    int i, ii, j, tmp0, tmp1, tmp2, tmp3;
    int n = N / 8 * 8;
    int m = M / 8 * 8;
    for (i = 0; i < n; i += 8) {
        for (j = 0; j < m; j += 8) {
            //左上
            for (ii = 0; ii < 4; ii++) {
                tmp0 = A[i + ii][j];
                tmp1 = A[i + ii][j + 1];
                tmp2 = A[i + ii][j + 2];
                tmp3 = A[i + ii][j + 3];
                B[j][i + ii] = tmp0;
                B[j + 1][i + ii] = tmp1;
                B[j + 2][i + ii] = tmp2;
                B[j + 3][i + ii] = tmp3;
            }
            // 右上
            for (ii = 0; ii < 4; ii++) {
                tmp0 = A[i + ii][j + 4];
                tmp1 = A[i + ii][j + 4 + 1];
                tmp2 = A[i + ii][j + 4 + 2];
                tmp3 = A[i + ii][j + 4 + 3];
                B[j + 4][i + ii] = tmp0;
                B[j + 4 + 1][i + ii] = tmp1;
                B[j + 4 + 2][i + ii] = tmp2;
                B[j + 4 + 3][i + ii] = tmp3;
            }
            // 右下
            for (ii = 4; ii < 8; ii++) {
                tmp0 = A[i + ii][j + 4];
                tmp1 = A[i + ii][j + 4 + 1];
                tmp2 = A[i + ii][j + 4 + 2];
                tmp3 = A[i + ii][j + 4 + 3];
                B[j + 4][i + ii] = tmp0;
                B[j + 4 + 1][i + ii] = tmp1;
                B[j + 4 + 2][i + ii] = tmp2;
                B[j + 4 + 3][i + ii] = tmp3;
            }

            // 左下
            for (ii = 4; ii < 8; ii++) {
                tmp0 = A[i + ii][j];
                tmp1 = A[i + ii][j + 1];
                tmp2 = A[i + ii][j + 2];
                tmp3 = A[i + ii][j + 3];
                B[j][i + ii] = tmp0;
                B[j + 1][i + ii] = tmp1;
                B[j + 2][i + ii] = tmp2;
                B[j + 3][i + ii] = tmp3;
            }
        }
    }
    for (i = n; i < N; i++)
        for (j = m; j < M; j++) {
            tmp0 = A[i][j];
            B[j][i] = tmp0;
        }
    for (i = 0; i < N; i++)
        for (j = m; j < M; j++) {
            tmp0 = A[i][j];
            B[j][i] = tmp0;
        }
    for (i = n; i < N; i++)
        for (j = 0; j < M; j++) {
            tmp0 = A[i][j];
            B[j][i] = tmp0;
        }
}

char transpose_submit_desc[] = "Transpose submission";

void transpose_submit(int M, int N, int A[N][M], int B[M][N]) {
    if (M == 64 && N == 64)
        transpose_submit_version4(M, N, A, B);
    else
        transpose_submit_version3(M, N, A, B);
}

/*
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started.
 */

/*
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";

void trans(int M, int N, int A[N][M], int B[M][N]) {
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
void registerFunctions() {
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc);
    registerTransFunction(transpose_submit_version1, transpose_submit_version1_desc);
    registerTransFunction(transpose_submit_version2, transpose_submit_version2_desc);
    registerTransFunction(transpose_submit_version3, transpose_submit_version3_desc);
    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc);

}

/*
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N]) {
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

