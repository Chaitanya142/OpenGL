#include<stdio.h>

int main(void){
int num_month;

printf("\n\nEnter Number Of Month (1 to 12) : ");
scanf("%d",&num_month);

printf("\n\n");

switch(num_month)
{
case 1:
printf("Month Number %d Is JANUARY!!!\n\n",num_month);
break;
case 2:
printf("Month Number %d Is FEBRUARY!!!\n\n",num_month);
break;
case 3:
printf("Month Number %d Is MARCH!!!\n\n",num_month);
break;
case 4:
printf("Month Number %d Is APRIL!!!\n\n",num_month);
break;
case 5:
printf("Month Number %d Is MAY!!!\n\n",num_month);
break;
case 6:
printf("Month Number %d Is JUNE!!!\n\n",num_month);
break;
case 7:
printf("Month Number %d Is JULY!!!\n\n",num_month);
break;
case 8:
printf("Month Number %d Is AUGUST!!!\n\n",num_month);
break;
case 9:
printf("Month Number %d Is SEPTEMBER!!!\n\n",num_month);
break;
case 10:
printf("Month Number %d Is OCTOMBER!!!\n\n",num_month);
break;
case 11:
printf("Month Number %d Is NOVEMBER!!!\n\n",num_month);
break;
case 12:
printf("Month Number %d Is DECEMBER!!!\n\n",num_month);
break;
default:
printf("Invalid Month Number %d Entered !!! Please Try Again...\n\n",num_month);
break;
}

printf("Switch Case Block Complete !!!\n");

return(0);
}

