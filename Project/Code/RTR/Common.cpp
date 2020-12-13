#include"Common.h"

void MakeModelMatricesIdentity()
{
	modelMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
}
