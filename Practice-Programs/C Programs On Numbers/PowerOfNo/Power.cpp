#include <stdio.h>

int main(void){

int no=0,power=0,result=1;

printf("Enter No:");
scanf("%d",&no);

printf("Enter Power:");
scanf("%d",&power);

for(int i=0;i<power;i++){
	result=result*no;
}
printf("%d",result);

return (0);
}
