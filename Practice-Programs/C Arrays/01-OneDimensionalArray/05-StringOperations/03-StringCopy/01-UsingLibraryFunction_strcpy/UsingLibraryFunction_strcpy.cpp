#include <stdio.h>
#include <string.h>

#define MAX_STRING_LENGTH 512

int main(void)
{
		char chArray_Original[MAX_STRING_LENGTH], chArray_Copy[MAX_STRING_LENGTH]; 

	printf("\n\nEnter A String : \n\n");
	gets_s(chArray_Original, MAX_STRING_LENGTH);

	strcpy(chArray_Copy, chArray_Original);

	printf("\n\nThe Original String Entered By You (i.e : 'chArray_Original[]') Is : \n\n");
	printf("%s\n", chArray_Original);

	printf("\n\nThe Copied String (i.e : 'chArray_Copy[]') Is : \n\n");
	printf("%s\n", chArray_Copy);

	return(0);
}
