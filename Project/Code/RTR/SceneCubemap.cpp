#include"SceneCubemap.h"
#include"Textures.h"

Sphere bubble;

int SceneCubemap::InitScene()
{

	const GLchar* vertexShaderSourceCodeCubeMap =
		"#version 430 core " \
		"\n " \
		"in vec3 vPosition;" \
		"in vec4 vColor;" \
		"out vec3 vTexCoord;"\
		"uniform mat4 projection;" \
		"uniform mat4 view;" \
		"out vec4 out_color;" \
		"void main(void)" \
		"{" \
		"vTexCoord=vPosition;"\
		"vec4 pos= projection * view* vec4(vTexCoord,1.0);" \
		"gl_Position=pos.xyww;"\
		"} ";

	const GLchar* fragmentShaderSourceCodeCubeMap =
		"#version 430 core " \
		"\n " \
		"out vec4 FragColor;"
		"uniform samplerCube skybox;"\
		"uniform float alphaCubeMap;"\
		"in vec3 vTexCoord;"\
		"void main(void)" \
		"{" \
		"FragColor=texture(skybox, vTexCoord);" \
		"FragColor.w = alphaCubeMap;"\
		"} ";

	Shader objShader_CubeMap(true, vertexShaderSourceCodeCubeMap, fragmentShaderSourceCodeCubeMap);

	shaders.push_back(objShader_CubeMap);

	const GLchar* vertexShaderSourceCodeBubble =
		"#version 430 core" \
		"\n" \
		"const float Air = 1.0;"\
		"const float Glass = 1.51714;"\
		"const float Eta = Air / Glass;"\

		"const float R0 = ((Air - Glass) * (Air - Glass)) / ((Air + Glass) * (Air + Glass));"\

		"uniform mat4 model;"\
		"uniform mat4 projection;" \
		"uniform mat4 view;" \

		"in vec4 vPosition;"\
		"in vec4 vNormal;"\

		"out vec3 v_reflection;"\
		"out vec3 v_refraction;"\
		"out float v_fresnel;"\

		"void main(void)"\
		"{"\
		"vec4 vertex = model*vPosition;"\
		"vec3 incident = normalize(vec3(vertex));"\
		"vec3 normal = vec3(vNormal);"\

		"v_refraction = refract(incident, normal, Eta);"\
		"v_reflection = reflect(incident, normal);"\

		"v_fresnel = R0 + (1.0 - R0) * pow((1.0 - dot(-incident, normal)), 8.0);"\

		"gl_Position = projection*view*vertex;"\
		"}";



	const GLchar* fragmentShaderSourceBubble =
		"#version 430 core " \
		"\n " \
		"uniform samplerCube skybox;"\
		"uniform float alphaCubeMap;"\
		"in vec3 v_refraction;"\
		"in vec3 v_reflection;"\
		"in float v_fresnel;"\
		"out vec4 FragColor;"\
		"void main(void)"\
		"{"\
		"vec4 refractionColor = texture(skybox, normalize(v_refraction));"\
		"vec4 reflectionColor = texture(skybox, normalize(v_reflection));"\
		"FragColor = mix(refractionColor, reflectionColor, v_fresnel);"\
		"FragColor.w = alphaCubeMap;"\
		"}";

	Shader objShader_Bubble(true, vertexShaderSourceCodeBubble, fragmentShaderSourceBubble);

	shaders.push_back(objShader_Bubble);


	const GLfloat skyboxVertices[] = {
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};
		

	glGenVertexArrays(1, &skyboxVAOCubeMap);
	glGenBuffers(1, &skyboxVBOCubeMap);
	glBindVertexArray(skyboxVAOCubeMap);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBOCubeMap);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	//vector<std::string> faces
	//{
	//	("right.jpg"),
	//	("left.jpg"),
	//	("top.jpg"),
	//	("bottom.jpg"),
	//	("front.jpg"),
	//	("back.jpg")
	//};

	//textures.push_back(loadCubemapTexture(faces));

	bubble.set(0.2f, 20, 20);

	return 0;

}
void SceneCubemap::DisplayScene()
{
	if (firstDisplay == 1) {
		firstDisplay = 0;
		camera.ResetCamera();
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_TEXTURE_2D);
	
	shaders[Shader_CubeMap].use();

	projection = perspective(camera.Zoom, (float)gWidth / (float)gHeight, 0.1f, 100.0f);//radians
	view = camera.GetViewMatrix();
	view[3] = 0.0f;
	view[7] = 0.0f;
	view[11] = 0.0f;
	
	view[12] = 0.0f;
	view[13] = 0.0f;
	view[14] = 0.0f;

	view[15] = 1.0f;

	//view = translate(0.0f, 0.0f, -2.35f);
	shaders[Shader_CubeMap].setMat4("view", view);
	shaders[Shader_CubeMap].setMat4("projection", projection);
	shaders[Shader_CubeMap].setFloat("alphaCubeMap", cubeMapAlpha);
	
	glBindVertexArray(skyboxVAOCubeMap);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textures[Texture_Cubemap]);
	shaders[Shader_CubeMap].setInt("skybox", 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	shaders[Shader_Bubble].use();

	projection = perspective(camera.Zoom, (float)gWidth / (float)gHeight, 0.1f, 100.0f);//radians
	view = camera.GetViewMatrix();
	shaders[Shader_Bubble].setMat4("projection", projection);
	shaders[Shader_Bubble].setMat4("view", view);
	shaders[Shader_Bubble].setFloat("alphaCubeMap", cubeMapAlpha);

	MakeModelMatricesIdentity();

	translationMatrix = translate(0.0f, 0.0f, 0.0f);
	modelMatrix = modelMatrix * translationMatrix;

	shaders[Shader_Bubble].setMat4("model", modelMatrix);

	glActiveTexture(GL_TEXTURE0);

	shaders[Shader_Bubble].setInt("skybox", 0);

	bubble.draw();

	glDisable(GL_BLEND);
}

void SceneCubemap::UpdateScene()

{
	if(step==1){
		camera.ProcessMouseMovement(movementVar, 0.0f);
		camera.ProcessKeyboard(LEFT, 0.0015f);
		if (movementVar < 0.001f)
			movementVar += 0.6f;
		//else
			//movementVar = 0.1f;
	}
	if (fadeInCubeMap == true && cubeMapAlpha <= 1.0)
	{
		cubeMapAlpha = cubeMapAlpha + 0.0035f;

		if (cubeMapAlpha > 1.0f)
		{
			fadeInCubeMap = false;
		}
	}
	else if (fadeInCubeMap == false && cubeMapAlpha > 0.0)
	{
		cubeMapAlpha = cubeMapAlpha - 0.001f;

		if (cubeMapAlpha <= 0.0f)
		{
			cubeMapAlpha = 0.0f;
		}
	}
}

void SceneCubemap::UninitializeScene()
{
	// Delete Vertex buffers : vbo's 
	DeleteVertexBuffer(&skyboxVBOCubeMap);

	// Delete Vertex Arrays : vao's
	DeleteVertexArray(&skyboxVAOCubeMap);
}
