#include "Uninitialize.h"

void DeleteVertexArray(GLuint *obj) {
	if (*obj) {
		glDeleteVertexArrays(1, obj);
		*obj = 0;
	}
}

void DeleteVertexBuffer(GLuint *obj) {
	if (*obj) {
		glDeleteBuffers(1, obj);
		*obj = 0;
	}
}

void DeleteShaderProgram(GLuint *gShaderProgramObject) {
	if (*gShaderProgramObject) {
		GLsizei shaderCount;
		GLsizei shaderNumber;
		glUseProgram(*gShaderProgramObject);

		glGetProgramiv(*gShaderProgramObject,
			GL_ATTACHED_SHADERS,
			&shaderCount);

		GLuint * pShaders = (GLuint *)malloc(shaderCount * sizeof(GLuint));

		if (pShaders) {
			glGetAttachedShaders(*gShaderProgramObject,
				shaderCount,
				&shaderCount,
				pShaders);

			for (shaderNumber = 0; shaderNumber < shaderCount; shaderNumber++) {
				glDetachShader(*gShaderProgramObject, pShaders[shaderNumber]);
				glDeleteShader(pShaders[shaderNumber]);
				pShaders[shaderNumber] = 0;
			}
			free(pShaders);
		}
		glDeleteProgram(*gShaderProgramObject);
		*gShaderProgramObject = 0;
		//glUseProgram(0);
	}
}
