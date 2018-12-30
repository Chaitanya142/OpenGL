#include<stdio.h>

int main(void){

int a;

printf("\n\n");

a=5;

if(a)
printf("if-block 1 : 'A' Exists And Has Value = %d !!!\n\n",a);

a=-5;


if(a)
printf("if-block 2 : 'A' Exists And Has Value = %d !!!\n\n",a);


a=0;

if(a)
printf("if-block 3 : 'A' Exists And Has Value = %d !!!\n\n",a);


printf("All Three if-statements Are Done !!!\n\n");
return(0);

}