#include<stdio.h>
#include<stdlib.h>

int main(void)
{
	int **ptr;
	int iRow, iColumn;
	int iLoop, jLoop;
	unsigned int i;
	
	printf("%d\t", sizeof(int *));
	printf("%d", sizeof(int));
	
	printf("\n\nEnter number of Rows:");
	scanf("%d", &iRow);
	
	
	ptr = (int **)malloc(iRow * sizeof(int*));
	if(ptr == NULL)
	{
		printf("Memory Allocation failed\n");
		exit(0);
	}
	
	for(iLoop = 0; iLoop < iRow; iLoop++)
	{
		printf("Enter number of columns in %d row: ", iLoop + 1);
		scanf("%d", &iColumn);
		ptr[iLoop] = (int*)malloc(iColumn * sizeof(int));
		if(NULL == ptr[iLoop])
		{
			printf("Memory Allocation failed\n");
			exit(0);
		}
	}
		
	printf("\nPrinting addresses allocated to each row base address \n\n");
	
	//printf("Address of ptr[%d] = %x\n", iLoop, ptr);
	for(iLoop = 0; iLoop < iRow; iLoop++)
	{
		printf("Address of ptr[%d] = %p\n", iLoop, ptr[iLoop]);
	}
	
	for(iLoop = iRow; iLoop >= 0; iLoop --)
	{
		free(ptr[i]);
		ptr[i] = NULL;
	}
	
	/*printf("Printing value stored at base address of each row which is address of first element in array\n\n");
	for(iLoop = 0; iLoop < iRow; iLoop++)
	{
		printf("Values at ptr[%d] = %p\n", iLoop, *(ptr[iLoop]));
	}*/
	
	return(0);
}