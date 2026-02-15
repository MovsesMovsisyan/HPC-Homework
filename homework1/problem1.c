#include <stdio.h>

int main() {
	int a = 7;
	int* ptr_a = &a;

	printf("%p\n", &a);
	printf("%p\n", ptr_a);

	*ptr_a = *ptr_a + 1;
	printf("%d\n", a);

	return 0;
}

