#pragma once
#include"Common.h"


class SceneComputer : public IScene
{
private:
	int SceneNo;

	FT_Face faceConstanb;
	FT_GlyphSlot gConstanb;
	std::map<GLchar, Character>CharatersConstanb;

	GLuint vaoFontConstanb;
	GLuint vboFontConstanb;

	GLuint vao_studytable;
	GLuint vbo_rectangle_position_studytable;
	GLuint vbo_rectangle_color_studytable;

	GLuint vao_rectangle1_computer;
	GLuint vao_rectangle2_computer;
	GLuint vao_rectangle3_computer;
	GLuint vao_rectangle4_computer;
	GLuint vao_rectangle5_computer;
	GLuint vao_rectangle6_computer;


	GLuint vbo_position_rectangle1_computer;
	GLuint vbo_position_rectangle2_computer;
	GLuint vbo_position_rectangle3_computer;
	GLuint vbo_position_rectangle4_computer;
	GLuint vbo_position_rectangle5_computer;
	GLuint vbo_position_rectangle6_computer;


	GLuint vbo_color_rectangle1_computer;
	GLuint vbo_color_rectangle2_computer;
	GLuint vbo_color_rectangle3_computer;
	GLuint vbo_color_rectangle4_computer;
	GLuint vbo_color_rectangle5_computer;
	GLuint vbo_color_rectangle6_computer;

	GLuint vao_smallSquare1_Computer;
	GLuint vao_smallSquare2_Computer;
	GLuint vao_smallSquare3_Computer;
	GLuint vao_smallSquare4_Computer;

	GLuint vbo_smallSquare1_position_computer;
	GLuint vbo_smallSquare2_position_computer;
	GLuint vbo_smallSquare3_position_computer;
	GLuint vbo_smallSquare4_position_computer;

	GLuint vbo_smallSquare1_color_computer;
	GLuint vbo_smallSquare2_color_computer;
	GLuint vbo_smallSquare3_color_computer;
	GLuint vbo_smallSqaure4_color_computer;

	GLuint vao_rectangle_wall;
	GLuint vbo_rectangle_position_wall;
	GLuint vbo_rectangle_color_wall;

	int counter = 0;
	int counter1 = 0;
	int counter2 = 0;
	int counter3 = 0;
	int counter4 = 0;
	int counter5 = 0;
	int counter6 = 0;
	int counter7 = 0;
	int counter8 = 0;
	int counter9 = 0;
	int counter10 = 0;
	int counter11 = 0;
	int counter12 = 0;
	int counter13 = 0;
	int counter14 = 0;
	int counter15 = 0;
	int counter16 = 0;
	int counter17 = 0;
	int counter18 = 0;
	int counter19 = 0;
	float counterTime = 0.0f;
	float counterTime1 = 0.0f;

	int stage = 0;

	float zMovement = 0.0004f;

	float translateFont = -97.0f;
	float translateScreen = -1.0f;

	void DrawFrontSequence();
	void DrawLastSequence();
	void RenderTextStart(std::string text, GLfloat x, GLfloat y, GLfloat scale, vmath::vec3 color, int ShaderId);

	int firstDisplay = 1;
	int PrevSceneNo = 0;

public:
	void DisplayModelAndChair();

	void SetSceneNo(int sceneNo);
	// Inherited via IScene
	virtual int InitScene() override;
	virtual void DisplayScene() override;
	virtual void UpdateScene() override;
	virtual void UninitializeScene() override;

};
