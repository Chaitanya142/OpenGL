#pragma once
#include"Common.h"


class SceneEndCredits :public IScene
{
private:
	FT_Face face;
	FT_GlyphSlot g;

	float aditiy = -7.0f;
	float amitay = -7.3f;
	float chaitanyay = -7.6f;
	float gayatriy = -7.9f;
	float moviey = -8.2f;
	float poety = -8.5f;
	float singery = -8.8f;
	float directory = -9.1f;
	float copyrighty = -9.4f;



	float mixColorName = 1.0f;
	float mixColorName1 = 1.0f;
	float mixColorCredit = 1.0f;
	float mixColorCredit1 = 1.0f;
	GLfloat mixUniformName = 0.0f;
	GLfloat mixUniformName1 = 0.0f;
	GLfloat mixUniformCredit = 0.0f;
	GLfloat mixUniformCredit1 = 0.0f;

	GLuint vaoFont_EndCredit;
	GLuint vboFont_EndCredit;
	

	int flagName = 0;
	int flagCredit = 0;

	std::map<GLchar, Character>Charaters;
	void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, vmath::vec3 color);

	void printNames();
	void updateNames();


public:


	// Inherited via IScene
	virtual int InitScene() override;
	virtual void DisplayScene() override;
	virtual void UpdateScene() override;
	virtual void UninitializeScene() override;
};
