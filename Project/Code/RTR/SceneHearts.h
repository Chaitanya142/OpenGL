#include"Common.h"


class SceneHearts : public IScene
{
private:
	float updateFactor = 0.04f;

	GLfloat* g_particule_cameraPos_size_dataHearts;
	Particle ParticlesContainerHeart[MAXPARTICLES];
	GLuint hearts_vertex_buffer;
	GLuint hearts_pos_size_buffer;
	int ParticlesCountHearts = 0;
	int LastUsedParticleHeart = 0;
	GLfloat fAngleForHeart = 0.0f;

	int FindUnusedParticleHearts(void);
	void SortParticlesHearts(void);
	float moveParticleHeart(float particleCurrentXPos);

	bool first = false;
public:

	float heartAlpha = 1.0f;

	// Inherited via IScene
	virtual int InitScene() override;

	virtual void DisplayScene() override;

	virtual void UpdateScene() override;

	virtual void UninitializeScene() override;

};
