#pragma once
#include"SceneTerrain.h"
#include"Textures.h"

int SceneTerrain::InitScene()
{
	//----------------------------------------------------------------------------------------------------------//
	//Terrain Scene
	//----------------------------------------------------------------------------------------------------------//
	const GLchar *vertexShaderSourceCodeTerrain =
		"#version 430 core" \
		"\n" \
		"out VS_OUT" \
		"{" \
		"vec2 tc;" \
		"} vs_out;" \

		"void main(void)" \
		"{" \
		"const vec4 vertices[] = vec4[](vec4(-0.5, 0.0, -0.5, 1.0)," \
		"vec4(0.5, 0.0, -0.5, 1.0)," \
		"vec4(-0.5, 0.0, 0.5, 1.0)," \
		"vec4(0.5, 0.0, 0.5, 1.0));" \

		"int x = gl_InstanceID & 63;" \
		"int y = gl_InstanceID >> 6;" \
		"vec2 offs = vec2(x,y);" \

		"vs_out.tc = (vertices[gl_VertexID].xz + offs + vec2(0.5)) / 64.0;" \
		"gl_Position = vertices[gl_VertexID] + vec4(float(x-32),0.0, float(y-32), 0.0);" \
		"}";

	const GLchar *tessellationControlShaderSourceCodeTerrain =
		"#version 430" \
		"\n" \
		"layout(vertices=4)out;" \

		"in VS_OUT" \
		"{" \
		"vec2 tc;" \
		"}tcs_in[];" \

		"out TCS_OUT" \
		"{" \
		"vec2 tc;" \
		"} tcs_out[];" \

		"uniform mat4 model;" \
		"uniform mat4 view;" \
		"uniform mat4 projection;" \

		"mat4 u_mvpUniform = projection * view * model;" \

		"void main(void)" \
		"{" \
		"if (gl_InvocationID == 0)" \
		"{" \
		"vec4 p0 = u_mvpUniform * gl_in[0].gl_Position;" \
		"vec4 p1 = u_mvpUniform * gl_in[1].gl_Position;" \
		"vec4 p2 = u_mvpUniform * gl_in[2].gl_Position;" \
		"vec4 p3 = u_mvpUniform * gl_in[3].gl_Position;" \
		"p0 /= p0.w;" \
		"p1 /= p1.w;" \
		"p2 /= p2.w;" \
		"p3 /= p3.w;" \
		"if (p0.z <= 0.0 || " \
		"p1.z <= 0.0 || " \
		"p2.z <= 0.0 || " \
		"p3.z <= 0.0)" \
		"{" \
		"gl_TessLevelOuter[0] = 0.0;" \
		"gl_TessLevelOuter[1] = 0.0;" \
		"gl_TessLevelOuter[2] = 0.0;" \
		"gl_TessLevelOuter[3] = 0.0;" \
		"}" \
		"else" \
		"{" \
		"float l0 = length(p2.xy - p0.xy) * 16.0 + 1.0;" \
		"float l1 = length(p3.xy - p2.xy) * 16.0 + 1.0;" \
		"float l2 = length(p3.xy - p1.xy) * 16.0 + 1.0;" \
		"float l3 = length(p1.xy - p0.xy) * 16.0 + 1.0;" \
		"gl_TessLevelOuter[0] = l0;" \
		"gl_TessLevelOuter[1] = l1;" \
		"gl_TessLevelOuter[2] = l2;" \
		"gl_TessLevelOuter[3] = l3;" \
		"gl_TessLevelInner[0] = min(11,13);" \
		"gl_TessLevelInner[1] = min(10,12);" \
		"}" \
		"}"

		"gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;" \
		"tcs_out[gl_InvocationID].tc = tcs_in[gl_InvocationID].tc;" \
		"}";

	const GLchar *tessellationEvaluationShaderSourceCodeTerrain =
		"#version 430" \
		"\n" \
		"layout(quads, fractional_odd_spacing)in;" \

		"uniform mat4 model;" \
		"uniform mat4 view;" \
		"uniform mat4 projection;" \

		"mat4 u_mvUniform = view * model;" \

		"uniform sampler2D u_tex_displacement;" \
		"uniform float u_dmap_depth;" \

		"in TCS_OUT" \
		"{" \
		"vec2 tc;" \
		"}tes_in[];" \

		"out TES_OUT" \
		"{" \
		"vec2 tc;" \
		"vec3 world_coord;" \
		"vec3 eye_coord;" \
		"}tes_out;" \

		"void main(void)" \
		"{" \
		"vec2 tc1 = mix(tes_in[0].tc, tes_in[1].tc, gl_TessCoord.x);" \
		"vec2 tc2 = mix(tes_in[2].tc, tes_in[3].tc, gl_TessCoord.x);" \
		"vec2 tc = mix(tc2, tc1, gl_TessCoord.y);"

		"vec4 p1 = mix(gl_in[0].gl_Position," \
		"gl_in[1].gl_Position," \
		"gl_TessCoord.x);"
		"vec4 p2 = mix(gl_in[2].gl_Position," \
		"gl_in[3].gl_Position," \
		"gl_TessCoord.x);"
		"vec4 p = mix(p2,p1,gl_TessCoord.y);" \
		"p.y += texture(u_tex_displacement,tc).r * u_dmap_depth;" \

		"vec4 P_eye = u_mvUniform * p;" \
		"tes_out.tc = tc;" \
		"tes_out.world_coord = p.xyz;" \
		"tes_out.eye_coord =  P_eye.xyz;" \

		"gl_Position = projection * P_eye;" \
		"}";

	const GLchar *fragmentShaderSourceCodeTerrain =
		"#version 430 core" \
		"\n" \
		"out vec4 color;" \
		"layout(binding = 1) uniform sampler2D u_tex_color;" \
		"uniform vec4 fog_color = vec4(0.2, 0.8, 0.9, 0.0);" \
		"uniform float alphaTerrain;"\

		"in TES_OUT" \
		"{" \
		"vec2 tc;" \
		"vec3 world_coord;" \
		"vec3 eye_coord;" \
		"}fs_in;" \

		"vec4 fog(vec4 c)" \
		"{" \
		"float z = length(fs_in.eye_coord);" \

		"float de = 0.025 * smoothstep(0.0, 6.0, 10.0 - fs_in.world_coord.y);" \
		"float di = 0.045 * (smoothstep(0.0, 50.0, 20.0 - fs_in.world_coord.y));" \

		"float extinction = exp(-z * de);" \
		"float inscattering = exp(-z * di);" \

		"return c * extinction + fog_color * (1.0 - inscattering);" \
		"}" \

		"void main(void)" \
		"{" \
		"color = fog(texture(u_tex_color, fs_in.tc)) + vec4(0.0,0.4,0.0,1.0);" \
		"color.w = alphaTerrain;" \
		"}";

	// vec4(0.2,0.62,0.69,1.0)
	// vec4(0.4,0.59,0.56,1.0)
	// vec4(0.0,0.4,0.0,1.0)

	Shader objShader_Terrain(true, vertexShaderSourceCodeTerrain, fragmentShaderSourceCodeTerrain, nullptr, nullptr, tessellationControlShaderSourceCodeTerrain, tessellationEvaluationShaderSourceCodeTerrain);
	shaders.push_back(objShader_Terrain);

	glGenVertexArrays(1, &vaoTerrain); // how many arrays, address of array
	glBindVertexArray(vaoTerrain); // start recording
	glBindVertexArray(0); // stop recording in vaoTerrain

	//////////////**********************
	//** ButterFlySunRays
	const GLchar *vertexShaderSourceCodeButterflySunrays =
		"#version 430 core" \
		"\n" \
		"in vec4 vPosition;"\
		"in vec3 vColor;"\

		"uniform mat4 model;" \
		"uniform mat4 view;" \
		"uniform mat4 projection;" \

		"out vec3 out_color;" \
		"void main(void)" \
		"{" \
		"gl_Position = projection * view * model * vPosition;" \
		"out_color = vColor;" \
		"}";

	const GLchar *geometryShaderSourceCodeButterflySunrays =
		"#version 430" \
		"\n" \
		"in vec3 out_color[];" \

		"layout(triangles)in;" \
		"layout(triangle_strip,max_vertices = 27)out;" \

		"uniform mat4 model;" \
		"uniform mat4 view;" \
		"uniform mat4 projection;" \

		"uniform int u_useGeometryShader;" \
		"uniform int u_pointsToConvert;" \

		"out vec3 color;" \

		"void main(void)" \
		"{" \
		"if(u_useGeometryShader == 1)" \
		"{" \
		"for(int vertex = 0; vertex < u_pointsToConvert; vertex++)" \
		"{" \
		"gl_Position = projection * view * model * (gl_in[vertex].gl_Position + (vec4(-0.6,-0.1,0.0,0.0)));" \
		"EmitVertex();" \
		"color = vec3(1.0, 0.5, 0.0);" \
		"gl_Position = projection * view * model * (gl_in[vertex].gl_Position * vec4(-0.6,0.2,0.0,0.0));" \
		"EmitVertex();" \
		"color = vec3(1.0, 0.75, 0.0);" \
		"gl_Position = projection * view * model * (gl_in[vertex].gl_Position * vec4(-0.6,0.3,0.0,0.0));" \
		"EmitVertex();" \
		"color = vec3(0.8, 0.65, 0.0);" \
		"gl_Position = projection * view * model * (gl_in[0].gl_Position);" \
		"color = out_color[0];" \
		"EmitVertex();" \
		"EndPrimitive();" \

		"gl_Position = projection * view * model * (gl_in[vertex].gl_Position + vec4(0.6,-0.2,0.0,0.0));" \
		"EmitVertex();" \
		"color = vec3(1.0, 0.5, 0.0);" \
		"gl_Position = projection * view * model * (gl_in[vertex].gl_Position * vec4(0.6,0.2,0.0,0.0));" \
		"EmitVertex();" \
		"gl_Position = projection * view * model * (gl_in[vertex].gl_Position * vec4(0.6,0.3,0.0,0.0));" \
		"EmitVertex();" \
		"color = vec3(1.0, 0.75, 0.0);" \
		"gl_Position = projection * view * model * (gl_in[vertex].gl_Position * vec4(0.6,0.4,0.0,0.0));" \
		"EmitVertex();" \
		"color = vec3(0.8, 0.65, 0.0);" \
		"gl_Position = projection * view * model * (gl_in[0].gl_Position);" \
		"EmitVertex();" \
		"color = out_color[0];" \
		"EndPrimitive();" \
		"}" \
		"}" \
		"else" \
		"{" \
		"for(int vertex = 0; vertex < gl_in.length(); vertex++)" \
		"{" \
		"gl_Position = gl_in[vertex].gl_Position;" \
		"color = out_color[vertex];" \
		"EmitVertex();" \
		"}" \
		"EndPrimitive();" \
		"}" \
		"}";

	const GLchar *fragmentShaderSourceCodeButterflySunrays =
		"#version 430 core" \
		"\n" \
		"in vec3 color;" \
		"out vec4 FragColor;" \
		"uniform float alphaSunRays;" \

		"void main(void)" \
		"{" \
		"FragColor = vec4(color, 1.0);" \
		"FragColor.w = alphaSunRays;" \
		"}";

	Shader objShader_ButterflySunrays(true,vertexShaderSourceCodeButterflySunrays, fragmentShaderSourceCodeButterflySunrays, geometryShaderSourceCodeButterflySunrays, nullptr, nullptr, nullptr);
	shaders.push_back(objShader_ButterflySunrays);


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
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f
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
	sphereSun.setTextureColor(255, 125, 0);
	sphereSun.set(0.5, 30, 30, true);
	//////////////**********************
	return 1;
}

