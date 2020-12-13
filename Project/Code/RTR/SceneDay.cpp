#include"SceneDay.h"


int SceneDay::InitScene()
{
	//float MidPoint(float, float);

	const GLchar* vertexShaderSourceCodeCloud =
	{
		"#version 430 core"\
		"\n"\
		"uniform mat4 model;"\
		"uniform mat4 view;"\
		"uniform mat4 projection;"\

		"layout(location = 0) in vec4 vPos;"\
		"layout(location = 3) in vec2 vtexCoord;"\

		"out vec2 fsPos;"\
		"out vec2 fsTex;"\
		"void main(void)"\
		"{"\
		"gl_Position = projection * view * model * vPos;"\
		"fsPos = vPos.xy;"\
		"fsTex = vtexCoord;"\
		"}"\
	};

	const GLchar* fragmentShaderSourceCodeCloud =
	{
		"#version 430 core"\
		"\n"\
		"out vec4 FragColor;"\
		"uniform float dt;"\

		"vec3 C1 = vec3(0.1, 0.1, 1.0);"\
		"vec3 C2 = vec3(0.9, 0.9, 0.9);"\

		/*"vec3 C1 = vec3(1.0, 0.5, 0.0);"\
		"vec3 C2 = vec3(0.98, 0.93, 0.01);"\*/

		"in vec2 fsPos;"\
		"in vec2 fsTex;"\

		"uniform float alpha;"

		"float Noise1(ivec2 n)"\
		"{"\
		"int x = n[0] + n[1] * 57;"\
		"x = (x << 13) ^ x;"\
		"return (1.0 - ((x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);"\
		"}"\

		"float SmoothedNoise_1(ivec2 x)"\
		"{"\
		"return Noise1(x) / 2 + Noise1(x - 1) / 4 + Noise1(x + 1) / 4;"\
		"}"\

		"float Interpolate(float a, float b, float f)"\
		"{"\
		"float ft = f * 3.1415926;"\
		"float v = (1 - cos(ft)) * 0.5f;"\
		"return a * (1 - v) + b * v;"\
		"}"\

		"float InterpolatedNoise_1(vec2 x)"\
		"{"\

		"ivec2 integer_X = ivec2(x);"\
		"vec2 fractional_X = x - integer_X;"\

		"float v1 = SmoothedNoise_1(ivec2(integer_X));"\
		"float v2 = SmoothedNoise_1(ivec2(integer_X[0] + 1, integer_X[1]));"\
		"float v3 = SmoothedNoise_1(ivec2(integer_X[0], integer_X[1] + 1));"\
		"float v4 = SmoothedNoise_1(ivec2(integer_X[0] + 1, integer_X[1] + 1));"\

		"float i1 = Interpolate(v1, v2, fractional_X[0]);"\
		"float i2 = Interpolate(v3, v4, fractional_X[0]);"\

		"return Interpolate(i1, i2, fractional_X[1]);"\
		"}"\

		"float persistence = 1.0f / 1.30f;"\
		"int Number_Of_Octaves = 7;"\

		"float PerlinNoise_2D(vec2 x)"\
		"{"\

		"float total = 0;"\
		"float p = persistence;"\
		"int n = Number_Of_Octaves;"\
		"float frequency = 1.0f;"\
		"float amplitude = 1.0f;"\
		"for (int i = 1; i <= 2; i++)"\
		"{"\
		"frequency *= 2.0;"\
		"amplitude *= p;"\
		"total = total + InterpolatedNoise_1(x * frequency) * amplitude;"\
		"}"\

		"for (int i = 3; i <= n; i++)"\
		"{"\
		"frequency *= 2.0;"\
		"amplitude *= p;"\
		"total = total + InterpolatedNoise_1(x * frequency) * amplitude;"\
		"}"\
		"return total;"\
		"}"\


		"void main()"\
		"{"\
		"vec2 Tex = fsTex;"\

		"float f;"\
		"float f2 = 4.0;"\

		"f = PerlinNoise_2D(Tex + dt);"\
		"f = clamp(f * f2, 0.0, 1.0);"\
		"vec3 color = mix(C1, C2, f);"\

		"FragColor = vec4(color, alpha);"\
		"}"\

	};

	Shader objShader_Cloud(true, vertexShaderSourceCodeCloud, fragmentShaderSourceCodeCloud);


	shaders.push_back(objShader_Cloud);

	//time_uniform = glGetUniformLocation(gCloudShaderProgramObject, "dt");



	const GLfloat quadVertices[] = { -2.5f, -0.3f, 0.0f,
		2.5f, -0.3f, 0.0f,
		2.5f, 2.0f, 0.0f,

		-2.5f, -0.3f, 0.0f,
		2.5f, 2.0f, 0.0f,
		-2.5f, 2.0f, 0.0f };

	const float quadtex[] = { 0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f };

	/*Create vao_cloud*/
	glGenVertexArrays(1, &vao_cloud);
	glBindVertexArray(vao_cloud);
	glGenBuffers(1, &vbo_cloud_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cloud_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	/*Unbinding the buffer*/
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_cloud_texture);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cloud_texture);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadtex), quadtex, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD0);
	/*Unbinding the buffer*/
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//----------------------------------------------------------------------------------------------------------//

	const GLchar* grassvertexShaderSourceCode =
	{
		"#version 430 core"\
		"\n"\
		"in vec4 vPosition;"\
		"out vec4 color;"\

		"uniform mat4 model;"\
		"uniform mat4 view;"\
		"uniform mat4 projection;"\

		"layout (binding = 0) uniform sampler1D grasspalette_texture;"\
		"layout (binding = 1) uniform sampler2D length_texture;"\
		"layout (binding = 2) uniform sampler2D orientation_texture;"\
		"layout (binding = 3) uniform sampler2D grasscolor_texture;"\
		"layout (binding = 4) uniform sampler2D bend_texture;"\

		"int random(int seed, int iterations)"\
		"{"\
			"int value = seed;"\
			"int n;"\

			"for(n = 0; n < iterations; n++)"\
			"{"\
				"value = ((value >> 7) ^ (value << 9)) * 15485863;"\
			"}"\
			"return(value);"\
		"}"\

		"vec4 random_vector(int seed)"\
		"{"\
			"int r = random(gl_InstanceID, 4);"\
			"int g = random(r, 2);"\
			"int b = random(g, 2);"\
			"int a = random(b, 2);"\

			"return(vec4(float(r & 0x3FF) / 1024.0,"\
						"float(g & 0x3FF) / 1024.0,"\
						"float(b & 0x3FF) / 1024.0,"\
						"float(a & 0x3FF) / 1024.0));"\
		"}"\

		"mat4 construct_rotation_matrix(float angle)"\
		"{"\
			"float st = sin(angle);"\
			"float ct = cos(angle);"\

			"return(mat4(vec4(ct, 0.0, st, 0.0),"\
						"vec4(0.0, 1.0, 0.0, 0.0),"\
						"vec4(-st, 0.0, ct, 0.0),"\
						"vec4(0.0, 0.0, 0.0, 1.0)));"\
		"}"\

		"void main(void)"\
		"{"\
			"vec4 offset = vec4(float(gl_InstanceID >> 10) - 512.0, 0.0,"\
								"float(gl_InstanceID & 0x3FF) - 512.0, 0.0);"\

			"int number1 = random(gl_InstanceID, 3);"\
			"int number2 = random(number1, 2);"\

			"offset += vec4(float(number1 & 0xFF) / 256.0, 0.0f,"\
						   "float(number2 & 0xFF) / 256.0, 0.0f);"\

			"vec2 texcoord = offset.xz / 1024.0 + vec2(0.5);"\
			"float bend_factor = texture(bend_texture, texcoord).r * 2.0f;"\
			"float bend_amount = cos(vPosition.y);"\

			"float angle = texture(orientation_texture, texcoord).r * 2.0 * 3.141592;"\
			"mat4 rot = construct_rotation_matrix(angle);"\
			"vec4 position = (rot * (vPosition + vec4(0.0, 0.0, bend_amount * bend_factor, 0.0))) + offset;"\
			"position *= vec4(1.0f, texture(length_texture, texcoord).r * 0.9 + 0.3, 1.0, 1.0);"\

			"gl_Position =  projection * view * model * position;"\
			"color = texture(grasspalette_texture, texture(grasscolor_texture, texcoord).r)+"\
					"vec4(random_vector(gl_InstanceID).xyz * vec3(0.1, 0.5, 0.1), 1.0);"\
		"}"\
	};

	const GLchar* grassfragmentShaderSourceCode =
	{
		"#version 430 core"\
		"\n"\
		"in vec4 color;"\
		"out vec4 FragColor;"\
		"uniform float alpha;"\
		"void main(void)"\
		"{"\
		"FragColor = color * vec4(0.0,1.0,0.0,alpha);"
		"}"
	};

	Shader objShader_Grass(true, grassvertexShaderSourceCode, grassfragmentShaderSourceCode);

	shaders.push_back(objShader_Grass);

	const GLfloat grass_blade[] = { -0.3f, 0.0f,
									0.3f, 0.0f,
									-0.20f, 1.0f,
									0.1f, 1.3f,
									-0.05f, 2.3f,
									0.0f, 3.3f };

	/*Create vao_grass*/
	glGenVertexArrays(1, &vao_grass);
	glBindVertexArray(vao_grass);
	glGenBuffers(1, &vbo_grass);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_grass);
	glBufferData(GL_ARRAY_BUFFER, sizeof(grass_blade), grass_blade, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	/*Unbinding the buffer*/
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//----------------------------------------------------------------------------------------------------------//

	const GLchar* flowervertexShaderSourceCode =
	{
		"#version 430 core " \
		"\n " \
		"in vec4 vPosition;" \
		"in vec4 vColor;" \
		"uniform mat4 model;" \
		"uniform mat4 view;" \
		"uniform mat4 projection;" \
		"out vec4 out_color;" \
		"uniform vec2 offsets[100];"\
		"void main(void)" \
		"{" \
		"gl_Position=projection * view * model * vPosition;" \
		"vec2 offset = offsets[gl_InstanceID];"\
		"gl_Position[0] = gl_Position[0] + offset[0];"\
		"gl_Position[1] = gl_Position[1] + offset[1];"\
		"out_color = vColor;"\
		"} "
	};

	const GLchar* flowerfragmentShaderSourceCode =
	{
		"#version 430 core " \
		"\n " \
		"out vec4 fragColor;"
		"in vec4 out_color;"\
		"uniform float alpha;"\
		"void main(void)" \
		"{" \
			"fragColor = out_color;" \
			"fragColor[3] = alpha;"\
		"} "
	};

	Shader objShader_Flower(true, flowervertexShaderSourceCode, flowerfragmentShaderSourceCode);

	shaders.push_back(objShader_Flower);
	Point ctrlPoint[5];
	GLfloat angle = 0.0f;

	//Flower generation
	for (int icount = 0; icount < 5; icount++)
	{
		angle = (GLfloat)(2.0f * M_PI * icount / 5);
		ctrlPoint[icount].x = 0.25f * (GLfloat)cos(angle);
		ctrlPoint[icount].y = 0.25f * (GLfloat)sin(angle);
	}

	control_points[0].x = ctrlPoint[0].x;
	control_points[0].y = ctrlPoint[0].y;

	control_points[1].x = ctrlPoint[1].x;
	control_points[1].y = ctrlPoint[1].y;

	control_points[2].x = ctrlPoint[2].x;
	control_points[2].y = ctrlPoint[2].y;

	control_points[3].x = ctrlPoint[3].x;
	control_points[3].y = ctrlPoint[3].y;

	control_points[4].x = ctrlPoint[4].x;
	control_points[4].y = ctrlPoint[4].y;

	Prev_point = rand() % 5;
	P_Prev.x = control_points[Prev_point].x;
	P_Prev.y = control_points[Prev_point].y;

	for (int k = 0; k < 2500; k++)
	{
		Next_point = rand() % 5;
		P_Next.x = control_points[Next_point].x;
		P_Next.y = control_points[Next_point].y;

		P_New.x = MidPoint(P_New.x, P_Prev.x);
		P_New.y = MidPoint(P_New.y, P_Prev.y);

		flowerPoints.push_back(P_New.x);
		flowerPoints.push_back(P_New.y);
		flowerPoints.push_back(0.0f);

		P_Prev = P_Next;
		Prev_point = Next_point;

		flowerColor.push_back(1.0f);
		flowerColor.push_back(0.0f);
		flowerColor.push_back(0.5f);
		flowerColor.push_back(0.00455f);
	}

	//Create vao
	//Save everying in single set
	glGenVertexArrays(1, &vao_flower);

	glBindVertexArray(vao_flower);


	//TRIANGLE
	//Generate Buffer
	glGenBuffers(1, &vbo_flower_position);
	//Bind Generated Buffer
	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_flower_position);
	//Fill Buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * flowerPoints.size(), &flowerPoints[0], GL_STATIC_DRAW);
	//Set Vertex Attrib Pointer
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		NULL);
	//Enable Vertex Attrib Array
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	//Unbind Buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_flower_color);
	//Bind Generated Buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo_flower_color);
	//Fill Buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * flowerColor.size(), &flowerColor[0], GL_STATIC_DRAW);
	//Set Vertex Attrib Pointer
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	//Enable Vertex Attrib Array
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	//Unbind Buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Unbind array
	glBindVertexArray(0);

	circle_position.clear();
	circle_color.clear();
	radius = 0.03f;

	for (int i_count = 0; i_count < 5000; i_count++)
	{
		GLfloat angle = (GLfloat)(2 * M_PI * i_count) / 5000;

		/*center*/
		circle_position.push_back(0.0f);
		circle_position.push_back(0.0f);
		circle_position.push_back(0.0f);

		circle_position.push_back(cos(angle) * radius);
		circle_position.push_back(sin(angle) * radius);
		circle_position.push_back(0.0f);

		circle_position.push_back(prev_point_x);
		circle_position.push_back(prev_point_y);
		circle_position.push_back(prev_point_z);

		prev_point_x = cos(angle) * radius;
		prev_point_y = sin(angle) * radius;
		prev_point_z = 0.0f;

		circle_color.push_back(1.0f);
		circle_color.push_back(1.0f);
		circle_color.push_back(0.0f);
		circle_color.push_back(0.5f);

		circle_color.push_back(1.0f);
		circle_color.push_back(1.0f);
		circle_color.push_back(0.0f);
		circle_color.push_back(0.5f);

		circle_color.push_back(1.0f);
		circle_color.push_back(1.0f);
		circle_color.push_back(0.0f);
		circle_color.push_back(0.5f);
	}

	glGenVertexArrays(1, &vao_flower_circle);
	glBindVertexArray(vao_flower_circle);
	/*Position buffer*/
	glGenBuffers(1, &vbo_flower_circle_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_flower_circle_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * circle_position.size(), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	/*Unbinding buffer*/
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/*Color buffer*/
	glGenBuffers(1, &vbo_flower_circle_color);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_flower_circle_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * circle_color.size(), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	int index = 0;

	for (int y = -130; y < 30; y += 30)
	{
		for (int x = -120; x < 20; x += 10)
		{
			float offset1 = float(rand() % 7);
			float offset2 = float(rand() % 5);
			vec2 translation;
			translation[0] = (float)x / 15.0f + offset1;
			translation[1] = (float)y / 15.0f + offset2;
			translations[index++] = translation[0];
			translations[index++] = translation[1];
		}
	}

	/// ******* vertices, colors, shader attribs, vboPositionButterflySunrays, vaoButterflySunrays initialization *****
	const GLfloat butterflySunraysVertices[] = {
		0.0f, 0.0f, 0.0f,
		1.0f, -0.75f, 0.0f,
		1.0f, 0.75f, 0.0f,

		0.0f, 0.0f, 0.0f,
		-1.0f, -0.75f, 0.0f,
		-1.0f, 0.75f, 0.0f,

		0.11f, 0.4f, 0.0f,
		-0.11f, 0.4f, 0.0f,
		0.0f, -0.4f, 0.0f
	};

	const GLfloat butterflySunraysColors[] = {
		1.0f, 0.0f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.7f, 0.0f,
		1.0f, 0.7f, 0.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.7f, 0.0f
	};

	// create vaoButterflyFlower
	glGenVertexArrays(1, &vaoButterflySunrays); // how many arrays, address of array

	glBindVertexArray(vaoButterflySunrays); // start recording

	glGenBuffers(1, &vboPositionButterflySunrays); // how many buffers, address of buffer

	glBindBuffer(GL_ARRAY_BUFFER, vboPositionButterflySunrays); // bind point/target, buffer

	glBufferData(GL_ARRAY_BUFFER, // target / bind point
		sizeof(butterflySunraysVertices), // size of data
		butterflySunraysVertices, // actual data
		GL_STATIC_DRAW); // when to put data, right now, statically, not run time

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, // cpu attribute index
		3, // how many co-ordinates in each vertex 
		GL_FLOAT, // vertex data type
		GL_FALSE, // is vertex data normalised, no
		0, // stride, 
		NULL); // buffer offset: no stride hence do not need to cur buffer 

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION); // enable attribute array so that it can be seen by gpu

	glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind buffer array

									  //////////////
	glGenBuffers(1, &vboColorButterflySunrays); // how many buffers, address of buffer

	glBindBuffer(GL_ARRAY_BUFFER, vboColorButterflySunrays); // bind point/target, buffer

	glBufferData(GL_ARRAY_BUFFER, // target / bind point
		sizeof(butterflySunraysColors), // size of data
		butterflySunraysColors, // actual data
		GL_STATIC_DRAW); // when to put data, right now, statically, not run time

	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, // cpu attribute index
		3, // how many co-ordinates in each vertex 
		GL_FLOAT, // vertex data type
		GL_FALSE, // is vertex data normalised, no
		0, // stride, 
		NULL); // buffer offset: no stride hence do not need to cur buffer 

	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR); // enable attribute array so that it can be seen by gpu

	glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind buffer array

	glBindVertexArray(0); // stop recording in vaoButterflySunrays

						  //////////////**********************

	return 1;
}

