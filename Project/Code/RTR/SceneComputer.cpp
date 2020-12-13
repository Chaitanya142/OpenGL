#include"SceneComputer.h"\


void SceneComputer::DrawLastSequence()
{
	shaders[Shader_FontFront].use();

	projection = perspective(camera.Zoom, (float)gWidth / (float)gHeight, 0.1f, 100.0f);//radians
	view = camera.GetViewMatrix();
	shaders[Shader_FontFront].setMat4("projection", projection);
	shaders[Shader_FontFront].setMat4("view", view);

	MakeModelMatricesIdentity();
	//Transformation

	modelMatrix = translate(34.0f, 21.0f, -97.0f);

	shaders[Shader_FontFront].setMat4("model", modelMatrix);

	RenderTextStart("glBindAttribLocation", -50.0f, -2.0f, 0.04f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);
	//counter = 1;


	MakeModelMatricesIdentity();
	modelMatrix = translate(34.0f, 19.0f, -97.0f);
	shaders[Shader_FontFront].setMat4("model", modelMatrix);
	RenderTextStart("(gShaderProgramObject, ", -50.0f, -2.0f, 0.04f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);



	MakeModelMatricesIdentity();
	modelMatrix = translate(34.0f, 17.0f, -97.0f);
	shaders[Shader_FontFront].setMat4("model", modelMatrix);
	RenderTextStart("AMC_ATTRIBUTE_POSITION, ", -50.0f, -2.0f, 0.04f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);

	GLfloat translateZ = 0.0f;
	GLfloat translateZ1 = 0.0f;


	//if (stage == 11) {
	//	translateZ1 = -30.0f;
	//}
	//else {
	//	translateZ1 = -97.0f;
	//}
	translateZ1 = -97.0f;

/*
	if (counter11 != 1)
	{
		translateZ = -97.0f;
	}
	else
	{
		translateZ = -30.0f;

	}

	if (counter11 == 1) {
		translateZ1 = -97.0f;
	}
	else {
		translateZ1 = -30.0f;
	}*/

	MakeModelMatricesIdentity();
	modelMatrix = translate(34.0f, 15.0f, translateZ);

	shaders[Shader_FontFront].setMat4("model", modelMatrix);
	RenderTextStart("''vPos'", -52.0f, -2.0f, 0.04f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);

	MakeModelMatricesIdentity();

	modelMatrix = translate(34.0f, 15.0f, translateZ1);

	shaders[Shader_FontFront].setMat4("model", modelMatrix);

	switch (stage) {
	case 8:
		RenderTextStart("''vPosi", -50.0f, -2.0f, 0.04f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);
		break;
	case 9:
		RenderTextStart("''vPosit", -50.0f, -2.0f, 0.04f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);
		break;
	case 10:
		RenderTextStart("''vPositi", -50.0f, -2.0f, 0.04f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);
		break;
	case 11:
		RenderTextStart("''vPositio", -50.0f, -2.0f, 0.04f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);
		break;
	case 12:
		RenderTextStart("''vPosition", -50.0f, -2.0f, 0.04f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);
		break;
	case 13:
		RenderTextStart("''vPosition'')", -50.0f, -2.0f, 0.04f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);

		break;
	case 14:
		RenderTextStart("''vPosition'');", -50.0f, -2.0f, 0.04f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);

		break;
	}

/*
	if (counter8 == 1)
	{
		MakeModelMatricesIdentity();

		modelMatrix = translate(39.0f, 15.0f, translateZ1);


		shaders[Shader_FontFront].setMat4("model", modelMatrix);
		RenderTextStart("''vPosi", -60.0f, -2.0f, 0.04f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);
	}
	if (counter9 == 1)
	{
		MakeModelMatricesIdentity();

		modelMatrix = translate(39.0f, 15.0f, translateZ1);

		shaders[Shader_FontFront].setMat4("model", modelMatrix);
		RenderTextStart("             t", -60.0f, -2.0f, 0.04f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);
	}

	if (counter10 == 1)
	{
		MakeModelMatricesIdentity();

		modelMatrix = translate(39.0f, 15.0f, translateZ1);

		shaders[Shader_FontFront].setMat4("model", modelMatrix);
		RenderTextStart("               i", -60.0f, -2.0f, 0.04f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);
	}

	if (counter11 == 1)
	{
		MakeModelMatricesIdentity();

		modelMatrix = translate(39.0f, 15.0f, translateZ1);

		shaders[Shader_FontFront].setMat4("model", modelMatrix);
		RenderTextStart("                 o", -60.0f, -2.0f, 0.04f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);
	}

	if (counter12 == 1)
	{
		MakeModelMatricesIdentity();

		modelMatrix = translate(39.0f, 15.0f, translateZ1);

		shaders[Shader_FontFront].setMat4("model", modelMatrix);
		RenderTextStart("                   n", -60.0f, -2.0f, 0.04f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);
	}

	if (counter13 == 1)
	{
		MakeModelMatricesIdentity();

		modelMatrix = translate(39.0f, 15.0f, translateZ1);

		shaders[Shader_FontFront].setMat4("model", modelMatrix);
		RenderTextStart("                      '')", -60.0f, -2.0f, 0.04f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);
	}

	if (counter14 == 1)
	{
		MakeModelMatricesIdentity();

		modelMatrix = translate(39.0f, 15.0f, translateZ1);

		shaders[Shader_FontFront].setMat4("model", modelMatrix);
		RenderTextStart("                         ;", -60.0f, -2.0f, 0.04f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);
	}
*/

}



