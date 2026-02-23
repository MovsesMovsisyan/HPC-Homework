#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

#define N 20000000
#define N_THREADS 10

int primes_seq(int start, int end) {
    int count = 0;

    if (start <= 2 && end >= 2)
        count++;

    if (start < 3)
        start = 3;

    if (start % 2 == 0)
        start++;  

    for (int i = start; i <= end; i += 2) {
        if (i % 3 == 0 && i != 3)
            continue;

        bool prime = true;

        for (int j = 5; j * j <= i; j += 6) {
            if (i % j == 0 || i % (j + 2) == 0) {
                prime = false;
                break;
            }
        }

        if (prime)
            count++;
    }

    return count;
}

pthread_t thread[N_THREADS];

void* thread_function(void* arg) {
	int* start = (int*) arg;
	int* count = malloc(sizeof(int));
	*count = 0;
	*count = primes_seq(*start, *start + N / N_THREADS - 1);
	return (void*) count;
}



void* primes(void* arg) {
	int* start = (int *) arg;
}

int main() {
	struct timespec start, end;

	printf("sequentially\n");
    	clock_gettime(CLOCK_MONOTONIC, &start);
	printf("number of primes = %d ", primes_seq(1, N));
    	clock_gettime(CLOCK_MONOTONIC, &end);
	double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
	printf("elapsed_time = %f\n", elapsed_time);


	clock_gettime(CLOCK_MONOTONIC, &start);
	int n_primes_threads = 0;
	int args[N_THREADS];
	for(int i = 0; i < N_THREADS; i++) {
		args[i] = i * N / N_THREADS + 1;
		pthread_create(&thread[i], NULL, thread_function, args + i);
	}

	for(int i = 0; i < N_THREADS; i++) {
		int* a;
		pthread_join(thread[i], (void**)&a);
		n_primes_threads += *a;
		free(a);
	}
	clock_gettime(CLOCK_MONOTONIC, &end);
	elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
	printf("using N threads\n");
	printf("number of primes = %d elapsed_time = %f\n", n_primes_threads, elapsed_time);


	return 0;
}
