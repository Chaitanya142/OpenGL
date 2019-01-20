#include "pch.h"
#include <stdio.h>

void change_count(void);

int main()
{
	int a = 5;
	printf("a=%d\n", a);
	change_count();
	change_count();
	change_count();
	return 0;
}

void change_count(void) {

	static int local_variable = 0;
	local_variable++;
	printf("local_variable=%d\n", local_variable);
}
