#include <stdio.h>

int main(void){
int i,j;

printf("\n\nPrinting Digits 1 to 10 :\n\n");
i=10;
j=100;
while(i>=1,j>=10){
	printf("\t%d\t%d\n",i,j);
	i--;
	j=j-10;
}

printf("\n\n");
return (0);
}