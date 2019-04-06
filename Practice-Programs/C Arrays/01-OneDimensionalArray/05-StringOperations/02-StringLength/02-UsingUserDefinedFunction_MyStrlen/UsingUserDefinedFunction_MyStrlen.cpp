#include <stdio.h>
#include <string.h>
#define MAX_STRING_LENGTH 512

int main(void)
{
	int MyStrlen(char[]);
	int iStringLength = 0;
	char chArray[MAX_STRING_LENGTH];

	printf("\n\nEnter A String : \n\n");
	gets_s(chArray, MAX_STRING_LENGTH);

	printf("\n\nString Entered By You Is : \n\n");
	printf("%s\n", chArray);

	printf("\n\n");
	iStringLength = MyStrlen(chArray);
	printf("Length Of String Is = %d Characters !!!\n\n", iStringLength);

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
