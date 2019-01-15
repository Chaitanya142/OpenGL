#include<stdio.h>

void FileWrite();

int main(void){
	FileWrite();
	
	return 0;
}


void FileWrite(){
	FILE *fp=NULL;
	
	fp=fopen("temp.txt","w");
	
	if(fp == NULL)
		printf("Cannot open file");
	else 
	{	
		printf("Successfully file opened");
		fprintf(fp,"Successfully file opened");
	}
	fclose(fp);
}