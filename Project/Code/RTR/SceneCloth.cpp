#include"SceneCloth.h"
#include <glm\gtc\matrix_transform.hpp>
#include <glm\glm.hpp>

int SceneCloth::InitScene()
{
	const GLchar* vertexShaderSourceCodeFlag =
	{
		"#version 430 core"\
		"\n"\
		"in vec4 vPosition;"\
		"in vec2 vTexCoord;"\
		"in vec3 vNormal;"\

		"out vec2 out_texCoord;"\
		"out vec3 Position;"\
		"out vec3 Normal;"\

		"uniform mat4 model;"\
		"uniform mat4 view;"\
		"uniform mat4 projection;"\

		"void main(void)"\
		"{"\
		"mat3 NormalMatrix = mat3(transpose(inverse(view * model)));"\
		"Normal = normalize(NormalMatrix * vNormal);"\
		"Position = vec3(view * model * vPosition);"\
		"out_texCoord = vTexCoord;"\
		"gl_Position = projection * view * model * vPosition;"\
		"}"
	};

	const GLchar* fragmentShaderSourceCodeFlag =
	{
		"#version 430 core"\
		"\n"\
		"in vec2 out_texCoord;"\
		"in vec3 Position;"\
		"in vec3 Normal;"\

		"uniform vec4 LightPosition;"\
		"uniform vec3 LightIntensity;"\

		"uniform vec3 Kd;"\
		"uniform vec3 Ka;"\
		"uniform vec3 Ks;"\
		"uniform float shininess;"\

		"uniform sampler2D u_sampler;"\
		"out vec4 FragColor;"\

		"uniform int keyPressed;"\

		"vec3 ads()"\
		"{"\
		"vec3 s = normalize(vec3(LightPosition) - Position);"\
		"vec3 v = normalize(vec3(-Position));"\
		"vec3 r = reflect(-s, Normal);"\

		"return(LightIntensity * (Ka + Kd * max(dot(s, Normal), 0.0) + Ks * pow( max( dot(r, v), 0.0), shininess)));"\
		"}"\
		"void main(void)"\
		"{"\
		"vec4 texColor = texture(u_sampler, out_texCoord);"\
		"if(keyPressed == 1)"\
		"{"\
		"FragColor =  vec4(ads(), 1.0) * texColor;"\
		"}"\
		"else"\
		"{"\
		"FragColor = texColor;"\
		"}"\
		"}"
	};

	Shader objShader_Flag(true, vertexShaderSourceCodeFlag, fragmentShaderSourceCodeFlag);

	shaders.push_back(objShader_Flag);



	//----------------------------------------------------------------------------------------------------------//



	const GLchar* computeShaderSourceCodeFlag =
	{
		"#version 430"\
		"\n"\
		"layout(local_size_x = 10, local_size_y = 10) in;"\

		"uniform vec3 Gravity = vec3(0,-10,0);"\
		"uniform float ParticleMass = 0.1;"\
		"uniform float ParticleInvMass = 1.0 / 0.1;"\
		"uniform float SpringK = 2000.0;"\
		"uniform float RestLengthHoriz;"\
		"uniform float RestLengthVert;"\
		"uniform float RestLengthDiag;"\
		"uniform float DeltaT = 0.000005;"\
		"uniform float DampingConst = 0.1;"\

		"layout(std430, binding = 0) buffer PosIn {"\
		"vec4 PositionIn[];"\
		"};"\
		"layout(std430, binding = 1) buffer PosOut {"\
		"vec4 PositionOut[];"\
		"};"\
		"layout(std430, binding = 2) buffer VelIn {"\
		"vec4 VelocityIn[];"\
		"};"\
		"layout(std430, binding = 3) buffer VelOut {"\
		"vec4 VelocityOut[];"\
		"};"\

		"void main() {"\
		"uvec3 nParticles = gl_NumWorkGroups * gl_WorkGroupSize;"\
		"uint idx = gl_GlobalInvocationID.y * nParticles.x + gl_GlobalInvocationID.x;"\

		"vec3 p = vec3(PositionIn[idx]);"\
		"vec3 v = vec3(VelocityIn[idx]), r;"\

		// Start with gravitational acceleration and add the spring
		// forces from each neighbor
		"vec3 force = Gravity * ParticleMass;"\

		// Particle above
		"if (gl_GlobalInvocationID.y < nParticles.y - 1) {"\
		"r = PositionIn[idx + nParticles.x].xyz - p;"\
		"force += normalize(r) * SpringK * (length(r) - RestLengthVert);"\
		"}"\
		// Below
		"if (gl_GlobalInvocationID.y > 0) {"\
		"r = PositionIn[idx - nParticles.x].xyz - p;"\
		"force += normalize(r) * SpringK * (length(r) - RestLengthVert);"\
		"}"\
		// Left
		"if (gl_GlobalInvocationID.x > 0) {"\
		"r = PositionIn[idx - 1].xyz - p;"\
		"force += normalize(r) * SpringK * (length(r) - RestLengthHoriz);"\
		"}"\
		// Right
		"if (gl_GlobalInvocationID.x < nParticles.x - 1) {"\
		"r = PositionIn[idx + 1].xyz - p;"\
		"force += normalize(r) * SpringK * (length(r) - RestLengthHoriz);"\
		"}"\

		// Diagonals
		// Upper-left
		"if (gl_GlobalInvocationID.x > 0 && gl_GlobalInvocationID.y < nParticles.y - 1) {"\
		"r = PositionIn[idx + nParticles.x - 1].xyz - p;"\
		"force += normalize(r) * SpringK * (length(r) - RestLengthDiag);"\
		"}"\
		// Upper-right
		"if (gl_GlobalInvocationID.x < nParticles.x - 1 && gl_GlobalInvocationID.y < nParticles.y - 1) {"\
		"r = PositionIn[idx + nParticles.x + 1].xyz - p;"\
		"force += normalize(r) * SpringK * (length(r) - RestLengthDiag);"\
		"}"\
		// lower -left
		"if (gl_GlobalInvocationID.x > 0 && gl_GlobalInvocationID.y > 0) {"\
		"r = PositionIn[idx - nParticles.x - 1].xyz - p;"\
		"force += normalize(r) * SpringK * (length(r) - RestLengthDiag);"\
		"}"
		// lower-right
		"if (gl_GlobalInvocationID.x < nParticles.x - 1 && gl_GlobalInvocationID.y > 0) {"\
		"r = PositionIn[idx - nParticles.x + 1].xyz - p;"\
		"force += normalize(r) * SpringK * (length(r) - RestLengthDiag);"\
		"}"\

		"force += -DampingConst * v;"\

		// Apply simple Euler integrator
		"vec3 a = force * ParticleInvMass;"\
		"PositionOut[idx] = vec4("\
		"p + v * DeltaT + 0.5 * a * DeltaT * DeltaT, 1.0);"\
		"VelocityOut[idx] = vec4(v + a * DeltaT, 0.0);"\

		// Pin a few of the top verts
		"if (gl_GlobalInvocationID.y == nParticles.y - 1 &&"\
		"(gl_GlobalInvocationID.x == 0 ||"\
		"gl_GlobalInvocationID.x == nParticles.x / 4 ||"\
		"gl_GlobalInvocationID.x == nParticles.x * 2 / 4 ||"\
		"gl_GlobalInvocationID.x == nParticles.x * 3 / 4 ||"\
		"gl_GlobalInvocationID.x == nParticles.x - 1)) {"\
		"PositionOut[idx] = vec4(p,1.0);"\
		"VelocityOut[idx] = vec4(0,0,0,0);"\
		"}"\
		"}"\
	};



	Shader objShader_ComputeFlag(true, nullptr, nullptr, nullptr, computeShaderSourceCodeFlag);

	shaders.push_back(objShader_ComputeFlag);

	//----------------------------------------------------------------------------------------------------------//

	//--Compute Shader for NOrmla Calculation--

	const GLchar* ComputeNormalShaderSourceCode =
	{
		"#version 430 core"\
		"\n"\
		"layout(local_size_x = 10, local_size_y = 10)in;"\
		"layout(std430, binding = 0)buffer PosIn{"\
		"vec4 Position[];"\
		"};"\

		"layout(std430, binding = 4)buffer NormalOut{"\
		"vec4 Normal[];"\
		"};"\

		"void main()"\
		"{"\
		"uvec3 nParticles = gl_NumWorkGroups * gl_WorkGroupSize;"\
		"uint idx = gl_GlobalInvocationID.y * nParticles.x + gl_GlobalInvocationID.x;"\

		"vec3 p = vec3(Position[idx]);"\
		"vec3 n = vec3(0);"\
		"vec3 a, b, c;"\

		"if(gl_GlobalInvocationID.y < nParticles.y - 1)"\
		"{"\
		"c = Position[idx + nParticles.x].xyz - p;"\
		"if(gl_GlobalInvocationID.x < nParticles.x - 1)"\
		"{"\
		"a = Position[idx + 1].xyz - p;"\
		"b = Position[idx + nParticles.x + 1].xyz - p;"\
		"n += cross(a, b);"\
		"n += cross(b, c);"\
		"}"\
		"if(gl_GlobalInvocationID.x > 0)"\
		"{"\
		"a = c;"\
		"b = Position[idx + nParticles.x - 1].xyz - p;"\
		"c = Position[idx - 1].xyz - p;"\
		"n += cross(a,b);"\
		"n += cross(b, c);"\
		"}"\
		"}"\

		"if(gl_GlobalInvocationID.y > 0)"\
		"{"\
		"c = Position[idx - nParticles.x].xyz - p;"\
		"if(gl_GlobalInvocationID.x > 0)"\
		"{"\
		"a = Position[idx - 1].xyz - p;"\
		"b = Position[idx - nParticles.x - 1].xyz - p;"\
		"n += cross(a,b);"\
		"n += cross(b,c);"\
		"}"\

		"if(gl_GlobalInvocationID.x < nParticles.x - 1)"\
		"{"\
		"a = c;"\
		"b = Position[idx - nParticles.x + 1].xyz - p;"\
		"c = Position[idx + 1].xyz - p;"\
		"n += cross(a, b);"\
		"n += cross(b, c);"\
		"}"\
		"}"\
		"Normal[idx] = vec4(normalize(n), 0.0);"\
		"}"\
	};

	Shader objShader_ComputeNormalFlag(true, nullptr, nullptr, nullptr, ComputeNormalShaderSourceCode);

	shaders.push_back(objShader_ComputeNormalFlag);


	glm::mat4 transf = glm::translate(glm::mat4(1.0), glm::vec3(0, clothSize_y, 0));
	transf = glm::rotate(transf, glm::radians(-80.0f), glm::vec3(1, 0, 0));
	transf = glm::translate(transf, glm::vec3(0, -clothSize_y, 0));

	// Initial transform
	float dx = clothSize_x / (nParticles_x - 1);
	float dy = clothSize_y / (nParticles_y - 1);
	float ds = 1.0f / (nParticles_x - 1);
	float dt = 1.0f / (nParticles_y - 1);
	glm::vec4 p(0.0f, 0.0f, 0.0f, 1.0f);

	std::vector<float> initTc;
	std::vector<float> initvel(nParticles_x * nParticles_y * 4, 0.0f);

	for (int i = 0; i < nParticles_y; i++)
	{
		for (int j = 0; j < nParticles_x; j++)
		{
			p[0] = dx * j;
			p[1] = dy * i;
			p[2] = 0.0f;

			p = transf * p;

			initpos.push_back(p[0]);
			initpos.push_back(p[1]);
			initpos.push_back(p[2]);
			initpos.push_back(1.0f);

			initTc.push_back(ds * j);
			initTc.push_back(dt * i);
		}
	}


	for (int row = 0; row < nParticles_y - 1; row++) {
		for (int col = 0; col < nParticles_x; col++) {
			el.push_back((row + 1) * nParticles_x + (col));
			el.push_back((row)* nParticles_x + (col));
		}
		el.push_back(PRIM_RESTART);
	}

	int part = nParticles_x * nParticles_y;

	/*bind vao*/
	/*Create vao of triangle*/
	glGenVertexArrays(1, &vao_cloth);
	glBindVertexArray(vao_cloth);

	//SSBO
	glGenBuffers(1, &ssbo_cloth_position[0]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo_cloth_position[0]);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_cloth_position[0]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLfloat) * part * 4, &initpos[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glGenBuffers(1, &ssbo_cloth_position[1]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo_cloth_position[1]);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_cloth_position[1]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLfloat) * part * 4, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glGenBuffers(1, &ssbo_cloth_velocity[0]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssbo_cloth_velocity[0]);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_cloth_velocity[0]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLfloat) * part * 4, &initvel[0], GL_DYNAMIC_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glGenBuffers(1, &ssbo_cloth_velocity[1]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo_cloth_velocity[1]);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_cloth_velocity[1]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLfloat) * part * 4, NULL, GL_DYNAMIC_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glGenBuffers(1, &vbo_cloth_normal);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, vbo_cloth_normal);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vbo_cloth_normal);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLfloat) * part * 4, NULL, GL_DYNAMIC_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, ssbo_cloth_position[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * initpos.size(), &initpos[0], GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_cloth_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * part * 4, NULL, GL_DYNAMIC_COPY);
	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_cloth_texture);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cloth_texture);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * initTc.size(), &initTc[0], GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD0);

	glGenBuffers(1, &vbo_cloth_element);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_cloth_element);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * el.size(), &el[0], GL_DYNAMIC_COPY);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	return 1;
}

