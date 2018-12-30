#include <stdio.h>

int main(void){

int i,j;
printf("\n\nPrinting Digits 1 to 10 and 10 to 100 : \n\n");

for(i=1,j=10;i<=10,j<=100;i++,j=j+10)
printf("\t%d\t%d\n",i,j);

return(0);
}