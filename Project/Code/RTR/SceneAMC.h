#pragma once
#include"Common.h"


class SceneAMC :public IScene 
{
private:
	FT_Face face;
	FT_GlyphSlot g;


	GLuint vaoFont;
	GLuint vboFont;
	GLfloat mixColor = 1.0f;

	std::map<GLchar, Character>Charaters;
	void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, vmath::vec3 color);

	void printAMC();
	void printAssignment();

	void updateAMC();
	void updateAssignment();

	int printAssignmentFlag = 0;
	int initAssignment = 1;

public:
	
	void changeToAssignment();

	// Inherited via IScene
	virtual int InitScene() override;
	virtual void DisplayScene() override;
	virtual void UpdateScene() override;
	virtual void UninitializeScene() override;
};