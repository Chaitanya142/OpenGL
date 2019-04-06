#include<stdio.h>

int main(void){
int iArray[]={9,30,6,12,98,95,20,23,2,45};
int int_size;
int iArray_size;
int iArray_num_elements;

float fArray[]={1.2f,2.3f,4.5f,5.6f,6.7f,7.8f,8.9f};
int float_size;
int fArray_size;
int fArray_num_elements;

char cArray[]={'A','S','T','R','O','M','E','D','I','C','O','M','P'};

int char_size;
int cArray_size;
int cArray_num_elements;

printf("\n\nIn-line Initialization And Piece-meal Display Of Elements Of Array 'iArray[]':\n\n");

printf("iArray[0] (1st Element)  = %d\n",iArray[0]);
printf("iArray[0] (2nd Element)  = %d\n",iArray[1]);
printf("iArray[0] (3rd Element)  = %d\n",iArray[2]);
printf("iArray[0] (4th Element)  = %d\n",iArray[3]);
printf("iArray[0] (5th Element)  = %d\n",iArray[4]);
printf("iArray[0] (6th Element)  = %d\n",iArray[5]);
printf("iArray[0] (7th Element)  = %d\n",iArray[6]);
printf("iArray[0] (8th Element)  = %d\n",iArray[7]);
printf("iArray[0] (9th Element)  = %d\n",iArray[8]);
printf("iArray[0] (10th Element) = %d\n\n",iArray[9]);

int_size=sizeof(int);
iArray_size=sizeof(iArray);
iArray_num_elements=iArray_size/int_size;

printf("Size Of Data type 'int'								=%d bytes\n",int_size);
printf("Number Of Elements In 'int' Array 'iArray[]' 		=%d Elements\n",iArray_num_elements);
printf("Size Of Array 'iArray[]' (%d Elements * %d Bytes)	= %d Bytes\n\n",iArray_num_elements,int_size,iArray_size);

printf("\n\nIn-line Initialization And Piece-meal Display Of Elements Of Array 'fArray[]':\n\n");

printf("fArray[0] (1st Element)  = %f\n",fArray[0]);
printf("fArray[0] (2nd Element)  = %f\n",fArray[1]);
printf("fArray[0] (3rd Element)  = %f\n",fArray[2]);
printf("fArray[0] (4th Element)  = %f\n",fArray[3]);
printf("fArray[0] (5th Element)  = %f\n",fArray[4]);
printf("fArray[0] (6th Element)  = %f\n",fArray[5]);
printf("fArray[0] (7th Element)  = %f\n",fArray[6]);
printf("fArray[0] (8th Element)  = %f\n",fArray[7]);
printf("fArray[0] (9th Element)  = %f\n",fArray[8]);
printf("fArray[0] (10th Element) = %f\n\n",fArray[9]);

float_size=sizeof(float);
fArray_size=sizeof(fArray);
fArray_num_elements=fArray_size/float_size;

printf("Size Of Data type 'float'							=%d bytes\n",float_size);
printf("Number Of Elements In 'float' Array 'fArray[]' 		=%d Elements\n",fArray_num_elements);
printf("Size Of Array 'fArray[]' (%d Elements * %d Bytes)	= %d Bytes\n\n",fArray_num_elements,float_size,fArray_size);


printf("\n\nIn-line Initialization And Piece-meal Display Of Elements Of Array 'cArray[]':\n\n");

printf("cArray[0] (1st Element)  = %c\n",cArray[0]);
printf("cArray[0] (2nd Element)  = %c\n",cArray[1]);
printf("cArray[0] (3rd Element)  = %c\n",cArray[2]);
printf("cArray[0] (4th Element)  = %c\n",cArray[3]);
printf("cArray[0] (5th Element)  = %c\n",cArray[4]);
printf("cArray[0] (6th Element)  = %c\n",cArray[5]);
printf("cArray[0] (7th Element)  = %c\n",cArray[6]);
printf("cArray[0] (8th Element)  = %c\n",cArray[7]);
printf("cArray[0] (9th Element)  = %c\n",cArray[8]);
printf("cArray[0] (10th Element) = %c\n",cArray[9]);
printf("cArray[0] (11th Element) = %c\n",cArray[10]);
printf("cArray[0] (12th Element) = %c\n",cArray[11]);
printf("cArray[0] (13th Element) = %c\n",cArray[12]);

char_size=sizeof(char);
cArray_size=sizeof(cArray);
cArray_num_elements=cArray_size/char_size;

printf("Size Of Data type 'char'							=%d bytes\n",char_size);
printf("Number Of Elements In 'char' Array 'cArray[]' 		=%d Elements\n",cArray_num_elements);
printf("Size Of Array 'cArray[]' (%d Elements * %d Bytes)	= %d Bytes\n\n",cArray_num_elements,char_size,cArray_size);

return(0);


}