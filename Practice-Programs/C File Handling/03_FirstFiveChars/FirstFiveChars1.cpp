#include<stdio.h>

void PrintFirstFiveChars();

int main(void){
PrintFirstFiveChars();
return 0;
}

void PrintFirstFiveChars(){
FILE *fp=NULL;
int counter=1;
char ch;
fp=fopen("temp1.txt","r");
if(fp==NULL){
            printf("Cannot Open File");
            return;
}
else
            printf("file opened successfully\n");

while(counter<=5){
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
