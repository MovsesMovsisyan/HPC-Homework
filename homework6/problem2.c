#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <float.h>

#define N 50000000

int main() {
    double *A = (double *)malloc(N * sizeof(double));
    if (A == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
        exit(1);
    }

    double min_diff = DBL_MAX;
    double start, end;

    for (int i = 0; i < N; i++) {
        A[i] = (double)rand() / RAND_MAX * 1000.0;
    }

    start = omp_get_wtime();

    #pragma omp parallel for reduction(min: min_diff)
    for (int i = 1; i < N; i++) {
        double diff = fabs(A[i] - A[i - 1]);
        if (diff < min_diff) {
            min_diff = diff;
        }
    }

    end = omp_get_wtime();

    printf("Minimum absolute difference: %f\n", min_diff);
    printf("Execution Time: %f seconds\n", end - start);

    free(A);
    return 0;
}
