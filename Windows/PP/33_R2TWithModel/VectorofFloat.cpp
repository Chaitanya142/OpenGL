#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"VectorofFloat.h"

//int main(void)
//{
//	struct vec_float *p_vec_float = NULL;
//	p_vec_float = create_vec_float();
//	if(p_vec_float)
//		printf("creation success\n");
//	for(int i = 0; i < 10; i++)
//	{
//		push_back_vec_float(p_vec_float, (i+1) * 10 / 7);
//	}	
//	show_vec_float(p_vec_float);
//	destroy_vec_float(p_vec_float);
//	p_vec_float = NULL;
//	return(0);
//}

struct vec_float* create_vec_float(void)
{
	struct vec_float* p_new = NULL;
	p_new = (struct vec_float*)malloc(sizeof(struct vec_float));
	if(p_new == NULL)
	{
		puts("Can not allocate memory!\n");
		exit(0);
	}
	memset(p_new, 0, sizeof(struct vec_float));
	return(p_new);
}

int push_back_vec_float(struct vec_float* p_vec, float new_data)
{
	p_vec->p = (float*)realloc(p_vec->p, (p_vec->size + 1) * sizeof(float));
	if(p_vec->p == NULL)
	{
		puts("can not allocate memory!\n");
		exit(0);
	}
	p_vec->size = p_vec->size + 1;
	p_vec->p[p_vec->size - 1] = new_data;
	return(SUCCESS);
}

void show_vec_float(struct vec_float* p_vec)
{
	for(int i = 0; i < p_vec->size; i++)
	{
		printf("p[%d]:%f\n", i, p_vec->p[i]);
	}
}

int destroy_vec_float(struct vec_float* p_vec)
{
	free(p_vec->p);
	free(p_vec);
	p_vec = NULL;
	return(SUCCESS);
}
	