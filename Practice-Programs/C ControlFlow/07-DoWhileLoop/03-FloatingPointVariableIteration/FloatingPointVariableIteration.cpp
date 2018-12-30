#include<stdio.h>

int main(void){

float f,f_num=1.7f;

printf("\n\nPrinting Numbers %f to %f : \n\n",f_num,(f_num*10.0f));

f=f_num;

do 
{
	printf("\t%f\n",f);
	f=f+f_num;
}while(f<=(f_num*10.0f));

printf("\n\n");

return(0);
}