#include <stdio.h>

int main(void){
int i,j,k;
printf("\n\n");
i=1;
do{

	printf("i = %d\n-----------\n\n",i);
	j=1;
	do{
		printf("\tj= %d\n-----------\n\n",j);
		k=1;
		do{
			printf("\t\tk = %d\n",k);
			k++;
		}while(k<=3);
		j++;
		printf("\n\n");
	}while(j<=5);
	i++;
	printf("\n\n");
}while(i<=10);
return(0);

}