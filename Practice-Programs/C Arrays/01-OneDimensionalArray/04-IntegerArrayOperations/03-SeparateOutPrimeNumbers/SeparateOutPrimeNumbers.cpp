#include <stdio.h>

#define NUM_ELEMENTS 10

int main(void)
{
	int iArray[NUM_ELEMENTS];
	int i, num, j, count = 0;

	printf("\n\nEnter Integer Elememts For Array : \n\n");
	for (i = 0; i < NUM_ELEMENTS; i++)
	{
		scanf("%d", &num);

		if (num < 0)
			num = -1 * num;

		iArray[i] = num;
	}

	printf("\n\nArray Elements Are : \n\n");
	for (i = 0; i < NUM_ELEMENTS; i++)
		printf("%d\n", iArray[i]);

	printf("\n\nPrime Numbers Amongst The Array Elements Are : \n\n");
	for (i = 0; i < NUM_ELEMENTS; i++)
	{
		for (j = 1; j <= iArray[i]; j++)
		{
			if ((iArray[i] % j) == 0)
				count++;
		}

		if (count == 2)
			printf("%d\n", iArray[i]);

		count = 0; 
	}

	return(0);
}

