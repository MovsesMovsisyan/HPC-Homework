#define _GNU_SOURCE

#include <stdio.h>
#include <pthread.h>
#include <sched.h>
#include <stdlib.h>

#define BIG_NUMBER 50000000
#define N 10

void* thread_function(void* arg) {
	int* n = (int*) arg;
	for(int i = 0; i < BIG_NUMBER; i++)
		printf("thread %d: CPU core ID: %d\n", *n, sched_getcpu());
	return NULL;	
}


int main() {
	pthread_t thread[N];

	int arr[N];
	for(int i = 0; i < N; i++) {
		arr[i] = i;
		pthread_create(&thread[i], NULL, thread_function, arr + i);
	}

	for(int i = 0; i < N; i++)
		pthread_join(thread[i], NULL);

	return 0;
}

