#include<stdio.h>


int main(void){
int i,j;
printf("\n\n");
for(i=1;i<=20;i++)
{
	for(j=1;j<=20;j++)
	{
		if(j>i)
			break;
		else
			printf("* ");
	}
	printf("\n");
}
printf("\n\n");
return(0);
}