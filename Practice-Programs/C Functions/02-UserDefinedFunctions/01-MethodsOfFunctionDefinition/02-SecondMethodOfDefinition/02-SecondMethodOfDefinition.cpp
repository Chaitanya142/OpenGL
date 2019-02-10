#include<stdio.h>

int main(int argc,char* argv[],char* envp[]){

	int MyAddition(void);
	
	int result;
	
	result=MyAddition();
	
	printf("\n\n Sum = %d\n\n",result);

	return(0);
}


int MyAddition(){
	int a,b,sum;
	
	printf("\n\n Enter Integer Value For 'A' : ");
	scanf("%d",&a);

	printf("\n\n Enter Integer Value For 'B' : ");
	scanf("%d",&b);
	
	sum=a+b;
	
	return(sum);
}