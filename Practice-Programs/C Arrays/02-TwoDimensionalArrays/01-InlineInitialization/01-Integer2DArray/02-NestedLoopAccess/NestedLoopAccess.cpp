#include <stdio.h>
int main(void)
{
	int iArray[5][3] = { {1, 2, 3}, {2, 4, 6}, {3, 6, 9}, {4, 8, 12}, {5, 10, 15} }; 
	int int_size;
	int iArray_size;
	int iArray_num_elements, iArray_num_rows, iArray_num_columns;
	int i, j;

	int_size = sizeof(int);

	iArray_size = sizeof(iArray);
	printf("\n\nSize Of Two Dimensional ( 2D ) Integer Array Is = %d\n\n", iArray_size);

	iArray_num_rows = iArray_size / sizeof(iArray[0]);
	printf("Number of Rows In Two Dimensional ( 2D ) Integer Array Is = %d\n\n", iArray_num_rows);

	iArray_num_columns = sizeof(iArray[0]) / int_size;
	printf("Number of Columns In Two Dimensional ( 2D ) Integer Array Is = %d\n\n", iArray_num_columns);

	iArray_num_elements = iArray_num_rows * iArray_num_columns;
	printf("Number of Elements In Two Dimensional ( 2D ) Integer Array Is = %d\n\n", iArray_num_elements);

	printf("\n\nElements In The 2D Array : \n\n");
	
	for (i = 0; i < iArray_num_rows; i++)
	{
		printf("****** ROW %d ******\n", (i + 1));
		for (j = 0; j < iArray_num_columns; j++)
		{
			printf("iArray[%d][%d] = %d\n", i, j, iArray[i][j]);
		}
		printf("\n\n");
	}

	return(0);
}

