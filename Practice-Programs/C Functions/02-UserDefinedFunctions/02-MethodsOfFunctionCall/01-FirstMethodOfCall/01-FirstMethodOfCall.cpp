#include<stdio.h>

int main(int argc,char* argv[], char* envp[]){

void MyAddition(void);
int MySubstraction(void);
void MyMultiplication(int,int);
int MyDivision(int,int);

int result_substraction;
int a_multiplication,b_multiplication;
int a_division,b_division,result_division;

MyAddition();

result_substraction=MySubstraction();

printf("\n\nSubstraction Yields Result = %d\n",result_substraction);

printf("\n\nEnter Integer Value 'A' For Multiplication : ");
scanf("%d",&a_multiplication);


printf("\n\nEnter Integer Value 'B' For Multiplication : ");
scanf("%d",&b_multiplication);

MyMultiplication(a_multiplication,b_multiplication);

printf("\n\nEnter Integer Value 'A' For Division : ");
scanf("%d",&a_division);


printf("\n\nEnter Integer Value 'B' For Division : ");
scanf("%d",&b_division);

result_division=MyDivision(a_division,b_division);

printf("\n\nDivision Of %d And %d Gives = %d (Quotient)\n\n",a_division,b_division,result_division);
return (0);
}

void MyAddition(void){
	int a,b,sum;
	printf("\n\nEnter Integer Value 'A' For Addition : ");
	scanf("%d",&a);


	printf("\n\nEnter Integer Value 'B' For Addition : ");
	scanf("%d",&b);
	
	sum=a+b;
	
	printf("\n\nSum Of %d And %d Gives = %d \n\n",a,b,sum);
}

int MySubstraction(void){
	int a,b,substraction;
	printf("\n\nEnter Integer Value 'A' For Substraction : ");
	scanf("%d",&a);


	printf("\n\nEnter Integer Value 'B' For Substraction : ");
	scanf("%d",&b);
	
	substraction=a-b;
	
	return(substraction);
	
}

void MyMultiplication(int a,int b)
{
	int multiplication;
	multiplication=a*b;
	printf("\n\nMultiplicationOf %d And %d = %d\n\n",a,b,multiplication);
}

int MyDivision(int a, int b){
	int division_quotient;
	
	if(a>b)
		division_quotient=a/b;
	else
		division_quotient=b/a;
	
	return (division_quotient);
}