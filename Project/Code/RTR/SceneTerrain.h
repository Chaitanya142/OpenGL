#pragma once
#include"Common.h"


class SceneTerrain : public IScene
{
private:
	Sphere sphereSun;

	GLuint vaoTerrain;
	GLuint vaoButterflySunrays;
	GLuint vboPositionButterflySunrays;
	GLuint vboColorButterflySunrays;

	GLuint pointsToConvertUniform;
	GLuint useGeometryShaderUniform;

	float depthValue = 6.0f;

	boolean gbUseGeometryShader = TRUE;
	int pointsToConvert = 0;
	int counterForRays = 0;

	GLfloat sunYPosition = 5.6f;
	GLfloat terrainAlpha = 1.0f;
	GLfloat sunRaysAlpha = 0.0f;
	bool fadeInTerrainScene = true;

	GLfloat clearColorR = 1.0f;
	GLfloat clearColorG = 0.8f;
	GLfloat clearColorB = 0.0f;

	void displayTerrain(void);
	void displayButterflySunRays(void);
	void displaySun(void);

public:

	// Inherited via IScene
	virtual int InitScene() override;

	virtual void DisplayScene() override;

	virtual void UpdateScene() override;

	virtual void UninitializeScene() override;

};
