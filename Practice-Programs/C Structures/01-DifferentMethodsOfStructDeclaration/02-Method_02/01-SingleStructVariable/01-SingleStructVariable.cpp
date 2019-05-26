#include <stdio.h>


struct MyData
{
	int i;
	float f;
	double d;
};

struct MyData data; 

int main(void)
{

	int i_size;
	int f_size;
	int d_size;
	int struct_MyData_size;


	data.i = 30;
	data.f = 11.45f;
	data.d = 1.2995;


	printf("\n\n");
	printf("DATA MEMBERS OF 'struct MyData' ARE : \n\n");
	printf("i = %d\n", data.i);
	printf("f = %f\n", data.f);
	printf("d = %lf\n", data.d);


	i_size = sizeof(data.i);
	f_size = sizeof(data.f);
	d_size = sizeof(data.d);

	printf("\n\n");
	printf("SIZES (in bytes) OF DATA MEMBERS OF 'struct MyData' ARE : \n\n");
	printf("Size of 'i' = %d bytes\n", i_size);
	printf("Size of 'f' = %d bytes\n", f_size);
	printf("Size of 'd' = %d bytes\n", d_size);

	struct_MyData_size = sizeof(struct MyData); //can also give struct name -> sizeof(MyData)

	printf("\n\n");
	printf("Size of 'struct MyData' : %d bytes\n\n", struct_MyData_size);

	return(0);
}
