#include<stdio.h>

#define NUM_ELEMENTS 10

int main(void){

int iArray[NUM_ELEMENTS];
int i,num,sum=0;

printf("\n\nEnter Integer Elememts For Array : \n\n");
for (i = 0; i < NUM_ELEMENTS; i++)
{
	scanf("%d", &num);
	iArray[i] = num;
}

printf("\n\nEven Numbers Amongst The Array Elememts Are : \n\n");

for(i = 0; i < NUM_ELEMENTS; i++)
{
if ((iArray[i] % 2) == 0)
			printf("%d\n", iArray[i]);
}

printf("\n\nOdd Numbers Amongst The Array Elememts Are : \n\n");

for(i = 0; i < NUM_ELEMENTS; i++)
{
if ((iArray[i] % 2) != 0)
			printf("%d\n", iArray[i]);
}

return(0);

}
