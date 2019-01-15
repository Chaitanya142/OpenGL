#include<stdio.h>

void FileOpen();

int main(void){
	
	FileOpen();
	
	return 0;
}

void FileOpen(){
	FILE *fp=NULL;
	
	fp=fopen("temp.txt","w");
	
	if(fp == NULL)
		printf("Cannot open file");
	else 
		printf("Successfully file opened");
	
	fclose(fp);
	
}
