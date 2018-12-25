#include <stdio.h>
#include <stdlib.h>

int main(void)
{

	int totalNo,temp,maxNo=0,secondMaxNo=0;
	printf("Enter Total Nos:");

	scanf("%d",&totalNo);
	
	for(int i=0;i<totalNo;i++){
		scanf("%d",&temp);
		
		if(i==0)
		{
			maxNo=temp;
			secondMaxNo=temp;
		}
		
		if(temp>maxNo){
		secondMaxNo=maxNo;
		maxNo=temp;
		}else if(temp>secondMaxNo){
			secondMaxNo=temp;
		}
		
	}

	printf("Second Max No:%d",secondMaxNo);
	
}