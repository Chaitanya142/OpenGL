#include "pch.h"
#include <stdio.h>

int global_count = 0;

int main()
{
	void change_count_one(void);
	void change_count_two(void);
	printf("Global Variable=%d\n", global_count);
	change_count_one();
	change_count_two();
	return 0;
}

void change_count_one(void) {
	global_count++;
	printf("Global varible%d\n",global_count);
}

void change_count_two(void) {
	global_count++;
	printf("Global varible%d\n", global_count);

}
