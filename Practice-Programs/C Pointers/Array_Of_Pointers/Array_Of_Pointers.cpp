/*Representing 2-D array using array of pointers*/
#include<stdio.h>
#include<stdlib.h>


int main(void)
{

	int *a[5];
	int iLoop, jLoop;
	int values[15] = {4, 9, 1, 2, 3, 5, 7, 8, 6, 13, 14, 11, 12, 15, 10};
	
	for(iLoop = 0; iLoop < 5; iLoop++)
	{
		a[iLoop] = (int*)malloc(3 * sizeof(int));
		
		if(a[iLoop] == NULL)
		{
			printf("Memory can not be allocated\n");
			exit(0);
		}
	}
	
	printf("\nDisplaying Addresses stored inside Array of integer pointer a[5]\n\n");
	for(iLoop = 0; iLoop < 5; iLoop++)
	{
		printf("Value at a[%d] = %p \n", iLoop , *(a[iLoop]));
	}
	
	printf("\nStoring value to array a[5][3]\n\n");
	for(iLoop = 0; iLoop < 5; iLoop++)
	{
		for(jLoop = 0; jLoop < 3; jLoop++)
		{
			*(a[iLoop] + jLoop) = values[(3 * iLoop) + jLoop];
			printf("%d\t", *(a[iLoop] + jLoop));
		}
		printf("\n");
	}
	
	printf("\nDisplay addresses of each element\n\n");
	for(iLoop = 0; iLoop < 5; iLoop++)
	{
		for(jLoop = 0; jLoop < 3; jLoop++)
		{
			printf("%p\t", (a[iLoop] + jLoop)); 
		}
		printf("\n");
	}
	
	
	for(iLoop = 4; iLoop >= 4; iLoop--)
	{
		free(a[iLoop]);
		a[iLoop] = NULL;
	}
	return(0);
}