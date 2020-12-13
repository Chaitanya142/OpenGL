#pragma once
#include"Common.h"

class SceneRain : public IScene
{
private:
	int amntCount = 0;
	int gAmnt = 200;
	mat4* modelMatrices = new mat4[gAmnt];
	unsigned int buffer;
	float delT = 0.0f;
	unsigned int amount = 200;

	GLuint vao_backgroundRain;
	GLuint vbo_backgroundRain_position;
	GLuint vbo_backgroundRain_texture;

	bool InitSceneFlag = true;

	void GenerateModelMatrices(unsigned int amount);

public:


	// Inherited via IScene
	virtual int InitScene() override;

	virtual void DisplayScene() override;

	virtual void UpdateScene() override;

	virtual void UninitializeScene() override;

};
