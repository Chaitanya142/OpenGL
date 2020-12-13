#include"SceneRain.h"



int SceneRain::InitScene()
{

	const GLchar* vertexShaderSourceCodeRain =
		"#version 330 core"\
		"\n"
		"layout (location = 0) in vec3 aPos;"\
		"layout (location = 2) in vec2 aTexCoords;"\
		"layout (location = 3) in mat4 aInstanceMatrix;"\

		"out vec2 TexCoords;"\

		"uniform mat4 projection;"\
		"uniform mat4 view;"\
		"uniform float deltaT;"\

		"void main()"\
		"{"\
		"    TexCoords = aTexCoords;"\
		"    gl_Position = projection * view * aInstanceMatrix* vec4(aPos, 1.0f);"\
		"	 gl_Position.y=gl_Position.y-deltaT;"\
		"}";

	const GLchar* fragementShaderSourceCodeRain =
		"#version 330 core"\
		"\n"
		"out vec4 FragColor;"\
		"in vec2 TexCoords;"\
		"uniform sampler2D texture_diffuse1;"\
		"void main()"\
		"{"\
		"      vec4 color = texture(texture_diffuse1, TexCoords);"\
		"      FragColor=vec4(color.x,color.y,color.z,(color.a/9));"\
		"}";


	Shader objShader_Rain(true, vertexShaderSourceCodeRain, fragementShaderSourceCodeRain);

	shaders.push_back(objShader_Rain);


	const GLchar* backgroundRainVertexShaderSourceCode =
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

	const GLchar* backgroundRainFragmentShaderSourceCode =
	{
		"#version 430 core"\
		"\n"\
		"in vec2 out_texCoord;"\
		"uniform sampler2D tex_backgroundRain;"\
		"out vec4 FragColor;"\
		"void main(void)"\
		"{"\
			"FragColor = texture(tex_backgroundRain, out_texCoord);"\
		"}"
	};

	//	backgroundRain_sampler_uniform = glGetUniformLocation(gShaderProgramObjectbackgroundRain, "tex_backgroundRain");

	Shader objShader_BackgroundRain(true, backgroundRainVertexShaderSourceCode, backgroundRainFragmentShaderSourceCode);

	shaders.push_back(objShader_BackgroundRain);

	const GLfloat QuadTexCoord[] = { 1.0f, 1.0f,
									 0.0f, 1.0f,
									 0.0f, 0.0f,
									 1.0f, 0.0f };

	const GLfloat QuadVertices[] = { 1.5f, 1.0f, 0.0f,
								-1.5f, 1.0f, 0.0f,
								-1.5f, -1.0f, 0.0f,
								 1.5f, -1.0f, 0.0f };

	/*Create vao*/
	glGenVertexArrays(1, &vao_backgroundRain);
	glBindVertexArray(vao_backgroundRain);
	glGenBuffers(1, &vbo_backgroundRain_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_backgroundRain_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(QuadVertices), QuadVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	/*Texture buffer*/
	glGenBuffers(1, &vbo_backgroundRain_texture);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_backgroundRain_texture);
	glBufferData(GL_ARRAY_BUFFER, sizeof(QuadTexCoord), QuadTexCoord, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD0);
	/*Unbinding the buffer*/
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	unsigned int amount = 200;
	
	GenerateModelMatrices(amount);

	for (unsigned int i = 0; i < models[Model_Rain].meshes.size(); i++)
	{
		unsigned int VAO = models[Model_Rain].meshes[i].VAO;
		glBindVertexArray(VAO);
		// set attribute pointers for matrix (4 times vec4)
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(2 * sizeof(vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(3 * sizeof(vec4)));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}

	return 1;
}

void SceneRain::GenerateModelMatrices(unsigned int amount)
{
	//srand(0.75); // initialize random seed	
	float radius = 20.0;
	float offset = 100.0f;
	for (unsigned int i = 0; i < amount; i++)
	{
		//glm::mat4 model = glm::mat4(1.0f);

		mat4 model = mat4::identity();
		// 1. translation: displace along circle with 'radius' in range [-offset, offset]
		float angle = (float)i / (float)amount * 360.0f;
		float displacement = (rand() % (int)(2 * offset * 40)) / 40.0f - offset;
		float x = sin(angle) * radius + displacement;
		displacement = (rand() % (int)(2 * offset * 40)) / 40.0f - offset;
		float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
		displacement = (rand() % (int)(2 * offset * 40)) / 40.0f - offset;
		float z = cos(angle) * radius;//+ displacement;
		model = model * translate(x, y, z);

		// 2. scale: Scale between 0.05 and 0.25f
		float scale1 = (rand() % 20) / 18.0f + 0.05f;
		//model = glm::scale(model, glm::vec3(scale));
		model = model * scale(scale1, 20.0f, 1.0f);

		// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
		//float rotAngle = (rand() % 360);
		//model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		// 4. now add to list of matrices
		modelMatrices[amntCount] = model;
		amntCount++;
	}

	// configure instanced array
	// -------------------------
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, gAmnt * sizeof(mat4), &modelMatrices[0], GL_STATIC_DRAW);
}

