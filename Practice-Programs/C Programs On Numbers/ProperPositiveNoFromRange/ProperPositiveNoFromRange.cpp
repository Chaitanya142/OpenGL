#include <stdio.h>
#include <stdlib.h>

void CheckPrimeNo(int no);

int main(void)
{

	int totalNo,*ptr;
	printf("Enter Total Nos:");

	scanf("%d",&totalNo);
	
	ptr=(int*)malloc(totalNo*(sizeof(int)));

	if(ptr==NULL){
	printf("Memory ALlocation Failed");
	exit(0);
	}
	
	for(int i=0;i<totalNo;i++)
		scanf("%d",ptr+i);

	printf("Perfect Nos:\n");
	
	for(int i=0;i<totalNo;i++)
	{
		CheckPrimeNo(*(ptr+i));
	}
	
	free(ptr);
}

void CheckPrimeNo(int no){
	int sum=0,temp=0;
	for(int i=1;i<no;i++){
		temp=no%i;
		if(temp==0){
			sum=sum+i;
		}	
	}
	
	if(sum==no){
	printf("%d ",no);
	}
}
