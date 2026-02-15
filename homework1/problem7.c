#include <stdio.h>
#include <stdlib.h>
#define N 10 //less than or equal to 100
int main() {
	char* arr[N];
	for(int i = 0; i < N; i++) {
		arr[i] = malloc(10);
		snprintf(arr[i], 10, "String %d", i);
	}

	printf("The array before modification\n");
	for(int i = 0; i < N; i++) {
		printf("%s\n", *(arr + i));
	}
	
	free(arr[N - 1]);
	arr[N - 1] = "The last string";
	printf("The array after modifciation\n");
	for(int i = 0; i < N; i++) {
		printf("%s\n", *(arr + i));
	}

	for(int i = 0; i < N - 1; i++) {
		free(arr[i]);
	}

	return 0;
}
