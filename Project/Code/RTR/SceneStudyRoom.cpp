#include "SceneStudyRoom.h"



int SceneStudyRoom::InitScene()
{

	const GLchar* StudyRoomWallVertexShaderSourceCode =
	{
		"#version 430 core"\
		"\n"\
		"in vec4 vPosition;"\
		"in vec2 vTexCoord;"\
		"uniform mat4 model;"\
		"uniform mat4 view;"\
		"uniform mat4 projection;"\
		"out vec2 out_texCoord;"\
		"void main(void)"\
		"{"\
			"gl_Position = projection * view * model * vPosition;"\
			"out_texCoord = vTexCoord;"\
		"}"
	};

	const GLchar* StudyRoomWallFragmentShaderSourceCode =
	{
		"#version 430 core"\
		"\n"\
		"in vec2 out_texCoord;"\
		"uniform sampler2D tex_StudyRoomWall;"\
		"out vec4 FragColor;"\
		"void main(void)"\
		"{"\
			"FragColor = texture(tex_StudyRoomWall, out_texCoord);"\
		"}"
	};

	Shader objShader_StudyRoomWall(true, StudyRoomWallVertexShaderSourceCode, StudyRoomWallFragmentShaderSourceCode);

	shaders.push_back(objShader_StudyRoomWall);

	const GLfloat QuadTexCoord[] = { 1.0f, 1.0f,
									 0.0f, 1.0f,
									 0.0f, 0.0f,
									 1.0f, 0.0f };

	const GLfloat QuadVertices[] = { 1.5f, 1.0f, 0.0f,
								-1.5f, 1.0f, 0.0f,
								-1.5f, -1.0f, 0.0f,
								 1.5f, -1.0f, 0.0f };

	/*Create vao*/
	glGenVertexArrays(1, &vao_StudyRoomWall);
	glBindVertexArray(vao_StudyRoomWall);
	glGenBuffers(1, &vbo_StudyRoomWall_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_StudyRoomWall_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(QuadVertices), QuadVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	/*Texture buffer*/
	glGenBuffers(1, &vbo_StudyRoomWall_texture);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_StudyRoomWall_texture);
	glBufferData(GL_ARRAY_BUFFER, sizeof(QuadTexCoord), QuadTexCoord, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD0);
	/*Unbinding the buffer*/
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return 1;

}


void SceneStudyRoom::DisplayScene()
{
	shaders[Shader_StudyRoomWall].use();

	projection = perspective(camera.Zoom, (float)gWidth / (float)gHeight, 0.1f, 100.0f);//radians
	view = camera.GetViewMatrix();
	shaders[Shader_StudyRoomWall].setMat4("projection", projection);
	shaders[Shader_StudyRoomWall].setMat4("view", view);

	MakeModelMatricesIdentity();
	//Transformation

	translationMatrix = translate(0.0f, 0.0f, -2.0f);
	modelMatrix = translationMatrix * rotationMatrix;
	scaleMatrix = scale(2.5f, 2.5f, 2.5f);
	modelMatrix = modelMatrix * scaleMatrix;

	shaders[Shader_StudyRoomWall].setMat4("model", modelMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[Texture_StudyRoom]);

	shaders[Shader_StudyRoomWall].setInt("tex_StudyRoomWall", 0);

	/*Bind with vao (this will avoid any repetative binding with vbo)*/
	glBindVertexArray(vao_StudyRoomWall);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	/*Unbind vao*/
	glBindVertexArray(0);
}


void SceneStudyRoom::UpdateScene()
{
}

void SceneStudyRoom::UninitializeScene()
{
	// Delete Vertex buffers : vbo's 
	DeleteVertexBuffer(&vbo_StudyRoomWall_position);
	DeleteVertexBuffer(&vbo_StudyRoomWall_texture);

	// Delete Vertex Arrays : vao's
	DeleteVertexArray(&vao_StudyRoomWall);
}
