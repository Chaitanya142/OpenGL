#include"SceneLeaf.h"

int SceneLeaf::InitScene()
{

	const GLchar *vertexShaderSourceCodeLeafs = {
		"#version 430 core" \
		"\n" \

		"layout (location = 0) in vec4 vPosition;" \
		"layout (location = 4) in vec3 vVertex;" \

		"uniform vec3 u_camera_cameraRight;" \
		"uniform vec3 u_camera_up;" \
		"uniform mat4 model;" \
		"uniform mat4 view;" \
		"uniform mat4 projection;" \
		"out vec2 texcord;" \

		"void main()" \
		"{" \
		"float particleSize = vPosition.w;" \

		"vec3 vertexPosition =" \
		"vPosition.xyz" \
		"+ u_camera_cameraRight * vVertex.x * particleSize" \
		"+ u_camera_up * vVertex.y * particleSize;" \

		"gl_Position = projection * view * model * vec4(vertexPosition, 1.0f);" \
		"texcord = vVertex.xy + vec2(0.5, 0.5);" \
		"}"
	};

	const GLchar *fragmentShaderSourceCodeLeafs =
	{
		"#version 430 core" \
		"\n" \

		"in vec2 texcord;"\
		"uniform sampler2D u_sampler;"\
		"vec4 color;"\
		"uniform float alpha;"\
		"out vec4 FragColor;" \

		"void main() {"\
		"color = texture(u_sampler, texcord);"\
		"if(color.xyz ==  vec3(0.0,0.0,0.0))" \
		"{" \
		"discard;" \
		"}" \
		"FragColor = color;" \
		"FragColor.w = alpha;"\
		"}"
	};

	Shader objShader_Leaf(true, vertexShaderSourceCodeLeafs, fragmentShaderSourceCodeLeafs);

	shaders.push_back(objShader_Leaf);

	g_particule_cameraPos_size_data = new GLfloat[MAXPARTICLES * 4];

	for (int i = 0; i < MAXPARTICLES; i++) {
		ParticlesContainer[i].life = -1.0f;
	}

	static const GLfloat particle_vertices[] =
	{
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f,
		0.5f,  0.5f, 0.0f,
	};

	glGenBuffers(1, &particles_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particles_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_vertices), particle_vertices, GL_STATIC_DRAW);

	// sizes of the particles
	glGenBuffers(1, &particles_pos_size_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particles_pos_size_buffer);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, MAXPARTICLES * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

	return 1;
}

void SceneLeaf::DisplayScene()
{
	 shaders[Shader_Leaf].use();
	glEnable(GL_BLEND);
	//glBlendFunc(GL_ONE, GL_ONE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	projection = perspective(camera.Zoom, (float)gWidth / (float)gHeight, 0.1f, 100.0f);//radians
	view = camera.GetViewMatrix();
	 shaders[Shader_Leaf].setMat4("projection", projection);
	shaders[Shader_Leaf].setMat4("view", view);

	MakeModelMatricesIdentity();
	//Transformation

	translationMatrix = translate(3.0f, 3.0f, -3.0f);
	rotationMatrix = rotate(0.5f, 1.0f, 0.0f, 0.0f);

	modelMatrix = translationMatrix * rotationMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	shaders[Shader_Leaf].setMat4("model", modelMatrix);
	shaders[Shader_Leaf].setFloat("alpha", leafAlpha);

	// particle vertices
	glEnableVertexAttribArray(shaders[Shader_Leaf].AMC_ATTRIBUTE_VERTEX);
	glBindBuffer(GL_ARRAY_BUFFER, particles_vertex_buffer);
	glVertexAttribPointer(
		shaders[Shader_Leaf].AMC_ATTRIBUTE_VERTEX,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glBindBuffer(GL_ARRAY_BUFFER, particles_pos_size_buffer);
	glBufferData(GL_ARRAY_BUFFER, MAXPARTICLES * 4 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLfloat) * 4, g_particule_cameraPos_size_data);

	// cameraPoss of particle centers
	glEnableVertexAttribArray(shaders[Shader_Leaf].AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, particles_pos_size_buffer);
	glVertexAttribPointer(
		shaders[Shader_Leaf].AMC_ATTRIBUTE_POSITION,
		4,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	//ABU : Active - Bind - Uniform
	glActiveTexture(GL_TEXTURE0);  //Active -> texture units 
	glBindTexture(GL_TEXTURE_2D, textures[Texture_Leaf]); //Bind
	glEnable(GL_POINT_SPRITE);
	glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
	glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);

	glVertexAttribDivisor(shaders[Shader_Leaf].AMC_ATTRIBUTE_VERTEX, 0); // particle vertices
	glVertexAttribDivisor(shaders[Shader_Leaf].AMC_ATTRIBUTE_POSITION, 1); // particle Pos

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, ParticlesCount);

	glDisableVertexAttribArray(shaders[Shader_Leaf].AMC_ATTRIBUTE_VERTEX);
	glDisableVertexAttribArray(shaders[Shader_Leaf].AMC_ATTRIBUTE_POSITION);


	shaders[Shader_Leaf].setVec3("u_camera_cameraRight", camera.Right[0], camera.Right[1], camera.Right[2]);
	shaders[Shader_Leaf].setVec3("u_camera_up", camera.Up[0], camera.Up[1], camera.Up[2]);

	glDisable(GL_BLEND);
	glDisable(GL_POINT_SPRITE);

}

