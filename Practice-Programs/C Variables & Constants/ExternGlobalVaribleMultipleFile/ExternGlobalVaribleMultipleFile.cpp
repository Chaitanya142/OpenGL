#include "pch.h"
#include <stdio.h>

int global_count = 0;

int main()
{
	void change_count_one(void);
	void change_count_two(void);
	void change_count(void);
	printf("Global Variable=%d\n", global_count);
	change_count_one();
	change_count_two();

	change_count();
	return 0;
}

void change_count(void) {
	global_count++;
	printf("Global variable=%d\n", global_count);
}
