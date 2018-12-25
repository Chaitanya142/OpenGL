#include<stdio.h>

int PowerOfNo(int no,int Power);

int main(void){
int no,digits=0,sum=0;

printf("Enter No:\n");
scanf("%d",&no);

int tempNo=no;

while(tempNo>0){
	tempNo=tempNo/10;
	digits++;
}

tempNo=no;

while(tempNo>0){
	sum=sum+PowerOfNo(tempNo%10,digits);
	tempNo=tempNo/10;
}

if(sum==no)
	printf("%d is Amstrong No",no);
else 
	printf("%d is not Amstrong No",no);

return (0);
}

int PowerOfNo(int no,int power){
int result=1;
for(int i=0;i<power;i++){
	result=result*no;
}

return result;
}
