#include <stdio.h>

void swap(int* a, int*b) {
	*a = *a + *b;
	*b = *a - *b;
	*a -= *b;
}

int main() {
	int a = 2; 
	int b = 4;
	printf("before the swap\n a = %d b = %d\n", a, b);

	swap(&a, &b);
	printf("after the swap\n a = %d b = %d\n", a, b);
	return 0;

}
