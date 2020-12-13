#pragma once

#include"Common.h"

class SceneStudyRoom : public IScene
{
private:

	GLuint vao_StudyRoomWall;
	GLuint vbo_StudyRoomWall_position;
	GLuint vbo_StudyRoomWall_texture;

public:


	// Inherited via IScene
	virtual int InitScene() override;

	virtual void DisplayScene() override;

	virtual void UpdateScene() override;

	virtual void UninitializeScene() override;

};

