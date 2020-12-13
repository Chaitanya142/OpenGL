#include"SceneCouple.h"


int SceneCouple::InitScene()
{
	const GLchar* vertextShaderSourceCodeForHeart =
		"#version 430 core " \
		"\n " \
		"layout(location = 0) in vec3 vPosition;"\
		"layout(location = 1) in vec3 vNormal;"\
		"layout(location = 2) in vec2 vTexCoord;"\
		"uniform mat4 model;"\
		"uniform mat4 view;"\
		"uniform mat4 projection;"\
		"out vec2 TexCoords;"\
		"void main(void)" \
		"{" \
		"gl_Position=projection * view * model * vec4(vPosition, 1.0);" \
		"TexCoords = vTexCoord;"\
		"} ";


	const GLchar* fragementShaderSourceCodeForHeart =
		"#version 430 core " \
		"\n " \
		"out vec4 FragColor;"\
		"in vec2 TexCoords;"\
		"uniform sampler2D texture_diffuse1;"\
		"void main(void)" \
		"{" \
		"FragColor = texture(texture_diffuse1, TexCoords);" \
		"} ";


	Shader objShader_Character(true, vertextShaderSourceCodeForHeart, fragementShaderSourceCodeForHeart);
	shaders.push_back(objShader_Character);

	//----------------------------------------------------------------------------------------------------------//

	//----------------------------------------------------------------------------------------------------------//
	const GLchar *vertexShaderSourceCodeCouple =
		"#version 430 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec4 vColor;" \

		"uniform mat4 model;" \
		"uniform mat4 view;" \
		"uniform mat4 projection;" \

		"out vec4 out_color;" \

		"void main(void)" \
		"{" \
		"gl_Position = projection * view * model * vPosition;" \
		"out_color = vColor;" \
		"}";

	const GLchar *fragmentShaderSourceCodeCouple =
		"#version 430 core" \
		"\n" \
		"in vec4 out_color;" \

		"out vec4 FragColor;" \
		"uniform float alpha_val;"\
		"void main(void)" \
		"{" \
		"FragColor = vec4((vec3(out_color)), alpha_val);" \
		"}";

	Shader objShader_Couple(true, vertexShaderSourceCodeCouple, fragmentShaderSourceCodeCouple);

	shaders.push_back(objShader_Couple);


	//----------------------------------------------------------------------------------------------------------//
	const GLchar* heartVertexShaderSourceCode =
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

	const GLchar* heartFragmentShaderSourceCode =
	{
		"#version 430 core"\
		"\n"\
		"in vec2 out_texCoord;"\
		"uniform sampler2D tex_SingleHeart;"\
		"out vec4 FragColor;"\
		"uniform float Alpha;"\
		"void main(void)"\
		"{"\
			"FragColor = texture(tex_SingleHeart, out_texCoord);"\
			"FragColor.w = Alpha;"\
		"}"
	};

	Shader objShader_SingleHeart(true, heartVertexShaderSourceCode, heartFragmentShaderSourceCode);

	shaders.push_back(objShader_SingleHeart);

	//----------------------------------------------------------------------------------------------------------//


	///// Background1 
	const GLfloat background1VerticesPosition[] =
	{
		4.5f, 1.5f, 0.0f,
		-4.5f, 1.5f, 0.0f,
		-4.5f, -2.5f, 0.0f,
		4.5f, -2.5f, 0.0f
	};

	const GLfloat background1VerticesColor[] =
	{
		0.921f, 0.46f, 0.0f,
		0.921f, 0.46f, 0.0f,
		0.996f, 0.785f, 0.054f,
		0.996f, 0.785f, 0.054f
	};

	// create vao
	glGenVertexArrays(1, &vaoBackground1); // how many arrays, address of array

	glBindVertexArray(vaoBackground1); // start recordin

	///// ***********Background1Position
	glGenBuffers(1, &vboPositionBackground1); // how many buffers, address of buffer

	glBindBuffer(GL_ARRAY_BUFFER, vboPositionBackground1); // bind point/target, buffer

	glBufferData(GL_ARRAY_BUFFER, // target / bind point
		sizeof(background1VerticesPosition), // size of data
		background1VerticesPosition, // actual data
		GL_STATIC_DRAW); // when to put data, right now, statically, not run time

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, // cpu attribute index
		3, // how many co-ordinates in each vertex
		GL_FLOAT, // vertex data type
		GL_FALSE, // is vertex data normalised, no
		0, // stride, 
		NULL); // buffer offset: no stride hence do not need to cur buffer 

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION); // enable attribute array so that it can be seen by gpu

	glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind buffer array position
	/////////// ***********Background1PositionEnd

	///// *********** Background1 Color
	glGenBuffers(1, &vboColorBackground1); // how many buffers, buffer address

	glBindBuffer(GL_ARRAY_BUFFER, vboColorBackground1); // bind point, buffer

	glBufferData(GL_ARRAY_BUFFER, // target
		sizeof(background1VerticesColor), // size of data
		background1VerticesColor, // actual data
		GL_STATIC_DRAW); // statically put data

	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, // cpu attribute
		3, // how many co-ordinates in each vertex
		GL_FLOAT, // type of data
		GL_FALSE, // is data normalised : no
		0, // stride
		NULL); // offset

	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind buffer array background1 color

	glBindVertexArray(0); // stop recording in vao


    // create vao
	glGenVertexArrays(1, &vaoBackground2); // how many arrays, address of array

	glBindVertexArray(vaoBackground2); // start recording

	//// *********** Background2 Position
	//// *********** Background2 Color
	///// background2
	const GLfloat background2VerticesPosition[] =
	{
		4.5f, -2.5f, 0.0f,
		-4.5f, -2.5f, 0.0f,
		-4.5f, -3.5f, 0.0f,
		4.5f, -3.5f, 0.0f
	};

	const GLfloat background2VerticesColor[] =
	{
		0.5f, 1.0f, 0.0f,
		0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f
	};
	glGenBuffers(1, &vboPositionBackground2); // how many buffers, address of buffer

	glBindBuffer(GL_ARRAY_BUFFER, vboPositionBackground2); // bind point/target, buffer

	glBufferData(GL_ARRAY_BUFFER, // target / bind point
		sizeof(background2VerticesPosition), // size of data
		background2VerticesPosition, // actual data
		GL_STATIC_DRAW); // when to put data, right now, statically, not run time

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, // cpu attribute index
		3, // how many co-ordinates in each vertex
		GL_FLOAT, // vertex data type
		GL_FALSE, // is vertex data normalised, no
		0, // stride, 
		NULL); // buffer offset: no stride hence do not need to cur buffer 

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION); // enable attribute array so that it can be seen by gpu

	glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind buffer array Rectnagle position
	//// *********** Background2 Position End

	glGenBuffers(1, &vboColorBackgrond2); // how many buffers, buffer address

	glBindBuffer(GL_ARRAY_BUFFER, vboColorBackgrond2); // bind point, buffer

	glBufferData(GL_ARRAY_BUFFER, // target
		sizeof(background2VerticesColor), // size of data
		background2VerticesColor, // actual data
		GL_STATIC_DRAW); // statically put data

	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, // cpu attribute
		3, // how many co-ordinates in each vertex
		GL_FLOAT, // type of data
		GL_FALSE, // is data normalized: no
		0, // stride
		NULL); // buffer offset

	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

	// Why: It will work?
	// Because we have done prelink binding for AMC_ATTRIBUTE_COLOR with vColor
	// Here we want to give single color for rectange hence above line is enough.
	// Where multiple colors are required all above steps are mandetory.
	//glVertexAttrib3f(AMC_ATTRIBUTE_COLOR, 0.0f, 0.0f, 1.0f);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind rectnagle color buffer
	//// *********** Background2 Color End

	glBindVertexArray(0); // stop recording in vao

	sphereBody.setTextureColor(0, 0, 0);
	sphereBody.set(1.0, 30, 30);

	sphereHead.setTextureColor(0, 0, 0);
	sphereHead.set(0.5, 30, 30);

	///// man Leg
	const GLfloat manLegVerticesPosition[] =
	{
		0.07f, 0.3f, 0.0f,
		-0.07f, 0.3f, 0.0f,
		-0.08f, -0.27f, 0.0f,
		-0.04f, -0.27f, 0.0f
	};

	const GLfloat manLegVerticesColor[] =
	{
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f
	};

	// create vao
	glGenVertexArrays(1, &vaoManLeg); // how many arrays, address of array

	glBindVertexArray(vaoManLeg); // start recordin

	//// ***********ManLegPosition
	glGenBuffers(1, &vboPositionManLeg); // how many buffers, address of buffer

	glBindBuffer(GL_ARRAY_BUFFER, vboPositionManLeg); // bind point/target, buffer

	glBufferData(GL_ARRAY_BUFFER, // target / bind point
		sizeof(manLegVerticesPosition), // size of data
		manLegVerticesPosition, // actual data
		GL_STATIC_DRAW); // when to put data, right now, statically, not run time

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, // cpu attribute index
		3, // how many co-ordinates in each vertex
		GL_FLOAT, // vertex data type
		GL_FALSE, // is vertex data normalised, no
		0, // stride, 
		NULL); // buffer offset: no stride hence do not need to cur buffer 

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION); // enable attribute array so that it can be seen by gpu

	glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind buffer array position
	/////////// ***********ManLegPositionEnd

	///// *********** Man Leg Color
	glGenBuffers(1, &vboColorManLeg); // how many buffers, buffer address

	glBindBuffer(GL_ARRAY_BUFFER, vboColorManLeg); // bind point, buffer

	glBufferData(GL_ARRAY_BUFFER, // target
		sizeof(manLegVerticesColor), // size of data
		manLegVerticesColor, // actual data
		GL_STATIC_DRAW); // statically put data

	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, // cpu attribute
		3, // how many co-ordinates in each vertex
		GL_FLOAT, // type of data
		GL_FALSE, // is data normalised : no
		0, // stride
		NULL); // offset

	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind buffer array benchLeg color

	glBindVertexArray(0); // stop recording in vao

	/*Women Leg*/
	const GLfloat WomanLegVerticesPosition[] =
	{
		0.07f, 0.3f, 0.0f,
		-0.07f, 0.3f, 0.0f,
		-0.2f, -0.27f, 0.0f,
		-0.15f, -0.27f, 0.0f
	};

	const GLfloat WomanLegVerticesColor[] =
	{
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f
	};
	// create vao
	glGenVertexArrays(1, &vaoWomanLeg); // how many arrays, address of array

	glBindVertexArray(vaoWomanLeg); // start recordin

	//// ***********ManLegPosition
	glGenBuffers(1, &vboPositionWomanLeg); // how many buffers, address of buffer

	glBindBuffer(GL_ARRAY_BUFFER, vboPositionWomanLeg); // bind point/target, buffer

	glBufferData(GL_ARRAY_BUFFER, // target / bind point
		sizeof(WomanLegVerticesPosition), // size of data
		WomanLegVerticesPosition, // actual data
		GL_STATIC_DRAW); // when to put data, right now, statically, not run time

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, // cpu attribute index
		3, // how many co-ordinates in each vertex
		GL_FLOAT, // vertex data type
		GL_FALSE, // is vertex data normalised, no
		0, // stride, 
		NULL); // buffer offset: no stride hence do not need to cur buffer 

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION); // enable attribute array so that it can be seen by gpu

	glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind buffer array position
	/////////// ***********ManLegPositionEnd

	///// *********** Man Leg Color
	glGenBuffers(1, &vboColorManLeg); // how many buffers, buffer address

	glBindBuffer(GL_ARRAY_BUFFER, vboColorManLeg); // bind point, buffer

	glBufferData(GL_ARRAY_BUFFER, // target
		sizeof(WomanLegVerticesColor), // size of data
		WomanLegVerticesColor, // actual data
		GL_STATIC_DRAW); // statically put data

	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, // cpu attribute
		3, // how many co-ordinates in each vertex
		GL_FLOAT, // type of data
		GL_FALSE, // is data normalised : no
		0, // stride
		NULL); // offset

	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind buffer array benchLeg color

	glBindVertexArray(0); // stop recording in vao

	///////////////////////////////////////////

	///// Bench
	const GLfloat benchVerticesPosition[] =
	{
		2.0f, 0.1f, 0.0f,
		-2.0f, 0.1f, 0.0f,
		-2.0f, -0.1f, 0.0f,
		2.0f, -0.1f, 0.0f
	};

	const GLfloat benchVerticesColor[] =
	{
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f
	};

	// create vao
	glGenVertexArrays(1, &vaoBench); // how many arrays, address of array

	glBindVertexArray(vaoBench); // start recordin

	///// ***********BenchPosition
	glGenBuffers(1, &vboPositionBench); // how many buffers, address of buffer

	glBindBuffer(GL_ARRAY_BUFFER, vboPositionBench); // bind point/target, buffer

	glBufferData(GL_ARRAY_BUFFER, // target / bind point
		sizeof(benchVerticesPosition), // size of data
		benchVerticesPosition, // actual data
		GL_STATIC_DRAW); // when to put data, right now, statically, not run time

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, // cpu attribute index
		3, // how many co-ordinates in each vertex
		GL_FLOAT, // vertex data type
		GL_FALSE, // is vertex data normalised, no
		0, // stride, 
		NULL); // buffer offset: no stride hence do not need to cur buffer 

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION); // enable attribute array so that it can be seen by gpu

	glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind buffer array position
	/////////// ***********BenchPositionEnd

	///// *********** Bench Color
	glGenBuffers(1, &vboColorBench); // how many buffers, buffer address

	glBindBuffer(GL_ARRAY_BUFFER, vboColorBench); // bind point, buffer

	glBufferData(GL_ARRAY_BUFFER, // target
		sizeof(benchVerticesColor), // size of data
		benchVerticesColor, // actual data
		GL_STATIC_DRAW); // statically put data

	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, // cpu attribute
		3, // how many co-ordinates in each vertex
		GL_FLOAT, // type of data
		GL_FALSE, // is data normalised : no
		0, // stride
		NULL); // offset

	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind buffer array background1 color

	glBindVertexArray(0); // stop recording in vao

	///// Bench Leg
	const GLfloat benchLegVerticesPosition[] =
	{
		0.07f, 0.7f, 0.0f,
		-0.07f, 0.7f, 0.0f,
		-0.07f, -0.7f, 0.0f,
		0.07f, -0.7f, 0.0f
	};

	const GLfloat benchLegVerticesColor[] =
	{
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f
	};

	// create vao
	glGenVertexArrays(1, &vaoBenchLeg); // how many arrays, address of array

	glBindVertexArray(vaoBenchLeg); // start recordin

	//// ***********BenchLegPosition
	glGenBuffers(1, &vboPositionBenchLeg); // how many buffers, address of buffer

	glBindBuffer(GL_ARRAY_BUFFER, vboPositionBenchLeg); // bind point/target, buffer

	glBufferData(GL_ARRAY_BUFFER, // target / bind point
		sizeof(benchLegVerticesPosition), // size of data
		benchLegVerticesPosition, // actual data
		GL_STATIC_DRAW); // when to put data, right now, statically, not run time

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, // cpu attribute index
		3, // how many co-ordinates in each vertex
		GL_FLOAT, // vertex data type
		GL_FALSE, // is vertex data normalised, no
		0, // stride, 
		NULL); // buffer offset: no stride hence do not need to cur buffer 

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION); // enable attribute array so that it can be seen by gpu

	glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind buffer array position
	/////////// ***********BenchLegPositionEnd

	///// *********** Bench Leg Color
	glGenBuffers(1, &vboColorBenchLeg); // how many buffers, buffer address

	glBindBuffer(GL_ARRAY_BUFFER, vboColorBenchLeg); // bind point, buffer

	glBufferData(GL_ARRAY_BUFFER, // target
		sizeof(benchLegVerticesColor), // size of data
		benchLegVerticesColor, // actual data
		GL_STATIC_DRAW); // statically put data

	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, // cpu attribute
		3, // how many co-ordinates in each vertex
		GL_FLOAT, // type of data
		GL_FALSE, // is data normalised : no
		0, // stride
		NULL); // offset

	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind buffer array benchLeg color

	glBindVertexArray(0); // stop recording in vao


	/*center*/
	circle_body_position.push_back(0.0f);
	circle_body_position.push_back(0.0f);
	circle_body_position.push_back(0.0f);

	/*Circle*/
	for (int i_count = 0; i_count <= circlePoints; i_count++)
	{
		GLfloat angle = (GLfloat)(2 * M_PI * i_count) / circlePoints;

		circle_body_position.push_back(cos(angle) * radius_body);
		circle_body_position.push_back(sin(angle) * radius_body);
		circle_body_position.push_back(0.0f);

		circle_body_position.push_back(body_prev_point_x);
		circle_body_position.push_back(body_prev_point_y);
		circle_body_position.push_back(body_prev_point_z);

		body_prev_point_x = cos(angle) * radius_body;
		body_prev_point_y = sin(angle) * radius_body;
		body_prev_point_z = 0.0f;


		circle_body_color.push_back(1.0f);
		circle_body_color.push_back(1.0f);
		circle_body_color.push_back(0.878f);
		circle_body_color.push_back(0.5f);

		circle_body_color.push_back(1.0f);
		circle_body_color.push_back(0.843f);
		circle_body_color.push_back(0.0f);
		circle_body_color.push_back(0.5f);

		circle_body_color.push_back(1.0f);
		circle_body_color.push_back(0.843f);
		circle_body_color.push_back(0.0f);
		circle_body_color.push_back(0.5f);
	}

	glGenVertexArrays(1, &vao_circle);
	glBindVertexArray(vao_circle);
	/*Position buffer*/
	glGenBuffers(1, &vbo_circle_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_circle_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * circle_body_position.size(), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	/*Unbinding buffer*/
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/*Color buffer*/
	glGenBuffers(1, &vbo_circle_color);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_circle_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * circle_body_color.size(), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	/*Create vao_rectangle for RECTANGLE*/
	glGenVertexArrays(1, &vao_lamp);
	glBindVertexArray(vao_lamp);
	glGenBuffers(1, &vbo_lamp_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_lamp_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 3, NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	/*Unbinding vertex array*/
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_lamp_color);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_lamp_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 3, NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	/*Unbinding the buffer*/
	glBindVertexArray(0);

	for (int i = 0; i <= circlePoints; i++)
	{
		lamp_color.push_back(0.0f);
		lamp_color.push_back(0.0f);
		lamp_color.push_back(0.0f);
		lamp_color.push_back(1.0f);

		lamp_color.push_back(0.0f);
		lamp_color.push_back(0.0f);
		lamp_color.push_back(0.0f);
		lamp_color.push_back(1.0f);

		lamp_color.push_back(0.0f);
		lamp_color.push_back(0.0f);
		lamp_color.push_back(0.0f);
		lamp_color.push_back(1.0f);
	}

	Alpha_value = 0.0f;
	couple_alpha_value = 1.0f;
	heart_translate_y = -0.1f;
	heart_translate_x = 0.0f;
	return 1;
}