void SceneComputer::RenderTextStart(std::string text, GLfloat x, GLfloat y, GLfloat scale, vmath::vec3 color, int ShaderId)
{
	glUniform3fv(glGetUniformLocation(ShaderId, "textColor"), 1, color);
	glActiveTexture(GL_TEXTURE0);

	glBindVertexArray(vaoFontConstanb);

	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = CharatersConstanb[*c];
		GLfloat xpos = x + ch.Bearing[0] * scale;
		GLfloat ypos = y - (ch.Size[1] - ch.Bearing[1])*scale;

		GLfloat w = ch.Size[0] * scale;
		GLfloat h = ch.Size[1] * scale;

		GLfloat vertices[6][4] = {
			{xpos,ypos + h,0.0,0.0},
		{xpos,ypos,0.0,1.0},
		{xpos + w,ypos,1.0,1.0},
		{xpos,ypos + h,0.0,0.0},
		{xpos + w,ypos,1.0,1.0},
		{xpos + w,ypos + h,1.0,0.0}
		};



		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		glBindBuffer(GL_ARRAY_BUFFER, vboFontConstanb);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		x += (ch.Advance >> 6)*scale;

	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

}


void SceneComputer::DrawFrontSequence()
{
	shaders[Shader_FontFront].use();

	projection = perspective(camera.Zoom, (float)gWidth / (float)gHeight, 0.1f, 100.0f);//radians
	view = camera.GetViewMatrix();
	shaders[Shader_FontFront].setMat4("projection", projection);
	shaders[Shader_FontFront].setMat4("view", view);

	MakeModelMatricesIdentity();
	//Transformation

	modelMatrix = translate(39.0f, 21.0f, translateFont);

	shaders[Shader_FontFront].setMat4("model", modelMatrix);

	RenderTextStart("G:\\RTR>", -55.0f, -2.0f, 0.035f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);
	//counter = 1;

	MakeModelMatricesIdentity();
	modelMatrix = translate(39.0f, 21.0f, translateFont);
	shaders[Shader_FontFront].setMat4("model", modelMatrix);

	switch(stage) {
		case 1:
			RenderTextStart("                 O", -55.0f, -2.0f, 0.035f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);

			break;
		case 2:
			RenderTextStart("                 Op", -55.0f, -2.0f, 0.035f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);

			break;
		case 3:
			RenderTextStart("                 Ope", -55.0f, -2.0f, 0.035f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);

			break;
		case 4:
			RenderTextStart("                 Open", -55.0f, -2.0f, 0.035f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);

			break;
		case 5:
			RenderTextStart("                 OpenC", -55.0f, -2.0f, 0.035f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);

			break;
		case 6:
			RenderTextStart("                 OpenCL", -55.0f, -2.0f, 0.035f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);

			break;
		case 7:
			RenderTextStart("                 OpenCLI", -55.0f, -2.0f, 0.035f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);

			break;
		case 8:
			RenderTextStart("                 OpenCLIn", -55.0f, -2.0f, 0.035f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);

			break;
		case 9:
			RenderTextStart("                 OpenCLInt", -55.0f, -2.0f, 0.035f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);

			break;
		case 10:
			RenderTextStart("                 OpenCLInte", -55.0f, -2.0f, 0.035f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);

			break;
		case 11:
			RenderTextStart("                 OpenCLInter", -55.0f, -2.0f, 0.035f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);

			break;
		case 12:
			RenderTextStart("                 OpenCLIntero", -55.0f, -2.0f, 0.035f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);

			break;
		case 13:
			RenderTextStart("                 OpenCLInterop", -55.0f, -2.0f, 0.035f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);

			break;
		case 14:
			RenderTextStart("                 OpenCLInterop.", -55.0f, -2.0f, 0.035f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);

			break;
		case 15:
			RenderTextStart("                 OpenCLInterop.e", -55.0f, -2.0f, 0.035f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);

			break;
		case 16:
			RenderTextStart("                 OpenCLInterop.ex", -55.0f, -2.0f, 0.035f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);

			break;
		case 17:
			RenderTextStart("                 OpenCLInterop.exe", -55.0f, -2.0f, 0.035f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);
			counter18 = 1;
			break;
	}

	//if (counter == 1)
	//{
	//	MakeModelMatricesIdentity();
	//	modelMatrix = translate(39.0f, 21.0f, translateFont);
	//	shaders[Shader_FontFront].setMat4("model", modelMatrix);
	//	RenderTextStart("                 O", -55.0f, -2.0f, 0.035f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);

	//}
	//if (counter1 == 1)
	//{
	//	MakeModelMatricesIdentity();

	//	modelMatrix = translate(39.0f, 21.0f, translateFont);

	//	shaders[Shader_FontFront].setMat4("model", modelMatrix);
	//	RenderTextStart("                    p", -55.0f, -2.0f, 0.035f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);
	//}
	//if (counter2 == 1)
	//{
	//	MakeModelMatricesIdentity();

	//	modelMatrix = translate(39.0f, 21.0f, translateFont);

	//	shaders[Shader_FontFront].setMat4("model", modelMatrix);
	//	RenderTextStart("                       e", -55.0f, -2.0f, 0.035f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);
	//}
	//if (counter3 == 1)
	//{
	//	MakeModelMatricesIdentity();

	//	modelMatrix = translate(39.0f, 21.0f, translateFont);

	//	shaders[Shader_FontFront].setMat4("model", modelMatrix);
	//	RenderTextStart("                         n", -55.0f, -2.0f, 0.035f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);
	//}
	//if (counter4 == 1)
	//{
	//	MakeModelMatricesIdentity();

	//	modelMatrix = translate(39.0f, 21.0f, translateFont);

	//	shaders[Shader_FontFront].setMat4("model", modelMatrix);
	//	RenderTextStart("                            C", -55.0f, -2.0f, 0.035f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);
	//}
	//if (counter5 == 1)
	//{
	//	MakeModelMatricesIdentity();

	//	modelMatrix = translate(39.0f, 21.0f, translateFont);

	//	shaders[Shader_FontFront].setMat4("model", modelMatrix);
	//	RenderTextStart("                               L", -55.0f, -2.0f, 0.035f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);
	//}
	//if (counter6 == 1)
	//{
	//	MakeModelMatricesIdentity();

	//	modelMatrix = translate(39.0f, 21.0f, translateFont);

	//	shaders[Shader_FontFront].setMat4("model", modelMatrix);
	//	RenderTextStart("                                  I", -55.0f, -2.0f, 0.035f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);
	//}

	//if (counter7 == 1)
	//{
	//	MakeModelMatricesIdentity();

	//	modelMatrix = translate(39.0f, 21.0f, translateFont);

	//	shaders[Shader_FontFront].setMat4("model", modelMatrix);
	//	RenderTextStart("                                    n", -55.0f, -2.0f, 0.035f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);
	//}

	//if (counter8 == 1)
	//{
	//	MakeModelMatricesIdentity();

	//	modelMatrix = translate(39.0f, 21.0f, translateFont);

	//	shaders[Shader_FontFront].setMat4("model", modelMatrix);
	//	RenderTextStart("                                       t", -55.0f, -2.0f, 0.035f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);
	//}

	//if (counter9 == 1)
	//{
	//	MakeModelMatricesIdentity();

	//	modelMatrix = translate(39.0f, 21.0f, translateFont);

	//	shaders[Shader_FontFront].setMat4("model", modelMatrix);
	//	RenderTextStart("                                         e", -55.0f, -2.0f, 0.035f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);
	//}

	//if (counter10 == 1)
	//{
	//	MakeModelMatricesIdentity();

	//	modelMatrix = translate(39.0f, 21.0f, translateFont);

	//	shaders[Shader_FontFront].setMat4("model", modelMatrix);
	//	RenderTextStart("                                            r", -55.0f, -2.0f, 0.035f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);
	//}

	//if (counter11 == 1)
	//{
	//	MakeModelMatricesIdentity();

	//	modelMatrix = translate(39.0f, 21.0f, translateFont);

	//	shaders[Shader_FontFront].setMat4("model", modelMatrix);
	//	RenderTextStart("                                              o", -55.0f, -2.0f, 0.035f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);
	//}

	//if (counter12 == 1)
	//{
	//	MakeModelMatricesIdentity();

	//	modelMatrix = translate(39.0f, 21.0f, translateFont);

	//	shaders[Shader_FontFront].setMat4("model", modelMatrix);
	//	RenderTextStart("                                                p", -55.0f, -2.0f, 0.035f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);
	//}

	//if (counter13 == 1)
	//{
	//	MakeModelMatricesIdentity();

	//	modelMatrix = translate(39.0f, 21.0f, translateFont);

	//	shaders[Shader_FontFront].setMat4("model", modelMatrix);
	//	RenderTextStart("                                                   .", -55.0f, -2.0f, 0.035f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);
	//}
	//if (counter14 == 1)
	//{
	//	MakeModelMatricesIdentity();

	//	modelMatrix = translate(39.0f, 21.0f, translateFont);

	//	shaders[Shader_FontFront].setMat4("model", modelMatrix);
	//	RenderTextStart("                                                     e", -55.0f, -2.0f, 0.035f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);
	//}
	//if (counter15 == 1)
	//{
	//	MakeModelMatricesIdentity();

	//	modelMatrix = translate(39.0f, 21.0f, translateFont);

	//	shaders[Shader_FontFront].setMat4("model", modelMatrix);
	//	RenderTextStart("                                                       x", -55.0f, -2.0f, 0.035f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);
	//}
	//if (counter16 == 1)
	//{
	//	MakeModelMatricesIdentity();

	//	modelMatrix = translate(39.0f, 21.0f, translateFont);

	//	shaders[Shader_FontFront].setMat4("model", modelMatrix);
	//	RenderTextStart("                                                         e", -55.0f, -2.0f, 0.035f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);

	//}
	//if (counter17 == 1)
	//{
	//	MakeModelMatricesIdentity();

	//	modelMatrix = translate(39.0f, 21.0f, translateFont);

	//	shaders[Shader_FontFront].setMat4("model", modelMatrix);
	//	RenderTextStart("                                                           ", -55.0f, -2.0f, 0.035f, vmath::vec3(1.0f, 1.0f, 1.0f), shaders[Shader_FontFront].ID);
	//	
	//	counter18 = 1;
	//}
}

