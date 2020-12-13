#pragma once
#include"Common.h"



class SceneStarField : public IScene
{
private:
	GLuint vao_StarFiled;
	GLuint vbo_StarFiled;
	GLuint vbo_texture_StarFiled;

	float currentTime;
	float Alpha = 1.0f;

	// Random number generator
	unsigned int seed = 0x13371337;

	inline float random_float();
	enum
	{
		NUM_STARS = 3000
	};
	struct star_t
	{
		vec3 position;
		vec3 color;
	};

	int firstDisplay = 1;

public:
	// Inherited via IScene
	virtual int InitScene() override;

	virtual void DisplayScene() override;

	virtual void UpdateScene() override;

	virtual void UninitializeScene() override;

};