void SceneCouple::DisplayScene()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_FRONT_AND_BACK);

	float lampPosition[12];
	float lampColor[12];

	shaders[Shader_Couple].use();

	projection = perspective(camera.Zoom, (float)gWidth / (float)gHeight, 0.1f, 100.0f);//radians
	view = camera.GetViewMatrix();


	shaders[Shader_Couple].setMat4("projection", projection);
	shaders[Shader_Couple].setMat4("view", view);

	//////////// **********Background1

	MakeModelMatricesIdentity();
	//Transformation

	translationMatrix = translate(0.0f, 1.0f, -3.05f);
	modelMatrix = translationMatrix * rotationMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	shaders[Shader_Couple].setMat4("model", modelMatrix);
	shaders[Shader_Couple].setFloat("alpha_val", couple_alpha_value);

	// bind with vao (this will avoid many repeatative bindings with vbo
	glBindVertexArray(vaoBackground1);

	// bind with textures if any

	// draw necessory scene
	glDrawArrays(GL_TRIANGLE_FAN, // what to draw from vertices 
		0, // from where to start taking array elements
		4); // how may vertices to draw ; 4 for quads

	// unbind vao
	glBindVertexArray(0);

	glBindVertexArray(vaoBackground2);

	// bind with textures if any

	// draw necessory scene
	glDrawArrays(GL_TRIANGLE_FAN, // what to draw from vertices 
		0, // from where to start taking array elements
		4); // how may vertices to draw ; 4 for quads

	// unbind vao
	glBindVertexArray(0);



	MakeModelMatricesIdentity();
	//Transformation

	translationMatrix = translate(0.0f, 0.1f, -3.0f);
	modelMatrix = translationMatrix * rotationMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	shaders[Shader_Couple].setMat4("model", modelMatrix);

	glBindVertexArray(vao_circle);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_circle_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * circle_body_color.size(), &circle_body_color[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_circle_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * circle_body_position.size(), &circle_body_position[0], GL_DYNAMIC_DRAW);
	glDrawArrays(GL_TRIANGLE_FAN, 0, circle_body_position.size());
	glBindVertexArray(0);

	/////////// **********Man Legs Left

	MakeModelMatricesIdentity();
	//Transformation

	translationMatrix = translate(boyTranslateX, -1.0f, -3.0f);
	modelMatrix = translationMatrix * rotationMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	shaders[Shader_Couple].setMat4("model", modelMatrix);

	// bind with vao
	glBindVertexArray(vaoManLeg);

	// bind with textures if any

	// draw necessory scene
	glDrawArrays(GL_TRIANGLE_FAN, // what to draw from vertices
		0, // from where to start array elements
		4); // how many vertices

	// unbind vao
	glBindVertexArray(0);

	/*Man leg right*/

	MakeModelMatricesIdentity();
	//Transformation

	translationMatrix = translate(boyTranslateX - 0.2f, -1.0f, -3.0f);
	modelMatrix = translationMatrix * rotationMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	shaders[Shader_Couple].setMat4("model", modelMatrix);

	// bind with vao
	glBindVertexArray(vaoManLeg);

	// bind with textures if any

	// draw necessory scene
	glDrawArrays(GL_TRIANGLE_FAN, // what to draw from vertices
		0, // from where to start array elements
		4); // how many vertices

	// unbind vao
	glBindVertexArray(0);

	/*Women left leg*/

	MakeModelMatricesIdentity();
	//Transformation

	translationMatrix = translate(girlTranslateX - 0.2f, -1.0f, -3.0f);
	modelMatrix = translationMatrix * rotationMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	shaders[Shader_Couple].setMat4("model", modelMatrix);

	// bind with vao
	glBindVertexArray(vaoWomanLeg);

	// bind with textures if any

	// draw necessory scene
	glDrawArrays(GL_TRIANGLE_FAN, // what to draw from vertices
		0, // from where to start array elements
		4); // how many vertices

	// unbind vao
	glBindVertexArray(0);

	/*Women  Right leg*/
	/*Man leg right*/
	MakeModelMatricesIdentity();
	//Transformation

	translationMatrix = translate(girlTranslateX, -1.0f, -3.0f);
	modelMatrix = translationMatrix * rotationMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	shaders[Shader_Couple].setMat4("model", modelMatrix);

	// bind with vao
	glBindVertexArray(vaoManLeg);

	// bind with textures if any

	// draw necessory scene
	glDrawArrays(GL_TRIANGLE_FAN, // what to draw from vertices
		0, // from where to start array elements
		4); // how many vertices

	// unbind vao
	glBindVertexArray(0);
	////////////////////////////////////////
	//Bench
	/////////// **********Bench1

	MakeModelMatricesIdentity();
	//Transformation

	translationMatrix = translate(-0.2f, -0.2f, -2.5f);
	modelMatrix = translationMatrix * rotationMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	shaders[Shader_Couple].setMat4("model", modelMatrix);

	// bind with vao
	glBindVertexArray(vaoBench);

	// bind with textures if any

	// draw necessory scene
	glDrawArrays(GL_TRIANGLE_FAN, // what to draw from vertices
		0, // from where to start array elements
		4); // how many vertices

	// unbind vao
	glBindVertexArray(0);

	/////////// **********Bench2
	// initialize matrices to identity

	MakeModelMatricesIdentity();
	//Transformation

	translationMatrix = translate(-0.2f, -0.5f, -2.5f);
	modelMatrix = translationMatrix * rotationMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	shaders[Shader_Couple].setMat4("model", modelMatrix);

	// bind with vao
	glBindVertexArray(vaoBench);

	// bind with textures if any

	// draw necessory scene
	glDrawArrays(GL_TRIANGLE_FAN, // what to draw from vertices
		0, // from where to start array elements
		4); // how many vertices

	// unbind vao
	glBindVertexArray(0);

	/////////// **********Bench3

	MakeModelMatricesIdentity();
	//Transformation

	translationMatrix = translate(-0.2f, -0.8f, -2.5f);
	modelMatrix = translationMatrix * rotationMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	shaders[Shader_Couple].setMat4("model", modelMatrix);

	// bind with vao
	glBindVertexArray(vaoBench);

	// bind with textures if any

	// draw necessory scene
	glDrawArrays(GL_TRIANGLE_FAN, // what to draw from vertices
		0, // from where to start array elements
		4); // how many vertices

	// unbind vao
	glBindVertexArray(0);

	/////////// **********Bench Legs Left

	MakeModelMatricesIdentity();
	//Transformation

	translationMatrix = translate(-1.7f, -0.7f, -2.5f);
	modelMatrix = translationMatrix * rotationMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	shaders[Shader_Couple].setMat4("model", modelMatrix);

	// bind with vao
	glBindVertexArray(vaoBenchLeg);

	// bind with textures if any

	// draw necessory scene
	glDrawArrays(GL_TRIANGLE_FAN, // what to draw from vertices
		0, // from where to start array elements
		4); // how many vertices

	// unbind vao
	glBindVertexArray(0);

	/////////// **********Bench Legs Right

	MakeModelMatricesIdentity();
	//Transformation

	translationMatrix = translate(1.3f, -0.7f, -2.5f);
	modelMatrix = translationMatrix * rotationMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	shaders[Shader_Couple].setMat4("model", modelMatrix);


	// bind with vao
	glBindVertexArray(vaoBenchLeg);

	// bind with textures if any

	// draw necessory scene
	glDrawArrays(GL_TRIANGLE_FAN, // what to draw from vertices
		0, // from where to start array elements
		4); // how many vertices

	// unbind vao
	glBindVertexArray(0);

	/*Draw Lamp*/
	lampPosition[0] = 0.05f;
	lampPosition[1] = 1.0f;
	lampPosition[2] = 0.0f;

	lampPosition[3] = -0.05f;
	lampPosition[4] = 1.0f;
	lampPosition[5] = 0.0f;

	lampPosition[6] = -0.1f;
	lampPosition[7] = -1.5f;
	lampPosition[8] = 0.0f;

	lampPosition[9] = 0.1f;
	lampPosition[10] = -1.5f;
	lampPosition[11] = 0.0f;

	lampColor[0] = 0.0f;
	lampColor[1] = 0.0f;
	lampColor[2] = 0.0f;

	lampColor[3] = 0.0f;
	lampColor[4] = 0.0f;
	lampColor[5] = 0.0f;

	lampColor[6] = 0.0f;
	lampColor[7] = 0.0f;
	lampColor[8] = 0.0f;

	lampColor[9] = 0.0f;
	lampColor[10] = 0.0f;
	lampColor[11] = 0.0f;

	MakeModelMatricesIdentity();
	//Transformation

	translationMatrix = translate(-2.7f, 0.0f, -2.5f);
	modelMatrix = translationMatrix * rotationMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	shaders[Shader_Couple].setMat4("model", modelMatrix);

	glBindVertexArray(vao_lamp);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_lamp_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lampColor), lampColor, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_lamp_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lampPosition), lampPosition, GL_DYNAMIC_DRAW);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	/*Lamp bulb*/
	MakeModelMatricesIdentity();
	//Transformation
	translationMatrix = translate(-2.7f, 1.1f, -2.5f);
	scaleMatrix = scale(0.15f, 0.15f, 0.15f);
	modelMatrix = translationMatrix * rotationMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	shaders[Shader_Couple].setMat4("model", modelMatrix);

	glBindVertexArray(vao_circle);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_circle_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * lamp_color.size(), &lamp_color[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_circle_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * circle_body_position.size(), &circle_body_position[0], GL_DYNAMIC_DRAW);
	glDrawArrays(GL_TRIANGLE_FAN, 0, circle_body_position.size());
	glBindVertexArray(0);

	shaders[Shader_Character].use();

	projection = perspective(camera.Zoom, (float)gWidth / (float)gHeight, 0.1f, 100.0f);//radians
	view = camera.GetViewMatrix();
	shaders[Shader_Character].setMat4("projection", projection);
	shaders[Shader_Character].setMat4("view", view);

	//////////// **********Background1

	MakeModelMatricesIdentity();
	//Transformation

	translationMatrix = translate(boyTranslateX, 0.2f, -3.0f);
	scaleMatrix = scale(0.45f, 0.45f, 0.45f);
	modelMatrix = translationMatrix * rotationMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	shaders[Shader_Character].setMat4("model", modelMatrix);
	sphereHead.draw();

	///////////////////////////////////////////////////
	/////////// ********** Man body

	MakeModelMatricesIdentity();
	//Transformation

	translationMatrix = translate(boyTranslateX, -0.4f, -3.0f);
	scaleMatrix = scale(0.8f, 0.9f, 0.9f);
	modelMatrix = translationMatrix * rotationMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	shaders[Shader_Character].setMat4("model", modelMatrix);

	sphereHead.draw();

	///////////////////////////////////////////////////
	/////////// ********** Woman head

	MakeModelMatricesIdentity();
	//Transformation

	translationMatrix = translate(girlTranslateX, 0.1f, -3.0f);
	scaleMatrix = scale(0.42f, 0.42f, 0.42f);
	modelMatrix = translationMatrix * rotationMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	shaders[Shader_Character].setMat4("model", modelMatrix);

	sphereHead.draw();


	///////////////////////////////////////////////////
	/////////// ********** Woman body

	MakeModelMatricesIdentity();
	//Transformation

	translationMatrix = translate(girlTranslateX, -0.4f, -3.0f);
	scaleMatrix = scale(0.35f, 0.42f, 0.50f);
	modelMatrix = translationMatrix * rotationMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	shaders[Shader_Character].setMat4("model", modelMatrix);

	sphereBody.draw();

	if (translateDone == true)
	{
		shaders[Shader_SingleHeart].use();

		projection = perspective(camera.Zoom, (float)gWidth / (float)gHeight, 0.1f, 100.0f);//radians
		view = camera.GetViewMatrix();
		shaders[Shader_SingleHeart].setMat4("projection", projection);
		shaders[Shader_SingleHeart].setMat4("view", view);

		MakeModelMatricesIdentity();
		//Transformation
		rotationMatrix = rotate(-90.0f, 1.0f, 0.0f, 0.0f);
		translationMatrix = translate(-0.25f, -heart_translate_y, -2.0f);
		scaleMatrix = scale(scale_value, scale_value, scale_value);
	
		//scaleMatrix = scale(0.03f, 0.03f, 0.03f);

		modelMatrix = translationMatrix * rotationMatrix;
										   
		modelMatrix = modelMatrix * scaleMatrix;

		shaders[Shader_SingleHeart].setMat4("model", modelMatrix);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[Texture_Heart_Pink]);

		shaders[Shader_SingleHeart].setInt("tex_SingleHeart", 0);
		shaders[Shader_SingleHeart].setFloat("Alpha", Alpha_value);

		models[Model_Heart].Draw(shaders[Shader_SingleHeart]);
	}

	glDisable(GL_BLEND);
	glDisable(GL_FRONT_AND_BACK);

}

