#include <stdio.h>

int main(void){
int i,j;
printf("\n\n");
i=1;
do{

	printf("i = %d\n-----------\n\n",i);
	j=1;
	do{
		printf("\tj= %d\n",j);
		j++;
	}while(j<=5);
	i++;
	printf("\n\n");
}while(i<=10);
return(0);

}