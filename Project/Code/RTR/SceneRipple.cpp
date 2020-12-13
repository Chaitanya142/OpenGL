#include "SceneRipple.h"

void SceneRipple::calcwater()
{
	int x, y;
	float n;

	for (y = 1; y < WATERSIZE - 1; y++) {
		for (x = 1; x < WATERSIZE - 1; x++) {
			n = (water[t][x - 1][y] +
				water[t][x + 1][y] +
				water[t][x][y - 1] +
				water[t][x][y + 1]
				) / 2;
			n -= water[f][x][y];
			n = n - (n / DAMP);
			water[f][x][y] = n;
		}
	}
}

int SceneRipple::InitScene()
{
	const GLchar *vertexShaderSourceCode =
		"#version 430 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec4 vColor;" \

		"uniform mat4 model;" \
		"uniform mat4 view;" \
		"uniform mat4 projection;" \

		"out vec4 color;" \

		"void main(void)" \
		"{" \
		"gl_Position = projection * view * model * vPosition;" \
		"color = vColor;" \
		"}";

	const GLchar *fragmentShaderSourceCode =
		"#version 430 core" \
		"\n" \
		"in vec4 color;" \
		"uniform float u_alpha;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor = color;" \
		"}";

	Shader objShader_Ripple(true, vertexShaderSourceCode, fragmentShaderSourceCode);

	shaders.push_back(objShader_Ripple);

	//waterAlphaUniform = glGetUniformLocation(gShaderProgramObjectWater, "u_alpha");


	GLfloat pointVertices[] =
	{ 0.0f, 0.0f, 0.0f };

	// create vaoWaterRipple
	glGenVertexArrays(1, &vaoWaterRipple); // how many arrays, address of array

	glBindVertexArray(vaoWaterRipple); // start recording

	glGenBuffers(1, &vboWaterRipplePosition); // how many buffers, address of buffer

	glBindBuffer(GL_ARRAY_BUFFER, vboWaterRipplePosition); // bind point/target, buffer

	glBufferData(GL_ARRAY_BUFFER, // target / bind point
		sizeof(pointVertices), // size of data
		pointVertices, // actual data
		GL_DYNAMIC_DRAW); // when to put data, right now, statically, not run time

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, // cpu attribute index
		3, // how many co-ordinates in each vertex
		GL_FLOAT, // vertex data type
		GL_FALSE, // is vertex data normalised, no
		0, // stride, 
		NULL); // buffer offset: no stride hence do not need to cur buffer 

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION); // enable attribute array so that it can be seen by gpu

	glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind buffer array

	// Ripple color
	glGenBuffers(1, &vboWaterRippleColor); // how many buffers, address of buffer

	glBindBuffer(GL_ARRAY_BUFFER, vboWaterRippleColor); // bind point/target, buffer

	glBufferData(GL_ARRAY_BUFFER, // target / bind point
		4 * sizeof(float), // size of data
		NULL, // actual data
		GL_DYNAMIC_DRAW); // when to put data, right now, statically, not run time

	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, // cpu attribute index
		4, // how many co-ordinates in each vertex
		GL_FLOAT, // vertex data type
		GL_FALSE, // is vertex data normalised, no
		0, // stride, 
		NULL); // buffer offset: no stride hence do not need to cur buffer 

	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR); // enable attribute array so that it can be seen by gpu

	glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind buffer array

	glBindVertexArray(0); // stop recording in vaoWaterRipple

	for (int i = 0; i < WATERSIZE; i++)
	{
		for (int j = 0; j < WATERSIZE; j++)
		{
			water[0][j][i] = 0;
			water[1][j][i] = 0;
			water[2][j][i] = 0;
		}
	}

	return 1;
}

