#include<stdio.h>
#include<conio.h>


int main(void){
char option,ch='\0';


printf("\n\nOnce The Infinite Loop Begins, Enter 'Q' or 'q' To Quit The Infinite For Loop : \n\n");
printf("Enter 'Y' oy 'y' To Initiate User Controlled Infinite Loop : \n\n");

option = getch();

if(option == 'Y' || option == 'y'){

	do{
		
		printf("In Loop...\n");
		ch=getch();
		if(ch=='Q' || ch=='q')
			break;
	
	}while(1);


	printf("\n\nEXITTING USER CONTROLLED INFINITE LOOP...\n\n");
	
}else
	printf("You Must Press 'Y' or 'y' To Initiate The User Controlled Infinite Loop....Please Try Again...\n\n");

return(0);
}