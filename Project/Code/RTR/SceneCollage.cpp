#include "SceneCollage.h"
#include"Textures.h"

int SceneCollage::InitScene()
{
	const GLchar *vertexShaderSourceCodeCollage =
		"#version 430 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec2 vTexCoord;" \

		"uniform mat4 model;"\
		"uniform mat4 view;"\
		"uniform mat4 projection;"\

		"out vec2 out_texCoord;"\

		"void main(void)" \
		"{" \
		"gl_Position = projection * view * model * vPosition;"\
		"out_texCoord = vTexCoord;"\
		"}";

	const GLchar *fragmentShaderSourceCodeCollage =
		"#version 430 core" \
		"\n" \
		"in vec2 out_texCoord;"\
		"uniform sampler2D u_tex_collage;"\
		"out vec4 FragColor;"\
		"void main(void)"\
		"{"\
		"FragColor = texture(u_tex_collage, out_texCoord);"\
		"}";

	Shader objShader_Collage(true, vertexShaderSourceCodeCollage, fragmentShaderSourceCodeCollage);

	shaders.push_back(objShader_Collage);

	const GLfloat collageTexCord[] =
	{
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f
	};

	// create vao
	glGenVertexArrays(1, &vaoCollage); // how many arrays, address of array
	glBindVertexArray(vaoCollage); // start recording

	// For rectangle Position
	glGenBuffers(1, &vboPositionCollage); // how many buffers, address of buffer
	glBindBuffer(GL_ARRAY_BUFFER, vboPositionCollage); // bind point/target, buffer
	glBufferData(GL_ARRAY_BUFFER, // target / bind point
		4 * 3 * sizeof(GLfloat), // size of data
		NULL, // actual data
		GL_DYNAMIC_DRAW); // when to put data, right now, statically, not run time

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, // cpu attribute index
		3, // how many co-ordinates in each vertex
		GL_FLOAT, // vertex data type
		GL_FALSE, // is vertex data normalised, no
		0, // stride, 
		NULL); // buffer offset: no stride hence do not need to cur buffer 

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION); // enable attribute array so that it can be seen by gpu

	glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind buffer array position
	// For rectangle Position End

	// For texture co-ordinates
	glGenBuffers(1, &vboTextureCollage); // how many buffers, address of buffer
	glBindBuffer(GL_ARRAY_BUFFER, vboTextureCollage); // bind point, buffer
	glBufferData(GL_ARRAY_BUFFER, // target
		sizeof(collageTexCord), // size of data
		collageTexCord, // actual data
		GL_STATIC_DRAW); // statically put data now

	glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD0, // cpu attribute
		2, // how many co-ordinates in each vertex
		GL_FLOAT, // vertex data type
		GL_FALSE, // is vertex data normalized: no
		0, // stride
		NULL); // buffer offset

	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD0);

	glBindVertexArray(0); // stop recording in vao
	return 1;
}

void SceneCollage::DisplayScene()
{
	if (firstDisplay == 1) {
		//Reset View Port
		glViewport(0, 0, (GLsizei)gWidth, (GLsizei)gHeight);
		firstDisplay = 0;
		camera.ResetCamera();
	}
	projection = perspective(camera.Zoom, (float)gWidth / (float)gHeight, 0.1f, 100.0f);//radians
	view = camera.GetViewMatrix();

	shaders[Shader_Collage].use();

	shaders[Shader_Collage].setMat4("projection", projection);
	shaders[Shader_Collage].setMat4("view", view);

	MakeModelMatricesIdentity();

	//Transformation
	translationMatrix = translate(0.0f, 0.0f, -3.5f);
	modelMatrix = translationMatrix;

	shaders[Shader_Collage].setMat4("model", modelMatrix);

	const GLfloat collageVertices[] =
	{
		collageRectX, collageRectY, 0.0f,
		-collageRectX, collageRectY, 0.0f,
		-collageRectX, -collageRectY, 0.0f,
		collageRectX, -collageRectY, 0.0f
	};

	glBindVertexArray(vaoCollage);

	glBindBuffer(GL_ARRAY_BUFFER, vboPositionCollage); // bind point/target, buffer
	glBufferData(GL_ARRAY_BUFFER, // target / bind point
		sizeof(collageVertices), // size of data
		collageVertices, // actual data
		GL_DYNAMIC_DRAW); // when to put data, right now, statically, not run time
	glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind buffer array position

	glActiveTexture(GL_TEXTURE0); // which texture to activate, unit 0

	glBindTexture(GL_TEXTURE_2D, textures[Texture_Collage]);

	shaders[Shader_Collage].setInt("u_tex_collage", 0);

	// draw necessory scene
	glDrawArrays(GL_TRIANGLE_FAN, // what to draw from vertices 
		0, // from where to start taking array elements
		4); // how may vertices to draw ; 4 for quads

	// unbind vao
	glBindVertexArray(0);

}

void SceneCollage::UpdateScene()
{
	if (scaleIn == true && collageRectX < 3.0)
	{
		collageRectX = collageRectX + 0.02f;
		collageRectY = collageRectY + 0.02f;

		if (collageRectX >= 3.0f)
		{
			scaleIn = false;
		}
	}
	else if (scaleIn == false && collageRectX >= 0.0)
	{
		collageRectX = collageRectX - 0.032f;
		collageRectY = collageRectY - 0.032f;
	}
}

void SceneCollage::UninitializeScene()
{
	// Delete Vertex buffers : vbo's 
	DeleteVertexBuffer(&vboPositionCollage);
	DeleteVertexBuffer(&vboTextureCollage);

	// Delete Vertex Arrays : vao's
	DeleteVertexArray(&vaoCollage);
}
