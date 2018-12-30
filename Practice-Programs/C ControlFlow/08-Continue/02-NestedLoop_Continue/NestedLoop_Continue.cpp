#include <stdio.h>

int main(void){

int i,j;

printf("\n\nOuter Loop Prints Odd Numbers Between 1 and 10. \n\n");
printf("Inner Loop Prints Even Numbers Between 1 and 10 For Every Odd Number Printed By Outer Loop. \n\n");

for(i=1;i<=10;i++){
	if(i%2!=0){
		printf("i = %d\n----------\n",i);
		for(j=1;j<=10;j++){
			if(j%2==0)
				printf("\tj = %d\n",j);
			else
				continue;
		}
		printf("\n\n");
	}
	else{
		continue;
	}
}

return(0);
}