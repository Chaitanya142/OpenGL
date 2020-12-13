#pragma once

#include"Common.h"


class SceneLeaf : public IScene
{
private:
	float updateFactor = 0.04f;

	Particle ParticlesContainer[MAXPARTICLES];
	GLfloat* g_particule_cameraPos_size_data;

	GLuint particles_vertex_buffer;
	GLuint particles_pos_size_buffer;
	int ParticlesCount = 0;
	int LastUsedParticle = 0;

	int FindUnusedParticle(void);
	void SortParticles(void);
	float moveParticle(float particleCurrentXPos);

public:

	float leafAlpha = 1.0f;

	// Inherited via IScene
	virtual int InitScene() override;

	virtual void DisplayScene() override;

	virtual void UpdateScene() override;

	virtual void UninitializeScene() override;

};