void SceneCouple::UpdateScene()
{
	if (first == false)
	{
		/*Reset camera*/
		camera.ResetCamera();
		//camera.ProcessKeyboard(FORWARD, 2.5f);
		camera.ProcessKeyboard(FORWARD, 2.2f);
		first = true;
	}
	else
	{
		//if (couple_camera_z < 0.00188651297f)
		if (couple_camera_z < 0.0058f)
		{
			camera.ProcessKeyboard(BACKWARD, couple_camera_z);
			couple_camera_z += 0.000008f;
		}
		else
		{
			boyTranslateX += 0.00165f;
			if (boyTranslateX >= -0.5f)
			{
				boyTranslateX = -0.5f;
			}
			girlTranslateX -= 0.00165f;
			if (girlTranslateX <= 0.0f)
			{
				girlTranslateX = 0.0f;
				translateDone = true;
			}
		}
	}

	if (translateDone == true)
	{
		Alpha_value += 0.01f;
		if (Alpha_value >= 1.0f)
		{
			heart_translate_y = heart_translate_y + 0.05f;
			couple_alpha_value = couple_alpha_value - 0.005f;
			scale_value = scale_value + 0.005f;
		}
	}
}

void SceneCouple::UninitializeScene()
{
	// Delete Vertex buffers : vbo's 
	DeleteVertexBuffer(&vboPositionBackground1);
	DeleteVertexBuffer(&vboColorBackground1);
	DeleteVertexBuffer(&vboPositionBackground2);
	DeleteVertexBuffer(&vboColorBackgrond2);

	DeleteVertexBuffer(&vboPositionManLeg);
	DeleteVertexBuffer(&vboColorManLeg);
	DeleteVertexBuffer(&vboPositionWomanLeg);
	DeleteVertexBuffer(&vboColorManLeg);

	DeleteVertexBuffer(&vboPositionBench);
	DeleteVertexBuffer(&vboPositionBenchLeg);
	DeleteVertexBuffer(&vboColorBenchLeg);
	DeleteVertexBuffer(&vboColorBench);

	DeleteVertexBuffer(&vbo_lamp_position);
	DeleteVertexBuffer(&vbo_circle_color);
	DeleteVertexBuffer(&vbo_lamp_color);
	DeleteVertexBuffer(&vbo_circle_position);

	// Delete Vertex Arrays : vao's
	DeleteVertexArray(&vaoBackground1);
	DeleteVertexArray(&vaoBackground2);

	DeleteVertexArray(&vaoManLeg);
	DeleteVertexArray(&vaoWomanLeg);

	DeleteVertexArray(&vaoBench);
	DeleteVertexArray(&vaoBenchLeg);

	DeleteVertexArray(&vao_lamp);
	DeleteVertexArray(&vao_circle);
}
