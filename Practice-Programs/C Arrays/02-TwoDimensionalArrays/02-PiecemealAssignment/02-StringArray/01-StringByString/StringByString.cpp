#include <stdio.h>

#define MAX_STRING_LENGTH 512

int main(void)
{
	void MyStrcpy(char[], char[]);

	char strArray[5][10]; 
	int char_size;
	int strArray_size;
	int strArray_num_elements, strArray_num_rows, strArray_num_columns;
	int i;

	char_size = sizeof(char);

	strArray_size = sizeof(strArray);
	printf("\n\nSize Of Two Dimensional ( 2D ) Character Array (String Array) Is = %d\n\n", strArray_size);

	strArray_num_rows = strArray_size / sizeof(strArray[0]);
	printf("Number of Rows (Strings) In Two Dimensional ( 2D ) Character Array (String Array) Is = %d\n\n", strArray_num_rows);

	strArray_num_columns = sizeof(strArray[0]) / char_size;
	printf("Number of Columns In Two Dimensional ( 2D ) Character Array (String Array) Is = %d\n\n", strArray_num_columns);

	strArray_num_elements = strArray_num_rows * strArray_num_columns;
	printf("Maximum Number of Elements (Characters) In Two Dimensional ( 2D ) Character Array (String Array) Is = %d\n\n", strArray_num_elements);

	MyStrcpy(strArray[0], "My");
	MyStrcpy(strArray[1], "Name");
	MyStrcpy(strArray[2], "Is");
	MyStrcpy(strArray[3], "Pradnya");
	MyStrcpy(strArray[4], "Gokhale");

	printf("\n\nThe Strings In the 2D Character Array Are : \n\n");

	for (i = 0; i < strArray_num_rows; i++)
		printf("%s ", strArray[i]);

	printf("\n\n");

	return(0);
}

void MyStrcpy(char str_destination[], char str_source[])
{
	int MyStrlen(char[]);

	int iStringLength = 0;
	int j;

	iStringLength = MyStrlen(str_source);
	for (j = 0; j < iStringLength; j++)
		str_destination[j] = str_source[j];

	str_destination[j] = '\0';
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
