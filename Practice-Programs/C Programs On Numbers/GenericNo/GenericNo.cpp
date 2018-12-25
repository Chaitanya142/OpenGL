#include <stdio.h>

int main(void){

int no,temp=0,temp1=0,root=0;
printf("Enter No\n");
scanf("%d",&no);

while(no>0){
temp=temp+(no%10);
no=no/10;
if(no==0 && temp/10!=0)
{	
	no=temp;
	temp=0;
}
else if(no==0 && temp/10==0)
{
	root=temp;
}
}

printf("Generic Root %d",root);

return (0);
}