void SceneComputer::DisplayModelAndChair() {

	MakeModelMatricesIdentity();
	shaders[Shader_ModelLoading].use();
	projection = perspective(camera.Zoom, (float)gWidth / (float)gHeight, 0.1f, 100.0f);//radians

	view = camera.GetViewMatrix();
	shaders[Shader_ModelLoading].setMat4("projection", projection);
	shaders[Shader_ModelLoading].setMat4("view", view);

	

	translationMatrix = translate(-0.5f, -0.8f, 0.1f);
	//translationMatrix = translate(-12.0f, -10.0f, -20.0f);
	rotationMatrix = rotate(180.0f, 0.0f, 1.0f, 0.0f);
	//scaleMatrix = scale(0.1f, 0.1f, 0.1f);
	modelMatrix = translationMatrix * rotationMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	shaders[Shader_ModelLoading].setMat4("model", modelMatrix);
	models[Model_ModelForChair].Draw(shaders[Shader_ModelLoading]);


	shaders[Shader_ModelLoading].use();
	projection = perspective(camera.Zoom, (float)gWidth / (float)gHeight, 0.1f, 100.0f);//radians
	view = camera.GetViewMatrix();
	shaders[Shader_ModelLoading].setMat4("projection", projection);
	shaders[Shader_ModelLoading].setMat4("view", view);

	MakeModelMatricesIdentity();

	translationMatrix = translate(-0.6f, -1.0f, 0.5f);
	//translationMatrix = translate(-12.0f, -10.0f, -20.0f);
	rotationMatrix = rotate(270.0f, 0.0f, 1.0f, 0.0f);
	scaleMatrix = scale(0.025f, 0.025f, 0.025f);
	modelMatrix = translationMatrix * rotationMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	shaders[Shader_ModelLoading].setMat4("model", modelMatrix);
	models[Model_Chair].Draw(shaders[Shader_ModelLoading]);
}



void SceneComputer::SetSceneNo(int sceneNo)
{
	SceneNo = sceneNo;
}

