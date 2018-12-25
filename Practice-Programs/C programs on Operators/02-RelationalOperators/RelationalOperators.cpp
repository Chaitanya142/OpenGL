#include<stdio.h>

int main(void){
int a,b,result;

printf("\n\nEnter One Integer : ");
scanf("%d",&a);

printf("\n\nEnter Another Integer : ");
scanf("%d",&b);

printf("\n\n");
printf("If Answer = 0, It Is 'False'.\n");
printf("If Answer = 1, It Is 'True'.\n\n");

result=(a<b);
printf("(a < b) A = %d Is Less Than B = %d                               \t Answer =%d \n",a,b,result);

result=(a>b);
printf("(a > b) A = %d Is Greater Than B = %d 				\t Answer =%d \n",a,b,result);

result=(a<=b);
printf("(a <= b) A = %d Is Less Than Or Equal To B = %d 		\t Answer =%d \n",a,b,result);

result=(a>=b);
printf("(a >= b) A = %d Is Greater Than Or Equal To B = %d 		\t Answer =%d \n",a,b,result);

result=(a==b);
printf("(a >= b) A = %d Is Equal To B = %d				    \t Answer =%d \n",a,b,result);

result=(a!=b);
printf("(a != b) A = %d Is Not Equal To B = %d 				\t Answer =%d \n",a,b,result);


printf("\n\n");

return(0);
}