void SceneCloth::DisplayScene()
{

	if (firstDisplay == 1) {
		firstDisplay = 0;
		camera.ResetCamera();
	}

	GLfloat dx = clothSize_x / (nParticles_x - 1);
	GLfloat dy = clothSize_y / (nParticles_y - 1);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(PRIM_RESTART);


	shaders[Shader_ComputeFlag].use();

	shaders[Shader_ComputeFlag].setFloat("RestLengthHoriz", dx);
	shaders[Shader_ComputeFlag].setFloat("RestLengthVert", dy);
	shaders[Shader_ComputeFlag].setFloat("RestLengthDiag", sqrtf(dx * dx + dy * dy));


	for (int i = 0; i < 1000; i++)
	{
		glDispatchCompute(nParticles_x / 10, nParticles_y / 10, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		// Swap buffers
		readBuf = 1 - readBuf;
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo_cloth_position[readBuf]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo_cloth_position[1 - readBuf]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssbo_cloth_velocity[readBuf]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo_cloth_velocity[1 - readBuf]);
	}

	//glUseProgram(0);

	//Compute Normal
	shaders[Shader_ComputeNormalFlag].use();

	glDispatchCompute(nParticles_x / 10, nParticles_y / 10, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glUseProgram(0);

	/*code*/
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shaders[Shader_Flag].use();

	projection = perspective(camera.Zoom, (float)gWidth / (float)gHeight, 0.1f, 100.0f);//radians

	view = camera.GetViewMatrix();

	//view = lookat(vec3(3.0f, 2.0f, 5.0f), vec3(2.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

	shaders[Shader_Flag].setMat4("projection", projection);
	shaders[Shader_Flag].setMat4("view", view);


	MakeModelMatricesIdentity();

	translationMatrix = translate(-2.5f, -1.5f, -3.0f);
	modelMatrix = translationMatrix * rotationMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	shaders[Shader_Flag].setMat4("model", modelMatrix);


	//view = lookat(vec3(3.0f, 2.0f, 5.0f), vec3(2.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

	float lightpostion[4] = { 100.0f, 100.0f, 100.0f, 1.0f };
	float lightIntensity[3] = { 1.0f, 1.0f, 1.0f };
	float kd[3] = { 0.8f, 0.8f, 0.8f };
	float ka[3] = { 0.2f, 0.2f, 0.2f };
	float ks[3] = { 1.0f, 1.0f, 1.0f };
	float Shininess = 128.0f;

	//if (gLighting == 1)
	//{
	shaders[Shader_Flag].setInt("keyPressed", 1);
	//}
	//else
	//{
	//	shaders[Shader_Couple].setInt("keyPressed", 0);
	//}

	//Light Uniform
	shaders[Shader_Flag].setVec4("LightPosition", lightpostion[0], lightpostion[1], lightpostion[2], lightpostion[3]);
	shaders[Shader_Flag].setVec3("LightIntensity", lightIntensity[0], lightIntensity[1], lightIntensity[2]);
	shaders[Shader_Flag].setVec3("Ka", ka[0], ka[1], ka[2]);
	shaders[Shader_Flag].setVec3("Kd", kd[0], kd[1], kd[2]);
	shaders[Shader_Flag].setVec3("Ks", ks[0], ks[1], ks[2]);
	shaders[Shader_Flag].setFloat("shininess", Shininess);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[Texture_Sir]);
	shaders[Shader_Flag].setInt("u_sampler", 0);

	/*Bind with vao (this will avoid any repetative binding with vbo)*/
	glBindVertexArray(vao_cloth);
	//glDrawElements(GL_TRIANGLE_STRIP, sizeof(quad_elements), GL_UNSIGNED_INT, quad_elements);
	//glDrawElements(GL_TRIANGLE_STRIP, sizeof(unsigned int) * el.size(), GL_UNSIGNED_INT, &el[0]);
	glDrawElements(GL_TRIANGLE_STRIP, el.size(), GL_UNSIGNED_INT, &el[0]);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//glUseProgram(0);


	glDisable(GL_TEXTURE_2D);
	glDisable(GL_PRIMITIVE_RESTART);
	//update();
}

void SceneCloth::UpdateScene()
{
}

void SceneCloth::UninitializeScene()
{
	// Delete Vertex buffers : vbo's 
	DeleteVertexBuffer(&ssbo_cloth_position[0]);
	DeleteVertexBuffer(&ssbo_cloth_position[1]);
	DeleteVertexBuffer(&ssbo_cloth_velocity[0]);
	DeleteVertexBuffer(&ssbo_cloth_velocity[1]);
	DeleteVertexBuffer(&vbo_cloth_normal);
	DeleteVertexBuffer(&vbo_cloth_texture);
	DeleteVertexBuffer(&vbo_cloth_element);

	// Delete Vertex Arrays : vao's
	DeleteVertexArray(&vao_cloth);
}