int SceneComputer::InitScene()
{
	char  char_array[255];
	strcpy(char_array, modulePath);//\\constanb.ttf
	strcat(char_array, "\\constanb.ttf");//Francesca W00 Gothic.ttf

	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		fprintf(gpFile, "Could not init freetype library\n");
		return 1;
	}



	if (FT_New_Face(ft, char_array, 0, &faceConstanb)) {
		fprintf(gpFile, "Could not open font\n");
		return 1;
	}

	FT_Set_Pixel_Sizes(faceConstanb, 0, 48);

	if (FT_Load_Char(faceConstanb, 'X', FT_LOAD_RENDER)) {
		fprintf(gpFile, "Could not load charachter 'X'\n");
		return 1;
	}

	const GLchar* vertexShaderSourceCodeFont =
		"#version 430 core" \
		"\n" \
		"in vec4 vPosition;" \
		"uniform mat4 model;" \
		"uniform mat4 view;" \
		"uniform mat4 projection;" \
		"out vec2 out_texcoord;" \
		"void main(void)" \
		"{" \
		"gl_Position=projection*view*model*vec4(vPosition.xy,0.0,1.0);" \
		"out_texcoord=vPosition.zw;" \
		"}";

	const GLchar* fragmentShaderSourceCodeFont =
		"#version 430 core" \
		"\n" \
		"in vec2 out_texcoord;" \
		"uniform sampler2D u_sampler;" \
		"uniform vec3 textColor;" \
		"uniform int flag;"\
		"uniform float mixColor;"\
		"uniform float mixColor1;"\
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"vec4 sampled=vec4(1.0,1.0,1.0,texture(u_sampler,out_texcoord));"

		"FragColor=(vec4(textColor,1.0)*sampled);" \


		"}";

	Shader objShader_FontFront(true, vertexShaderSourceCodeFont, fragmentShaderSourceCodeFont);

	shaders.push_back(objShader_FontFront);

	//uniform_texFont = glGetUniformLocation(gShaderProgramObjectFont, "u_sampler");
	//colorTextFont = glGetUniformLocation(gShaderProgramObjectFont, "textColor");


	for (GLubyte c = 0; c < 128; c++)
	{
		if (FT_Load_Char(faceConstanb, c, FT_LOAD_RENDER))
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
			faceConstanb->glyph->bitmap.width,
			faceConstanb->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			faceConstanb->glyph->bitmap.buffer
		);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		Character character =
		{
			texture,
			vmath::ivec2(faceConstanb->glyph->bitmap.width,faceConstanb->glyph->bitmap.rows),
			vmath::ivec2(faceConstanb->glyph->bitmap_left,faceConstanb->glyph->bitmap_top),
			faceConstanb->glyph->advance.x
		};

		CharatersConstanb.insert(std::pair<GLchar, Character>(c, character));

	}
	glBindTexture(GL_TEXTURE_2D, 0);


	FT_Done_Face(faceConstanb);
	FT_Done_FreeType(ft);




	glGenVertexArrays(1, &vaoFontConstanb);
	glBindVertexArray(vaoFontConstanb);
	glGenBuffers(1, &vboFontConstanb);
	glBindBuffer(GL_ARRAY_BUFFER, vboFontConstanb);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);




	const GLchar* vertexShaderSourceCodeFrontFontDisp =
		"#version 430" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec4 vColor;" \
		"uniform mat4 model;" \
		"uniform mat4 view;" \
		"uniform mat4 projection;" \
		"out vec4 out_color;" \
		"void main(void)" \
		"{" \
		"gl_Position=projection*view*model*vPosition;" \
		"out_color=vColor;"
		"}";

	const GLchar* fragmentShaderSourceCodeFrontFontDisp =
		"#version 430" \
		"\n" \
		"in vec4 out_color;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor=out_color;" \
		"}";


	Shader objShader_FrontFontDisp(true, vertexShaderSourceCodeFrontFontDisp, fragmentShaderSourceCodeFrontFontDisp);

	shaders.push_back(objShader_FrontFontDisp);

	const GLchar* vertexShaderSourceCodeFrontFontWall =
		"#version 430" \
		"\n" \
		"layout(location = 5) in vec4 vPosition;" \
		"in vec4 vColor;" \
		"uniform mat4 model;" \
		"uniform mat4 view;" \
		"uniform mat4 projection;" \
		"out vec4 out_color;" \
		"void main(void)" \
		"{" \
		"gl_Position=projection*view*model*vPosition;" \
		"out_color=vColor;"
		"}";

	const GLchar* fragmentShaderSourceCodeFrontFontWall =
		"#version 430" \
		"\n" \
		"in vec4 out_color;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor=out_color;" \
		"}";

	Shader objShader_FrontWall(true, vertexShaderSourceCodeFrontFontWall, fragmentShaderSourceCodeFrontFontWall);

	shaders.push_back(objShader_FrontWall);


	const GLfloat wallVertices[] =
	{
		1.45f, 3.3f, 0.0f,

		-1.0f, 3.3f, 0.0f,

		-1.0f, -2.3f, 0.0f,

		1.45f, -2.3f, 0.0f,


		6.45f,-0.25f,0.0f,

		1.45f,-0.25f,0.0f,

		1.45f,-2.3f,0.0f,

		6.45f,-2.3f,0.0f,


		6.45f,3.3f,0.0f,

		3.3f,3.3f,0.0f,

		3.3f,-0.25f,0.0f,

		6.45f,-0.25f,0.0f,


		3.3f,3.3f,0.0f,

		1.45f,3.3f,0.0f,

		1.45f,1.2f,0.0f,

		3.3f,1.2f,0.0f,

	};

	const GLfloat wallColor[] =
	{
		0.0f,1.0f,1.0f,
		0.0f,1.0f,1.0f,
		0.0f,1.0f,1.0f,
		0.0f,1.0f,1.0f,

		0.0f,1.0f,1.0f,
		0.0f,1.0f,1.0f,
		0.0f,1.0f,1.0f,
		0.0f,1.0f,1.0f,

		0.0f,1.0f,1.0f,
		0.0f,1.0f,1.0f,
		0.0f,1.0f,1.0f,
		0.0f,1.0f,1.0f,

		0.0f,1.0f,1.0f,
		0.0f,1.0f,1.0f,
		0.0f,1.0f,1.0f,
		0.0f,1.0f,1.0f
		//0.9725f,0.7176f,0.6392f,
	};

	const float blackRectanglePosition1[] = {
		0.5f,1.0f,0.0f,
		-1.0f,1.0f,0.0f,
		-1.0f,0.9f,0.0f,
		0.5f,0.90f,0.0f
	};

	const float blackRectangleColor1[] = {
		0.75f,0.75f,0.75f,
		0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,
	};

	const float blackRectanglePosition2[] = {
		-0.90f,0.90f,0.0f,
		-1.0f,0.9f,0.0f,
		-1.0f,0.1f,0.0f,
		-0.9f,0.1f,0.0f

	};

	const float blackRectangleColor2[] = {
		0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,
	};

	const float blackRectanglePosition3[] = {
		0.5f,0.9f,0.0f,
		0.4f,1.0f,0.0f,
		0.4f,0.1f,0.0f,
		0.5f,0.1f,0.0f

	};

	const float blackRectangleColor3[] = {
		0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,
	};

	const float blackRectanglePosition4[] = {
		0.5f,0.1f,0.0f,
		-1.0f,0.1f,0.0f,
		-1.0f,0.0f,0.0f,
		0.5f,0.0f,0.0f

	};

	const float blackRectangleColor4[] = {
		0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,
	};


	const float blackRectanglePosition5[] = {
		-0.15f,0.0f,0.0f,
		-0.35f,0.0f,0.0f,
		-0.35f,-0.2f,0.0f,
		-0.15f,-0.2f,0.0f
	};

	const float blackRectangleColor5[] = {
		0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,
	};

	const float blackRectanglePosition6[] = {
		-0.5f,-0.2f,0.0f,
		0.0f,-0.2f,0.0f,
		0.0f,-0.1f,0.0f,
		-0.5f,-0.1f,0.0f
	};

	const float blackRectangleColor6[] = {
		0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,0.75f,
	};


	const float smallComputerPosition1[] = {
		0.25f,0.33f,0.0f,
		-0.30f,0.33f,0.0f,
		-0.30f,0.0f,0.0f,
		0.25f,0.0f,0.0f
	};

	const float smallComputerColor[] = {
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,
	};


	const float smallComputerPosition2[] = {
		0.18f,0.3f,0.0f,
		-0.23f,0.3f,0.0f,
		-0.23f,0.04f,0.0f,
		0.18f,0.04f,0.0f
	};

	const float smallComputerColor2[] = {
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
	};


	const float table[] = {
		-1.85f,-0.25f,0.0f,
		1.15f,-0.25f,0.0f,
		1.15f,-0.35f,0.0f,
		-1.85f,-0.35f,0.0f,

		-1.25f,-0.35f,0.0f,
		-1.35f,-0.35f,0.0f,
		-1.35f,-1.5f,0.0f,
		-1.25f,-1.5f,0.0f,

		0.75f,-0.35f,0.0f,
		0.65f,-0.35f,0.0f,
		0.65f,-1.5f,0.0f,
		0.75f,-1.5f,0.0f,

		1.05f,-0.25f,0.0f,
		0.70f,-0.25f,0.0f,
		0.70f,-0.15f,0.0f,
		1.05f,-0.15f,0.0f,

		1.05f,-0.05f,0.0f,
		0.70f,-0.05f,0.0f,
		0.70f,-0.15f,0.0f,
		1.05f,-0.15f,0.0f,
	};


	const float tableColor[] = {
		0.4784313725490196f,0.20f,0.019f,
		0.4784313725490196f,0.20f,0.019f,
		0.4784313725490196f,0.20f,0.019f,
		0.4784313725490196f,0.20f,0.019f,

		0.4784313725490196f,0.20f,0.019f,
		0.4784313725490196f,0.20f,0.019f,
		0.4784313725490196f,0.20f,0.019f,
		0.4784313725490196f,0.20f,0.019f,


		0.4784313725490196f,0.20f,0.019f,
		0.4784313725490196f,0.20f,0.019f,
		0.4784313725490196f,0.20f,0.019f,
		0.4784313725490196f,0.20f,0.019f,

		1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f,
		1.0f,0.0f,0.0f,
		1.0f,1.0f,0.0f,

		1.0f,0.0f,0.0f,
		1.0f,0.0f,0.0f,
		1.0f,0.0f,0.0f,
		1.0f,1.0f,0.0f,
	};



	glGenVertexArrays(1, &vao_studytable);
	glBindVertexArray(vao_studytable);
	glGenBuffers(1, &vbo_rectangle_position_studytable);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_rectangle_position_studytable);
	glBufferData(GL_ARRAY_BUFFER, sizeof(table), table, GL_STATIC_DRAW);
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(5);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, &vbo_rectangle_color_studytable);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_rectangle_color_studytable);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tableColor), tableColor, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//Pyramid
	glGenVertexArrays(1, &vao_rectangle1_computer);
	glBindVertexArray(vao_rectangle1_computer);
	glGenBuffers(1, &vbo_position_rectangle1_computer);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_rectangle1_computer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(blackRectanglePosition1), blackRectanglePosition1, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, &vbo_color_rectangle1_computer);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color_rectangle1_computer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(blackRectangleColor1), blackRectangleColor1, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	glGenVertexArrays(1, &vao_rectangle2_computer);
	glBindVertexArray(vao_rectangle2_computer);
	glGenBuffers(1, &vbo_position_rectangle2_computer);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_rectangle2_computer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(blackRectanglePosition2), blackRectanglePosition2, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, &vbo_color_rectangle2_computer);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color_rectangle2_computer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(blackRectangleColor2), blackRectangleColor2, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	glGenVertexArrays(1, &vao_rectangle3_computer);
	glBindVertexArray(vao_rectangle3_computer);
	glGenBuffers(1, &vbo_position_rectangle3_computer);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_rectangle3_computer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(blackRectanglePosition3), blackRectanglePosition3, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, &vbo_color_rectangle3_computer);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color_rectangle3_computer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(blackRectangleColor3), blackRectangleColor3, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	glGenVertexArrays(1, &vao_rectangle4_computer);
	glBindVertexArray(vao_rectangle4_computer);
	glGenBuffers(1, &vbo_position_rectangle4_computer);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_rectangle4_computer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(blackRectanglePosition4), blackRectanglePosition4, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, &vbo_color_rectangle4_computer);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color_rectangle4_computer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(blackRectangleColor4), blackRectangleColor4, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glGenVertexArrays(1, &vao_rectangle5_computer);
	glBindVertexArray(vao_rectangle5_computer);
	glGenBuffers(1, &vbo_position_rectangle5_computer);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_rectangle5_computer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(blackRectanglePosition5), blackRectanglePosition5, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, &vbo_color_rectangle5_computer);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color_rectangle5_computer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(blackRectangleColor5), blackRectangleColor5, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	glGenVertexArrays(1, &vao_rectangle6_computer);
	glBindVertexArray(vao_rectangle6_computer);
	glGenBuffers(1, &vbo_position_rectangle6_computer);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_rectangle6_computer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(blackRectanglePosition6), blackRectanglePosition6, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, &vbo_color_rectangle6_computer);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color_rectangle6_computer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(blackRectangleColor6), blackRectangleColor6, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//=============================================================================================

	glGenVertexArrays(1, &vao_smallSquare1_Computer);
	glBindVertexArray(vao_smallSquare1_Computer);
	glGenBuffers(1, &vbo_smallSquare1_position_computer);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_smallSquare1_position_computer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(smallComputerPosition1), smallComputerPosition1, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, &vbo_smallSquare1_color_computer);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_smallSquare1_color_computer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(smallComputerColor), smallComputerColor, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	glGenVertexArrays(1, &vao_smallSquare2_Computer);
	glBindVertexArray(vao_smallSquare2_Computer);
	glGenBuffers(1, &vbo_smallSquare2_position_computer);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_smallSquare2_position_computer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(smallComputerPosition2), smallComputerPosition2, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, &vbo_smallSquare2_color_computer);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_smallSquare2_color_computer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(smallComputerColor2), smallComputerColor2, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	glGenVertexArrays(1, &vao_rectangle_wall);
	glBindVertexArray(vao_rectangle_wall);
	glGenBuffers(1, &vbo_rectangle_position_wall);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_rectangle_position_wall);
	glBufferData(GL_ARRAY_BUFFER, sizeof(wallVertices), wallVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(5);//Specified Layout=5
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_rectangle_color_wall);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_rectangle_color_wall);
	glBufferData(GL_ARRAY_BUFFER, sizeof(wallColor), wallColor, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return 1;
}

