#include<stdio.h>
#include<conio.h>

int main(void){
char option,ch='\0';

printf("\n\nOnce The Infinite Loop Begins, Enter 'Q' or 'q' To Quit The Infinite For Loop : \n\n");

do
{
	do{
		
		printf("\nIn Loop...\n");
		ch=getch();
	
	}while(ch!='Q' &&  ch!='q');

	printf("\n\nEXITTING USER CONTROLLED INFINITE LOOP...\n\n");
	
	printf("\n\nDO YOU WANT TO BEGIN USER CONTROLLED INFINITE LOOP AGAIN?...(Y/y - Yes, Any Other Key - No) : ");
	
	option = getch();
	
}while(option == 'Y' || option == 'y');


return(0);
}