#include <stdio.h>

#define MAX_STRING_LENGTH 512

int main(void)
{
	int MyStrlen(char[]);
	void MyStrcpy(char[], char[]);

	char chArray_Original[MAX_STRING_LENGTH], chArray_VowelsReplaced[MAX_STRING_LENGTH]; // A Character Array Is A String
	int iStringLength;
	int i;

	printf("\n\nEnter A String : \n\n");
	gets_s(chArray_Original, MAX_STRING_LENGTH);
	MyStrcpy(chArray_VowelsReplaced, chArray_Original);

	iStringLength = MyStrlen(chArray_VowelsReplaced);

	for (i = 0; i < iStringLength; i++)
	{
		switch (chArray_VowelsReplaced[i])
		{
		case 'A':
		case 'a':
		case 'E':
		case 'e':
		case 'I':
		case 'i':
		case 'O':
		case 'o':
		case 'U':
		case 'u':
			chArray_VowelsReplaced[i] = '*';
			break;
		default:
			break;
		}
	}

	printf("\n\nString Entered By You Is : \n\n");
	printf("%s\n", chArray_Original);

	printf("\n\nString After Replacement Of Vowels By * Is : \n\n");
	printf("%s\n", chArray_VowelsReplaced);

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
