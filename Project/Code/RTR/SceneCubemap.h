#pragma once

#include"Common.h"


class SceneCubemap : public IScene
{
private:
	unsigned int skyboxVAOCubeMap, skyboxVBOCubeMap;
	unsigned int step = 1;
	float movementVar = 0.0f;
	int firstDisplay = 1;
	
	GLfloat cubeMapAlpha = 0.0f;
	bool fadeInCubeMap = true;
public:
	// Inherited via IScene
	virtual int InitScene() override;

	virtual void DisplayScene() override;

	virtual void UpdateScene() override;

	virtual void UninitializeScene() override;

};
