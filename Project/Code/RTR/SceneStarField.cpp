#include"SceneStarField.h"



inline float SceneStarField::random_float()
{
	float res;
	unsigned int tmp;
	seed *= 16807;
	tmp = seed ^ (seed >> 4) ^ (seed << 15);
	*((unsigned int *)&res) = (tmp >> 9) | 0x3F800000;
	return (res - 1.0f);
}

int SceneStarField::InitScene()
{
	const GLchar* vertexShaderSourceCodeStarField =

		"#version 430 core"\
		"\n"\
		"in vec4 vPosition;"\
		"in vec4 vColor;"\
		"uniform float time;"\

		"uniform mat4 model;"\
		"uniform mat4 view;"\
		"uniform mat4 projection;"\

		"flat out vec4 starcolor;"\
		"void main(void)"\
		"{"\
		"vec4 newVertex = vPosition;"\
		"newVertex.z += time;"\
		"newVertex.z = fract(newVertex.z);"\

		"float size = (20.0 * newVertex.z + newVertex.z);"\
		"starcolor = smoothstep(1.0, 7.0, size) * vColor;"\

		"newVertex.z = (999.9 * newVertex.z) - 1000.0;"\
		"gl_Position = projection * newVertex;"\
		"gl_PointSize = size;"\
		"}";

	const GLchar* fragmentShaderSourceCodeStarField =
		"#version 430 core"\
		"\n"\
		"out vec4 FragColor;"\
		"uniform sampler2D tex_star;"\
		"flat in vec4 starcolor;"\
		"uniform float alpha;"\
		"void main(void)"\
		"{"\
		"FragColor = starcolor * texture(tex_star, gl_PointCoord);"\
		"FragColor.w = alpha;"\
		"}";

	Shader objShader_StarField(true, vertexShaderSourceCodeStarField, fragmentShaderSourceCodeStarField);

	shaders.push_back(objShader_StarField);

	/*Create vao*/
	glGenVertexArrays(1, &vao_StarFiled);
	glBindVertexArray(vao_StarFiled);

	glGenBuffers(1, &vbo_StarFiled);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_StarFiled);
	glBufferData(GL_ARRAY_BUFFER, NUM_STARS * sizeof(star_t), NULL, GL_STATIC_DRAW);

	star_t *star = (star_t*)glMapBufferRange(GL_ARRAY_BUFFER, 0, NUM_STARS * sizeof(star_t), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

	for (int i = 0; i < 1000; i++)
	{
		star[i].position[0] = (random_float() * 2.0f - 1.0f) * 100.0f;
		star[i].position[1] = (random_float() * 2.0f - 1.0f) * 100.0f;
		star[i].position[2] = random_float();

		star[i].color[0] = 0.8f + random_float() * 0.2f;
		star[i].color[1] = 0.8f + random_float() * 0.2f;
		star[i].color[2] = 0.8f + random_float() * 0.2f;
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(star_t), NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, sizeof(star_t), (void*)sizeof(vec3));
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	/*Unbinding the buffer*/
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	float point_texcoord[] = { 0.0f, 1.0f };

	glGenBuffers(1, &vbo_texture_StarFiled);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texture_StarFiled);
	glBufferData(GL_ARRAY_BUFFER, sizeof(point_texcoord), point_texcoord, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD0);

	glBindVertexArray(0);

	return 1;
}


void SceneStarField::DisplayScene()
{
	if (firstDisplay == 1) {
		firstDisplay = 0;
		camera.ResetCamera();
	}

	glEnable(GL_BLEND);
	//glBlendFunc(GL_ONE, GL_ONE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_POINT_SPRITE);

	float t = currentTime;

	t *= 0.1f;
	t -= floor(t);

	/*code*/

	shaders[Shader_StarField].use();

	projection = perspective(camera.Zoom, (float)gWidth / (float)gHeight, 0.1f, 100.0f);//radians
	view = camera.GetViewMatrix();
	shaders[Shader_StarField].setMat4("projection", projection);
	shaders[Shader_StarField].setMat4("view", view);
	MakeModelMatricesIdentity();
	//Transformation
	shaders[Shader_StarField].setMat4("model", modelMatrix);

	//Send this necessary matrices to shaders in respective uniforms
	shaders[Shader_StarField].setFloat("time", t);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[Texture_Star]);
	shaders[Shader_StarField].setInt("tex_star", 0);
	shaders[Shader_StarField].setFloat("alpha", Alpha);
	/*Bind with vao (this will avoid any repetative binding with vbo)*/
	glBindVertexArray(vao_StarFiled);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glDrawArrays(GL_POINTS, 0, NUM_STARS);
	/*Unbind vao*/
	glBindVertexArray(0);

	glDisable(GL_BLEND);
	glDisable(GL_POINT_SPRITE);
}

void SceneStarField::UpdateScene()
{
	currentTime += 0.008f;
	Alpha -= 0.009f;
}


void SceneStarField::UninitializeScene()
{
	// Delete Vertex buffers : vbo's 
	DeleteVertexBuffer(&vbo_StarFiled);
	DeleteVertexBuffer(&vbo_texture_StarFiled);

	// Delete Vertex Arrays : vao's
	DeleteVertexArray(&vao_StarFiled);
}
