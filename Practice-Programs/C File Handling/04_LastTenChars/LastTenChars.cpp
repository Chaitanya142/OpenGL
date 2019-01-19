#include<stdio.h>
#include<string.h>

void PrintLastTenChars();

int main(void){
PrintLastTenChars();
return 0;
}

void PrintLastTenChars(){
FILE *fp=NULL;
int counter=1;
char ch;
char fileName[50];
printf("Enter File Name\n");
fgets(fileName, 50, stdin);

//printf("%s",fileName);
strtok(fileName, "\n");
//gets(fileName);
fp=fopen(fileName,"r");
//fp=fopen("temp1.txt","r");
if(fp==NULL){
            printf("Cannot Open File");
            return;
}
else
            printf("file opened successfully\n");

fseek(fp,-10,SEEK_END);//SEEK_BEGIN,SEEK_CUR//rewind to swap from read mode to write mode
while(counter<=10){
            if(!feof(fp))
            {
            ch=fgetc(fp);
            printf("%c",ch);
            }
            else
            break;
            counter++;
}
fclose(fp);
}
