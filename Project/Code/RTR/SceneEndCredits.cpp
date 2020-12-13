#include"SceneEndCredits.h"

void SceneEndCredits::RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, vmath::vec3 color)
{
	glUniform3fv(glGetUniformLocation(shaders[Shader_EndCredits].ID, "textColor"), 1, color);
	glActiveTexture(GL_TEXTURE0);

	glBindVertexArray(vaoFont_EndCredit);

	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Charaters[*c];
		GLfloat xpos = x + ch.Bearing[0] * scale;
		GLfloat ypos = y - (ch.Size[1] - ch.Bearing[1])*scale;

		GLfloat w = ch.Size[0] * scale;
		GLfloat h = ch.Size[1] * scale;

		GLfloat vertices[6][4] = {
			{ xpos,ypos + h,0.0,0.0 },
		{ xpos,ypos,0.0,1.0 },
		{ xpos + w,ypos,1.0,1.0 },
		{ xpos,ypos + h,0.0,0.0 },
		{ xpos + w,ypos,1.0,1.0 },
		{ xpos + w,ypos + h,1.0,0.0 }
		};

		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		glBindBuffer(GL_ARRAY_BUFFER, vboFont_EndCredit);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		x += (ch.Advance >> 6)*scale;

	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

}

void SceneEndCredits::printNames()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	shaders[Shader_EndCredits].use();

	projection = perspective(camera.Zoom, (float)gWidth / (float)gHeight, 0.1f, 100.0f);//radians
	view = camera.GetViewMatrix();
	shaders[Shader_EndCredits].setMat4("projection", projection);
	shaders[Shader_EndCredits].setMat4("view", view);


	if (flagName == 1)
	{
		MakeModelMatricesIdentity();
		//Transformation

		translationMatrix = translate(4.3f, -4.5f, -3.0f);
		modelMatrix = translationMatrix * rotationMatrix;
		modelMatrix = modelMatrix * scaleMatrix;
		shaders[Shader_EndCredits].setMat4("model", modelMatrix);
		shaders[Shader_EndCredits].setFloat("mixColorName", mixColorName);
		RenderText("vkfnrh dksjMs", -5.0f, 5.0f, 0.0045f, vmath::vec3(1.0f, 1.0f, 1.0f));

		//Amita
		MakeModelMatricesIdentity();
		//Transformation

		translationMatrix = translate(4.25f, -4.8f, -3.0f);
		modelMatrix = translationMatrix * rotationMatrix;
		modelMatrix = modelMatrix * scaleMatrix;
		shaders[Shader_EndCredits].setMat4("model", modelMatrix);
		shaders[Shader_EndCredits].setFloat("mixColorName", mixColorName);
		RenderText("vferk dadjst", -5.0f, 5.0f, 0.0045f, vmath::vec3(1.0f, 1.0f, 1.0f));

		//Chaitanya
		MakeModelMatricesIdentity();
		//Transformation

		translationMatrix = translate(4.35f, -5.1f, -3.0f);
		modelMatrix = translationMatrix * rotationMatrix;
		modelMatrix = modelMatrix * scaleMatrix;
		shaders[Shader_EndCredits].setMat4("model", modelMatrix);
		shaders[Shader_EndCredits].setFloat("mixColorName", mixColorName);
		RenderText("pSrU; izHkq.ks", -5.0f, 5.0f, 0.0045f, vmath::vec3(1.0f, 1.0f, 1.0f));


		//Gayatri
		MakeModelMatricesIdentity();
		//Transformation

		translationMatrix = translate(4.4f, -5.4f, -3.0f);
		modelMatrix = translationMatrix * rotationMatrix;
		modelMatrix = modelMatrix * scaleMatrix;
		shaders[Shader_EndCredits].setMat4("model", modelMatrix);
		shaders[Shader_EndCredits].setFloat("mixColorName", mixColorName);
		RenderText("Xkk;=h <ksjs", -5.0f, 5.0f, 0.0045f, vmath::vec3(1.0f, 1.0f, 1.0f));
	}
	if (flagCredit == 1)
	{
		//Movie
		MakeModelMatricesIdentity();
		//Transformation

		translationMatrix = translate(4.1f, -4.5f, -3.0f);
		modelMatrix = translationMatrix * rotationMatrix;
		modelMatrix = modelMatrix * scaleMatrix;
		shaders[Shader_EndCredits].setMat4("model", modelMatrix);
		shaders[Shader_EndCredits].setFloat("mixColorCredit", mixColorCredit);
		RenderText("fp=iV % ekfuuh", -5.0f, 5.0f, 0.0045f, vmath::vec3(1.0f, 1.0f, 1.0f));


		//Bahinabai
		MakeModelMatricesIdentity();
		//Transformation

		translationMatrix = translate(3.8f, -4.8f, -3.0f);
		modelMatrix = translationMatrix * rotationMatrix;
		modelMatrix = modelMatrix * scaleMatrix;
		shaders[Shader_EndCredits].setMat4("model", modelMatrix);
		shaders[Shader_EndCredits].setFloat("mixColorCredit", mixColorCredit);
		RenderText("dof;=h % cfg.kkckbZ pkS/kjh", -5.0f, 5.0f, 0.0045f, vmath::vec3(1.0f, 1.0f, 1.0f));


		//Singer
		MakeModelMatricesIdentity();
		//Transformation

		translationMatrix = translate(4.1f, -5.1f, -3.0f);
		modelMatrix = translationMatrix * rotationMatrix;
		modelMatrix = modelMatrix * scaleMatrix;
		shaders[Shader_EndCredits].setMat4("model", modelMatrix);
		shaders[Shader_EndCredits].setFloat("mixColorCredit", mixColorCredit);
		RenderText("xkf;dk % vk'kk Hkkslys", -5.0f, 5.0f, 0.0045f, vmath::vec3(1.0f, 1.0f, 1.0f));


		//Director
		MakeModelMatricesIdentity();
		//Transformation

		translationMatrix = translate(3.8f, -5.4f, -3.0f);
		modelMatrix = translationMatrix * rotationMatrix;
		modelMatrix = modelMatrix * scaleMatrix;
		shaders[Shader_EndCredits].setMat4("model", modelMatrix);
		shaders[Shader_EndCredits].setFloat("mixColorCredit", mixColorCredit);
		RenderText("laxhr fnXn'kZd % olar iokj", -5.0f, 5.0f, 0.0045f, vmath::vec3(1.0f, 1.0f, 1.0f));
		//
		MakeModelMatricesIdentity();
		//Transformation

		translationMatrix = translate(4.1f, -5.7f, -3.0f);
		modelMatrix = translationMatrix * rotationMatrix;
		modelMatrix = modelMatrix * scaleMatrix;
		shaders[Shader_EndCredits].setMat4("model", modelMatrix);
		shaders[Shader_EndCredits].setFloat("mixColorCredit", mixColorCredit);
		RenderText("dkWihjkbV % lkjsxkek", -5.0f, 5.0f, 0.0045f, vmath::vec3(1.0f, 1.0f, 1.0f));
	}
	glDisable(GL_BLEND);
}


