#include <stdio.h>
#include <string.h> // for strrev()

#define MAX_STRING_LENGTH 512

int main(void)
{
	char chArray_Original[MAX_STRING_LENGTH]; 

	printf("\n\nEnter A String : \n\n");
	gets_s(chArray_Original, MAX_STRING_LENGTH);

	printf("\n\nThe Original String Entered By You (i.e : 'chArray_Original[]') Is : \n\n");
	printf("%s\n", chArray_Original);

	printf("\n\nThe Reversed String (i.e : 'chArray_Reverse[]') Is : \n\n");
	printf("%s\n", strrev(chArray_Original));

	return(0);
}
