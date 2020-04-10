#pragma once

#define SUCCESS 0

struct vec_float* create_vec_float(void);
int push_back_vec_float(struct vec_float*, float);
void show_vec_float(struct vec_float*);
int destroy_vec_float(struct vec_float*);

/*Vector Structure*/
struct vec_float
{
	float *p;
	int size;
};
