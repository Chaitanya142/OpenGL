#include <stdio.h>
int main(void)
{
	int iArray[5][3] = { {1, 2, 3}, {2, 4, 6}, {3, 6, 9}, {4, 8, 12}, {5, 10, 15} }; 
	int int_size;
	int iArray_size;
	int iArray_num_elements, iArray_num_rows, iArray_num_columns;

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
	
	printf("****** ROW 1 ******\n");
	printf("iArray[0][0] = %d\n", iArray[0][0]); 
	printf("iArray[0][1] = %d\n", iArray[0][1]); 
	printf("iArray[0][2] = %d\n", iArray[0][2]); 
	
	printf("\n\n****** ROW 2 ******\n");
	printf("iArray[1][0] = %d\n", iArray[1][0]); 
	printf("iArray[1][1] = %d\n", iArray[1][1]); 
	printf("iArray[1][2] = %d\n", iArray[1][2]); 
	
	printf("\n\n****** ROW 3 ******\n");
	printf("iArray[2][0] = %d\n", iArray[2][0]); 
	printf("iArray[2][1] = %d\n", iArray[2][1]);
	printf("iArray[2][2] = %d\n", iArray[2][2]); 

	printf("\n\n****** ROW 4 ******\n");
	printf("iArray[3][0] = %d\n", iArray[3][0]); 
	printf("iArray[3][1] = %d\n", iArray[3][1]); 
	printf("iArray[3][2] = %d\n", iArray[3][2]); 

	printf("\n\n****** ROW 5 ******\n");
	printf("iArray[4][0] = %d\n", iArray[4][0]); 
	printf("iArray[4][1] = %d\n", iArray[4][1]); 
	printf("iArray[4][2] = %d\n", iArray[4][2]); 

	printf("\n\n");

	return(0);
}

