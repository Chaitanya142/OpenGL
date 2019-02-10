#include <stdio.h>

int main(int argc,char* argv[],char*envp[])
{
void display_information(void);
void Function_Country(void);

display_information();
Function_Country();

return(0);

}


void display_information(void){
	void Function_My(void);
	void Function_Name(void);
	void Function_Is(void);
	void Function_FirstName(void);
	void Function_MiddleName(void);
	void Function_Surname(void);
	void Function_OfAMC(void);
	
	Function_My();
	Function_Name();
	Function_Is();
	Function_FirstName();
	Function_MiddleName();
	Function_Surname();
	Function_OfAMC();
	
}

void Function_My(void){
	printf("\n\nMy");
}

void Function_Name(void){
	printf("\n\nName");
}

void Function_Is(void){
	printf("\n\nIs");
}

void Function_FirstName(void){
	printf("\n\nChaitanya");
}

void Function_MiddleName(void){
	printf("\n\nSunildatta");	
}

void Function_Surname(void){
	printf("\n\nPrabhune");	
}

void Function_OfAMC(void){
	printf("\n\nOF ATROMEDICOMP");
}

void Function_Country(void){
	printf("\n\nI Live In India\n\n");
}

