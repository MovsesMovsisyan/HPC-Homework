#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#define N 5

pthread_barrier_t barrier;
void* thread_function(void* arg) {
	int id = *(int*) arg;
	int sleep_time = rand() % 20 + 1;
	printf("Thread %d started stage 1(%d seconds)\n",  id, sleep_time);
	sleep(sleep_time);
	printf("Thread %d finished stage 1\n", id);
	pthread_barrier_wait(&barrier);
	
	sleep_time = rand() % 20 + 1;
	printf("Thread %d started stage 2(%d seconds)\n",  id, sleep_time);
	sleep(sleep_time);
	printf("Thread %d finished stage 2\n", id);
	pthread_barrier_wait(&barrier);
	
	sleep_time = rand() % 20 + 1;
	printf("Thread %d started stage 3(%d seconds)\n",  id, sleep_time);
	sleep(sleep_time);
	printf("Thread %d finished stage 3\n", id);
	pthread_barrier_wait(&barrier);
	return NULL;
}

pthread_t thread[N];

int main() {
	srand(42);
	pthread_barrier_init(&barrier, NULL, N + 1);

	int ids[N];
	for(int i = 0; i < N; i++) {
		ids[i] = i;
		pthread_create(&thread[i], NULL, thread_function, &ids[i]);
	}

	pthread_barrier_wait(&barrier);	
	pthread_barrier_wait(&barrier);	
	pthread_barrier_wait(&barrier);	
	pthread_barrier_destroy(&barrier);

	return 0;
}
