#include <stdio.h>

int main(void){

int i,j;
printf("\n\nPrinting Digits 10 to 1 and 100 to 10 : \n\n");

for(i=10,j=100;i>=1,j>=10;i--,j=j-10)
printf("\t%d\t%d\n",i,j);

return(0);
}