#include<stdio.h>

int main(int argc,char *argv[]){
int i;

printf("\n\nHello World !!!\n\n");
printf("Number of Arguments passed to this program are : %d\n\n",argc);

printf("Command Line Arguments Passed To This Function Are : \n\n");
for(i=0;i<argc;i++){

	printf("Command Line Argument Number %d = %s\n", (i + 1),argv[i]);
	
}

printf("\n\n");

return(0);
}