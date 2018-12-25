#include <stdio.h>
#include <stdlib.h>

int main(void)
{

	int totalNo,temp,maxNo=0;
	printf("Enter Total Nos:");

	scanf("%d",&totalNo);
	
	for(int i=0;i<totalNo;i++){
		scanf("%d",&temp);
		if(i==0)
		{
			maxNo=temp;
		}
		if(temp>maxNo)
			maxNo=temp;
	}

	printf("Max No:%d",maxNo);
	
}