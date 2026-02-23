#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define SIZE 50000000
#define N 10


int sequential(int arr[], int size) {
	int max = 0;
	for(int i = 0; i < size; i++) {
		if(arr[i] > max)
			max = arr[i];
	}
	return max;
}

pthread_t thread[N];
typedef struct {
	int* arr;
	int max;
} t_args;

void* thread_function(void* args) {
	t_args* data = (t_args *) args;
	int* arr = data->arr;
	int max = 0;
	for(int i = 0; i < SIZE / N; i++) {
		if(*(arr + i) > max) {
			max = *(arr + i);
		}
	}
	data->max = max;
}

int main() {
	srand(42);
	int *arr = malloc(sizeof(int) * SIZE);
	for (int i = 0; i < SIZE; i++) 
		arr[i] = rand();

	struct timespec start, end;	
	clock_gettime(CLOCK_MONOTONIC, &start);
	int max_seq = sequential(arr, SIZE);
	
	clock_gettime(CLOCK_MONOTONIC, &end);

	double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
	printf("sequentially\n");
	printf("max = %d, elapsed_time = %f\n", max_seq, elapsed_time);
		
	clock_gettime(CLOCK_MONOTONIC, &start);
	t_args args[N];
	int max_threads = 0;
	for(int i = 0; i < N; i++) {
		args[i].max = 0;
		args[i].arr = arr + i * SIZE / N;
	}

	for(int i = 0; i < N; i++)
		pthread_create(&thread[i], NULL, thread_function, (void**) &args[i]);

	for(int i = 0; i < N; i++) {
		pthread_join(thread[i], NULL);
		if(args[i].max > max_threads)
			max_threads = args[i].max;
	}
	clock_gettime(CLOCK_MONOTONIC, &end);
	elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
	printf("using N threads\n");
	printf("max = %d, elapsed_time = %f\n", max_threads, elapsed_time);
	
	free(arr);
	return 0;
}


