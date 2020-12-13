#include"SceneHearts.h"



int SceneHearts::InitScene()
{

	const GLchar *vertexShaderSourceCodeHearts = {
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

	const GLchar *fragmentShaderSourceCodeHearts =
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

	Shader objShader_Hearts(true, vertexShaderSourceCodeHearts, fragmentShaderSourceCodeHearts);

	shaders.push_back(objShader_Hearts);

	g_particule_cameraPos_size_dataHearts = new GLfloat[MAXPARTICLES * 4];

	for (int i = 0; i < MAXPARTICLES; i++) {
		ParticlesContainerHeart[i].life = -1.0f;
	}

	static const GLfloat particle_vertices_hearts[] =
	{
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f,
		0.5f,  0.5f, 0.0f,
	};

	glGenBuffers(1, &hearts_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, hearts_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_vertices_hearts), particle_vertices_hearts, GL_STATIC_DRAW);

	// sizes of the particles
	glGenBuffers(1, &hearts_pos_size_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, hearts_pos_size_buffer);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, MAXPARTICLES * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

	return 1;
}

void SceneHearts::DisplayScene()
{

	shaders[Shader_Hearts].use();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	projection = perspective(camera.Zoom, (float)gWidth / (float)gHeight, 0.1f, 100.0f);//radians
	view = camera.GetViewMatrix();
	shaders[Shader_Hearts].setMat4("projection", projection);
	shaders[Shader_Hearts].setMat4("view", view);
	glEnable(GL_POINT_SPRITE);
	
	MakeModelMatricesIdentity();
	//Transformation
	
	translationMatrix = translate(3.0f, 3.0f, -3.0f);
	rotationMatrix = rotate(0.5f, 1.0f, 0.0f, 0.0f);
	
	modelMatrix = translationMatrix * rotationMatrix;
	modelMatrix = modelMatrix * scaleMatrix;
	
	shaders[Shader_Hearts].setMat4("model", modelMatrix);
	shaders[Shader_Hearts].setFloat("alpha", heartAlpha);
	// particle vertices
	glEnableVertexAttribArray(shaders[Shader_Hearts].AMC_ATTRIBUTE_VERTEX);
	glBindBuffer(GL_ARRAY_BUFFER, hearts_vertex_buffer);
	glVertexAttribPointer(
		shaders[Shader_Hearts].AMC_ATTRIBUTE_VERTEX,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);
	
	glBindBuffer(GL_ARRAY_BUFFER, hearts_pos_size_buffer);
	glBufferData(GL_ARRAY_BUFFER, MAXPARTICLES * 4 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCountHearts * sizeof(GLfloat) * 4, g_particule_cameraPos_size_dataHearts);
	
	// cameraPoss of particle centers
	glEnableVertexAttribArray(shaders[Shader_Hearts].AMC_ATTRIBUTE_POSITION);
	
	glBindBuffer(GL_ARRAY_BUFFER, hearts_pos_size_buffer);
	glVertexAttribPointer(
		shaders[Shader_Hearts].AMC_ATTRIBUTE_POSITION,
		4,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);
	
	//ABU : Active - Bind - Uniform
	glActiveTexture(GL_TEXTURE0);  //Active -> texture units 
	glBindTexture(GL_TEXTURE_2D, textures[Texture_Heart]); //Bind
	glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
	glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);

	shaders[Shader_Hearts].setInt("u_sampler", 0);

	glVertexAttribDivisor(shaders[Shader_Hearts].AMC_ATTRIBUTE_VERTEX, 0); // particle vertices
	glVertexAttribDivisor(shaders[Shader_Hearts].AMC_ATTRIBUTE_POSITION, 1); // particle Pos
	
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, ParticlesCountHearts);
	
	glDisableVertexAttribArray(shaders[Shader_Hearts].AMC_ATTRIBUTE_VERTEX);
	glDisableVertexAttribArray(shaders[Shader_Hearts].AMC_ATTRIBUTE_POSITION);
	
	
	shaders[Shader_Hearts].setVec3("u_camera_cameraRight", camera.Right[0], camera.Right[1], camera.Right[2]);
	shaders[Shader_Hearts].setVec3("u_camera_up", camera.Up[0], camera.Up[1], camera.Up[2]);
	
	
	glDisable(GL_POINT_SPRITE);
	glDisable(GL_BLEND);
}

