#include<stdio.h>

int main(int argc,char* argv[],char* envp[]){

void MyAddition(void);
MyAddition();

return(0);
}

void MyAddition(void){

int a,b,sum;

printf("\n\nEnter Integer Value Of 'A' :");
scanf("%d",&a);


printf("\n\nEnter Integer Value Of 'B' :");
scanf("%d",&b);

sum=a+b;

printf("\n\nSum Of %d And %d = %d\n\n",a,b,sum);

}