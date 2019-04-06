#include <stdio.h>
#include <conio.h>
#include <string.h>

#define MAX_STRING_LENGTH 512

int main(void)
{
	char chArray[MAX_STRING_LENGTH]; 
	int iStringLength = 0;

	printf("\n\nEnter A String : \n\n");
	gets_s(chArray, MAX_STRING_LENGTH);

	printf("\n\nString Entered By You Is : \n\n");
	printf("%s\n", chArray);

	printf("\n\n");
	iStringLength = strlen(chArray);
	printf("Length Of String Is = %d Characters !!!\n\n", iStringLength);

	return(0);
}
