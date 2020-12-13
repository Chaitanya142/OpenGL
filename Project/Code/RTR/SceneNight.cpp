#include"SceneNight.h"

inline float SceneNight::random_float()
{
	float res;
	unsigned int tmp;
	seed *= 16807;
	tmp = seed ^ (seed >> 4) ^ (seed << 15);
	*((unsigned int *)&res) = (tmp >> 9) | 0x3F800000;
	return (res - 1.0f);
}

int SceneNight::InitScene()
{
	/*Model Loading Shader*/
	//Vertex Shader Code
	const GLchar* vertextShaderSourceCode =
		"#version 430 core " \
		"\n " \
		"layout(location = 0) in vec3 aPos;"\
		"layout(location = 1) in vec3 aNormal;"\
		"layout(location = 2) in vec2 aTexCoords;"\
		"uniform mat4 model;"\
		"uniform mat4 view;"\
		"uniform mat4 projection;"\
		"out vec2 TexCoords;"\
		"void main(void)" \
		"{" \
		"gl_Position=projection * view * model * vec4(aPos, 1.0);" \
		"TexCoords = aTexCoords;"\
		"} ";

	const GLchar* fragementShaderSourceCode =
		"#version 430 core " \
		"\n " \
		"out vec4 FragColor;"\
		"in vec2 TexCoords;"\
		"uniform sampler2D texture_diffuse;"\
		"uniform sampler2D texture_specular;"\
		"uniform sampler2D texture_normal;"\
		"uniform sampler2D texture_height;"\
		"void main(void)" \
		"{" \
		"FragColor = texture(texture_diffuse, TexCoords);" \
		"} ";


	Shader objShader_ModelLoading(true, vertextShaderSourceCode, fragementShaderSourceCode);

	shaders.push_back(objShader_ModelLoading);

	const GLchar* vertexShaderSourceCodeNightScene =
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
		"out_texCoord = vTexCoord;"\
		"gl_Position = projection * view * model * vPosition;"\
		"}";


	const GLchar* fragmentShaderSourceCodeNightScene =
		"#version 430 core"\
		"\n"\
		"in vec2 out_texCoord;"\
		"uniform sampler2D u_sampler;"\
		"out vec4 FragColor;"\
		"void main(void)"\
		"{"\
		"FragColor = texture(u_sampler, out_texCoord);"\
		"}";



	Shader objShader_NightScene(true, vertexShaderSourceCodeNightScene, fragmentShaderSourceCodeNightScene);

	shaders.push_back(objShader_NightScene);

	//----------------------------------------------------------------------------------------------------------//
	const GLchar* displayVertexShaderSourceCode =
		"#version 430 core"\
		"\n"\
		"void main(void)"\
		"{"\
		"const vec4 vertex[] = vec4[] (vec4(-1.0, -1.0, 0.5, 1.0),"\
		"vec4(1.0, -1.0, 0.5, 1.0),"\
		"vec4(-1.0, 1.0, 0.5, 1.0),"\
		"vec4(1.0, 1.0, 0.5, 1.0));"\
		"gl_Position = vertex[gl_VertexID];"\
		"}";

	const GLchar* displayfragmentShaderSourceCode =
		"#version 430 core"\
		"\n"\
		"uniform sampler2D input_image;"\
		"out vec4 color;"\
		"uniform float focal_distance = 50.0;"\
		"uniform float focal_depth = 30.0;"\
		"void main(void)"\
		"{"\
		"vec2 s = 1.0 / textureSize(input_image, 0);"\
		"vec2 C = gl_FragCoord.xy;"\
		"vec4 v = texelFetch(input_image, ivec2(gl_FragCoord.xy), 0).rgba;"\

		"float m;"\
		"if(v.w == 0.0)"\
		"{"\
		"m = 0.5;"\
		"}"\
		"else"\
		"{"\
		//calculate circle of confusion
		"m = abs(v.w - focal_distance);"\
		"m = 0.5 + smoothstep(0.0, focal_depth, m) * 7.5;"\
		"}"\

		//Calculate the four corners of our area to sample from
		"vec2 P0 = vec2(C * 1.0) + vec2(-m, -m);"\
		"vec2 P1 = vec2(C * 1.0) + vec2(-m, m);"\
		"vec2 P2 = vec2(C * 1.0) + vec2(m, -m);"\
		"vec2 P3 = vec2(C * 1.0) + vec2(m, m);"\

		//scale our coordinates
		"P0 *= s;"\
		"P1 *= s;"\
		"P2 *= s;"\
		"P3 *= s;"\

		"vec3 a = textureLod(input_image, P0, 0).rgb;"\
		"vec3 b = textureLod(input_image, P1, 0).rgb;"\
		"vec3 c = textureLod(input_image, P2, 0).rgb;"\
		"vec3 d = textureLod(input_image, P3, 0).rgb;"\

		//Calculate the sum of all pixels inside the kernel
		"vec3 f = a - b - c + d;"\
		"m *= 2;"\
		"f /= float(m * m);"\
		"color = vec4(f, 1.0);"\
		"}";

	Shader objShader_DisplayNightScene(true, displayVertexShaderSourceCode, displayfragmentShaderSourceCode);

	shaders.push_back(objShader_DisplayNightScene);

	//----------------------------------------------------------------------------------------------------------//



	//----------------------------------------------------------------------------------------------------------//

	const GLchar* computeShaderSourceCode =
		"#version 430 core"\
		"\n"\

		"layout(local_size_x  = 1024) in;"\

		"shared vec3 shared_data[gl_WorkGroupSize.x * 2];"\

		"layout (binding = 0, rgba32f) readonly uniform image2D input_image;"\
		"layout (binding = 1, rgba32f) writeonly uniform image2D output_image;"\

		"void main(void)"\
		"{"\
		"uint id = gl_LocalInvocationID.x;"\
		"uint rd_id;"\
		"uint wr_id;"\
		"uint mask;"\
		"ivec2 P0 = ivec2(id * 2, gl_WorkGroupID.x);"\
		"ivec2 P1 = ivec2(id * 2 + 1, gl_WorkGroupID.x);"\

		"const uint steps = uint(log2(gl_WorkGroupSize.x)) + 1;"\
		"uint step = 0;"\

		"vec4 i0 = imageLoad(input_image, P0);"\
		"vec4 i1 = imageLoad(input_image, P1);"\

		"shared_data[P0.x] = i0.rgb;"\
		"shared_data[P1.x] = i1.rgb;"\

		"barrier();"\

		"for(step = 0; step < steps; step++)"\
		"{"\
		"mask = (1 << step) - 1;"\
		"rd_id = ((id >> step) << (step + 1)) + mask;"\
		"wr_id = rd_id + 1 + (id & mask);"\

		"shared_data[wr_id] += shared_data[rd_id];"\

		"barrier();"\
		"}"\

		"imageStore(output_image, P0.yx, vec4(shared_data[P0.x], i0.a));"\
		"imageStore(output_image, P1.yx, vec4(shared_data[P1.x], i1.a));"\
		"}";

	Shader objShader_FilterSceneNightScene(true, nullptr, nullptr, nullptr, computeShaderSourceCode);
	shaders.push_back(objShader_FilterSceneNightScene);

	//----------------------------------------------------------------------------------------------------------//


	/*RECT rect;
	if (GetWindowRect(ghwnd, &rect))
	{
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;
	}*/

	//FBO FACILITIES

	glGenFramebuffers(1, &depth_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);

	glGenTextures(1, &depth_tex);
	glBindTexture(GL_TEXTURE_2D, depth_tex);
	glTexStorage2D(GL_TEXTURE_2D, 11, GL_DEPTH_COMPONENT32F, FBO_SIZE, FBO_SIZE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenTextures(1, &color_tex);
	glBindTexture(GL_TEXTURE_2D, color_tex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, FBO_SIZE, FBO_SIZE);

	glGenTextures(1, &temp_tex);
	glBindTexture(GL_TEXTURE_2D, temp_tex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, FBO_SIZE, FBO_SIZE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_tex, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color_tex, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//----------------------------------------------------------------------------------------------------------//

	Moon.extTexture = true;
	Moon.txtImage = &textures[Texture_Moon];
	Moon.set(0.5f, 50, 50, true);

	//----------------------------------------------------------------------------------------------------------//

	const GLchar* ColorvertexShaderSourceCode =
		"#version 430 core"\
		"\n"\
		"in vec4 vPos;"\
		"in vec4 vColor;"\
		"out vec4 out_color;"\
		"uniform mat4 model;"\
		"uniform mat4 view;"\
		"uniform mat4 projection;"\
		"void main(void)"\
		"{"\
		"gl_Position = projection * view * model * vPos;"\
		"out_color = vColor;"\
		"}"
		;

	const GLchar* ColorfragmentShaderSourceCode =
		"#version 430 core"\
		"\n"\
		"in vec4 out_color;"\
		"out vec4 FragColor;"\
		"void main(void)"\
		"{"\
		"FragColor = out_color;"\
		"}";

	Shader objShader_ColorNightScene(true, ColorvertexShaderSourceCode, ColorfragmentShaderSourceCode);

	shaders.push_back(objShader_ColorNightScene);

	//----------------------------------------------------------------------------------------------------------//


	/*Create vao_rectangle_night for RECTANGLE*/
	glGenVertexArrays(1, &vao_rectangle_night);
	glBindVertexArray(vao_rectangle_night);
	glGenBuffers(1, &vbo_rectangle_position_night);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_rectangle_position_night);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * 4, NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	/*Unbinding vertex array*/
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_rectangle_color_night);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_rectangle_color_night);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 4, NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	/*Unbinding the buffer*/
	glBindVertexArray(0);



	//----------------------------------------------------------------------------------------------------------//


	//Vertex Shader Code
	const GLchar* vertextShaderSourceCodeTree =
		"#version 430 core " \
		"\n " \
		"layout(location = 0) in vec3 aPos;"\
		"layout(location = 1) in vec3 aNormal;"\
		"layout(location = 2) in vec2 aTexCoords;"\
		"uniform mat4 model;"\
		"uniform mat4 view;"\
		"uniform mat4 projection;"\
		"out vec2 TexCoords;"\
		"void main(void)" \
		"{" \
		"gl_Position=projection * view * model * vec4(aPos, 1.0);" \
		"TexCoords = aTexCoords;"\
		"} ";

	const GLchar* fragementShaderSourceCodeTree =
		"#version 430 core " \
		"\n " \
		"out vec4 FragColor;"\
		"in vec2 TexCoords;"\
		"uniform sampler2D texture_diffuse1;"\
		"void main(void)" \
		"{" \
		"FragColor = vec4(0.0f,0.0f,0.0f,1.0f);" \
		"} ";

	/*const GLchar* fragementShaderSourceCodeTree =
	"#version 430 core " \
	"\n " \
	"out vec4 FragColor;"\
	"in vec2 TexCoords;"\
	"uniform sampler2D texture_diffuse1;"\
	"void main(void)" \
	"{" \
	"FragColor = texture(texture_diffuse1, TexCoords);" \
	"} ";

	*/
	Shader objShader_NightSceneTree(true, vertextShaderSourceCodeTree, fragementShaderSourceCodeTree);

	shaders.push_back(objShader_NightSceneTree);

	const GLchar* starvertexShaderSourceCode =
	{
		"#version 430 core"\
		"\n"\
		"in vec4 vPosition;"\
		"in vec4 vColor;"\

		"uniform mat4 projection;"\

		"flat out vec4 starcolor;"\
		"void main(void)"\
		"{"\
			"vec4 newVertex = vPosition;"\
			"float size = (20.0 * newVertex.z + newVertex.z);"\
			"starcolor = smoothstep(1.0, 7.0, size) * vColor;"\

			"newVertex.z = (29.9 * newVertex.z) - 30.0;"\
			"gl_Position = projection * newVertex;"\
			"gl_PointSize = size;"\
		"}"
	};

	const GLchar* starfragmentShaderSourceCode =
	{
		"#version 430 core"\
		"\n"\
		"out vec4 FragColor;"\
		"uniform sampler2D tex_star;"\
		"flat in vec4 starcolor;"\
		"void main(void)"\
		"{"\
			"FragColor = starcolor * texture(tex_star, gl_PointCoord);"\
		"}"
	};

	Shader objShader_NightSceneStar(true, starvertexShaderSourceCode, starfragmentShaderSourceCode);

	shaders.push_back(objShader_NightSceneStar);

	glGenVertexArrays(1, &vao_static_star);
	glBindVertexArray(vao_static_star);

	std::vector<float> star_position;
	std::vector<float> star_color;
	std::vector<float> star_texcoord;

	for (int i = 0; i < 200; i++)
	{

		star_position.push_back((random_float() * 2.0f - 1.0f) * 12.0f);
		star_position.push_back(4.5f + (random_float() * 2.0f - 1.0f) * 2.5f);
		star_position.push_back(random_float());

		star_color.push_back(0.8f + random_float() * 0.2f);
		star_color.push_back(0.8f + random_float() * 0.2f);
		star_color.push_back(0.8f + random_float() * 0.2f);
		star_color.push_back(1.0f);

		star_texcoord.push_back(0.0f);
		star_texcoord.push_back(1.0f);

	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glGenBuffers(1, &vbo_static_star_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_static_star_position);
	glBufferData(GL_ARRAY_BUFFER, star_position.size() * sizeof(float), &star_position[0], GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glGenBuffers(1, &vbo_static_star_color);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_static_star_color);
	glBufferData(GL_ARRAY_BUFFER, star_color.size() * sizeof(float), &star_color[0], GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

	/*Unbinding the buffer*/
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glGenBuffers(1, &vbo_static_star_texture);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_static_star_texture);
	glBufferData(GL_ARRAY_BUFFER, star_texcoord.size() * sizeof(float), &star_texcoord[0], GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD0);

	glBindVertexArray(0);

	return 1;
}


