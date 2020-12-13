#pragma once

#include"Common.h"

class SceneCollage : public IScene
{
private:
	GLuint vaoCollage;
	GLuint vboPositionCollage;
	GLuint vboTextureCollage;

	GLfloat collageRectX = 0.0f;
	GLfloat collageRectY = 0.0f;

	bool scaleIn = true;

	int firstDisplay = 1;
public:
	// Inherited via IScene
	virtual int InitScene() override;
	virtual void DisplayScene() override;
	virtual void UpdateScene() override;
	virtual void UninitializeScene() override;
};