void SceneHearts::UpdateScene()
{

	if (first == false)
	{
		/*Reset camera*/
		camera.ResetCamera();
		first = true;
	}

	if (heartAlpha > 0.0)
	{
		heartAlpha = heartAlpha - 0.0015f;
	}
	else
	{
		heartAlpha = 0.0f;
	}
	
	int newparticles = (int)(updateFactor * 50.0);
	if (newparticles > (int)(0.02f * 10000.0))
		newparticles = (int)(0.016f * 10000.0);

	for (int i = 0; i < newparticles; i++)
	{
		int particleIndex = FindUnusedParticleHearts();
		ParticlesContainerHeart[particleIndex].life = 10.0f; // This particle will live 5 seconds.
		ParticlesContainerHeart[particleIndex].pos = vec3(0.0f, 15.0f, -10.0f);

		float spread = 4.5f;
		vec3 maindir = vec3(0.0f, 3.0f, 0.0f);

		vec3 randomdir =
			vec3(
			(rand() % 2000 - 1000.0f) / 1000.0f,
				(rand() % 2000 - 2000.0f) / 1000.0f,
				(rand() % 5000 - 2000.0f) / 1000.0f
			);

		ParticlesContainerHeart[particleIndex].speed = (maindir + randomdir * spread); // + sin(angle);

		ParticlesContainerHeart[particleIndex].size = (rand() % 1000) / 1000.0f + 1.3f;
	}

	// Simulate all particles
	ParticlesCountHearts = 0;

	for (int i = 0; i < MAXPARTICLES; i++)
	{

		Particle& p = ParticlesContainerHeart[i];

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

				float pos = p.pos[0] + moveParticleHeart(p.pos[0]);

				fAngleForHeart = fAngleForHeart + 0.0001f;
				if (fAngleForHeart < 360.0f)
				{
					fAngleForHeart = fAngleForHeart + 0.0001f;
				}

				p.pos[2] = p.pos[2] + sinf(fAngleForHeart); // for love

				// Fill the GPU buffer
				g_particule_cameraPos_size_dataHearts[4 * ParticlesCountHearts + 0] = p.pos[0];
				g_particule_cameraPos_size_dataHearts[4 * ParticlesCountHearts + 1] = p.pos[1];
				g_particule_cameraPos_size_dataHearts[4 * ParticlesCountHearts + 2] = p.pos[2];

				g_particule_cameraPos_size_dataHearts[4 * ParticlesCountHearts + 3] = p.size;
			}
			// Particles that just died will be put at the end of the buffer in SortParticles();
			//p.cameradistance = -3.0f;
			ParticlesCountHearts++;
		}
	}

	SortParticlesHearts();
}

// Finds a Particle in ParticlesContainer which isn't used yet.
// (i.e. life < 0);
int SceneHearts::FindUnusedParticleHearts(void)
{
	for (int i = LastUsedParticleHeart; i < MAXPARTICLES; i++) {
		if (ParticlesContainerHeart[i].life < 0) {
			LastUsedParticleHeart = i;
			return i;
		}
	}

	for (int i = 0; i < LastUsedParticleHeart; i++) {
		if (ParticlesContainerHeart[i].life < 0) {
			LastUsedParticleHeart = i;
			return i;
		}
	}

	return 0; // All particles are taken, override the first one
}


void SceneHearts::SortParticlesHearts(void)
{
	sort(&ParticlesContainerHeart[0], &ParticlesContainerHeart[MAXPARTICLES]);
}

// move particle
float SceneHearts::moveParticleHeart(float particleCurrentXPos)
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


void SceneHearts::UninitializeScene()
{
	// Delete Vertex buffers : vbo's 
	DeleteVertexBuffer(&hearts_pos_size_buffer);
	DeleteVertexBuffer(&hearts_vertex_buffer);
}
