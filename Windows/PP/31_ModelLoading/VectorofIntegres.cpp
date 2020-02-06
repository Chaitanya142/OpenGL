#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"VectorofIntegres.h"

//int main(void)
//{
//	struct vec_int *p_vec_int = NULL;
//	p_vec_int = create_vec_int();
//	if(p_vec_int)
//		printf("creation success\n");
//	for(int i = 0; i < 10; i++)
//	{
//		push_back_vec_int(p_vec_int, (i+1) * 10);
//	}	
//	show_vec_int(p_vec_int);
//	destroy_vec_int(p_vec_int);
//	p_vec_int = NULL;
//	return(0);
//}

struct vec_int* create_vec_int(void)
{
	struct vec_int* p_new = NULL;
	p_new = (struct vec_int*)malloc(sizeof(struct vec_int));
	if(p_new == NULL)
	{
		puts("Can not allocate memory!\n");
		exit(0);
	}
	memset(p_new, 0, sizeof(struct vec_int));
	return(p_new);
}

int push_back_vec_int(struct vec_int* p_vec, int new_data)
{
	p_vec->p = (int*)realloc(p_vec->p, (p_vec->size + 1) * sizeof(int));
	if(p_vec->p == NULL)
	{
		puts("can not allocate memory!\n");
		exit(0);
	}
	p_vec->size = p_vec->size + 1;
	p_vec->p[p_vec->size - 1] = new_data;
	return(SUCCESS);
}

void show_vec_int(struct vec_int* p_vec)
{
	for(int i = 0; i < p_vec->size; i++)
	{
		printf("p[%d]:%d\n", i, p_vec->p[i]);
	}
}

int destroy_vec_int(struct vec_int* p_vec)
{
	free(p_vec->p);
	free(p_vec);
	p_vec = NULL;
	return(SUCCESS);
}
	