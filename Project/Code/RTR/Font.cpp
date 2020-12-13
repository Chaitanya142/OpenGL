#include "Font.h"

void Font::RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, vmath::vec3 color)
{
	//Shader sh = *(this->objShader_Font);
	Shader sh = shVec[0];

	glUniform3fv(glGetUniformLocation( sh.ID, "textColor"), 1, color);
	//glUniform3fv(glGetUniformLocation(this->objShader_Font.ID, "textColor"), 1, color);

	glActiveTexture(GL_TEXTURE0);

	glBindVertexArray(vaoFont);

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
		glBindBuffer(GL_ARRAY_BUFFER, vboFont);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		x += (ch.Advance >> 6)*scale;

	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

}

//Font::Font()
//{
//}

int Font::InitFont()
{
	char  char_array[255];
	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		fprintf(gpFile, "Could not init freetype library\n");
		return 1;
	}

	strcpy(char_array, modulePath);
	strcat(char_array, "\\Francesca W00 Gothic.ttf");

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

	const GLchar* vertexShaderSourceCodeLetter =
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

	const GLchar* fragmentShaderSourceCodeLetter =
		"#version 430 core" \
		"\n" \
		"in vec2 out_texcoord;" \
		"uniform sampler2D u_sampler;" \
		"uniform vec3 textColor;" \
		"uniform float mixColor;"\
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"vec4 sampled = vec4(1.0,1.0,1.0,texture(u_sampler,out_texcoord));" \
		"FragColor = mix((vec4(textColor,1.0)*sampled),vec4(0.0),mixColor);" \
		"}";

	Shader objShader_Font(true, vertexShaderSourceCodeLetter, fragmentShaderSourceCodeLetter);

	this->objShader_Font = &objShader_Font;

	shVec.push_back(objShader_Font);

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

	glGenVertexArrays(1, &vaoFont);
	glBindVertexArray(vaoFont);
	glGenBuffers(1, &vboFont);
	glBindBuffer(GL_ARRAY_BUFFER, vboFont);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return 1;
}

void Font::DisplayText(string text, float startX, float startY, vec3 color)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//this->objShader_Font->use();


	//Shader sh=*(this->objShader_Font);
	Shader sh = shVec[0];

	sh.use();

	//projection = perspective(camera.Zoom, (float)gWidth / (float)gHeight, 0.1f, 100.0f);//radians
	projection = perspective(45.0f, (float)gWidth / (float)gHeight, 0.1f, 100.0f);//radians

	//view = camera.GetViewMatrix();
	view= mat4::identity();

	/*this->objShader_Font.setMat4("projection", projection);
	this->objShader_Font.setMat4("view", view);*/

	sh.setMat4("projection", projection);
	sh.setMat4("view", view);


	MakeModelMatricesIdentity();
	//Transformation

	translationMatrix = translate(0.35f, -0.35f, -1.0f);

	modelMatrix = translationMatrix * rotationMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	sh.setMat4("model", modelMatrix);
	sh.setFloat("mixColor", mixColor);

	//RenderText(text, 0.0f, 0.0f, 0.0008f,color);
	RenderText(text, startX, startY, 0.0008f, color);

	glDisable(GL_BLEND);
}



