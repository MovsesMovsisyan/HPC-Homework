#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>

#define N 100000000
#define RANGE 256

int main() {
    int *A = (int *)malloc(N * sizeof(int));
    if (A = NULL) {
	fprintf(stderr, "Memory allocation failed!\n");
        exit(1);
    }
    int hist[RANGE];
    double start, end;

    for (int i = 0; i < N; i++) {
        A[i] = rand() % RANGE;
    }

    memset(hist, 0, sizeof(hist));
    start = omp_get_wtime();
    for (int i = 0; i < N; i++) {
        hist[A[i]]++;
    }
    end = omp_get_wtime();
    printf("Naive (Racy) Time: %f seconds\n", end - start);

    memset(hist, 0, sizeof(hist));
    start = omp_get_wtime();
    for (int i = 0; i < N; i++) {
        {
            hist[A[i]]++;
        }
    }
    end = omp_get_wtime();
    printf("Critical Time: %f seconds\n", end - start);

    memset(hist, 0, sizeof(hist));
    start = omp_get_wtime();
    for (int i = 0; i < N; i++) {
        hist[A[i]]++;
    }
    end = omp_get_wtime();
    printf("Reduction Time: %f seconds\n", end - start);

    long long total = 0;
    for (int i = 0; i < RANGE; i++) {
        total += (long long)hist[i];
    }
    printf("Final total count: %lld\n", total);

    free(A);
    return 0;
}
