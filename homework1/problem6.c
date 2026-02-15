#include <stdio.h>
#include <string.h>

int str_length(char* str) {
	int length = 0;
	char* a = str;
	while(*a != 0) {
		length++;
		a += 1;
	}
	return length;
}

int main() { 
	char* s = "Missed the homework 1 deadline";

	for(int i = 0; i < strlen(s); i++) {
		printf("%c\n", *(s + i));
	}

	printf("length %d\n", str_length(s)); 
	return 0;
}
