#include "pch.h"
#include <stdio.h>

int main()
{
	void change_count(void);
	extern int global_count;
	printf("Global Variable=%d\n", global_count);
	change_count();
	return 0;
}

int global_count = 1;

void change_count(void) {
	global_count = global_count + 1;
	printf("Global Variable=%d\n", global_count);
}