void SceneLeaf::UpdateScene()
{

	leafAlpha -= 0.0045f;
	if (leafAlpha <= 0.0f)
		leafAlpha = 0.0f;

	int newparticles = (int)(updateFactor * 50.0);
	if (newparticles > (int)(0.02f * 10000.0))
		newparticles = (int)(0.016f * 10000.0);

	for (int i = 0; i < newparticles; i++)
	{
		int particleIndex = FindUnusedParticle();
		ParticlesContainer[particleIndex].life = 10.0f; // This particle will live 5 seconds.
		ParticlesContainer[particleIndex].pos = vec3(0.0f, 15.0f, -10.0f);

		float spread = 4.5f;
		vec3 maindir = vec3(0.0f, 3.0f, 0.0f);

		vec3 randomdir =
			vec3(
			(rand() % 2000 - 1000.0f) / 1000.0f,
				(rand() % 2000 - 2000.0f) / 1000.0f,
				(rand() % 5000 - 2000.0f) / 1000.0f
			);

		ParticlesContainer[particleIndex].speed = (maindir + randomdir * spread); // + sin(angle);

		ParticlesContainer[particleIndex].size = (rand() % 1000) / 1000.0f + 1.3f;
	}

	// Simulate all particles
	ParticlesCount = 0;

	for (int i = 0; i < MAXPARTICLES; i++)
	{

		Particle& p = ParticlesContainer[i];

		if (p.life > 0.0f)
		{
			// Decrease life
			p.life -= (float)updateFactor;
			if (p.life > 0.0f)
			{
				float rotation = float(rand() * M_PI * 0.5f);
				vec3 CameraPosition = vec3(0.0f, 0.0f, 0.0f);
				p.speed += vec3(0.0f, -0.5f, 0.0f) * (float)updateFactor * 0.00001f;
				p.pos += p.speed * (float)updateFactor;

				float pos = p.pos[0] + moveParticle(p.pos[0]);// *sin(0.5f);

				//fAngle = fAngle + 0.0001f;
				//if (fAngle < 360.0f)
				{
					//fAngle = fAngle + 0.0001f;
				}

				//p.pos[0] = p.pos[0] + sinf(fAngle);
				//p.pos[0] = p.pos[0] + sinf(fAngle) - 0.0005f; // for leaves
				//p.pos[2] = p.pos[2] + sinf(fAngle); // for love

				// Fill the GPU buffer
				g_particule_cameraPos_size_data[4 * ParticlesCount + 0] = p.pos[0];
				g_particule_cameraPos_size_data[4 * ParticlesCount + 1] = p.pos[1];
				g_particule_cameraPos_size_data[4 * ParticlesCount + 2] = p.pos[2];

				g_particule_cameraPos_size_data[4 * ParticlesCount + 3] = p.size;
			}
			// Particles that just died will be put at the end of the buffer in SortParticles();
			//p.cameradistance = -3.0f;
			ParticlesCount++;
		}
	}

	SortParticles();
}

int SceneLeaf::FindUnusedParticle(void)
{

	for (int i = LastUsedParticle; i < MAXPARTICLES; i++) {
		if (ParticlesContainer[i].life < 0) {
			LastUsedParticle = i;
			return i;
		}
	}

	for (int i = 0; i < LastUsedParticle; i++) {
		if (ParticlesContainer[i].life < 0) {
			LastUsedParticle = i;
			return i;
		}
	}

	return 0; // All particles are taken, override the first one
}

void SceneLeaf::SortParticles(void)
{
	sort(&ParticlesContainer[0], &ParticlesContainer[MAXPARTICLES]);
}

// move particle
float SceneLeaf::moveParticle(float particleCurrentXPos)
{
	float particleXPos = -5.0f;
	float particleYPos = 3.0f;
	float particleYDelta = 0.055f;

	float temp = 0.0f;

	while (particleXPos < particleCurrentXPos) {
		temp = particleYPos;
		particleYPos = particleYPos + particleYDelta / 30.0f * (particleCurrentXPos + particleYPos + 0.0001f * 0.0001f * particleCurrentXPos * particleYPos);
		particleXPos = particleXPos + particleYDelta / 30.0f;
	}

	return particleYPos;
}


void SceneLeaf::UninitializeScene()
{
	// Delete Vertex buffers : vbo's 
	DeleteVertexBuffer(&particles_vertex_buffer);
	DeleteVertexBuffer(&particles_pos_size_buffer);
}
