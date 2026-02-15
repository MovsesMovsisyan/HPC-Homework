#include <stdio.h>
#include <stdlib.h>


int main() {
	int* a;
      	a = malloc(sizeof(int));
	*a = 4;
	printf("a = %d\n", *a);
	free(a);

	int n = 5;
	int* arr;
	arr = malloc(n * sizeof(int));
	for(int i = 0; i < n; i++) {
		*(arr + i) = i;
	}
	for(int i = 0; i < n; i++) {
		printf("arr[%d] = %d\n", i, arr[i]);
	}
	free(arr);
	return 0;
}