void SceneComputer::DisplayScene()
{
	if (SceneNo == 0) {
		glEnable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


		DrawFrontSequence();

		shaders[Shader_FrontFontDisp].use();

		projection = perspective(camera.Zoom, (float)gWidth / (float)gHeight, 0.1f, 100.0f);//radians
		view = camera.GetViewMatrix();
		shaders[Shader_FrontFontDisp].setMat4("projection", projection);
		shaders[Shader_FrontFontDisp].setMat4("view", view);

		MakeModelMatricesIdentity();
		//Transformation

		modelMatrix = translate(0.0f, 0.0f, -1.0f);;

		shaders[Shader_FrontFontDisp].setMat4("model", modelMatrix);

		glBindVertexArray(vao_rectangle1_computer);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glBindVertexArray(0);

		MakeModelMatricesIdentity();

		modelMatrix = translate(0.0f, 0.0f, -1.0f);
		shaders[Shader_FrontFontDisp].setMat4("model", modelMatrix);

		glBindVertexArray(vao_rectangle2_computer);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glBindVertexArray(0);

		MakeModelMatricesIdentity();

		modelMatrix = translate(0.0f, 0.0f, -1.0f);
		shaders[Shader_FrontFontDisp].setMat4("model", modelMatrix);

		glBindVertexArray(vao_rectangle3_computer);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glBindVertexArray(0);


		MakeModelMatricesIdentity();

		modelMatrix = translate(0.0f, 0.0f, -1.0f);
		shaders[Shader_FrontFontDisp].setMat4("model", modelMatrix);

		glBindVertexArray(vao_rectangle4_computer);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glBindVertexArray(0);

		MakeModelMatricesIdentity();

		modelMatrix = translate(0.0f, 0.0f, -1.0f);
		shaders[Shader_FrontFontDisp].setMat4("model", modelMatrix);

		glBindVertexArray(vao_rectangle5_computer);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glBindVertexArray(0);

		MakeModelMatricesIdentity();

		modelMatrix = translate(0.0f, 0.0f, -1.0f);
		shaders[Shader_FrontFontDisp].setMat4("model", modelMatrix);

		glBindVertexArray(vao_rectangle6_computer);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glBindVertexArray(0);

		if (counter18 == 1)
		{

			MakeModelMatricesIdentity();

			modelMatrix = translate(0.0f, 0.3f, translateScreen);
			shaders[Shader_FrontFontDisp].setMat4("model", modelMatrix);

			glBindVertexArray(vao_smallSquare1_Computer);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			glBindVertexArray(0);



			MakeModelMatricesIdentity();

			modelMatrix = translate(0.0f, 0.3f, translateScreen);
			shaders[Shader_FrontFontDisp].setMat4("model", modelMatrix);

			glBindVertexArray(vao_smallSquare2_Computer);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			glBindVertexArray(0);

			//counter19 = 1;

		}
		glUseProgram(0);

		shaders[Shader_FrontWall].use();


		projection = perspective(camera.Zoom, (float)gWidth / (float)gHeight, 0.1f, 100.0f);//radians
		view = camera.GetViewMatrix();
		shaders[Shader_FrontWall].setMat4("projection", projection);
		shaders[Shader_FrontWall].setMat4("view", view);

		MakeModelMatricesIdentity();
		//Transformation

		modelMatrix = translate(-2.7f, 0.0f, -2.0f);

		modelMatrix = modelMatrix * scaleMatrix;

		shaders[Shader_FrontWall].setMat4("model", modelMatrix);


		glBindVertexArray(vao_rectangle_wall);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
		glBindVertexArray(0);

		MakeModelMatricesIdentity();

		modelMatrix = translate(0.0f, 0.0f, -1.0f);

		shaders[Shader_FrontWall].setMat4("model", modelMatrix);

		glBindVertexArray(vao_studytable);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
		glBindVertexArray(0);

		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);

	}
	else {
		if (firstDisplay == 1) {
			firstDisplay = 0;
			camera.ResetCamera();
		}

		glEnable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		DrawLastSequence();

		shaders[Shader_FrontFontDisp].use();

		projection = perspective(camera.Zoom, (float)gWidth / (float)gHeight, 0.1f, 100.0f);//radians
		view = camera.GetViewMatrix();
		shaders[Shader_FrontFontDisp].setMat4("projection", projection);
		shaders[Shader_FrontFontDisp].setMat4("view", view);

		MakeModelMatricesIdentity();
		//Transformation

		modelMatrix = translate(0.0f, 0.0f, -1.0f);;

		shaders[Shader_FrontFontDisp].setMat4("model", modelMatrix);

		glBindVertexArray(vao_rectangle1_computer);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glBindVertexArray(0);

		MakeModelMatricesIdentity();

		modelMatrix = translate(0.0f, 0.0f, -1.0f);
		shaders[Shader_FrontFontDisp].setMat4("model", modelMatrix);

		glBindVertexArray(vao_rectangle2_computer);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glBindVertexArray(0);

		MakeModelMatricesIdentity();

		modelMatrix = translate(0.0f, 0.0f, -1.0f);
		shaders[Shader_FrontFontDisp].setMat4("model", modelMatrix);

		glBindVertexArray(vao_rectangle3_computer);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glBindVertexArray(0);


		MakeModelMatricesIdentity();

		modelMatrix = translate(0.0f, 0.0f, -1.0f);
		shaders[Shader_FrontFontDisp].setMat4("model", modelMatrix);

		glBindVertexArray(vao_rectangle4_computer);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glBindVertexArray(0);

		MakeModelMatricesIdentity();

		modelMatrix = translate(0.0f, 0.0f, -1.0f);
		shaders[Shader_FrontFontDisp].setMat4("model", modelMatrix);

		glBindVertexArray(vao_rectangle5_computer);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glBindVertexArray(0);

		MakeModelMatricesIdentity();

		modelMatrix = translate(0.0f, 0.0f, -1.0f);
		shaders[Shader_FrontFontDisp].setMat4("model", modelMatrix);

		glBindVertexArray(vao_rectangle6_computer);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glBindVertexArray(0);


		glUseProgram(0);

		shaders[Shader_FrontWall].use();


		projection = perspective(camera.Zoom, (float)gWidth / (float)gHeight, 0.1f, 100.0f);//radians
		view = camera.GetViewMatrix();
		shaders[Shader_FrontWall].setMat4("projection", projection);
		shaders[Shader_FrontWall].setMat4("view", view);

		MakeModelMatricesIdentity();
		//Transformation

		modelMatrix = translate(-2.7f, 0.0f, -2.0f);

		shaders[Shader_FrontWall].setMat4("model", modelMatrix);


		glBindVertexArray(vao_rectangle_wall);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
		glBindVertexArray(0);

		MakeModelMatricesIdentity();

		modelMatrix = translate(0.0f, 0.0f, -1.0f);

		shaders[Shader_FrontWall].setMat4("model", modelMatrix);

		glBindVertexArray(vao_studytable);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
		glBindVertexArray(0);

		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
	}
}