void SceneTerrain::displayTerrain(void)
{
	glEnable(GL_CULL_FACE);
	//glClearColor(0.0f, 0.749f, 1.0f, 1.0f);
	static double currentTime = 2.0;
	static const GLfloat black[] = { 0.85f, 0.95f, 1.0f, 1.0f };
	static const GLfloat one = 1.0f;
	static double last_time = 0.0;
	static double total_time = 0.0;

	total_time += (currentTime - last_time);
	last_time = currentTime;

	float t = (float)total_time * 0.03f;
	float r = sinf(t * 5.37f) * 15.0f + 16.0f;
	float h = cosf(t * 4.79f) * 0.4f + 3.2f;

	//glViewport(0, 0, giWinWidth, giWinHeight);

	view = lookat(vec3(sinf(t) * r, h, cosf(t) * r), vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
	projection = perspective(camera.Zoom, (float)gWidth / (float)gHeight, 0.1f, 100.0f);//radians

	shaders[Shader_Terrain].use();

	shaders[Shader_Terrain].setMat4("projection", projection);
	shaders[Shader_Terrain].setMat4("view", view);
	shaders[Shader_Terrain].setFloat("alphaTerrain", terrainAlpha);
	
	MakeModelMatricesIdentity();
	//Transformation

	translationMatrix = translate(0.0f, 0.0f, -6.0f);
	//rotationMatrix = rotate(angleTriangle, 0.0f, 1.0f, 0.0f);
	//scaleMatrix = scale(0.2f, 0.2f, 0.2f);
	modelMatrix = translationMatrix * rotationMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	shaders[Shader_Terrain].setMat4("model", modelMatrix);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBindVertexArray(vaoTerrain);

	// bind with textures if any
	glActiveTexture(GL_TEXTURE0); // which texture to activate, unit 0

	glBindTexture(GL_TEXTURE_2D, textures[Texture_Terraingen]);

	shaders[Shader_Terrain].setInt("u_tex_displacement", 0);

	// bind with textures if any
	glActiveTexture(GL_TEXTURE1); // which texture to activate, unit 0

	glBindTexture(GL_TEXTURE_2D, textures[Texture_Terraincolor]);

	shaders[Shader_Terrain].setInt("u_tex_color", 0);
	shaders[Shader_Terrain].setFloat("u_dmap_depth", depthValue);

	glPatchParameteri(GL_PATCH_VERTICES, 4); // create patch of 4 vertices

	// draw necessory scene
	glDrawArraysInstanced(GL_PATCHES, // what to draw from vertices 
		0, // from where to start taking array elements
		4, // how may vertices to draw ; no of vertices
		64 * 64);

	// unbind vaoTerrain
	glBindVertexArray(0);

	glDisable(GL_CULL_FACE);
}

void SceneTerrain::displayButterflySunRays(void)
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
		translationMatrix = translate(0.0f, 1.0f, -65.0f);
		scaleMatrix = scale(0.9f, 0.7f, 0.0f);
	}
	else
	{
		translationMatrix = translate(0.0f, 0.0f, -5.0f);
		scaleMatrix = scale(0.4f, 0.4f, 0.4f);
	}

	// multiply required metrices
	modelMatrix = translationMatrix * scaleMatrix;

	shaders[Shader_ButterflySunrays].setMat4("projection", projection);
	shaders[Shader_ButterflySunrays].setMat4("view", view);
	shaders[Shader_ButterflySunrays].setMat4("model", modelMatrix);
	shaders[Shader_ButterflySunrays].setFloat("alphaSunRays", sunRaysAlpha);

	if (gbUseGeometryShader == TRUE)
	{
		pointsToConvert = counterForRays / 10;
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

void SceneTerrain::displaySun(void)
{
	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	shaders[Shader_BackgroundRain].use();

	projection = perspective(camera.Zoom, (float)gWidth / (float)gHeight, 0.1f, 100.0f);//radians
	view = camera.GetViewMatrix();

	shaders[Shader_BackgroundRain].setMat4("projection", projection);
	shaders[Shader_BackgroundRain].setMat4("view", view);

	MakeModelMatricesIdentity();
	//Transformation

	translationMatrix = translate(0.0f, sunYPosition, -37.0f);
	scaleMatrix = scale(5.5f, 5.5f, 5.5f);
	modelMatrix = translationMatrix * scaleMatrix;

	shaders[Shader_BackgroundRain].setMat4("model", modelMatrix);
	shaders[Shader_BackgroundRain].setInt("tex_backgroundRain", 0);

	sphereSun.draw();

	glEnable(GL_BLEND);
}
void SceneTerrain::DisplayScene()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glClearColor(1.0f, 0.7f, 0.0f, 0.0f);
	//glClearColor(0.0f, 0.749f, 1.0f, 1.0f);
	glClearColor(clearColorR, clearColorG, clearColorB, 0.0f);
	displaySun();
	displayButterflySunRays();
	displayTerrain();

	glDisable(GL_BLEND);
}

