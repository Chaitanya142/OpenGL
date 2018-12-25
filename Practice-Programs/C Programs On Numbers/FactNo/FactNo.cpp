#include<stdio.h>

int main(void){
	
printf("Enter No\n");
	
int no,factNo=1;
scanf("%d",&no);

for(int i=1;i<=no;i++){
factNo=factNo*i;
}

if(no==0)
	printf("Fact of %d is 0",no);
else
printf("Fact of %d is %d",no,factNo);

return (0);
}