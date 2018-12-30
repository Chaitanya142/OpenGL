#include<stdio.h>

int main(void){

int i,j;

printf("\n\n");
i=1;

while(i<=10)
{
	printf("i = %d\n-----------\n\n",i);
	j=1;
	while(j<=5){
		printf("\tj = %d\n",j);
		j++;
	}
	i++;
	printf("\n\n");

}

return(0);

}