#include "ModelAnimation.h"

float tranzRain = 0;
float tranyRain = 0;

float tranxStudyRoom = 0;

int animationFrameId = 0;


void DrawModelRain()
{
	shaders[Shader_ModelLoading].use();

	// view/projection transformations
	projection = perspective(camera.Zoom, (float)gWidth / (float)gHeight, 0.1f, 100.0f);//radians
	view = camera.GetViewMatrix();
	shaders[Shader_ModelLoading].setMat4("projection", projection);
	shaders[Shader_ModelLoading].setMat4("view", view);

	MakeModelMatricesIdentity();
	//Transformation

	translationMatrix = translate(0.0f, -2.0f + tranyRain, 60.0f - tranzRain);
	rotationMatrix = rotate(180.0f, 0.0f, 1.0f, 0.0f);
	scaleMatrix = scale(4.0f, 4.0f, 4.0f);
	modelMatrix = translationMatrix * rotationMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	shaders[Shader_ModelLoading].setMat4("model", modelMatrix);
	AnimationModels[animationFrameId].Draw(shaders[Shader_ModelLoading]);
}

int gNoOfFrames = 0;

void AnimationLoading(char modulePath[255], int noOfFrames)
{
	gNoOfFrames = noOfFrames;
	for (int i = 1; i <= noOfFrames; i++) {
		char char_array[255];
		char buffer[10];
		_itoa(i, buffer, 10);
		strcpy(char_array, modulePath);

		strcat(char_array, "\\Resources\\CartoonAnimation\\Animation_Cartoon_Starter_000");
		if (i < 10) {
			strcat(char_array, "00");
		}
		else if (i < 100) {
			strcat(char_array, "0");
		}
		strcat(char_array, buffer);
		strcat(char_array, ".obj");

		Model model(char_array);
		AnimationModels.push_back(model);

		showLoading();
	}

}


void UpdateModelRain() {
	animationFrameId++;

	if (animationFrameId >= gNoOfFrames) {
		animationFrameId = 0;
	}

	tranzRain = tranzRain + 0.040f;
	tranyRain = tranyRain + 0.0040f;
}


void UpdateModelStudyRoom() {
	animationFrameId++;

	if (animationFrameId >= gNoOfFrames) {
		animationFrameId = 0;
	}

	tranxStudyRoom = tranxStudyRoom + 0.0060f;
}

void DrawModelStudyRoom()
{
	shaders[Shader_ModelLoading].use();

	// view/projection transformations
	projection = perspective(camera.Zoom, (float)gWidth / (float)gHeight, 0.1f, 100.0f);//radians
	view = camera.GetViewMatrix();
	shaders[Shader_ModelLoading].setMat4("projection", projection);
	shaders[Shader_ModelLoading].setMat4("view", view);

	MakeModelMatricesIdentity();
	//Transformation

	translationMatrix = translate(-1.5f+ tranxStudyRoom, -1.5f, -1.0f);
	rotationMatrix = rotate(90.0f, 0.0f, 1.0f, 0.0f);
	//scaleMatrix = scale(4.0f, 4.0f, 4.0f);
	modelMatrix = translationMatrix * rotationMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	shaders[Shader_ModelLoading].setMat4("model", modelMatrix);
	AnimationModels[animationFrameId].Draw(shaders[Shader_ModelLoading]);

}