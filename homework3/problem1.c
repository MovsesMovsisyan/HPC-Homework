#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>


#define R 10
#define N 5

pthread_barrier_t barrier;
int res[N] = {0};
int round_score[N];

void* thread_function(void* arg) {
	int id = *(int*) arg;
	for (int i = 0; i < R; i++) {
		round_score[id] = rand() % 6 + 1;
		printf("Round %d, thread%d score %d\n", i, id, round_score[id]);	
		pthread_barrier_wait(&barrier);
		pthread_barrier_wait(&barrier);
	}
	return NULL;
}

pthread_t thread[N];

int main() {
	srand(42);
	pthread_barrier_init(&barrier, NULL, N + 1);

	int ids[N];
	for(int i = 0; i < N; i++) {
		ids[i] = i;
	}
	for(int i = 0; i < N; i++) {
		pthread_create(&thread[i], NULL, thread_function, &ids[i]);
	}
	
	for(int j = 0; j < R; j++) {
		pthread_barrier_wait(&barrier);
		int max = 0;
		int max_id = 0;
		for(int i = 0; i < N; i++){
			if(round_score[i] > max) {
				max = round_score[i];
				max_id = i;
			}
		}
		res[max_id]++;
		printf("%d won\n\n", max_id);
		pthread_barrier_wait(&barrier);
	}

	int winner_score = 0;
	int winner_id = 0;
	for(int i = 0; i < N; i++) {
		if(res[i] > winner_score) {
			winner_score = res[i];
			winner_id = i;
		}
	}

	for(int i = 0; i < N; i++) {
	    pthread_join(thread[i], NULL);
	}
	
	pthread_barrier_destroy(&barrier);

	printf("the winner is thread%d with a score %d\n", winner_id, winner_score);
	return 0;
}
