#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	int no,sum=0,temp=0;
	printf("Enter No:");
	scanf("%d",&no);
	
	for(int i=1;i<no;i++){
		temp=no%i;
		if(temp==0){
			sum=sum+i;
		}	
	}
	
	if(sum==no){
	printf("%d is perfect no",no);
	}else{
	printf("%d is not perfect no",no);	
	}

}