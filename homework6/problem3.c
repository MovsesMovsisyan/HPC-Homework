#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 50000000

int main() {
    double *A = (double *)malloc(N * sizeof(double));
    if (A == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
        exit(1);
    }

    double max_val = 0.0;
    double sum = 0.0;
    double start, end;

    for (int i = 0; i < N; i++) {
        A[i] = (double)rand() / RAND_MAX;
    }

    start = omp_get_wtime();

    #pragma omp parallel for reduction(max: max_val)
    for (int i = 0; i < N; i++) {
        if (A[i] > max_val) {
            max_val = A[i];
        }
    }

    double threshold = 0.8 * max_val;

    #pragma omp parallel for reduction(+: sum)
    for (int i = 0; i < N; i++) {
        if (A[i] > threshold) {
            sum += A[i];
        }
    }

    end = omp_get_wtime();

    printf("Max Value: %f\n", max_val);
    printf("Threshold: %f\n", threshold);
    printf("Filtered Sum: %f\n", sum);
    printf("Execution Time: %f seconds\n", end - start);

    free(A);
    return 0;
}