void SceneDay::displayButterflySunRays(void)
{
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	MakeModelMatricesIdentity();

	// view/projection transformations
	projection = perspective(camera.Zoom, (float)gWidth / (float)gHeight, 0.1f, 100.0f);//radians
	view = camera.GetViewMatrix();

	shaders[Shader_ButterflySunrays].use();

	if (gbUseGeometryShader == TRUE)
	{
		translationMatrix = translate(0.0f, 0.0f, -65.0f);
		scaleMatrix = scale(0.9f, 0.7f, 0.0f);
	}
	else
	{
		translationMatrix = translate(-0.1f, -1.8f, -5.0f);
		scaleMatrix = scale(0.2f, 0.2f, 0.2f);
	}

	// multiply required metrices
	modelMatrix = translationMatrix * scaleMatrix;

	shaders[Shader_ButterflySunrays].setMat4("projection", projection);
	shaders[Shader_ButterflySunrays].setMat4("view", view);
	shaders[Shader_ButterflySunrays].setMat4("model", modelMatrix);

	if (gbUseGeometryShader == TRUE)
	{
		pointsToConvert = counter / 100;
		shaders[Shader_ButterflySunrays].setInt("u_useGeometryShader", 1);
		shaders[Shader_ButterflySunrays].setInt("u_pointsToConvert", pointsToConvert);
	}
	else
	{
		pointsToConvert = 8;
		shaders[Shader_ButterflySunrays].setInt("u_useGeometryShader", 0);
		shaders[Shader_ButterflySunrays].setInt("u_pointsToConvert", pointsToConvert);
	}

	// bind with vaoButterflyFlower (this will avoid many repeatative bindings with vboPositionButterflyFlower
	glBindVertexArray(vaoButterflySunrays);

	// draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
	// glDrawElements() is always better than glDrawArrays() 

	// bind with textures if any

	// draw necessory scene
	glDrawArrays(GL_TRIANGLES, // what to draw from vertices 
		0, // from where to start taking array elements
		9); // how may vertices to draw ; no of vertices

			// unbind vaoButterflyFlower
	glBindVertexArray(0);

	//glDisable(GL_BLEND);
}

