#ifndef COMMON_H
#define COMMON_H

#include"Main\lib.h"
#include"Main\common.h"
#include"shlwapi.h"
#include"Uninitialize.h"

//Model
#include "Model\model.h"
#include "Model\shader_m.h"
#include "Model\camera.h"

//Time
#include <sys/timeb.h>
#include <time.h>

//Font
#include<map>
#include<ft2build.h>

#include FT_FREETYPE_H
#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib, "freetype.lib")


#define WGL_WGLEXT_PROTOTYPES
#include"GL\wglext.h"

#include "Sphere.h"

#include "Template.h"

#include"SOIL2.h"

#include"SB7\sb7ktx.h"
#include "LoadWav.h"
#include"vmath.h"

#include"IScene.h"

#define FPS 60
//Constants

#define FBO_SIZE 2048

#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define PRIM_RESTART 0xffffff
#define MAXPARTICLES 500

/*Enumerations*/
enum {
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXCOORD0,
	AMC_ATTRIBUTE_VERTEX
};

enum {
	Shader_ModelLoading = 0,
	Shader_NightScene,
	Shader_DisplayNightScene,
	Shader_FilterSceneNightScene,
	Shader_ColorNightScene,
	Shader_NightSceneTree,
	Shader_NightSceneStar,
	Shader_Leaf,
	Shader_Hearts,
	Shader_StarField,
	Shader_CubeMap,
	Shader_Bubble,
	Shader_Flag,
	Shader_ComputeFlag,
	Shader_ComputeNormalFlag,
	Shader_Character,
	Shader_Couple,
	Shader_SingleHeart,
	Shader_Terrain,
	Shader_ButterflySunrays,
	//Shader_Bark,
	//Shader_Tree,
	//Shader_Mango,
	Shader_Cloud,
	Shader_Grass,
	Shader_Flower,
	Shader_Font,
	Shader_FontFront,
	Shader_FrontFontDisp,
	Shader_FrontWall,
	Shader_StudyRoomWall,
	Shader_RTRDisplay,
	Shader_RTRFilter,
	Shader_RTRResolve,
	Shader_Collage,
	Shader_EndCredits,
	Shader_Rain,
	Shader_BackgroundRain,
	Shader_Ripple,
	Shader_Interop
};

enum {
	Scene_AstroMediComp = 0,
	Scene_Leaf,
	Scene_StudyRoom,
	Scene_FrontText,
	Scene_Star,
	Scene_Night,
	Scene_Couple,
	Scene_Hearts,
	Scene_Day,
	Scene_CubeMap,
	Scene_Rain,
	Scene_Assignment,
	Scene_LastText,
	Scene_Interop,
	Scene_Terrain,
	Scene_RTR,
	Scene_Collage,
	Scene_Flag,
	Scene_Ripple,
	Scene_EndCredits
};

enum {
	Model_Tree = 0,
	Model_Rain,
	Model_ModelForChair,
	Model_Chair,
	Model_Heart
};

enum {
	//NormalLoader
	Texture_Leaf = 0,
	Texture_Star,
	Texture_Moon,
	Texture_Heart,
	Texture_Sir,
	Texture_Astromedicomp,
	Texture_Bark,
	Texture_BackRain,
	Texture_StudyRoom,
	Texture_Collage,
	Texture_Heart_Pink,
	//LoadFORCubeMap
	Texture_Cubemap,
	//LoadFromSB7
	Texture_GrassLengh,
	Texture_GrassOrientation,
	Texture_GrassColor,
	Texture_GrassBend,
	Texture_Terraingen,
	Texture_Terraincolor,
	//LoadFromSOIL
	Texture_Aditi,
	Texture_Amita,
	Texture_Gayatri,
	Texture_Chai,
	Texture_Rain,
};

/*Common vectors*/
extern vector<Model> models;
extern vector<Shader> shaders;
extern vector<GLuint> textures;

/*Matrices*/
extern mat4 projection;
extern mat4 view;
extern mat4 modelMatrix;
extern mat4 translationMatrix;
extern mat4 rotationMatrix;
extern mat4 scaleMatrix;

//Camera
extern Camera camera;

/*Common Width and Height*/
extern float gWidth;
extern float gHeight;

extern FILE *gpFile;


/*Common Structure*/
struct Particle {
	vec3 pos;
	vec3 speed;
	float size;
	float angle;
	float life; // Remaining life of the particle. if <0 : dead and unused.

	float cameradistance; // hardcoded

	bool operator<(const Particle& that) const //
	{
		return this->cameradistance > that.cameradistance;
	}
};


struct Point
{
	GLfloat x, y;
};

struct Character
{
	GLuint TextureID; //ID handle of gylph
	vmath::ivec2 Size; //Size of gylph
	vmath::ivec2 Bearing; //offset from baseline to left/top of glyph
	FT_Pos Advance; //Horizontal offset to advance to next gylph;
};

static Point control_points[5];

extern Point P_Prev;
extern Point P_Prev_Sec;
extern Point P_Prev_thr;
extern Point P_Next;
extern Point P_New;

extern int Prev_point;
extern int Next_point;
extern int Prev_Sec_Point;
extern int Prev_thr_Point;

/*Common Functions*/
void MakeModelMatricesIdentity(void);

extern char modulePath[255];

void showLoading();

#endif
