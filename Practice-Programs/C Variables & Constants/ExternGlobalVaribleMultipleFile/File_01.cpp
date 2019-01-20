#include "pch.h"
#include <stdio.h>

extern int global_count ;

void change_count_one(void) {
	global_count++;
	printf("Global Variable=%d\n", global_count);
}