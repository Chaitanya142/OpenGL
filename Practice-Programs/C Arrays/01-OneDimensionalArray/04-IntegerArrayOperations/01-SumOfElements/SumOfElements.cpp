#include<stdio.h>

#define NUM_ELEMENTS 10

int main(void){

int iArray[NUM_ELEMENTS];
int i,num,sum=0;

printf("\n\nEnter Interger Elements For Array : \n\n");
for(i=0;i<NUM_ELEMENTS;i++){
scanf("%d",&num);
iArray[i]=num;
}

for(i=0;i<NUM_ELEMENTS;i++)
sum=sum+iArray[i];

printf("Sum Of ALL Elements Of Array = %d\n\n",sum);

return (0);
}
