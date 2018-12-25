#include<stdio.h>
#include<math.h>

int main(void){
	int no,sqrtOfNo,i=2,flag=0;
	printf("Enter no:\n");
	scanf("%d",&no);
	sqrtOfNo=(int)sqrt(no);
	
	for(i;i<=sqrtOfNo;i++){
		if(no%i==0){
		flag=1;
		break;
		}
	}
	if(flag==0)
		printf("%d is prime no",no);
	else
		printf("%d is not prime no",no);

	return (0);
}