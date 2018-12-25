#include <stdio.h>

int main(void){

int no,additionOfDigits=0;

printf("Enter No\n");
scanf("%d",&no);

while(no>0){
additionOfDigits=additionOfDigits+(no%10);
no=no/10;
}

if(additionOfDigits%2==0)
	printf("Addition of digits is even");
else
	printf("Addition of digits is odd");

return (0);
}