void SceneRipple::DisplayScene()
{
	int i, j, tmp;
	float xPos, yPos, zPos;

	calcwater();

	shaders[Shader_Ripple].use();

	projection = perspective(camera.Zoom, (float)gWidth / (float)gHeight, 0.1f, 1000.0f);//radians
	view = camera.GetViewMatrix();
	//view= mat4::identity();
	shaders[Shader_Ripple].setMat4("projection", projection);
	shaders[Shader_Ripple].setMat4("view", view);

	MakeModelMatricesIdentity();
	//Transformation

	translationMatrix = translate(0.0f, 0.0f, spin_z - 50.0f);
	mat4 rotationMatrixX = mat4::identity();
	mat4 rotationMatrixY = mat4::identity();

	rotationMatrixX = rotate(spin_y, 0.0f, 1.0f, 0.0f);
	rotationMatrixY = rotate(spin_x - 60, 1.0f, 0.0f, 0.0f);
	rotationMatrix = rotationMatrixX * rotationMatrixY;


	modelMatrix = translationMatrix * rotationMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	shaders[Shader_Ripple].setMat4("model", modelMatrix);

	shaders[Shader_Ripple].setFloat("u_alpha", waterAlpha);

	glBindVertexArray(vaoWaterRipple);

	glBindBuffer(GL_ARRAY_BUFFER, vboWaterRippleColor); // bind point/target, buffer

	float pointColor[] = { waterColorR, waterColorG, waterColorB, waterAlpha };

	glBufferData(GL_ARRAY_BUFFER, // target / bind point
		sizeof(pointColor), // size of data
		pointColor, // actual data
		GL_DYNAMIC_DRAW); // when to put data, right now, statically, not run time

	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, // cpu attribute index
		4, // how many co-ordinates in each vertex
		GL_FLOAT, // vertex data type
		GL_FALSE, // is vertex data normalised, no
		0, // stride, 
		NULL); // buffer offset: no stride hence do not need to cur buffer 

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// bind with textures if any
	glBindBuffer(GL_ARRAY_BUFFER, vboWaterRipplePosition); // bind point/target, buffer

	for (i = 0; i < WATERSIZE; i++)
	{
		for (j = 0; j < WATERSIZE; j++)
		{
			xPos = (float)(j - WATERSIZE / 2);
			yPos = (float)(i - WATERSIZE / 2);
			zPos = water[t][j][i];

			float pointVertices[] = { xPos, yPos, zPos };

			glBufferData(GL_ARRAY_BUFFER, // target / bind point
				sizeof(pointVertices), // size of data
				pointVertices, // actual data
				GL_DYNAMIC_DRAW); // when to put data, right now, statically, not run time

			glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, // cpu attribute index
				3, // how many co-ordinates in each vertex
				GL_FLOAT, // vertex data type
				GL_FALSE, // is vertex data normalised, no
				0, // stride, 
				NULL); // buffer offset: no stride hence do not need to cur buffer 

			glPointSize(1.0f);

			// draw necessory scene
			glDrawArrays(GL_POINTS, // what to draw from vertices 
				0, // from where to start taking array elements
				1); // how may vertices to draw ; 4 for quads
		}
	}

	// unbind vaoWaterRipple
	glBindVertexArray(0);

	tmp = t; t = f; f = tmp;

}

void SceneRipple::UpdateScene()
{
	//if (!(++num %delay))
	{
		water[f][rand() % WATERSIZE][rand() % WATERSIZE] = -float(rand() % 200);
		delay = rand() % 100 + 50;
	}

	waterColorR = waterColorR - 0.0f;
	waterColorG = waterColorG - 0.02f;
	waterColorB = waterColorB - 0.02f;

	if (waterColorG <= 0.0f || waterColorB <= 0.0f)
	{
		waterColorG = 1.0f;
		waterColorB = 1.0f;
	}
}

void SceneRipple::UninitializeScene()
{
	// Delete Vertex buffers : vbo's 
	DeleteVertexBuffer(&vboWaterRipplePosition);
	DeleteVertexBuffer(&vboWaterRippleColor);

	// Delete Vertex Arrays : vao's
	DeleteVertexArray(&vaoWaterRipple);
}
