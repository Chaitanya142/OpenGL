#include<stdio.h>

int main(int argc,char* argv[],char* envp[]){

	void MyAddition(int,int);
	
	int a,b;
	
	printf("\n\n Enter Integer Value For 'A' : ");
	scanf("%d",&a);

	printf("\n\n Enter Integer Value For 'B' : ");
	scanf("%d",&b);
	
	MyAddition(a,b);
	
	return(0);
}


void MyAddition(int a,int b){
	
	int sum;
	
	sum=a+b;
	
	printf("\n\n Sum of %d And %d = %d\n\n",a,b,sum);

}