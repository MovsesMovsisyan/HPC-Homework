#include <stdio.h>

int main() {
	int arr[] = {4, 3, 2, 1, 0};
	for (int i = 0; i < 5; i++) {
		printf("%d\t", *(arr + i));
	}
	printf("\n");

	for (int i = 0; i < 5; i++) {
		*(arr + i) = 4 - *(arr + i);	
	}

	for (int i = 0; i < 5; i++) {
		printf("%d\t", *(arr + i));
	}
	printf("\n");


	for (int i = 0; i < 5; i++) {
		printf("%d\t", arr[i]);
	}
	printf("\n");



	return 0;

}
