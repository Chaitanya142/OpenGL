#include <stdio.h>

int main(void){

int no,digits=0;
printf("Enter No\n");
scanf("%d",&no);

while(no>0){
	no=no/10;
	digits++;
}

printf("%d",digits);

return (0);
}