void SceneComputer::UpdateScene()
{
	if (SceneNo == 0) {

		if (PrevSceneNo != SceneNo) {
			//counter = 0;
			//counter1 = 0;
			//counter2 = 0;
			//counter3 = 0;
			//counter4 = 0;
			//counter5 = 0;
			//counter6 = 0;
			//counter7 = 0;
			//counter8 = 0;
			//counter9 = 0;
			//counter10 = 0;
			//counter11 = 0;
			//counter12 = 0;
			//counter13 = 0;
			//counter14 = 0;
			//counter15 = 0;
			//counter16 = 0;
			counter17 = 0;
			counter18 = 0;
			counter19 = 0;
			counterTime = 0.0f;
			PrevSceneNo = SceneNo;
			camera.ResetCamera();

			stage = 0;
			zMovement = 0.0004f;
			translateFont = -97.0f;
			translateScreen = -1.0f;

		}

		counterTime = counterTime + 0.01f;
		//counterTime1 = counterTime1 + 0.01f;
		if (counterTime >= 0.710000f&&counterTime <= 0.740000f)
		{
			stage = 1;
			//counter = 1;
		}

		if (counterTime >= 1.549999f&&counterTime <= 1.579999f)
		{
			stage = 1;
			//counter1 = 1;
		}

		if (counterTime >= 2.049999f&&counterTime <= 2.069999f)
		{
			stage = 2;
			//counter2 = 1;
		}

		if (counterTime >= 2.549998f&&counterTime <= 2.579998f)
		{
			stage = 3;
			//counter3 = 1;
		}
		if (counterTime >= 3.049998f&&counterTime <= 3.079998f)
		{
			stage = 4;
			//counter4 = 1;
		}
		if (counterTime >= 4.039998f&&counterTime <= 4.069998f)
		{
			stage = 5;
			//counter5 = 1;
		}
		if (counterTime >= 5.000020f&&counterTime <= 5.030020f)
		{
			stage = 6;
			//counter6 = 1;
		}
		if (counterTime >= 5.500031f&&counterTime <= 5.520031f)
		{
			stage = 7;
			//counter7 = 1;
		}
		if (counterTime >= 6.000042f&&counterTime <= 6.030043f)
		{
			stage = 8;
			//counter8 = 1;
		}
		if (counterTime >= 6.800061f&&counterTime <= 6.830061f)
		{
			stage = 9;
			//counter9 = 1;
		}
		if (counterTime >= 7.250071f&&counterTime <= 7.270072f)
		{
			stage = 10;
			//counter10 = 1;
		}

		if (counterTime >= 8.000088f&&counterTime <= 8.020088f)
		{
			stage = 11;
			//counter11 = 1;
		}
		if (counterTime >= 8.650103f&&counterTime <= 8.680103f)
		{
			stage = 12;
			//counter12 = 1;
		}
		if (counterTime >= 9.100113f&&counterTime <= 9.130114f)
		{
			stage = 13;
			//counter13 = 1;
		}
		if (counterTime >= 9.850130f&&counterTime <= 9.880131f)
		{
			stage = 14;
			//counter14 = 1;
		}
		if (counterTime >= 10.500145f&&counterTime <= 10.530146f)
		{
			stage = 15;
			//counter15 = 1;
		}
		if (counterTime >= 11.000156f&&counterTime <= 11.330f)
		{
			stage = 16;
			//counter16 = 1;
		}
		if (counterTime >= 11.330168f&&counterTime <= 12.000169f)
		{
			stage = 17;
			counter17 = 1;
		}
		if (counterTime >= 12.000209f)
		{
			counter19 = 1;
		}

		if (counter19 == 1)
		{
			//camera.ProcessKeyboard(FORWARD, 0.004f);
			if (camera.Pitch <= 4.5) {
				camera.ProcessMouseMovement(0.0f, 0.55f, true);
			}
			else {
				zMovement = zMovement * 1.3f;
				camera.ProcessKeyboard(FORWARD, zMovement);
			}
			translateFont = 3.0f;
			translateScreen = 3.0f;
		}
	}
	else {
		if (PrevSceneNo != SceneNo) {
			//counter = 0;
			//counter1 = 0;
			//counter2 = 0;
			//counter3 = 0;
			//counter4 = 0;
			//counter5 = 0;
			//counter6 = 0;
			//counter7 = 0;
			//counter8 = 0;
			//counter9 = 0;
			//counter10 = 0;
			//counter11 = 0;
			//counter12 = 0;
			//counter13 = 0;
			//counter14 = 0;
			//counter15 = 0;
			counter16 = 0;
			counter17 = 0;
			stage = 0;
			counter18 = 0;
			counter19 = 0;
			counterTime = 5.000020f;
			//counterTime1 = 0.0f;
			PrevSceneNo = SceneNo;
			camera.ResetCamera();
		}

		counterTime = counterTime + 0.01f;
		//counterTime1 = counterTime1 + 0.01f;

		if (counterTime >= 5.000020f)
		{
			stage = 6;
			//counter6 = 1;
		}
		if (counterTime >= 5.500031f)
		{
			stage = 7;

			//counter7 = 1;
		}
		if (counterTime >= 6.000042f)
		{
			stage = 8;

			//counter8 = 1;
		}
		if (counterTime >= 6.800061f)
		{
			stage = 9;

			//counter9 = 1;
		}
		if (counterTime >= 7.250071f)
		{
			stage = 10;
			//counter10 = 1;
		}

		if (counterTime >= 8.000088f)
		{
			stage = 11;
			//counter11 = 1;
		}
		if (counterTime >= 8.650103f)
		{
			stage = 12;
			//counter12 = 1;
		}
		if (counterTime >= 9.100113f)
		{
			stage = 13;
			//counter13 = 1;
		}
		if (counterTime >= 9.850130f)
		{
			stage = 14;
			//counter14 = 1;
		}
	
		/*	if (counterTime >= 11.000156f&&counterTime <= 1.639999f)
			{
				counter16 = 1;
			}
			if (counterTime >= 11.500168f&&counterTime <= 11.530169f)
			{
				counter17 = 1;
			}*/

	}

}

