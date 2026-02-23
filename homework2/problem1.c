#include <stdio.h>
#include <pthread.h>

pthread_t thread1;
pthread_t thread2;
pthread_t thread3;

void* function(void* ptr) {
	int *i = (int*) ptr;
	printf("thread %d is running\n", *i);
}

int main() {
	int args[3] = {0, 1, 2};
	pthread_create(&thread1, NULL, function, (void*)  args);
	pthread_create(&thread2, NULL, function, (void*) (args + 1));
	pthread_create(&thread3, NULL, function, (void*) (args + 2));

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	pthread_join(thread3, NULL);

	return 0;
}

