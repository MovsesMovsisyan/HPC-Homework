#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#define N 5

pthread_barrier_t barrier;
void* thread_function(void* arg) {
	int id = *(int*) arg;
	int sleep_time = rand() % 20 + 1;
	printf("Thread %d getting ready(%d seconds)\n",  id, sleep_time);
	sleep(sleep_time);
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
	printf("Game Started!\n");	
	pthread_barrier_destroy(&barrier);

	return 0;
}