void SceneTerrain::UpdateScene()
{
	counterForRays = counterForRays + 1;
	if (sunRaysAlpha <= 1.0 && pointsToConvert < 8)
	{
		sunYPosition = sunYPosition + 0.015f;
	}

	if (fadeInTerrainScene == true && (sunRaysAlpha <= 1.0))
	{
		//terrainAlpha = terrainAlpha + 0.05f;
		sunRaysAlpha = sunRaysAlpha + 0.006f;

		if (sunRaysAlpha > 1.0f)
		{
			fadeInTerrainScene = false;
		}
	}
	else if (fadeInTerrainScene == false && (sunRaysAlpha > 0.0 || terrainAlpha > 0.0))
	{
		//terrainAlpha = terrainAlpha - 0.01f;
		sunRaysAlpha = sunRaysAlpha - 0.004f;

		if (sunRaysAlpha <= 0.0f)
		{
			sunRaysAlpha = 0.0f;
			//terrainAlpha = 1.0f;
		}
	}
}

void SceneTerrain::UninitializeScene()
{
	// Delete Vertex buffers : vbo's 
	DeleteVertexBuffer(&vboPositionButterflySunrays);
	DeleteVertexBuffer(&vboColorButterflySunrays);

	// Delete Vertex Arrays : vao's
	DeleteVertexArray(&vaoTerrain);
	DeleteVertexArray(&vaoButterflySunrays);
}
