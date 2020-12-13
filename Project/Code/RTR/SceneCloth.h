#include"Common.h"

class SceneCloth : public IScene
{
private:
	GLuint vao_cloth;
	GLuint vbo_cloth_element;
	GLuint vbo_cloth_texture;
	GLuint vbo_cloth_normal;
	GLuint ssbo_cloth_position[2];
	GLuint ssbo_cloth_velocity[2];


	int readBuf = 0;
	int nParticles_y = 40;
	int nParticles_x = 40;
	std::vector<float> initpos;
	std::vector<unsigned int> el;
	float clothSize_x = 5.0f;
	float clothSize_y = 4.0f;

	int firstDisplay = 1;
public:
	// Inherited via IScene
	virtual int InitScene() override;
	virtual void DisplayScene() override;
	virtual void UpdateScene() override;
	virtual void UninitializeScene() override;
};