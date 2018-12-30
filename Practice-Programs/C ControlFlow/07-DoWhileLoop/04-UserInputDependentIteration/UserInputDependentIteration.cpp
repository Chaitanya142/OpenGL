#include<stdio.h>

int main(void){
int i_num,i,num;

printf("\n\nEnter An Integer Value From Which Iteration Must Begin : ");
scanf("%d",&i_num);
printf("How Many Digits Do You Want To Print From %d Onwards ? : ",i_num);
scanf("%d",&num);
printf("Printing Digits %d to %d : \n\n",i_num,(i_num+num));

i=i_num;
do{
	printf("\t%d\n",i);
	i++;

}while(i<=(i_num+num));

printf("\n\n");
return(0);

}