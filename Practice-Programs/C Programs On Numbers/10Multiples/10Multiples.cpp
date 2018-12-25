#include<stdio.h>

int main(void){
	
	int no;
	int i=1;
	
	printf("Enter No:");
	scanf("%d",&no);

	for(i;i<=10;i++)
		printf("%d\t",no*i);
	
	return 0;
}