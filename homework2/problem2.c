#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define SIZE 50000000
#define N 10

int sequential(int arr[], int size) {
	int count = 0;
	for(int i = 0; i < size; i++) 
		count += arr[i];
	return count;
}

pthread_t thread[N];

void* thread_function(void* args) {
	int* arr = (int *) args;
	int* partial_sum = malloc(sizeof(int));
	if(partial_sum == NULL) {
		fprintf(stderr, "Memory allocation failed!\n");
        	exit(1);
    	}
	for(int i = 0; i < SIZE / N; i++)
		*partial_sum += *(arr + i);
	return (void*) partial_sum;
}

int main() {
	srand(42);
	int *arr = malloc(sizeof(int) * SIZE);
	if (arr == NULL) {
		fprintf(stderr, "Memory allocation failed!\n");
        	exit(1);
    	}
	for (int i = 0; i < SIZE; i++) 
		arr[i] = rand() % 10 + 1;

	struct timespec start, end;	
	clock_gettime(CLOCK_MONOTONIC, &start);
	long long sum_seq = sequential(arr, SIZE);
	
	clock_gettime(CLOCK_MONOTONIC, &end);

	double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
	printf("sequentially\n");
	printf("sum = %lld, elapsed_time = %f\n", sum_seq, elapsed_time);
		
	clock_gettime(CLOCK_MONOTONIC, &start);
	for(int i = 0; i < N; i++)
		pthread_create(&thread[i], NULL, thread_function, arr + i * SIZE / N);

	long long sum_threads = 0;
	int *a;
	for(int i = 0; i < N; i++) {
		pthread_join(thread[i],(void**)&a);
		sum_threads += *a;
		free(a);
	}
	clock_gettime(CLOCK_MONOTONIC, &end);
	elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
	printf("using N threads\n");
	printf("sum = %lld, elapsed_time = %f\n", sum_threads, elapsed_time);

	return 0;
}


