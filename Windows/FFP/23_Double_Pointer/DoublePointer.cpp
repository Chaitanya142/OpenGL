#include<stdio.h>
#include<stdlib.h>


int main(void){

int **ptr=NULL;
int noOfRows,noOfElements;

printf("Enter No Of Rows\t");
scanf("%d",&noOfRows);

ptr=(int **)malloc(noOfRows*sizeof(int *));
if(ptr==NULL){
printf("Memory allocation failed\n");
exit(0);
}


printf("Enter No Of Elements\t");
scanf("%d",&noOfElements);

for(int i=0;i<noOfRows;i++){
ptr[i]=(int *)malloc(noOfElements*sizeof(int));
if(ptr[i]==NULL){
printf("Memory allocation failed\n");
exit(0);
}
}

for(int i=0;i<noOfRows;i++)
	for(int j=0;j<noOfElements;j++){
		printf("Enter Element %d of Row %d\t",j,i);
		scanf("%d",&ptr[i][j]);
	}
	
printf("\n\n");

for(int i=0;i<noOfRows;i++){
printf("ptr+%d : %p\n",i,ptr+i);
}	

printf("\n\n");

for(int i=0;i<noOfRows;i++){
printf("ptr[%d]: %p\n",i,ptr[i]);
}	

printf("\n\n");

for(int i=0;i<noOfRows;i++)
	for(int j=0;j<noOfElements;j++)
		printf("Address	ptr[%d][%d]: %p 	Value:	%d\n",i,j,&ptr[i][j],ptr[i][j]);

for(int i=noOfRows-1;i>=0;i++){
free(ptr[i]);
ptr[i]=NULL;
}	

if(ptr){
free(ptr);
ptr=NULL;
}	
	
return(0);
}