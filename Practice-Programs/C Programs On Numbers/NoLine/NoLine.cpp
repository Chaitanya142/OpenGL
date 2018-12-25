#include<stdio.h>

int main(void){
	
	int no;
	int negativeNo;
	
	printf("Enter No:");
	scanf("%d",&no);

	if(no<0)
		no=no*(-1);
	
	negativeNo=no*(-1);
	
	printf("No Line:\n");

	for(negativeNo;negativeNo<=no;negativeNo++)
		printf("%d\t",negativeNo);
	
	return 0;
}