#include <stdio.h>
#include <stdlib.h>

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

	printf("Even Nos:\n");

	
	for(int i=0;i<totalNo;i++)
		if(*(ptr+i)%2==0)
			printf("%d\t",*(ptr+i));
	
	free(ptr);
}