void SceneRain::DisplayScene()
{
	if (InitSceneFlag) {
		camera.ResetCamera();
		camera.ProcessKeyboard(BACKWARD, 30.0);
		InitSceneFlag = false;
	}

	shaders[Shader_BackgroundRain].use();

	projection = perspective(camera.Zoom, (float)gWidth / (float)gHeight, 0.1f, 100.0f);//radians
	view = camera.GetViewMatrix();
	shaders[Shader_BackgroundRain].setMat4("projection", projection);
	shaders[Shader_BackgroundRain].setMat4("view", view);

	MakeModelMatricesIdentity();
	//Transformation

	translationMatrix = translate(0.0f, 0.0f, -3.0f);
	modelMatrix = translationMatrix * rotationMatrix;
	scaleMatrix = scale(40.0f,40.0f,40.0f);
	modelMatrix = modelMatrix * scaleMatrix;

	shaders[Shader_BackgroundRain].setMat4("model", modelMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[Texture_BackRain]);

	shaders[Shader_BackgroundRain].setInt("tex_backgroundRain", 0);

	/*Bind with vao (this will avoid any repetative binding with vbo)*/
	glBindVertexArray(vao_backgroundRain);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	/*Unbind vao*/
	glBindVertexArray(0);


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	shaders[Shader_Rain].use();

	// view/projection transformations
	projection = perspective(camera.Zoom, (float)gWidth / (float)gHeight, 0.1f, 100.0f);//radians
	view = camera.GetViewMatrix();
	shaders[Shader_Rain].setMat4("projection", projection);
	shaders[Shader_Rain].setMat4("view", view);
	shaders[Shader_Rain].setFloat("deltaT", delT);


	shaders[Shader_Rain].setInt("texture_diffuse1", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	//glBindTexture(GL_TEXTURE_2D, models[Model_Rain].textures_loaded[0].id); // note: we also made the textures_loaded vector public (instead of private) from the model class.
	glBindTexture(GL_TEXTURE_2D, textures[Texture_Rain]);
	for (unsigned int i = 0; i < models[Model_Rain].meshes.size(); i++)
	{
		glBindVertexArray(models[Model_Rain].meshes[i].VAO);
		glDrawElementsInstanced(GL_TRIANGLES, models[Model_Rain].meshes[i].indices.size(), GL_UNSIGNED_INT, 0, amount);
		glBindVertexArray(0);
	}

	shaders[Shader_Rain].use();
	shaders[Shader_Rain].setFloat("deltaT", (delT - 125));
	// draw meteorites
	shaders[Shader_Rain].use();
	shaders[Shader_Rain].setInt("texture_diffuse1", 0);
	glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, models[Model_Rain].textures_loaded[0].id); // note: we also made the textures_loaded vector public (instead of private) from the model class.	
	glBindTexture(GL_TEXTURE_2D, textures[Texture_Rain]);
	for (unsigned int i = 0; i < models[Model_Rain].meshes.size(); i++)
	{
		glBindVertexArray(models[Model_Rain].meshes[i].VAO);
		glDrawElementsInstanced(GL_TRIANGLES, models[Model_Rain].meshes[i].indices.size(), GL_UNSIGNED_INT, 0, amount);
		glBindVertexArray(0);
	}

	shaders[Shader_Rain].use();
	shaders[Shader_Rain].setFloat("deltaT", (delT - 250));

	// draw meteorites
	shaders[Shader_Rain].use();
	shaders[Shader_Rain].setInt("texture_diffuse1", 0);
	glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, models[Model_Rain].textures_loaded[0].id); // note: we also made the textures_loaded vector public (instead of private) from the model class.
	glBindTexture(GL_TEXTURE_2D, textures[Texture_Rain]);

	for (unsigned int i = 0; i < models[Model_Rain].meshes.size(); i++)
	{
		glBindVertexArray(models[Model_Rain].meshes[i].VAO);
		glDrawElementsInstanced(GL_TRIANGLES, models[Model_Rain].meshes[i].indices.size(), GL_UNSIGNED_INT, 0, amount);
		glBindVertexArray(0);
	}

	shaders[Shader_Rain].use();
	shaders[Shader_Rain].setFloat("deltaT", (delT - 375));

	// draw meteorites
	shaders[Shader_Rain].use();
	shaders[Shader_Rain].setInt("texture_diffuse1", 0);
	glActiveTexture(GL_TEXTURE0);
	//	glBindTexture(GL_TEXTURE_2D, models[Model_Rain].textures_loaded[0].id); // note: we also made the textures_loaded vector public (instead of private) from the model class.
	glBindTexture(GL_TEXTURE_2D, textures[Texture_Rain]);

	for (unsigned int i = 0; i < models[Model_Rain].meshes.size(); i++)
	{
		glBindVertexArray(models[Model_Rain].meshes[i].VAO);
		glDrawElementsInstanced(GL_TRIANGLES, models[Model_Rain].meshes[i].indices.size(), GL_UNSIGNED_INT, 0, amount);
		glBindVertexArray(0);
	}

	shaders[Shader_Rain].use();
	shaders[Shader_Rain].setFloat("deltaT", (delT + 125));

	// draw meteorites
	shaders[Shader_Rain].use();
	shaders[Shader_Rain].setInt("texture_diffuse1", 0);
	glActiveTexture(GL_TEXTURE0);
	//	glBindTexture(GL_TEXTURE_2D, models[Model_Rain].textures_loaded[0].id); // note: we also made the textures_loaded vector public (instead of private) from the model class.
	glBindTexture(GL_TEXTURE_2D, textures[Texture_Rain]);

	for (unsigned int i = 0; i < models[Model_Rain].meshes.size(); i++)
	{
		glBindVertexArray(models[Model_Rain].meshes[i].VAO);
		glDrawElementsInstanced(GL_TRIANGLES, models[Model_Rain].meshes[i].indices.size(), GL_UNSIGNED_INT, 0, amount);
		glBindVertexArray(0);
	}

	shaders[Shader_Rain].use();
	shaders[Shader_Rain].setFloat("deltaT", (delT + 250));

	// draw meteorites
	shaders[Shader_Rain].use();
	shaders[Shader_Rain].setInt("texture_diffuse1", 0);
	glActiveTexture(GL_TEXTURE0);
	//	glBindTexture(GL_TEXTURE_2D, models[Model_Rain].textures_loaded[0].id); // note: we also made the textures_loaded vector public (instead of private) from the model class.
	glBindTexture(GL_TEXTURE_2D, textures[Texture_Rain]);

	for (unsigned int i = 0; i < models[Model_Rain].meshes.size(); i++)
	{
		glBindVertexArray(models[Model_Rain].meshes[i].VAO);
		glDrawElementsInstanced(GL_TRIANGLES, models[Model_Rain].meshes[i].indices.size(), GL_UNSIGNED_INT, 0, amount);
		glBindVertexArray(0);
	}



	shaders[Shader_Rain].use();
	shaders[Shader_Rain].setFloat("deltaT", (delT + 375));
	shaders[Shader_Rain].use();
	shaders[Shader_Rain].setInt("texture_diffuse1", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[Texture_Rain]);

	//glBindTexture(GL_TEXTURE_2D, models[Model_Rain].textures_loaded[0].id); // note: we also made the textures_loaded vector public (instead of private) from the model class.
	for (unsigned int i = 0; i < models[Model_Rain].meshes.size(); i++)
	{
		glBindVertexArray(models[Model_Rain].meshes[i].VAO);
		glDrawElementsInstanced(GL_TRIANGLES, models[Model_Rain].meshes[i].indices.size(), GL_UNSIGNED_INT, 0, amount);
		glBindVertexArray(0);
	}

	glDisable(GL_BLEND);

}

void SceneRain::UpdateScene()
{
	delT += 0.5;
	if (delT > 250.0) {
		delT = -250.0f;
	}

}


void SceneRain::UninitializeScene()
{
	// Delete Vertex buffers : vbo's 
	DeleteVertexBuffer(&vbo_backgroundRain_position);
	DeleteVertexBuffer(&vbo_backgroundRain_texture);
	DeleteVertexBuffer(&buffer);

	// Delete Vertex Arrays : vao's
	DeleteVertexArray(&vao_backgroundRain);
}