void SceneComputer::UninitializeScene()
{
	// Delete Vertex buffers : vbo's 
	DeleteVertexBuffer(&vbo_rectangle_position_studytable);
	DeleteVertexBuffer(&vbo_rectangle_color_studytable);

	DeleteVertexBuffer(&vbo_position_rectangle1_computer);
	DeleteVertexBuffer(&vbo_position_rectangle2_computer);
	DeleteVertexBuffer(&vbo_position_rectangle3_computer);
	DeleteVertexBuffer(&vbo_position_rectangle4_computer);
	DeleteVertexBuffer(&vbo_position_rectangle5_computer);
	DeleteVertexBuffer(&vbo_position_rectangle6_computer);

	DeleteVertexBuffer(&vbo_color_rectangle1_computer);
	DeleteVertexBuffer(&vbo_color_rectangle2_computer);
	DeleteVertexBuffer(&vbo_color_rectangle3_computer);
	DeleteVertexBuffer(&vbo_color_rectangle4_computer);
	DeleteVertexBuffer(&vbo_color_rectangle5_computer);
	DeleteVertexBuffer(&vbo_color_rectangle6_computer);

	DeleteVertexBuffer(&vbo_smallSquare1_position_computer);
	DeleteVertexBuffer(&vbo_smallSquare2_position_computer);

	DeleteVertexBuffer(&vbo_smallSquare1_color_computer);
	DeleteVertexBuffer(&vbo_smallSquare2_color_computer);

	DeleteVertexBuffer(&vbo_rectangle_position_wall);
	DeleteVertexBuffer(&vbo_rectangle_color_wall);

	// Delete Vertex Arrays : vao's
	DeleteVertexArray(&vaoFontConstanb);
	DeleteVertexArray(&vao_studytable);
	DeleteVertexArray(&vao_rectangle1_computer);
	DeleteVertexArray(&vao_rectangle2_computer);
	DeleteVertexArray(&vao_rectangle3_computer);
	DeleteVertexArray(&vao_rectangle4_computer);
	DeleteVertexArray(&vao_rectangle5_computer);
	DeleteVertexArray(&vao_rectangle6_computer);

	DeleteVertexArray(&vao_smallSquare1_Computer);
	DeleteVertexArray(&vao_smallSquare2_Computer);
	DeleteVertexArray(&vao_rectangle_wall);
}