float SceneDay::MidPoint(float x1, float x2)
{
	return((x1 + x2) / 3.0f);
}


void SceneDay::showCloud(void)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	shaders[Shader_Cloud].use();

	projection = perspective(camera.Zoom, (float)gWidth / (float)gHeight, 0.1f, 100.0f);//radians
	view = camera.GetViewMatrix();
	shaders[Shader_Cloud].setMat4("projection", projection);
	shaders[Shader_Cloud].setMat4("view", view);

	MakeModelMatricesIdentity();
	//Transformation

	translationMatrix = translate(0.0f, 0.0f, 0.0f);
	modelMatrix = translationMatrix * rotationMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	shaders[Shader_Cloud].setMat4("model", modelMatrix);

	/*Bind with vao_cloud (this will avoid any repetative binding with vbo_cloud_position)*/
	static unsigned int Time = GetTickCount();
	unsigned int timeNow = GetTickCount();
	float dt = float(timeNow - Time) / 100000.0f;
	shaders[Shader_Cloud].setFloat("dt", dt);
	shaders[Shader_Cloud].setFloat("alpha", DayAlpha);

	glBindVertexArray(vao_cloud);
	//Smilarly bind with texture if any
	//Draw necessary scene
	glDrawArrays(GL_TRIANGLES, 0, 6);

	/*Unbind vao_cloud*/
	glBindVertexArray(0);

	/*Unuse program*/
	glUseProgram(0);
	glDisable(GL_BLEND);
}

