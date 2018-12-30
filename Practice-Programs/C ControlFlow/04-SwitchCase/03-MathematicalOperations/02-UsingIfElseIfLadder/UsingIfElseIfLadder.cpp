#include<stdio.h>
#include<conio.h>

int main(void) {

	int a, b, result;
	char option, option_division;

	printf("\n\nEnter Value For 'A' : ");
	scanf("%d", &a);

	printf("\n\nEnter Value For 'B' : ");
	scanf("%d", &b);

	printf("Enter Option In Character : \n\n");
	printf("'A' or 'a' For Addtion \n");
	printf("'S' or 's' For Substraction \n");
	printf("'M' or 'm' For Multiplication \n");
	printf("'D' or 'd' For Division \n");

	printf("Enter Optioin : ");
	option = getch();

	printf("\n\n");

	
	if(option=='A'||option=='a')
	{
		result = a + b;
		printf("Addition Of A= %d And B= %d Gives Result % d !!!\n\n", a, b, result);
	}
	else if(option=='S'||option=='s')
	{
		if (a >= b)
		{
			result = a - b;
			printf("Substraction of B = %d From A = %d Gives Result %d !!!\n\n", b, a, result);
		}
		else
		{
			result = b - a;
			printf("Substraction of A = %d From B = %d Gives Result %d !!!\n\n", a, b, result);
		}
	}
	else if(option=='M'||option=='m')
	{
		result = a * b;
		printf("Multiplication Of A = %d And B = %d Gives Result %d !!!\n\n", a, b, result);
	}
	else if(option=='D'||option=='d')
	{
		printf("Enter Option In Character : \n\n");
		printf("'Q' or 'q' or '/' For Quotient Upon Division : \n");
		printf("'R' or 'r' or '%' For Remainder Upon Division : \n");

		printf("Enter Option : ");

		option_division = getch();
		option_division = getch();
		
		printf("\n\n");
		if(option_division=='Q'||option_division=='q'||option_division=='/')
		{
			if (a >= b)
			{
				result = a / b;
				printf("Division Of A = %d By B = %d Gives Quotient = %d !!!\n\n", a, b, result);
			}
			else
			{
				result = b / a;
				printf("Division Of B = %d By A = %d Gives Quotient = %d !!!\n\n", b, a, result);
			}
		}
		else if(option_division=='R'||option_division=='r'||option_division=='%')
		{
			if (a >= b)
			{
				result = a % b;
				printf("Division Of A = %d By B = %d Gives Remainder = %d !!!\n\n", a, b, result);
			}
			else
			{
				result = b % a;
				printf("Division Of B = %d By A = %d Gives Remainder = %d !!!\n\n", b, a, result);
			}
		}	
		else	
			printf("Invalid Character %c Entered For Division !!! Please Try Again...\n\n", option_division);
		
	}
	else
	printf("Invalid Character %c Entered !!! Please Try Again...\n\n", option);

	printf("If - Else If - Else Complete!!!\n");

	return(0);
}