#include "pch.h"
#include<stdio.h>

int main()
{
	int a = 13;
	printf("decimal value %d\n",a);
	printf("octal value  %o\n", a);
	printf("Hexadecimal values in Upper %X\n", a);
	printf("Hexadecimal values in Lower %x\n", a);

	char ch = 'P';
	printf("character %c\n", ch);
	
	char str[] = "Chaitanya";
	printf("string %s\n", str);

	long num = 30121995L;
	printf("long %ld\n", num);

	unsigned int b = 7;
	printf("unsigned int %u\n", b);

	float f_num = 3012.1955f;
	printf("float %f\n", f_num);
	printf("float %4.2f\n", f_num);
	printf("float %2.5f\n", f_num);

	double d_pi = 3.14159265358979323846;
	printf("double %g\n", d_pi);
	printf("Double Precision floating number without Exponential(Upper case) %E\n", d_pi);
	printf("Double Precision floating number without Exponential(Lower case) %e\n", d_pi);
	printf("Double Hexadecimal Value Of 'd_pi' (Hexadecimal Letters In Upper Case) %A\n", d_pi);
	printf("Double Hexadecimal Value Of 'd_pi' (Hexadecimal Letters In Lower Case) %a\n", d_pi);

	return 0;
}
