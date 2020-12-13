#pragma once
#include"Common.h"
#include"Textures.h"
#include"SceneCloth.h"
#include"SceneLeaf.h"
#include"SceneHearts.h"
#include"SceneStarField.h"
#include"SceneNight.h"
#include"SceneDay.h"
#include"SceneTerrain.h"
#include"SceneCouple.h"
#include"SceneCubemap.h"
#include"SceneAMC.h"
#include"SceneComputer.h"
#include"SceneRTR.h"
#include"SceneRain.h"
#include"ModelAnimation.h"
#include"SceneCollage.h"
#include"SceneStudyRoom.h"
#include"SceneRipple.h"
#include"SceneInterop.h"
#include"SceneEndCredits.h"


#include"Font.h"

/*Common vectors*/
vector<Model> models;
vector<Shader> shaders;
vector<GLuint> textures;

vector<Model> AnimationModels;
/*Matrices*/
mat4 projection;
mat4 view;
mat4 modelMatrix;
mat4 translationMatrix;
mat4 rotationMatrix;
mat4 scaleMatrix;

//Camera
Camera camera(vec3(0.0f, 0.0f, 3.0f));


/*Common Width and Height*/
float gWidth = 0;
float gHeight = 0;

#ifndef UNICODE  
typedef std::string String;
#else
typedef std::wstring String;
#endif


void UpdateScene(void);
bool updateCallPerSecond[FPS];

void ModelLoading(char modulePath[255], const char modelPath[255]);


///////////////////
// OpenAL
char* audioData;
int audioChannel, audioSampleRate, audiobps, audioSize;
ALCcontext* audioContext;
unsigned int audioBufferid, audioFormat;
unsigned int audioSourceid;
ALCdevice* audioDevice;
///////////////////

void InitAudio(char modulePath[255]);

clock_t deltaTime = 0;
clock_t currentFrameTime = 0;

//int resetFrameRate = 0;
//int prevScene = 0;
int scene = 0;
long gSec = 0;
clock_t gTimeInMS = 0;
double frameRate = 60;
int ActualFPS = 0;
int currentFrameRate = 0;

int loadingCount = 3;
int frate = 0;

//Global Variables
HWND ghwnd = NULL;
DWORD dwStyle;
bool bFullScreen = false;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
HDC ghdc = NULL;
HGLRC ghrc = NULL;
bool gbActiveWindow = false;
FILE *gpFile = NULL;
FILE *gpFileForFPS = NULL;

GLfloat angleTriangle = 0.0;

int fupdate = 0;

Point P_Prev;
Point P_Prev_Sec;
Point P_Prev_thr;
Point P_Next;
Point P_New;

int Prev_point;
int Next_point;
int Prev_Sec_Point;
int Prev_thr_Point;

char modulePath[255];

void uninitializeAudio(void);


IScene *objSceneRipple;
IScene *objSceneAMC;
IScene *objSceneCloth;
IScene *objSceneCollage;
SceneComputer objSceneComputer;
IScene *objSceneCouple;
IScene *objSceneCubemap;
IScene *objSceneHearts;
IScene *objSceneLeaf;
IScene *objSceneNight;
IScene *objSceneRain;
IScene *objSceneRTR;
IScene *objSceneStarField;
IScene *objSceneStudyRoom;
IScene *objSceneTerrain;
IScene *objSceneDay;
IScene *objSceneInterop;
IScene *objSceneEndCredits;

Font * sceneFeatures;

BOOL GetModulePath(TCHAR* pBuf, DWORD dwBufSize) {
	if (GetModuleFileName(NULL, pBuf, dwBufSize)) {
		//PathCchRemoveFileSpec(pBuf,dwBufSize); // remove executable name
		PathRemoveFileSpec(pBuf);
		return TRUE;
	}
	return FALSE;
}