#include "pch.h"
#include <stdio.h>
#define MY_PI  3.1415926535897932

enum {
	SUNDAY,
	MONDAY,
	TUESDAY,
	WEDNESDAY,
	THURSDAY,
	FRIDAY,
	SATURDAY
};
enum {
	JANUARY,
	FEBRUARY,
	MARCH,
	APRIL,
	MAY,
	JUNE,
	JULY,
	AUGUST,
	SEPTEMBER,
	OCTOMBER,
	NOVEMBER,
	DECEMBER
};
enum numbers
{
	ONE,
	TWO,
	THREE,
	FOUR,
	FIVE = 5,
	SIX,
	SEVEN,
	EIGHT,
	NINE
};
enum boolean
{
	TRUE = 1,
	FALSE = 0
};



int main()
{
	const double epsilon = 0.000001;
	char Days[][15] = {
	"SUNDAY",
	"MONDAY",
	"TUESDAY",
	"WEDNESDAY",
	"THURSDAY",
	"FRIDAY",
	"SATURDAY"
	};
	

	char Months[][15]= {
		"JANUARY",
		"FEBRUARY",
		"MARCH",
		"APRIL",
		"MAY",
		"JUNE",
		"JULY",
		"AUGUST",
		"SEPTEMBER",
		"OCTOMBER",
		"NOVEMBER",
		"DECEMBER"
	};
	char Numbers[][15] =
	{
		"ONE",
		"TWO",
		"THREE",
		"FOUR",
		"FIVE",
		"SIX",
		"SEVEN",
		"EIGHT",
		"NINE"
	};
	

	
	
	printf("My_Py= %g\n", MY_PI);
	printf("Epsilon= %g\n", epsilon);

	printf("\n");
	int i;
	for (i = SUNDAY; i <= SATURDAY; i++) {
		printf("%s is day Number in %d \n", Days[i], i);
	}

	printf("\n");
	for (i = JANUARY; i <= DECEMBER; i++) {
		printf("%s is Month Number in %d \n", Months[i], i);
	}

	printf("\n");
	printf("TRUE is Boolean Number in %d \n", TRUE);
	printf("FALSE is Boolean Number in %d \n",FALSE);


	printf("\n");
	for (i = ONE; i <= FOUR; i++) {
		printf("%s is day Number in %d \n", Numbers[i], i);
	}
	for (i = FIVE; i <= NINE; i++) {
		printf("%s is day Number in %d \n", Numbers[i-1], i);
	}

	return 0;
}
