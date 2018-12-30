#include<stdio.h>

int main(void){

float f,f_num=1.7f;

printf("\n\nPrinting Numbers %f to %f : \n\n",f_num,(f_num*10.f));

for(f=f_num;f<=(f_num*10.0f);f=f+f_num)
{
printf("\t%f\n",f);
}

printf("\n\n");
return(0);

}