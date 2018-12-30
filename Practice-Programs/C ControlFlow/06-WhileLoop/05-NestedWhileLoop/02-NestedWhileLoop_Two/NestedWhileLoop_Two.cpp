#include<stdio.h>

int main(void){

int i,j,k;

printf("\n\n");
i=1;

while(i<=10)
{
	printf("i = %d\n-----------\n\n",i);
	j=1;
	while(j<=5){
		printf("\tj = %d\n-----------\n\n",j);
		
		k=1;
		while(k<=3){
			printf("\t\tk = %d\n",k);
			k++;
		}
		j++;
		printf("\n\n");
	}
	i++;
	printf("\n\n");

}

return(0);

}