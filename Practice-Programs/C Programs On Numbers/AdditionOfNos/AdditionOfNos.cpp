#include <stdio.h>
#include <stdlib.h>

int main(void)
{

	int totalNo,temp,addition=0;
	printf("Enter Total Nos:");

	scanf("%d",&totalNo);
	
	for(int i=0;i<totalNo;i++){
		scanf("%d",&temp);
		addition=addition+temp;
	}

	printf("Addition:%d",addition);
	
}