void SceneDay::showGrass(void)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glActiveTexture(GL_TEXTURE1);
	glActiveTexture(GL_TEXTURE2);
	glActiveTexture(GL_TEXTURE3);
	glActiveTexture(GL_TEXTURE4);

	/*###################### GRASS ########################*/

	float r = 500.0f;

	vec3 eye(0.0f, 25.0f, 5.0f);
	vec3 center(0.0f, -50.0f, 0.0f);
	vec3 up(0.0f, 1.0f, 0.0f);

	eye[0] = sinf(current_time) * r;
	eye[1] = 25.0f;
	eye[2] = cosf(current_time) * r;



	shaders[Shader_Grass].use();

	projection = perspective(camera.Zoom, (float)gWidth / (float)gHeight, 0.1f, 100.0f);//radians
	view = camera.GetViewMatrix();
	view = mat4::identity();
	view = mat4::identity();
	shaders[Shader_Grass].setMat4("projection", projection);
	shaders[Shader_Grass].setMat4("view", view);

	MakeModelMatricesIdentity();
	//Transformation
	modelMatrix = lookat(eye, center, up);

	//translationMatrix = translate(0.0f, -12.0f, 0.0f);
	//modelMatrix = translationMatrix * rotationMatrix;
	//modelMatrix = modelMatrix * scaleMatrix;

	shaders[Shader_Grass].setMat4("model", modelMatrix);


	/*Bind with vao_grass (this will avoid any repetative binding with vbo_grass)*/
	shaders[Shader_Grass].setFloat("alpha", DayAlpha);
	glBindVertexArray(vao_grass);
	//Smilarly bind with texture if any
	//Draw necessary scene

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textures[Texture_GrassLengh]);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textures[Texture_GrassOrientation]);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, textures[Texture_GrassColor]);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, textures[Texture_GrassBend]);


	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 6, 1024 * 1024);
	/*Unbind vao_grass*/
	glBindVertexArray(0);
	//glUseProgram(0);
	glDisable(GL_BLEND);

}

