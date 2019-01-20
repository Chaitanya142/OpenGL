#include "pch.h"
#include <stdio.h>

extern int global_count ;

void change_count_two(void) {
	global_count++;
	printf("Global variable=%d\n", global_count);
}