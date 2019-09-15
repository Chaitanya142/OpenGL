
#ifdef _WIN32
#include <windows.h>    // include windows.h to avoid thousands of compile errors even though this class is not depending on Windows
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include<GL\glew.h>
#include<gl\GL.h>
#endif

#include <iostream>
#include <iomanip>
#include <cmath>
#include "MatrixStack.h"

///////////////////////////////////////////////////////////////////////////////
// ctor
///////////////////////////////////////////////////////////////////////////////
MatrixStack::MatrixStack()
{
	top = NULL;
}

MatrixStack::~MatrixStack()
{
	destroy();
}

// to empty and destroy the MatrixStack
void MatrixStack::destroy()
{
	struct node *temp;
	temp = top;
	while (temp!= NULL)
	{
		pop();
		temp = top;
	}
}

// to insert elements in MatrixStack
void MatrixStack::push(mat4 val)
{
	struct node *temp;
	temp = (struct node*)malloc(sizeof(struct node));
	temp->data = val;
	temp->link = top;
	top = temp;
}

// to delete elements from MatrixStack
mat4 MatrixStack::pop()
{
	struct node *temp;
	mat4 val;
	val.identity();
	if (top == NULL)
	{

	}
	else
	{
		temp = top;
		top = top->link;
		val = temp->data;
		free(temp);
	}
	return val;
}
