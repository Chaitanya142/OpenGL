#ifndef SHADER_H
#define SHADER_H

//#include <glad/glad.h>
//#include <glm/glm.hpp>
#include "vmath.h"
#include"GL\glew.h"

using namespace vmath;

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	enum {
		AMC_ATTRIBUTE_POSITION = 0,
		AMC_ATTRIBUTE_COLOR,
		AMC_ATTRIBUTE_NORMAL,
		AMC_ATTRIBUTE_TEXCOORD0,
		AMC_ATTRIBUTE_VERTEX
	};

	unsigned int ID;
	// constructor generates the shader on the fly
	// ------------------------------------------------------------------------
	//ToDo
	//Add remining shader support Create new file and leep original as it is
	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr)
	{
		// 1. retrieve the vertex/fragment source code from filePath
		std::string vertexCode;
		std::string fragmentCode;
		std::string geometryCode;

		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		std::ifstream gShaderFile;

		// ensure ifstream objects can throw exceptions:
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			// open files
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, fShaderStream;
			// read file's buffer contents into streams
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			// close file handlers
			vShaderFile.close();
			fShaderFile.close();
			// convert stream into string
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();

			// if geometry shader path is present, also load a geometry shader
			if (geometryPath != nullptr)
			{
				gShaderFile.open(geometryPath);
				std::stringstream gShaderStream;
				gShaderStream << gShaderFile.rdbuf();
				gShaderFile.close();
				geometryCode = gShaderStream.str();
			}
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}
		const char* vShaderCode = vertexCode.c_str();
		const char * fShaderCode = fragmentCode.c_str();
		// 2. compile shaders
		unsigned int vertex, fragment;
		// vertex shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		checkCompileErrors(vertex, "VERTEX");
		// fragment Shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		checkCompileErrors(fragment, "FRAGMENT");
		// if geometry shader is given, compile geometry shader
		unsigned int geometry;
		if (geometryPath != nullptr)
		{
			const char * gShaderCode = geometryCode.c_str();
			geometry = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometry, 1, &gShaderCode, NULL);
			glCompileShader(geometry);
			checkCompileErrors(geometry, "GEOMETRY");
		}
		// shader Program
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		if (geometryPath != nullptr)
			glAttachShader(ID, geometry);
		glLinkProgram(ID);
		checkCompileErrors(ID, "PROGRAM");
		// delete the shaders as they're linked into our program now and no longer necessery
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		if (geometryPath != nullptr)
			glDeleteShader(geometry);

	}

	Shader(bool passingSourceString, const char* vShaderCode = nullptr, const char* fShaderCode = nullptr, const char* gShaderCode = nullptr, const char* cShaderCode = nullptr,
		const char * tcShaderCode = nullptr, const char * teShaderCode = nullptr)
	{
		if (passingSourceString)
		{
			// 2. compile shaders
			unsigned int vertex, fragment;

			if (vShaderCode != nullptr)
			{
				// vertex shader
				vertex = glCreateShader(GL_VERTEX_SHADER);
				glShaderSource(vertex, 1, &vShaderCode, NULL);
				glCompileShader(vertex);
				checkCompileErrors(vertex, "VERTEX");
			}

			if (fShaderCode != nullptr) {
				// fragment Shader
				fragment = glCreateShader(GL_FRAGMENT_SHADER);
				glShaderSource(fragment, 1, &fShaderCode, NULL);
				glCompileShader(fragment);
				checkCompileErrors(fragment, "FRAGMENT");
			}

			//Geometry
			unsigned int geometry;
			if (gShaderCode != nullptr)
			{
				geometry = glCreateShader(GL_GEOMETRY_SHADER);
				glShaderSource(geometry, 1, &gShaderCode, NULL);
				glCompileShader(geometry);
				checkCompileErrors(geometry, "GEOMETRY");
			}

			unsigned int compute;
			if (cShaderCode != nullptr)
			{
				compute = glCreateShader(GL_COMPUTE_SHADER);
				glShaderSource(compute, 1, &cShaderCode, NULL);
				glCompileShader(compute);
				checkCompileErrors(compute, "COMPUTE");
			}

			unsigned int tesselationcontrol;
			if (tcShaderCode != nullptr)
			{
				tesselationcontrol = glCreateShader(GL_TESS_CONTROL_SHADER);
				glShaderSource(tesselationcontrol, 1, &tcShaderCode, NULL);
				glCompileShader(tesselationcontrol);
				checkCompileErrors(tesselationcontrol, "TESSELATIONCONTROL");
			}

			unsigned int tesselationevaluation;
			if (teShaderCode != nullptr)
			{
				tesselationevaluation = glCreateShader(GL_TESS_EVALUATION_SHADER);
				glShaderSource(tesselationevaluation, 1, &teShaderCode, NULL);
				glCompileShader(tesselationevaluation);
				checkCompileErrors(tesselationevaluation, "TESSELATIONEVALUATION");
			}

			// shader Program
			ID = glCreateProgram();

			if (vShaderCode != nullptr)
				glAttachShader(ID, vertex);

			if (fShaderCode != nullptr)
				glAttachShader(ID, fragment);

			if (gShaderCode != nullptr)
				glAttachShader(ID, geometry);

			if (cShaderCode != nullptr)
				glAttachShader(ID, compute);

			if (tcShaderCode != nullptr)
				glAttachShader(ID, tesselationcontrol);

			if (teShaderCode != nullptr)
				glAttachShader(ID, tesselationevaluation);

			if (vShaderCode != nullptr) {
				std::string vShaderCodeString(vShaderCode);
				//std::string fShaderCodeString(fShaderCode);

				if (vShaderCodeString.find("vPosition") != std::string::npos) {
					glBindAttribLocation(ID,
						AMC_ATTRIBUTE_POSITION,
						"vPosition");
				}
				if (vShaderCodeString.find("vColor") != std::string::npos) {
					glBindAttribLocation(ID,
						AMC_ATTRIBUTE_COLOR,
						"vColor");
				}
				if (vShaderCodeString.find("vNormal") != std::string::npos) {
					glBindAttribLocation(ID,
						AMC_ATTRIBUTE_NORMAL,
						"vNormal");
				}
				if (vShaderCodeString.find("vTexCoord") != std::string::npos) {
					glBindAttribLocation(ID,
						AMC_ATTRIBUTE_TEXCOORD0,
						"vTexCoord");
				}
				if (vShaderCodeString.find("vVertex") != std::string::npos) {
					glBindAttribLocation(ID,
						AMC_ATTRIBUTE_VERTEX,
						"vVertex");
				}
			}
			glLinkProgram(ID);
			checkCompileErrors(ID, "PROGRAM");
			// delete the shaders as they're linked into our program now and no longer necessery
			if (vShaderCode != nullptr)
				glDeleteShader(vertex);

			if (fShaderCode != nullptr)
				glDeleteShader(fragment);

			if (gShaderCode != nullptr)
				glDeleteShader(geometry);

			if (cShaderCode != nullptr)
				glDeleteShader(compute);

			if (tcShaderCode != nullptr)
				glDeleteShader(tesselationcontrol);

			if (teShaderCode != nullptr)
				glDeleteShader(tesselationevaluation);

		}

		//Giving error

	/*	if (tcShaderCode != nullptr)
			glDeleteShader(tesselationcontrol);
		}
		if (teshadercode != nullptr)
			glDeleteShader(tesselationevaluation);
		}*/


	}

	~Shader()
	{
		/* if (ID) {
		 GLsizei shaderCount;
		 GLsizei shaderNumber;
		 glUseProgram(ID);

		 glGetProgramiv(ID,
			 GL_ATTACHED_SHADERS,
			 &shaderCount);

		 GLuint * pShaders = (GLuint *)malloc(shaderCount * sizeof(GLuint));

		 if (pShaders) {
			 glGetAttachedShaders(ID,
				 shaderCount,
				 &shaderCount,
				 pShaders);

			 for (shaderNumber = 0; shaderNumber < shaderCount; shaderNumber++) {
				 glDetachShader(ID, pShaders[shaderNumber]);
				 glDeleteShader(pShaders[shaderNumber]);
				 pShaders[shaderNumber] = 0;
			 }
			 free(pShaders);
		 }
		 glDeleteProgram(ID);
		 ID = 0;
		 glUseProgram(0);
	 }*/
	}

	// activate the shader
	// ------------------------------------------------------------------------
	void use() const
	{
		glUseProgram(ID);
	}
	// utility uniform functions
	// ------------------------------------------------------------------------
	void setBool(const std::string &name, bool value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}
	// ------------------------------------------------------------------------
	void setInt(const std::string &name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	// ------------------------------------------------------------------------
	void setFloat(const std::string &name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}
	// ------------------------------------------------------------------------
	void setVec2(const std::string &name, const vec2 &value) const
	{
		glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec2(const std::string &name, float x, float y) const
	{
		glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
	}
	// ------------------------------------------------------------------------
	void setVec3(const std::string &name, const vec3 &value) const
	{
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec3(const std::string &name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
	}
	// ------------------------------------------------------------------------
	void setVec4(const std::string &name, const vec4 &value) const
	{
		glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec4(const std::string &name, float x, float y, float z, float w) const
	{
		glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
	}
	// ------------------------------------------------------------------------
	void setMat2(const std::string &name, const mat2 &mat) const
	{
		glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	// ------------------------------------------------------------------------
   /* void setMat3(const std::string &name, const mat3 &mat) const
	{
		glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}*/
	// ------------------------------------------------------------------------
	void setMat4(const std::string &name, const mat4 &mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

private:
	// utility function for checking shader compilation/linking errors.
	// ------------------------------------------------------------------------
	void checkCompileErrors(GLuint shader, std::string type)
	{
		GLint success;
		GLchar infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
	}
};
#endif