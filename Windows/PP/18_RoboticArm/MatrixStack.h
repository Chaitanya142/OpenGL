#pragma once
#ifndef MATRIX_STACK_H
#define MATRIX_STACK_H

#include "vmath.h"
using namespace vmath;

class MatrixStack
{
public:
	// ctor/dtor
	MatrixStack();
	~MatrixStack();

	void push(mat4 data);
	mat4 pop();
protected:

private:
	struct node
	{
		mat4 data = mat4::identity();
		struct node *link=NULL;
	}*top = NULL;

	void destroy();

};

#endif
