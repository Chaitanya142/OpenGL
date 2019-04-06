#include<stdio.h>

#define MAX_STRING_LENGTH 512

int main(void){

char chArray[MAX_STRING_LENGTH];

printf("\n\nEnter A String : \n\n");
fgets(chArray,MAX_STRING_LENGTH,stdin);


printf("\n\nString Entered By You Is : \n\n");
printf("%s\n",chArray);

return(0);

}
