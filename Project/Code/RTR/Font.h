#pragma once


#include"Common.h"

class Font
{
private:
	FT_Face face;
	FT_GlyphSlot g;


	GLuint vaoFont;
	GLuint vboFont;
	GLfloat mixColor = 0.0f;

	std::map<GLchar, Character>Charaters;
	void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, vmath::vec3 color);

	//Shader objShader_Font;
	Shader *objShader_Font;

	vector<Shader> shVec;

public:
	int InitFont();
	void DisplayText(string text, float startX, float startY, vec3 color);
};
