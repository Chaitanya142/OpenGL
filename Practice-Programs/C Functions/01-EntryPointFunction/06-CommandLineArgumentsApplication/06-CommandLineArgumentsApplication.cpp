#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>

int main(int argc,char *argv[],char *envp[]){

int i,num,sum=0;
// *** DUE TO USE OF atoi(), ALL COMMAND LINE ARGUMENTS OF TYPES OTHER THAN 'int' ARE IGNORED ***
printf("\n\nSum Of All Interger Command Line Arguments Is : %d \n\n",argc);

for(int i=0;i<argc;i++){
num=atoi(argv[i]);
sum=sum+num;
}

printf("Sum : %d\n\n",sum);

return (0);
}