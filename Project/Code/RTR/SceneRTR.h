#pragma once
#include"Common.h"


#define MAX_SCENE_WIDTH 2048
#define MAX_SCENE_HEIGHT 2048
#define SPHERE_COUNT 1


class SceneRTR : public IScene
{
private:

	/*FBO*/
	GLuint render_fbo;
	GLuint tex_scene;
	GLuint tex_brightpass;
	GLuint tex_depth;

	GLuint filter_fbo[2];
	GLuint tex_filter[2];
	GLuint tex_lut;
	GLuint ubo_material;
	GLuint ubo_transform;

	float bloom_factor = 0.5f;
	float bloom_thresh_max = 1.2f;
	float bloom_thresh_min = 0.1f;
	//float current_time = 0.0f;

	GLuint vao;
	GLuint vao_rtr_rectangle;
	GLuint vbo_rtr_rectangle_position;
	GLuint vbo_rtr_rectangle_normal;

	float rectangleNormal[372];
	GLfloat rectangleVertices[372];

	int firstDisplay = 1;

public:

	// Inherited via IScene
	virtual int InitScene() override;

	virtual void DisplayScene() override;

	virtual void UpdateScene() override;

	virtual void UninitializeScene() override;

};
