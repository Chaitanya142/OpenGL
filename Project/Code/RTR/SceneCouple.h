#pragma once
#include"Common.h"


class SceneCouple : public IScene
{
private:

	Sphere sphereBody;
	Sphere sphereHead;
	float boyTranslateX = -1.0f;
	float girlTranslateX = 1.0f;
	GLuint vaoBackground1;
	GLuint vaoBackground2;

	GLuint vboPositionBackground1;
	GLuint vboColorBackground1;

	GLuint vboPositionBackground2;
	GLuint vboColorBackgrond2;
	GLuint vaoManHead;

	GLuint vboPositionManHead;
	GLuint vboNormalsManHead;

	GLuint vaoManBody;

	GLuint vboPositionManBody;
	GLuint vboNormalsManBody;

	GLuint vaoManLeg;

	GLuint vboPositionManLeg;
	GLuint vboColorManLeg;

	GLuint vaoWomanLeg;
	GLuint vboPositionWomanLeg;
	GLuint vboColorWomanLeg;

	GLuint vboElementsSphere;

	GLuint vaoBench;
	GLuint vaoBenchLeg;

	GLuint vboPositionBench;
	GLuint vboColorBench;

	GLuint vboPositionBenchLeg;
	GLuint vboColorBenchLeg;

	GLuint vao_lamp;
	GLuint vbo_lamp_position;
	GLuint vbo_lamp_color;

	std::vector<float>circle_body_position;
	std::vector<float>circle_body_color;
	std::vector<float>lamp_color;

	GLint circlePoints = 5000;
	float radius_body = 1.5f;

	float body_prev_point_x = 0.0f;
	float body_prev_point_y = 0.0f;
	float body_prev_point_z = 0.0f;

	GLuint vao_circle;
	GLuint vbo_circle_position;
	GLuint vbo_circle_color;

	float couple_camera_z = 0.0f;
	float couple_camera_y = 1.5f;

	bool first = false;
	bool translateDone = false;
	float Alpha_value = 0.0f;
	float heart_translate_y;
	float heart_translate_x;
	float couple_alpha_value = 1.0f;
	float scale_value;
public:


	// Inherited via IScene
	virtual int InitScene() override;

	virtual void DisplayScene() override;

	virtual void UpdateScene() override;

	virtual void UninitializeScene() override;

};
