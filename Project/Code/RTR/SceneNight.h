#pragma once
#include"Common.h"

class SceneNight : public IScene
{
private:
	enum
	{
		NUM_STARS = 3000
	};
	GLuint depth_fbo;
	GLuint depth_tex;
	GLuint color_tex;
	GLuint temp_tex;
	Sphere Moon;

	GLuint vao_rectangle_night;
	GLuint vbo_rectangle_position_night;
	GLuint vbo_rectangle_color_night;

	GLfloat focal_distance = 50.0f;
	GLfloat focal_depth = 50.0f;

	GLuint vao_static_star;
	GLuint vbo_static_star_position;
	GLuint vbo_static_star_color;
	GLuint vbo_static_star_texture;

	float camera_y;
	float camera_z = 0.004f;
	float moon_scale = 1.0f;
	unsigned int seed = 0x13371337;

	int firstDisplay = 1;

	inline float random_float();

public:
	// Inherited via IScene
	virtual int InitScene() override;

	virtual void DisplayScene() override;

	virtual void UpdateScene() override;

	virtual void UninitializeScene() override;

};
