#pragma once
#include"Common.h"


class SceneDay : public IScene
{
private:
	GLuint vao_cloud;
	GLuint vbo_cloud_position;
	GLuint vbo_cloud_texture;

	GLuint vao_grass;
	GLuint vbo_grass;

	GLuint vao_flower;
	GLuint vbo_flower_position;
	GLuint vbo_flower_color;
	GLuint vao_flower_circle;
	GLuint vbo_flower_circle_position;
	GLuint vbo_flower_circle_color;

	std::vector<float>flowerPoints;
	std::vector<float>flowerColor;

	std::vector<float>circle_position;
	std::vector<float>circle_color;
	float radius = 0.03f;

	float prev_point_x = 0.0f;
	float prev_point_y = 0.0f;
	float prev_point_z = 0.0f;

	GLfloat translations[200];

	GLfloat current_time;

	float DayAlpha = 1.0f;
	float timer = 10.0f;

	GLuint vaoButterflySunrays;
	GLuint vboPositionButterflySunrays;
	GLuint vboColorButterflySunrays;

	GLuint pointsToConvertUniform;
	GLuint useGeometryShaderUniform;

	boolean gbUseGeometryShader = FALSE;
	int pointsToConvert = 0;
	int counter = 0;

	//float butterflyTranslate = 0.0f;

	void showCloud(void);
	void showFlowers(void);
	void showGrass(void);
	float MidPoint(float x1, float x2);
	void displayButterflySunRays(void);

public:

	// Inherited via IScene
	virtual int InitScene() override;

	virtual void DisplayScene() override;

	virtual void UpdateScene() override;

	virtual void UninitializeScene() override;

};
