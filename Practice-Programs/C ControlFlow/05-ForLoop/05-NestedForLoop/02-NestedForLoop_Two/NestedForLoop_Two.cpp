#include <stdio.h>

int main(void){
int i,j,k;
printf("\n\n");
for(i=1;i<=10;i++)
{
	printf("i = %d\n-----------\n\n",i);
	for(j=1;j<=5;j++){
		printf("\tj = %d\n\t-----------\n\n",j);
		for(k=1;k<=3;k++){
			printf("\t\tk = %d\n",k);
		}
		printf("\n\n");
	}
	printf("\n\n");
}
return(0);
}
