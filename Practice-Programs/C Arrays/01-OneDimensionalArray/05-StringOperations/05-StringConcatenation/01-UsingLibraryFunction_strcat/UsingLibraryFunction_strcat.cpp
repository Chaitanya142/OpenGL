#include <stdio.h>
#include<string.h>


#define MAX_STRING_LENGTH 512

int main(void)
{
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

	strcat(chArray_One, chArray_Two);

	printf("\n\n****** AFTER CONCATENATION ******");
	printf("\n\n'chArray_One[]' Is : \n\n");
	printf("%s\n", chArray_One);

	printf("\n\n'chArray_Two[]' Is : \n\n");
	printf("%s\n", chArray_Two);

	return(0);
}