void SceneEndCredits::updateNames()
{
	
	aditiy = aditiy + 0.001f;
	amitay = amitay + 0.001f;
	chaitanyay = chaitanyay + 0.001f;
	gayatriy = gayatriy + 0.001f;
	moviey = moviey + 0.001f;
	poety = poety + 0.001f;
	singery = singery + 0.001f;
	directory = directory + 0.001f;
	copyrighty = copyrighty + 0.001f;
	
	if (mixColorCredit > 0.0f)
	{
		flagCredit = 1;
		flagName = 0;
		mixColorCredit = mixColorCredit - 0.003f;
	}
	mixColorCredit1 = mixColorCredit1 - 0.003f;

	if (mixColorCredit < 0.0f)
	{

		flagCredit = 0;
		flagName = 1;

		mixColorName = mixColorName - 0.003f;
	}

	

	mixColorName1 = mixColorName1 - 0.001f;
}



int SceneEndCredits::InitScene()
{
	char  char_array[255];
	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		fprintf(gpFile, "Could not init freetype library\n");
		return 1;
	}

	strcpy(char_array, modulePath);
	strcat(char_array, "\\Agra-j8Gv.ttf");

	if (FT_New_Face(ft, char_array, 0, &face)) {
		fprintf(gpFile, "Could not open font\n");
		return 1;
	}

	FT_Set_Pixel_Sizes(face, 0, 48);

	if (FT_Load_Char(face, 'X', FT_LOAD_RENDER)) {
		fprintf(gpFile, "Could not load charachter 'X'\n");
		return 1;
	}

	//Letter

	//We can give "layout (location = 0) in vec4 vPosition;" to vposition and avoid prebinding steps .Here AMC_ATTRIBITUE_POSITION is 0 so we have specified layout=0

	const GLchar* vertexShaderSourceCodeName =
		"#version 430 core" \
		"\n" \
		"in vec4 vPosition;" \

		"uniform mat4 model;" \
		"uniform mat4 view;" \
		"uniform mat4 projection;" \

		"out vec2 out_texcoord;" \
		"void main(void)" \
		"{" \
		"gl_Position= projection * view * model * vec4(vPosition.xy,0.0,1.0);" \
		"out_texcoord=vPosition.zw;" \
		"}";

	const GLchar* fragmentShaderSourceCodeName =
		"#version 430 core" \
		"\n" \
		"in vec2 out_texcoord;" \
		"uniform sampler2D u_sampler;" \
		"uniform vec3 textColor;" \
		"uniform float mixColorName;"\
		"uniform float mixColorName1;"\
		"uniform float mixColorCredit;"\
		"uniform float mixColorCredit1;"\
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"if(mixColorCredit>0.0f)"
		"{"
		"vec4 sampled=vec4(1.0,1.0,1.0,texture(u_sampler,out_texcoord));"
		"FragColor = mix((vec4(textColor,1.0)*sampled),vec4(0.0),mixColorCredit);" \
		"}"
		"if(mixColorCredit<0.0f)"
		"{"
		"vec4 sampled=vec4(1.0,1.0,1.0,texture(u_sampler,out_texcoord));"
		"FragColor = mix(vec4(0.0),(vec4(textColor,1.0)*sampled),mixColorCredit1);" \
		"}"\
		"if(mixColorName>0.0f)"
		"{"
		"vec4 sampled=vec4(1.0,1.0,1.0,texture(u_sampler,out_texcoord));"
		"FragColor = mix((vec4(textColor,1.0)*sampled),vec4(0.0),mixColorName);" \
		"}"
		"if(mixColorName<0.0f)"
		"{"
		"vec4 sampled=vec4(1.0,1.0,1.0,texture(u_sampler,out_texcoord));"
		"FragColor = mix(vec4(0.0),(vec4(textColor,1.0)*sampled),mixColorName1);" \
		"}"\

		"}";

	Shader objShader_EndCredit(true, vertexShaderSourceCodeName, fragmentShaderSourceCodeName);

	shaders.push_back(objShader_EndCredit);


	

	for (GLubyte c = 0; c < 128; c++)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			fprintf(gpFile, "Could not load Glyph\n");
			continue;
		}

		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		Character character =
		{
			texture,
			vmath::ivec2(face->glyph->bitmap.width,face->glyph->bitmap.rows),
			vmath::ivec2(face->glyph->bitmap_left,face->glyph->bitmap_top),
			face->glyph->advance.x
		};

		Charaters.insert(std::pair<GLchar, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	glGenVertexArrays(1, &vaoFont_EndCredit);
	glBindVertexArray(vaoFont_EndCredit);
	glGenBuffers(1, &vboFont_EndCredit);
	glBindBuffer(GL_ARRAY_BUFFER, vboFont_EndCredit);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return 1;
}

void SceneEndCredits::DisplayScene()
{

	printNames();

}

void SceneEndCredits::UpdateScene()
{
	updateNames();

}

void SceneEndCredits::UninitializeScene()
{
	// Delete Vertex buffers : vbo's 
	DeleteVertexBuffer(&vboFont_EndCredit);

	// Delete Vertex Arrays : vao's
	DeleteVertexArray(&vaoFont_EndCredit);

	DeleteShaderProgram(&shaders[Shader_EndCredits].ID);
}
