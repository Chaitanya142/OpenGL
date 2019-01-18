#include<stdio.h>
#include<string>
#include<string.h>
#define BUFFER_SIZE 1000

void DisplayFileContents();
int main(void) {
            DisplayFileContents();
            return 0;
}

void DisplayFileContents() {

            FILE *fp = NULL;
            char ch;
            char str[BUFFER_SIZE];
            char fileName[50];
            printf("Enter File Name\n");
            fgets(fileName, 50, stdin);
            //gets(fileName);
            //scanf_s("%[^\n]%*c", str);
            char *context;
            char* token=strtok_s(fileName, "\n", &context);
			
            fopen_s(&fp, token, "r");//fp=fopen(fileName,"r")
            if (fp == NULL) {

                        printf("Cannot Open File");
                        return;
            }
            else
                        printf("file opened successfully\n");
            //fseek(fp,-10,SEEK_END);//SEEK_END,SEEK_BEGIN
            while (fgets(str, BUFFER_SIZE, fp) != NULL)
            {
                        printf("%s",str);//puts(str);
            }
            fclose(fp);
}
