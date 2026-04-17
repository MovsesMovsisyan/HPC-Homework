#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define THRESHOLD 10

long long fib_sequential(int n) {
    if (n <= 1) return n;
    return fib_sequential(n - 1) + fib_sequential(n - 2);
}

long long fib_parallel(int n) {
    long long x, y;

    if (n <= THRESHOLD) {
        return fib_sequential(n);
    }

    #pragma omp task shared(x)
    x = fib_parallel(n - 1);

    #pragma omp task shared(y)
    y = fib_parallel(n - 2);

    #pragma omp taskwait
    return x + y;
}

int main() {
    int num;
    long long result;
    double start_time, end_time;

    printf("Enter the Fibonacci index to calculate: ");
    if (scanf("%d", &num) != 1) return 1;

    start_time = omp_get_wtime();

    #pragma omp parallel
    {
        #pragma omp single
        {
            result = fib_parallel(num);
        }
    }

    end_time = omp_get_wtime();

    printf("F(%d) = %lld\n", num, result);
    printf("Execution Time: %f seconds\n", end_time - start_time);

    return 0;
}
