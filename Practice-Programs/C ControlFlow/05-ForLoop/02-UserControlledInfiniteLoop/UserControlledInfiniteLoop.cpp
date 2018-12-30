#include<stdio.h>
#include<conio.h>

int main(void){
char option,ch='\0';

printf("\n\nOnce The Infinite Loop Begins, Enter 'Q' or 'q' To Quit The Infinite Loop :\n\nEnter 'Y' or 'y' To Initiate User Controlled Infinite Loop : \n\n");
option=getch();

if(option == 'Y' || option == 'y')
{
	for(;;)
	{
		printf("In Loop ...\n");
		ch=getch();
		if(ch=='Q' || ch=='q')
			break;
	}

}

printf("\n\nEXITING USER CONTROLLED INFINITE LOOP...\n\n");

return (0);
}