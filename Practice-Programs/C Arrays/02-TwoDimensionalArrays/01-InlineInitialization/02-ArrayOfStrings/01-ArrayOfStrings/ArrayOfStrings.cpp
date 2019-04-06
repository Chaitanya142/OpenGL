#include <stdio.h>

#define MAX_STRING_LENGTH 512

int main(void)
{
	int MyStrlen(char[]);
		
	char strArray[10][15] = { "Hello!", "Welcome", "To", "Real", "Time", "Rendering", "Batch", "(2018-19-20)", "Of", "ASTROMEDICOMP." };
	int char_size;
	int strArray_size;
	int strArray_num_elements, strArray_num_rows, strArray_num_columns;
	int strActual_num_chars = 0;
	int i;

	char_size = sizeof(char);

	strArray_size = sizeof(strArray);
	printf("\n\nSize Of Two Dimensional ( 2D ) Character Array (String Array) Is = %d\n\n", strArray_size);

	strArray_num_rows = strArray_size / sizeof(strArray[0]);
	printf("\n\nNumber of Rows (Strings) In Two Dimensional ( 2D ) Character Array (String Array) Is = %d\n\n", strArray_num_rows);

	strArray_num_columns = sizeof(strArray[0]) / char_size;
	printf("Number of Columns In Two Dimensional ( 2D ) Character Array (String Array) Is = %d\n\n", strArray_num_columns);

	strArray_num_elements = strArray_num_rows * strArray_num_columns;
	printf("Maximum Number of Elements (Characters) In Two Dimensional ( 2D ) Character Array (String Array) Is = %d\n\n", strArray_num_elements);

	for (i = 0; i < strArray_num_rows; i++)
	{
		strActual_num_chars = strActual_num_chars + MyStrlen(strArray[i]);
	}
	printf("Actual Number of Elements (Characters) In Two Dimensional ( 2D ) Character Array (String Array) Is = %d\n\n", strActual_num_chars);

	printf("\n\n");
	printf("Strings In The 2D Array : \n\n");

	printf("%s ", strArray[0]);
	printf("%s ", strArray[1]);
	printf("%s ", strArray[2]);
	printf("%s ", strArray[3]);
	printf("%s ", strArray[4]);
	printf("%s ", strArray[5]);
	printf("%s ", strArray[6]);
	printf("%s ", strArray[7]);
	printf("%s ", strArray[8]);
	printf("%s\n\n", strArray[9]);

	return(0);
}

int MyStrlen(char str[])
{
	int j;
	int string_length = 0;

	for (j = 0; j < MAX_STRING_LENGTH; j++)
	{
		if (str[j] == '\0')
			break;
		else
			string_length++;
	}
	return(string_length);
}
