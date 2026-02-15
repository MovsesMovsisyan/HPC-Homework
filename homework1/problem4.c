#include <stdio.h>

int main() {
	int a = 10;
	int* p_a = &a;
	int** pp_a = &p_a;
	printf("using pointer: %d\n", *p_a);
	printf("using double-pointer: %d\n", *(*pp_a));
	return 0;
}
