#include <stdio.h>

#define MAX_STRING_LENGTH 512

int main(void)
{
	void MyStrcat(char[], char[]);
	
	char chArray_One[MAX_STRING_LENGTH], chArray_Two[MAX_STRING_LENGTH]; // A Character Array Is A String

	printf("\n\nEnter First String : \n\n");
	gets_s(chArray_One, MAX_STRING_LENGTH);

	printf("\n\nEnter Second String : \n\n");
	gets_s(chArray_Two, MAX_STRING_LENGTH);

	printf("\n\n****** BEFORE CONCATENATION ******");
	printf("\n\nThe Original First String Entered By You (i.e : 'chArray_One[]') Is : \n\n");
	printf("%s\n", chArray_One);

	printf("\n\nThe Original Second String Entered By You (i.e : 'chArray_Two[]') Is : \n\n");
	printf("%s\n", chArray_Two);

	MyStrcat(chArray_One, chArray_Two);

	printf("\n\n****** AFTER CONCATENATION ******");
	printf("\n\n'chArray_One[]' Is : \n\n");
	printf("%s\n", chArray_One);

	printf("\n\n'chArray_Two[]' Is : \n\n");
	printf("%s\n", chArray_Two);

	return(0);
}

void MyStrcat(char str_destination[], char str_source[])
{
	int MyStrlen(char[]);

	int iStringLength_Source = 0, iStringLength_Destination = 0;
	int i, j;

	iStringLength_Source = MyStrlen(str_source);
	iStringLength_Destination = MyStrlen(str_destination);

	for (i = iStringLength_Destination, j = 0; j < iStringLength_Source; i++, j++)
	{
		str_destination[i] = str_source[j];
	}

	str_destination[i] = '\0';
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