void SceneNight::DisplayScene()
{

	if (firstDisplay == 1) {
		firstDisplay = 0;
		camera.ResetCamera();
	}

	/*code*/
	//Render Scene to FrameBuffer
	static const GLenum attachments[] = { GL_COLOR_ATTACHMENT0 };
	static const GLfloat zero[] = { 0.0f };
	static const GLfloat gray[] = { 0.0f, 0.0f, 0.0f, 1.0f, };
	static const GLfloat ones[] = { 1.0f };
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);
	glDrawBuffers(1, attachments);
	glViewport(0, 0, (GLsizei)gWidth, (GLsizei)gHeight);
	glClearBufferfv(GL_COLOR, 0, gray);
	glClearBufferfv(GL_DEPTH, 0, ones);

	/**************************Night Scene*********************************/
	projection = perspective(camera.Zoom, (float)gWidth / (float)gHeight, 0.1f, 100.0f);//radians
	shaders[Shader_NightSceneStar].use();
	shaders[Shader_NightSceneStar].setMat4("projection", projection);
	glEnable(GL_POINT_SPRITE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[Texture_Star]);
	shaders[Shader_NightSceneStar].setInt("tex_star", 0);
	glBindVertexArray(vao_static_star);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glDrawArrays(GL_POINTS, 0, NUM_STARS);
	glBindVertexArray(0);
	glDisable(GL_POINT_SPRITE);

	/*********************************************/
	shaders[Shader_ColorNightScene].use();


	projection = perspective(camera.Zoom, (float)gWidth / (float)gHeight, 0.1f, 100.0f);//radians
	view = camera.GetViewMatrix();
	shaders[Shader_ColorNightScene].setMat4("projection", projection);
	shaders[Shader_ColorNightScene].setMat4("view", view);

	MakeModelMatricesIdentity();
	//Transformation

	translationMatrix = translate(0.0f, 0.0f, -3.0f);
	modelMatrix = translationMatrix * rotationMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	shaders[Shader_ColorNightScene].setMat4("model", modelMatrix);

	const GLfloat rectangleVertices[] = { 5.0f, 0.8f, 0.0f,
										-5.0f, 0.8f, 0.0f,
										-5.0f, -2.5f, 0.0f,
										 5.0f, -2.5f, 0.0f };

	const GLfloat rectangleColor[] = { 0.0f, 0.0f, 0.0f, 1.0f,
									  0.0f, 0.0f, 0.0f, 1.0f,
									  0.0625f, 0.0625f, 0.570f, 1.0f,
									  0.0625f, 0.0625f, 0.570f, 1.0f };

	glBindVertexArray(vao_rectangle_night);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_rectangle_color_night);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleColor), rectangleColor, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_rectangle_position_night);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_DYNAMIC_DRAW);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	/*********************************************/
	shaders[Shader_NightScene].use();

	projection = perspective(camera.Zoom, (float)gWidth / (float)gHeight, 0.1f, 100.0f);//radians
	view = camera.GetViewMatrix();
	shaders[Shader_NightScene].setMat4("projection", projection);
	shaders[Shader_NightScene].setMat4("view", view);

	MakeModelMatricesIdentity();
	//Transformation

	translationMatrix = translate(0.0f, 1.5f, -3.0f);
	scaleMatrix = scale(moon_scale, moon_scale, 1.0f);
	modelMatrix = translationMatrix * scaleMatrix * rotationMatrix;
	modelMatrix = translationMatrix * rotationMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	shaders[Shader_NightScene].setMat4("model", modelMatrix);

	Moon.txtImage = &textures[Texture_Moon];
	Moon.draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/*###################################################*/
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	shaders[Shader_FilterSceneNightScene].use();

	glBindImageTexture(0, color_tex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glBindImageTexture(1, temp_tex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	glDispatchCompute(1024, 1, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glBindImageTexture(0, temp_tex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glBindImageTexture(1, color_tex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	glDispatchCompute((GLuint)gWidth, 1, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, color_tex);
	glDisable(GL_DEPTH_TEST);

	shaders[Shader_DisplayNightScene].use();

	shaders[Shader_DisplayNightScene].setFloat("focal_distance", focal_distance);
	shaders[Shader_DisplayNightScene].setFloat("focal_depth", focal_depth);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, Moon.getIndexSize());

	Moon.txtImage = &color_tex;
	Moon.draw();

	shaders[Shader_NightSceneTree].use();
	projection = perspective(camera.Zoom, (float)gWidth / (float)gHeight, 0.1f, 100.0f);//radians
	view = camera.GetViewMatrix();
	shaders[Shader_NightSceneTree].setMat4("projection", projection);
	shaders[Shader_NightSceneTree].setMat4("view", view);

	MakeModelMatricesIdentity();

	translationMatrix = translate(-3.5f, -2.0f, -4.5f);
	modelMatrix = translationMatrix * rotationMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	shaders[Shader_NightSceneTree].setMat4("model", modelMatrix);
	models[Model_Tree].Draw(shaders[Shader_NightSceneTree]);
	glDisable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_DEPTH_TEST);

}
float zMovement=0.0004f;
void SceneNight::UpdateScene()
{
	/*
	Note: If below update fails to work on your pc,
	try below code, comment existing update code 

	if (focal_distance > 15.05f)
	{
		focal_distance /= 1.21f;
	}
	else
	{
		camera.ProcessMouseMovement(0.0f, 4.5f, true);
		moon_scale += 0.9f;
		camera_z = camera_z * 2.5f;
		camera.ProcessKeyboard(FORWARD, camera_z);
	}
	*/

	if (focal_distance > 15.05f)
	{
		//focal_distance /= 1.1f;
		focal_distance /= 1.06f;
	}
	else
	{
		
		//if (camera_y < 3.5f)
		if (camera.Pitch <= 4.5f)
		{
			camera.ProcessMouseMovement(0.0f, 2.3f, true);
		}
		else
		{
			moon_scale += 0.6f;
			camera_z = camera_z * 1.3f;
			camera.ProcessKeyboard(FORWARD, camera_z);
		}
	}
}



void SceneNight::UninitializeScene()
{
	//ToDo: free fbo : depth_fbo

	// Delete Vertex buffers : vbo's 
	DeleteVertexBuffer(&vbo_rectangle_position_night);
	DeleteVertexBuffer(&vbo_rectangle_color_night);

	DeleteVertexBuffer(&vbo_static_star_position);
	DeleteVertexBuffer(&vbo_static_star_color);
	DeleteVertexBuffer(&vbo_static_star_texture);

	// Delete Vertex Arrays : vao's
	DeleteVertexArray(&vao_rectangle_night);
	DeleteVertexArray(&vao_static_star);
}