void SceneDay::showFlowers(void)
{
	/*Flower*/
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	shaders[Shader_Flower].use();

	projection = perspective(camera.Zoom, (float)gWidth / (float)gHeight, 0.1f, 100.0f);//radians
	view = camera.GetViewMatrix();
	shaders[Shader_Flower].setMat4("projection", projection);
	shaders[Shader_Flower].setMat4("view", view);

	MakeModelMatricesIdentity();
	//Transformation

	translationMatrix = translate(0.0f, 0.0f, -6.0f);
	modelMatrix = translationMatrix * rotationMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	shaders[Shader_Flower].setMat4("model", modelMatrix);


	//ASK
	//shaders[Shader_Grass].setVec2("offsets", translations);
	//glUniform2fv(flower_instanceUniform, 20, translations);

	shaders[Shader_Flower].setFloat("alpha", DayAlpha);
	glUniform2fv(glGetUniformLocation(shaders[Shader_Flower].ID, "offsets"), 20, translations);

	glBindVertexArray(vao_flower);

	glDrawArraysInstanced(GL_POINTS, 0, flowerPoints.size(), 20);

	glBindVertexArray(0);

	glBindVertexArray(vao_flower_circle);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_flower_circle_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * circle_color.size(), &circle_color[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_flower_circle_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * circle_position.size(), &circle_position[0], GL_DYNAMIC_DRAW);
	//glDrawArrays(GL_TRIANGLES, 0, circle_position.size());
	glDrawArraysInstanced(GL_TRIANGLES, 0, circle_position.size(), 20);
	glBindVertexArray(0);

	glUseProgram(0);
	glDisable(GL_BLEND);
}



void SceneDay::UpdateScene()
{
	if (timer > 0.0f)
	{
		timer -= 0.03f;
	}
	else
	{
		DayAlpha -= 0.004f;
		if (DayAlpha <= 0.0f)
			DayAlpha = 0.0f;
	}
}

void SceneDay::DisplayScene()
{
	glDisable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f,0.0f);
	showCloud();
	showGrass();
	//showTree();
	showFlowers();
	displayButterflySunRays();
	glEnable(GL_DEPTH_TEST);
}


void SceneDay::UninitializeScene()
{
	// Delete Vertex buffers : vbo's 
	DeleteVertexBuffer(&vbo_cloud_position);
	DeleteVertexBuffer(&vbo_cloud_texture);

	DeleteVertexBuffer(&vbo_flower_position);
	DeleteVertexBuffer(&vbo_flower_color);
	DeleteVertexBuffer(&vbo_flower_circle_position);
	DeleteVertexBuffer(&vbo_flower_circle_color);

	// Delete Vertex Arrays : vao's
	DeleteVertexArray(&vao_cloud);
	DeleteVertexArray(&vao_grass);
	DeleteVertexArray(&vao_flower);
	DeleteVertexArray(&vao_flower_circle);
}
