#pragma once
#include "Common.h"

#define WATERSIZE 200
#define DAMP 20

class SceneRipple : public IScene {
private:
	GLfloat waterAlpha = 1.0f;
	GLfloat waterColorR = 0.0f;
	GLfloat waterColorG = 1.0f;
	GLfloat waterColorB = 1.0f;

	float water[3][WATERSIZE][WATERSIZE];

	float spin_x, spin_y, spin_z; // x-y & zoom
	int old_x, old_y = 50;
	int move_z;
	int t = 0, f = 1;

	int num = 0;
	int delay = 70;

	GLuint vaoWaterRipple;
	GLuint vboWaterRipplePosition;
	GLuint vboWaterRippleColor;

	GLfloat dampingFactor = 0.9f;

	void calcwater();
public:
	// Inherited via IScene
	virtual int InitScene() override;
	virtual void DisplayScene() override;
	virtual void UpdateScene() override;
	virtual void UninitializeScene() override;
};