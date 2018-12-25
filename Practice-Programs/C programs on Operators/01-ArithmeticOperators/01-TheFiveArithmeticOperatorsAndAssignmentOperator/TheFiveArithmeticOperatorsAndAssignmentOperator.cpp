#include<stdio.h>

int main(void){
int a,b,result;

printf("\n\nEnter A No : ");
scanf("%d",&a);

printf("\n\nEnter Another No : ");
scanf("%d",&b);

printf("\n\n");

result=a+b;
printf("Addition of A= %d & B= %d Gives %d.\n",a,b,result);

result=a-b;
printf("Substraction of A= %d & B= %d Gives %d.\n",a,b,result);

result=a*b;
printf("Multiplication of A= %d & B= %d Gives %d.\n",a,b,result);

result=a/b;
printf("Division of A= %d & B= %d Gives Quotient %d.\n",a,b,result);

result=a%b;
printf("Division of A= %d & B= %d Gives Remainder %d.\n",a,b,result);

printf("\n\n");
}