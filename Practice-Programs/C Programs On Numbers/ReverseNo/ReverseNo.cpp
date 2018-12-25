#include <stdio.h>

int main(void){

int no,reverseNo=0;
printf("Enter No\n");
scanf("%d",&no);

while(no>0){
	reverseNo=(reverseNo*10)+(no%10);
	no=no/10;
}

printf("%d",reverseNo);

return (0);
}
