#include <stdio.h>

int main(void){
void PrintBinaryFormOfNumber(unsigned int);

unsigned int a,b,result,left_shift_A,left_shift_B;

printf("\n\nEnter An Integer =");
scanf("%u",&a);

printf("\n\nEnter Another Integer =");
scanf("%u",&b);

printf("\n\nBy How Many Bits Do You Want To Shift A=%d To The Left?",a);
scanf("%u",&left_shift_A);


printf("\n\nBy How Many Bits Do You Want To Shift B=%d To The Left?",b);
scanf("%u",&left_shift_B);

result=a<<left_shift_A;
printf("\n\n\n\nBitwise LEFT-SHIFT By %d Bits Of \nA=%d (Decimal), %o (Octal), %X (Hexadecimal) \nGives The Result =%d (Decimal), %o (Octal), %X (Hexadecimal).\n\n",left_shift_A,a,a,a,result,result,result);

PrintBinaryFormOfNumber(a);
PrintBinaryFormOfNumber(result);

result=b<<left_shift_B;
printf("\n\n\n\nBitwise LEFT-SHIFT By %d Bits Of \nB=%d (Decimal), %o (Octal), %X (Hexadecimal) \nGives The Result =%d (Decimal), %o (Octal), %X (Hexadecimal).\n\n",left_shift_B,b,b,b,result,result,result);

PrintBinaryFormOfNumber(b);
PrintBinaryFormOfNumber(result);

return(0);
}


void PrintBinaryFormOfNumber(unsigned int decimal_number){

unsigned int quotient,remainder,num,binary_array[8],i;

for(i=0;i<8;i++)
binary_array[i]=0;

printf("The Binary Form Of The Decimal Integer %d Is\t=\t",decimal_number);
num=decimal_number;
i=7;

while(num!=0){
quotient=num/2;
remainder=num%2;
binary_array[i]=remainder;
num=quotient;
i--;
}
for(i=0;i<8;i++)
printf("%u",binary_array[i]);

printf("\n\n");

}