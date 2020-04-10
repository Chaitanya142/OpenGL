#pragma once

#define SUCCESS 0

struct vec_int* create_vec_int(void);
int push_back_vec_int(struct vec_int*,int);
void show_vec_int(struct vec_int*);
int destroy_vec_int(struct vec_int*);

/*Vector Structure*/
struct vec_int
{
	int *p;
	unsigned int size;
};
