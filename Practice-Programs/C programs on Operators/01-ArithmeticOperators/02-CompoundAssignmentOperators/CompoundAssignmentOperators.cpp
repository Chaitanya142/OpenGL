#include<stdio.h>

int main(void){
int a,b,x;

printf("\n\nEnter A No : ");
scanf("%d",&a);

printf("\n\nEnter Another No : ");
scanf("%d",&b);

printf("\n\n");

x=a;
a+=b;
printf("Addition of A= %d & B= %d Gives %d.\n",x,b,a);

x=a;
a-=b;
printf("Substraction of A= %d & B= %d Gives %d.\n",x,b,a);

x=a;
a*=b;
printf("Multiplication of A= %d & B= %d Gives %d.\n",x,b,a);

x=a;
a/=b;
printf("Division of A= %d & B= %d Gives Quotient %d.\n",x,b,a);

x=a;
a%=b;
printf("Division of A= %d & B= %d Gives Remainder %d.\n",x,b,a);

printf("\n\n");
}