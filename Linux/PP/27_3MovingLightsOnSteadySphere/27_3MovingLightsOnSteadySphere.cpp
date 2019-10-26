#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>
#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/XKBlib.h>
#include<X11/keysym.h>
#include<string>
#include<string.h>

#include</usr/include/GL/glew.h>
#include<GL/gl.h>
#include<GL/glx.h>
#include</usr/include/SOIL/SOIL.h>
#include"vmath.h"
#include"Sphere.h"

using namespace std;
using namespace vmath;

bool bFullScreen=false;
Display *gpDisplay=NULL;
XVisualInfo *gpXVisualInfo=NULL;
Colormap gColorMap;
Window gWindow;
int giWindowWidth=800;
int giWindowHeight=600;
GLXContext gGLXContext;
bool bDone=false;
FILE *gpFile ; 

typedef GLXContext(* glXCreateContextAttribsARBProc)(
Display *,
GLXFBConfig,
GLXContext,
bool,
GLint *        
);

glXCreateContextAttribsARBProc glXCreateContextAttribsARB=NULL;
GLXFBConfig gGLXFBConfig;
//Shader Program Objects
GLint gShaderProgramObject_PV;
GLint gShaderProgramObject_PF;

enum {
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXCOORD0
};

GLuint vao_sphere;

GLuint vbo_sphere_element;
GLuint vbo_sphere_position;
GLuint vbo_sphere_normal;

//GLuint vbo_color_cube;

GLuint mUniform;
GLuint vUniform;
GLuint pUniform;
GLuint lKeyIsPressedUniform;
GLuint ldUniform;
GLuint kdUniform;
GLuint laUniform;
GLuint kaUniform;
GLuint lsUniform;
GLuint ksUniform;
GLuint lightPositionUniform;
GLuint shininessUniform;

mat4 perspectiveProjectionMatrix;

//GLuint gb_Animation = 0;
GLuint gb_Lighting = 0;

struct Light {
	GLfloat lightAmbient[4];
	GLfloat lightDiffuse[4];
	GLfloat lightSpecular[4];
	GLfloat lightPosition[4];
};
struct Material {
	GLfloat materialAmbient[4];
	GLfloat materialDiffuse[4];
	GLfloat materialSpecular[4];
	GLfloat materialShininess;
};

const int noLights = 3;
float arrayPosition[4 * noLights];
float arrayAmbient[3 * noLights];
float arrayDiffuse[3 * noLights];
float arraySpecular[3 * noLights];
float tempArray[3 * noLights];

struct Light lights[noLights];
struct Material material;
float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];
int gNumVertices;
int gNumElements;

int perVertexOrFragement = 0;//0 Vertex 1 Fragment

void getUniformLocation(GLint gShaderProgramObject);

void Update(void);

int main(void)
{
void CreateWindow(void);
void ToggleFullScreen(void);
void UnInitialize();
void Initialize(void);
void Resize(int,int);
void DisplayOpenGL(void);
gpFile = fopen("ChaitanyaLog.c", "w+");
int winWidth=giWindowWidth;
int winHeight=giWindowHeight;
char keys[26];
CreateWindow();
XEvent event;
KeySym keysym;
Initialize();

while(bDone==false){
    while(XPending(gpDisplay)){
        XNextEvent(gpDisplay,&event);
        switch(event.type)
        {
                case MapNotify:
                break;

                //to get keysym code details type xev in terminal and press enter . press key to get details
                case KeyPress:
                        keysym=XkbKeycodeToKeysym(gpDisplay,event.xkey.keycode,0,0);
                        // static char buffer [60];
                        // static std::string s = std::to_string(keysym);
                        // static const char *cstr = s.c_str();
                        // fputs(cstr, filePointer) ;                   
                        switch(keysym)
                        {
                        case XK_Escape:
                        ToggleFullScreen();
                        break;
                        default:
                        break;
                        }
                        XLookupString(&event.xkey,keys,sizeof(keys),NULL,NULL);
                        switch(keys[0])
                        {
		case 'F':
		case 'f':
			getUniformLocation(gShaderProgramObject_PF);
			perVertexOrFragement = 1;
			break;
		case 'v':
		case 'V':
			getUniformLocation(gShaderProgramObject_PV);
			perVertexOrFragement = 0;
			break;
		case 'l':
		case 'L':
			if (gb_Lighting == 0)
				gb_Lighting = 1;
			else
				gb_Lighting = 0;
			break;
		case 'e':
		case 'E':
			bDone=true;
			break;

                        }
                break;
                case ButtonPress:
                        switch(event.xbutton.button)
                        {
                        case 1:
                        break;
                        case 2:
                        break;
                        case 3:
                        break;
                        default:
                        break;
                        }
                break;
                case MotionNotify:
                break;
                case ConfigureNotify:
                        winWidth=event.xconfigure.width;
                        winHeight=event.xconfigure.height;
                        Resize(winWidth,winHeight);
                break;
                case Expose:
                break;
                case DestroyNotify:
                break;
                case 33:
                    bDone=true;
                default:
                break;
        }        
    }

    Update();
    DisplayOpenGL();
}

//fclose(filePointer); 
UnInitialize();
return(0);
}

void CreateWindow(void)
{
void UnInitialize();
XSetWindowAttributes winAttribs;
int defaultScreen;
int defaultDepth;
int styleMask;

GLXFBConfig *pGLXFBConfig=NULL;
GLXFBConfig bestGLXFBConfig;
XVisualInfo *pTempXVisualInfo=NULL;
int iNoOfFBConfigs=0;

int bestFrameBufferConfig=-1;
int bestNoOfSamples=-1;
int worstFrameBufferConfig=-1;
int worstNoOfSamples=999;

static int FrameBufferAttributes[]={
GLX_X_RENDERABLE,true,
GLX_DRAWABLE_TYPE,GLX_WINDOW_BIT,
GLX_RENDER_TYPE,GLX_RGBA_BIT,
GLX_X_VISUAL_TYPE,GLX_TRUE_COLOR,
GLX_RED_SIZE,8,
GLX_GREEN_SIZE,8,
GLX_BLUE_SIZE,8,
GLX_ALPHA_SIZE,8,
GLX_DEPTH_SIZE,24,
GLX_STENCIL_SIZE,8,
GLX_DOUBLEBUFFER,true,
None    
};

gpDisplay=XOpenDisplay(NULL);
if(gpDisplay == NULL){
        printf("ERROR : Unable To Open X Display.\nExitting Now...\n");
        UnInitialize();
        exit(1);
}
defaultScreen=XDefaultScreen(gpDisplay);

//defaultDepth=DefaultDepth(gpDisplay,defaultScreen);  

pGLXFBConfig=glXChooseFBConfig(gpDisplay,
defaultScreen,
FrameBufferAttributes,
&iNoOfFBConfigs);

for(int i=0;i<iNoOfFBConfigs;i++){
        pTempXVisualInfo=glXGetVisualFromFBConfig(gpDisplay,
        pGLXFBConfig[i]);

        if(pTempXVisualInfo){
                int sampleBuffers,samples;
                
                glXGetFBConfigAttrib(gpDisplay,
                pGLXFBConfig[i],
                GLX_SAMPLE_BUFFERS,
                &sampleBuffers);

                glXGetFBConfigAttrib(gpDisplay,
                pGLXFBConfig[i],
                GLX_SAMPLES,
                &samples);

                if(bestFrameBufferConfig<0 || sampleBuffers && samples>bestNoOfSamples){
                        bestFrameBufferConfig=i;
                        bestNoOfSamples=samples;
                }
                if(worstFrameBufferConfig<0 || !sampleBuffers && samples<worstNoOfSamples){
                        worstFrameBufferConfig=i;
                        worstNoOfSamples=samples;
                }

        }
        XFree(pTempXVisualInfo);
}

bestGLXFBConfig=pGLXFBConfig[bestFrameBufferConfig];
gGLXFBConfig=bestGLXFBConfig;
XFree(pGLXFBConfig);

gpXVisualInfo=glXGetVisualFromFBConfig(gpDisplay,
bestGLXFBConfig);

if(gpXVisualInfo==NULL){
        printf("ERROR : Unable To Get A Visual.\nExitting Now...\n");
        UnInitialize();
        exit(1);
}

winAttribs.border_pixel=0;
winAttribs.background_pixmap=0;
winAttribs.colormap=XCreateColormap(gpDisplay,RootWindow(gpDisplay,gpXVisualInfo->screen),gpXVisualInfo->visual,AllocNone);
gColorMap=winAttribs.colormap;
winAttribs.background_pixel=BlackPixel(gpDisplay,defaultScreen);
winAttribs.border_pixmap=0;
winAttribs.event_mask=ExposureMask | VisibilityChangeMask | ButtonPressMask | KeyPressMask | PointerMotionMask | StructureNotifyMask;
styleMask= CWBorderPixel | CWBackPixel | CWEventMask | CWColormap;
gWindow=XCreateWindow(gpDisplay,RootWindow(gpDisplay,gpXVisualInfo->screen),0,0,giWindowWidth,giWindowHeight,0,gpXVisualInfo->depth,InputOutput,gpXVisualInfo->visual,styleMask,&winAttribs);
if(!gWindow)
{
        printf("ERROR : Failed To Create Main Window.\nExitting Now...\n");
        UnInitialize();
        exit(1);
}
XStoreName(gpDisplay,gWindow,"OGL Window");
Atom windowManagerDelete=XInternAtom(gpDisplay,"WM_DELETE_WINDOW",True);
XSetWMProtocols(gpDisplay,gWindow,&windowManagerDelete,1);
XMapWindow(gpDisplay,gWindow);
}

void ToggleFullScreen(void){

Atom wm_state;
Atom fullscreen;
XEvent xev={0};
wm_state=XInternAtom(gpDisplay,"_NET_WM_STATE",False); 
memset(&xev,0,sizeof(xev)); 
xev.type=ClientMessage;
xev.xclient.window=gWindow;
xev.xclient.message_type=wm_state;
xev.xclient.format=32;
xev.xclient.data.l[0]=bFullScreen?0:1;
fullscreen=XInternAtom(gpDisplay,"_NET_WM_STATE_FULLSCREEN",False);
xev.xclient.data.l[1]=fullscreen;
XSendEvent(gpDisplay,RootWindow(gpDisplay,gpXVisualInfo->screen),False,StructureNotifyMask,&xev);

}

void UnInitialize()
{
    if(bFullScreen){
        ToggleFullScreen();
    }

	if (vao_sphere) {
		glDeleteVertexArrays(1, &vao_sphere);
		vao_sphere = 0;
	}

	if (vbo_sphere_element) {
		glDeleteVertexArrays(1, &vbo_sphere_element);
		vbo_sphere_element = 0;
	}
	if (vbo_sphere_position) {
		glDeleteVertexArrays(1, &vbo_sphere_position);
		vbo_sphere_position = 0;
	}
	if (vbo_sphere_normal) {
		glDeleteVertexArrays(1, &vbo_sphere_normal);
		vbo_sphere_normal = 0;
	}

	if (gShaderProgramObject_PV) {
		GLsizei shaderCount;
		GLsizei shaderNumber;
		glUseProgram(gShaderProgramObject_PV);

		glGetProgramiv(gShaderProgramObject_PV,
			GL_ATTACHED_SHADERS,
			&shaderCount);

		GLuint * pShaders = (GLuint *)malloc(shaderCount * sizeof(GLuint));

		if (pShaders) {
			glGetAttachedShaders(gShaderProgramObject_PV,
				shaderCount,
				&shaderCount,
				pShaders);

			for (shaderNumber = 0; shaderNumber < shaderCount; shaderNumber++) {
				glDetachShader(gShaderProgramObject_PV, pShaders[shaderNumber]);
				glDeleteShader(pShaders[shaderNumber]);
				pShaders[shaderNumber] = 0;
			}
			free(pShaders);
		}
		glDeleteProgram(gShaderProgramObject_PV);
		gShaderProgramObject_PV = 0;
	}

		if (gShaderProgramObject_PF) {
		GLsizei shaderCount;
		GLsizei shaderNumber;
		glUseProgram(gShaderProgramObject_PF);

		glGetProgramiv(gShaderProgramObject_PF,
			GL_ATTACHED_SHADERS,
			&shaderCount);

		GLuint * pShaders = (GLuint *)malloc(shaderCount * sizeof(GLuint));

		if (pShaders) {
			glGetAttachedShaders(gShaderProgramObject_PF,
				shaderCount,
				&shaderCount,
				pShaders);

			for (shaderNumber = 0; shaderNumber < shaderCount; shaderNumber++) {
				glDetachShader(gShaderProgramObject_PF, pShaders[shaderNumber]);
				glDeleteShader(pShaders[shaderNumber]);
				pShaders[shaderNumber] = 0;
			}
			free(pShaders);
		}
		glDeleteProgram(gShaderProgramObject_PF);
		gShaderProgramObject_PF = 0;
		}
		glUseProgram(0);
	

GLXContext currentGLXContext=glXGetCurrentContext();

if(currentGLXContext != NULL && currentGLXContext== gGLXContext){
    glXMakeCurrent(gpDisplay,0,0);

    if(gGLXContext){
        glXDestroyContext(gpDisplay,gGLXContext);
    }
}

if(gWindow)
{
        XDestroyWindow(gpDisplay,gWindow);
}
if(gColorMap)
{
        XFreeColormap(gpDisplay,gColorMap);
}
if(gpXVisualInfo)
{
        free(gpXVisualInfo);
        gpXVisualInfo=NULL;
}
if(gpDisplay){
        XCloseDisplay(gpDisplay);
        gpDisplay=NULL;
}
if(gGLXContext){
        XFree(gGLXContext);
        gGLXContext=NULL;
}

}

void Initialize(void){
    //Function Declaration
        void Resize(int, int);

        glXCreateContextAttribsARB=(glXCreateContextAttribsARBProc)glXGetProcAddressARB((GLubyte *)"glXCreateContextAttribsARB");

        if(glXCreateContextAttribsARB==NULL){
                printf("ERROR : Failed To Get glXCreateContextAttribsARB .\nExitting Now...\n");
                UnInitialize();
                exit(1);
        }

        GLint Attribs[]={
            GLX_CONTEXT_MAJOR_VERSION_ARB,4,
            GLX_CONTEXT_MINOR_VERSION_ARB,5,
            GLX_CONTEXT_PROFILE_MASK_ARB,GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
            None
        };    

        gGLXContext=glXCreateContextAttribsARB(gpDisplay,
                                        gGLXFBConfig,
                                        0,
                                        true,
                                        Attribs);

        if(!gGLXContext){
                GLint Attribs[]={
                                GLX_CONTEXT_MAJOR_VERSION_ARB,1,
                                GLX_CONTEXT_MINOR_VERSION_ARB,0,
                                None
                };    

                gGLXContext=glXCreateContextAttribsARB(gpDisplay,
                                                gGLXFBConfig,
                                                0,
                                                true,
                                                Attribs);
        }

        if(!glXIsDirect(gpDisplay,gGLXContext)){
                printf("ERROR : The Obtained context is not H/W rendering context...\n");
        }else{
                printf("The Obtained context is H/W rendering context...\n");
        }
         
        glXMakeCurrent(gpDisplay,
        gWindow,
        gGLXContext);

        GLenum result;
        result=glewInit();
        if(result!=GLEW_OK){
                printf("ERROR : Failed To Initialize GLEW.\nExitting Now...\n");
                UnInitialize();
                exit(1);
        }


	result = glewInit();
	if (result != GLEW_OK) {
		fprintf(gpFile, "glewInit() failed\n");
		UnInitialize();
		exit(0);
	}

	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumElements = getNumberOfSphereElements();
	gNumVertices = getNumberOfSphereVertices();


	//Shader Objects
	GLint gVertexShaderObject_PF;
	GLint gFragementShaderObject_PF;

	//Vertex Shader
	//Define Shader Object
	gVertexShaderObject_PF = glCreateShader(GL_VERTEX_SHADER);

	//Vertex Shader Code
	const GLchar* vertextShaderSourceCode_PF =
		"#version 430 core " \
		"\n " \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
		"uniform mat4 u_m_matrix;" \
		"uniform mat4 u_v_matrix;" \
		"uniform mat4 u_p_matrix;" \
		"uniform int u_lkeyispressed;" \
		"uniform vec4 u_lightposition[3];" \
		"out vec3 tnorm;"\
		"out vec3 lightDirection[3];"\
		"out vec3 reflectionVector[3];"\
		"out vec3 viewerVector;"\
		"void main(void)" \
		"{" \
		"gl_Position=u_p_matrix*u_v_matrix*u_m_matrix * vPosition;" \
		"if(u_lkeyispressed==1)"\
		"{"\
		"vec4 eyecoordinates=u_v_matrix*u_m_matrix*vPosition;"\
		"tnorm=mat3(u_v_matrix*u_m_matrix)*vNormal;"\
		"viewerVector=vec3(-eyecoordinates);"\
		
		"for(int i=0;i<3;i++)"\
		"{"\
		"lightDirection[i]=vec3(u_lightposition[i]-eyecoordinates);"\
		"reflectionVector[i]=reflect(-lightDirection[i],tnorm);" \
		"}"\

		"}"\
		"}";

	//Shader object and source code mapping
	glShaderSource(gVertexShaderObject_PF, 1, &vertextShaderSourceCode_PF, NULL);

	//Compile Shader
	glCompileShader(gVertexShaderObject_PF);

	//Error Checking
	GLint iShaderCompileStatus = 0;
	GLint iInfoLogLength = 0;
	GLchar * szInfoLog = NULL;

	glGetShaderiv(gVertexShaderObject_PF, GL_COMPILE_STATUS, &iShaderCompileStatus);

	if (iShaderCompileStatus == GL_FALSE) {
		glGetShaderiv(gVertexShaderObject_PF, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0) {
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL) {
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject_PF,
					iInfoLogLength,
					&written,
					szInfoLog);
				fprintf(gpFile, "Vertex Shader PF Compilation Log %s\n", szInfoLog);
				free(szInfoLog);
				UnInitialize();
				exit(0);
			}
		}
	}

	//Fragement Shader
	//Define Shader Object
	gFragementShaderObject_PF = glCreateShader(GL_FRAGMENT_SHADER);

	//Fragement Shader Code
	const GLchar* fragementShaderSourceCode_PF =
		"#version 430 core " \
		"\n " \
		"out vec4 fragColor;"
		"uniform vec3 u_ld[3];" \
		"uniform vec3 u_kd;" \
		"uniform vec3 u_la[3];" \
		"uniform vec3 u_ka;" \
		"uniform vec3 u_ls[3];" \
		"uniform vec3 u_ks;" \
		"uniform float u_shininess;" \
		"uniform int u_lkeyispressed;" \
		"in vec3 tnorm;"\
		"in vec3 lightDirection[3];"\
		"in vec3 reflectionVector[3];"\
		"in vec3 viewerVector;"\
		"void main(void)" \
		"{" \
		"vec3 phongADSLight;"\
		"if(u_lkeyispressed==1)"\
		"{"\
		"vec3 tnorm_normalize=normalize(tnorm);"\
		"vec3 viewerVector_normalize=normalize(viewerVector);"\

		"phongADSLight = vec3(0.0f, 0.0f, 0.0f);"\
		"for(int i=0;i<3;i++)"\
		"{"\
		"vec3 lightDirection_normalize=normalize(lightDirection[i]);"\
		"vec3 reflectionVector_normalize=normalize(reflectionVector[i]);"\
		"float tn_dot_ld=max(dot(lightDirection_normalize,tnorm_normalize),0.0);"\
		"vec3 ambient=u_la[i]*u_ka;"\
		"vec3 diffuse=u_ld[i]*u_kd*tn_dot_ld;"
		"vec3 specular=u_ls[i]*u_ks*pow(max(dot(reflectionVector_normalize,viewerVector_normalize),0.0),u_shininess);"\
		"phongADSLight=phongADSLight+ambient+diffuse+specular;"\
		"}"\

		"}"\
		"else"\
		"{"\
		"phongADSLight=vec3(1.0f,1.0f,1.0f);"\
		"}"\
		"fragColor=vec4(phongADSLight,1.0f);"\
		"} ";

	//Shader object and source code mapping
	glShaderSource(gFragementShaderObject_PF, 1, &fragementShaderSourceCode_PF, NULL);

	//Compile Shader
	glCompileShader(gFragementShaderObject_PF);

	//Error Checking
	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(gFragementShaderObject_PF, GL_COMPILE_STATUS, &iShaderCompileStatus);

	if (iShaderCompileStatus == GL_FALSE) {
		glGetShaderiv(gFragementShaderObject_PF, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0) {
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL) {
				GLsizei written;
				glGetShaderInfoLog(gFragementShaderObject_PF,
					iInfoLogLength,
					&written,
					szInfoLog);
				fprintf(gpFile, "Fragement Shader Per Fragement Compilation Log %s\n", szInfoLog);
				free(szInfoLog);
				UnInitialize();
				exit(0);
			}
		}
	}

	//Program Object
	gShaderProgramObject_PF = glCreateProgram();
	//Attach Shaders
	glAttachShader(gShaderProgramObject_PF, gVertexShaderObject_PF);
	glAttachShader(gShaderProgramObject_PF, gFragementShaderObject_PF);

	//Prelinking
	//Binding to attribute
	glBindAttribLocation(gShaderProgramObject_PF,
		AMC_ATTRIBUTE_POSITION,
		"vPosition");

	glBindAttribLocation(gShaderProgramObject_PF,
		AMC_ATTRIBUTE_NORMAL,
		"vNormal");

	//Link Shader Program
	glLinkProgram(gShaderProgramObject_PF);

	//Error Checking
	GLint iShaderLinkStatus = 0;

	glGetProgramiv(gShaderProgramObject_PF, GL_LINK_STATUS, &iShaderLinkStatus);

	if (iShaderLinkStatus == GL_FALSE) {
		glGetProgramiv(gShaderProgramObject_PF, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0) {
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL) {
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject_PF,
					iInfoLogLength,
					&written,
					szInfoLog);
				fprintf(gpFile, "Program Link Log %s\n", szInfoLog);
				free(szInfoLog);
				UnInitialize();
				exit(0);
			}
		}
	}

	if (perVertexOrFragement == 1) {
		getUniformLocation(gShaderProgramObject_PF);
	}

	//----------------------------------------------------------------------------------------------------------------

	//Shader Objects
	GLint gVertexShaderObject_PV;
	GLint gFragementShaderObject_PV;

	//Vertex Shader
	//Define Shader Object
	gVertexShaderObject_PV = glCreateShader(GL_VERTEX_SHADER);

	//Vertex Shader Code
	const GLchar* vertextShaderSourceCode_PV =
		"#version 430 core " \
		"\n " \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
		"uniform mat4 u_m_matrix;" \
		"uniform mat4 u_v_matrix;" \
		"uniform mat4 u_p_matrix;" \
		"uniform int u_lkeyispressed;" \

		"uniform vec3 u_ld[3];" \
		"uniform vec3 u_kd;" \
		"uniform vec3 u_la[3];" \
		"uniform vec3 u_ka;" \
		"uniform vec3 u_ls[3];" \
		"uniform vec3 u_ks;" \
		"uniform float u_shininess;" \
		"uniform vec4 u_lightposition[3];" \

		"out vec3 phongADSLight;" \
		"void main(void)" \
		"{" \
		"gl_Position=u_p_matrix*u_v_matrix*u_m_matrix * vPosition;" \
		"if(u_lkeyispressed==1)"\
		"{"\
		"vec4 eyecoordinates=u_v_matrix*u_m_matrix*vPosition;"\
		"vec3 tnorm=normalize(mat3(u_v_matrix*u_m_matrix)*vNormal);"\
		"vec3 viewerVector=normalize(vec3(-eyecoordinates));"\
		"phongADSLight = vec3(0.0f, 0.0f, 0.0f);"\
		"for(int i=0;i<3;i++)"\
		"{"\

		"vec3 lightDirection=normalize(vec3(u_lightposition[i]-eyecoordinates));"\
		"float tn_dot_ld=max(dot(lightDirection,tnorm),0.0);"\
		"vec3 reflectionVector=reflect(-lightDirection,tnorm);"\
		"vec3 ambient=u_la[i]*u_ka;"\
		"vec3 diffuse=u_ld[i]*u_kd*tn_dot_ld;"
		"vec3 specular=u_ls[i]*u_ks*pow(max(dot(reflectionVector,viewerVector),0.0),u_shininess);"\

		"phongADSLight=phongADSLight+ambient+diffuse+specular;"\
		"}"\

		"}"\
		"else"\
		"{"\
		"phongADSLight=vec3(1.0f,1.0f,1.0f);"\
		"}"\
		"}";

	//Shader object and source code mapping
	glShaderSource(gVertexShaderObject_PV, 1, &vertextShaderSourceCode_PV, NULL);

	//Compile Shader
	glCompileShader(gVertexShaderObject_PV);

	//Error Checking
	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(gVertexShaderObject_PV, GL_COMPILE_STATUS, &iShaderCompileStatus);

	if (iShaderCompileStatus == GL_FALSE) {
		glGetShaderiv(gVertexShaderObject_PV, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0) {
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL) {
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject_PV,
					iInfoLogLength,
					&written,
					szInfoLog);
				fprintf(gpFile, "Vertex Shader Compilation Log %s\n", szInfoLog);
				free(szInfoLog);
				UnInitialize();
				exit(0);
			}
		}
	}

	//Fragement Shader
	//Define Shader Object
	gFragementShaderObject_PV = glCreateShader(GL_FRAGMENT_SHADER);

	//Fragement Shader Code
	const GLchar* fragementShaderSourceCode_PV =
		"#version 430 core " \
		"\n " \
		"out vec4 fragColor;"
		"in vec3 phongADSLight;" \
		"void main(void)" \
		"{" \
		"fragColor=vec4(phongADSLight,1.0f);"\
		"} ";

	//Shader object and source code mapping
	glShaderSource(gFragementShaderObject_PV, 1, &fragementShaderSourceCode_PV, NULL);

	//Compile Shader
	glCompileShader(gFragementShaderObject_PV);

	//Error Checking
	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(gFragementShaderObject_PV, GL_COMPILE_STATUS, &iShaderCompileStatus);

	if (iShaderCompileStatus == GL_FALSE) {
		glGetShaderiv(gFragementShaderObject_PV, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0) {
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL) {
				GLsizei written;
				glGetShaderInfoLog(gFragementShaderObject_PV,
					iInfoLogLength,
					&written,
					szInfoLog);
				fprintf(gpFile, "Fragement Shader Compilation Log %s\n", szInfoLog);
				free(szInfoLog);
				UnInitialize();
				exit(0);
			}
		}
	}

	//Program Object
	gShaderProgramObject_PV = glCreateProgram();
	//Attach Shaders
	glAttachShader(gShaderProgramObject_PV, gVertexShaderObject_PV);
	glAttachShader(gShaderProgramObject_PV, gFragementShaderObject_PV);

	//Prelinking
	//Binding to attribute
	glBindAttribLocation(gShaderProgramObject_PV,
		AMC_ATTRIBUTE_POSITION,
		"vPosition");

	glBindAttribLocation(gShaderProgramObject_PV,
		AMC_ATTRIBUTE_NORMAL,
		"vNormal");

	//Link Shader Program
	glLinkProgram(gShaderProgramObject_PV);

	//Error Checking
	iShaderLinkStatus = 0;

	glGetProgramiv(gShaderProgramObject_PV, GL_LINK_STATUS, &iShaderLinkStatus);

	if (iShaderLinkStatus == GL_FALSE) {
		glGetProgramiv(gShaderProgramObject_PV, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0) {
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL) {
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject_PV,
					iInfoLogLength,
					&written,
					szInfoLog);
				fprintf(gpFile, "Program Link Log %s\n", szInfoLog);
				free(szInfoLog);
				UnInitialize();
				exit(0);
			}
		}
	}

	if (perVertexOrFragement == 0) {
		getUniformLocation(gShaderProgramObject_PV);
	}


	// vao
	glGenVertexArrays(1, &vao_sphere);
	glBindVertexArray(vao_sphere);

	// position vbo
	glGenBuffers(1, &vbo_sphere_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_sphere_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// normal vbo
	glGenBuffers(1, &vbo_sphere_normal);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_sphere_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// element vbo
	glGenBuffers(1, &vbo_sphere_element);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// unbind vao
	glBindVertexArray(0);


	lights[0].lightAmbient[0] = 0.0f;
	lights[0].lightAmbient[1] = 0.0f;
	lights[0].lightAmbient[2] = 0.0f;
	lights[0].lightAmbient[3] = 1.0f;

	lights[0].lightDiffuse[0] = 1.0f;
	lights[0].lightDiffuse[1] = 0.0f;
	lights[0].lightDiffuse[2] = 0.0f;
	lights[0].lightDiffuse[3] = 1.0f;

	lights[0].lightSpecular[0] = 1.0f;
	lights[0].lightSpecular[1] = 0.0f;
	lights[0].lightSpecular[2] = 0.0f;
	lights[0].lightSpecular[3] = 1.0f;

	lights[0].lightPosition[0] = 0.0f;
	lights[0].lightPosition[1] = 0.0f;
	lights[0].lightPosition[2] = 0.0f;
	lights[0].lightPosition[3] = 1.0f;

	lights[1].lightAmbient[0] = 0.0f;
	lights[1].lightAmbient[1] = 0.0f;
	lights[1].lightAmbient[2] = 0.0f;
	lights[1].lightAmbient[3] = 1.0f;

	lights[1].lightDiffuse[0] = 0.0f;
	lights[1].lightDiffuse[1] = 1.0f;
	lights[1].lightDiffuse[2] = 0.0f;
	lights[1].lightDiffuse[3] = 1.0f;

	lights[1].lightSpecular[0] = 0.0f;
	lights[1].lightSpecular[1] = 1.0f;
	lights[1].lightSpecular[2] = 0.0f;
	lights[1].lightSpecular[3] = 1.0f;

	lights[1].lightPosition[0] = 0.0f;
	lights[1].lightPosition[1] = 0.0f;
	lights[1].lightPosition[2] = 0.0f;
	lights[1].lightPosition[3] = 1.0f;

	lights[2].lightAmbient[0] = 0.0f;
	lights[2].lightAmbient[1] = 0.0f;
	lights[2].lightAmbient[2] = 0.0f;
	lights[2].lightAmbient[3] = 1.0f;

	lights[2].lightDiffuse[0] = 0.0f;
	lights[2].lightDiffuse[1] = 0.0f;
	lights[2].lightDiffuse[2] = 1.0f;
	lights[2].lightDiffuse[3] = 1.0f;

	lights[2].lightSpecular[0] = 0.0f;
	lights[2].lightSpecular[1] = 0.0f;
	lights[2].lightSpecular[2] = 1.0f;
	lights[2].lightSpecular[3] = 1.0f;

	lights[2].lightPosition[0] = 0.0f;
	lights[2].lightPosition[1] = 0.0f;
	lights[2].lightPosition[2] = 0.0f;
	lights[2].lightPosition[3] = 1.0f;

	material.materialAmbient[0] = 0.5f;
	material.materialAmbient[1] = 0.5f;
	material.materialAmbient[2] = 0.5f;
	material.materialAmbient[3] = 1.0f;
	material.materialDiffuse[0] = 1.0f;
	material.materialDiffuse[1] = 1.0f;
	material.materialDiffuse[2] = 1.0f;
	material.materialDiffuse[3] = 1.0f;
	material.materialSpecular[0] = 1.0f;
	material.materialSpecular[1] = 1.0f;
	material.materialSpecular[2] = 1.0f;
	material.materialSpecular[3] = 1.0f;
	material.materialShininess = 128.0f;

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0f);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	perspectiveProjectionMatrix = mat4::identity();

	for (int i = 0; i < noLights; i++) {
		arrayDiffuse[3 * i] = lights[i].lightDiffuse[0];
		arrayDiffuse[3 * i + 1] = lights[i].lightDiffuse[1];
		arrayDiffuse[3 * i + 2] = lights[i].lightDiffuse[2];
	}

	for (int i = 0; i < noLights; i++) {
		arrayAmbient[3 * i] = lights[i].lightAmbient[0];
		arrayAmbient[3 * i + 1] = lights[i].lightAmbient[1];
		arrayAmbient[3 * i + 2] = lights[i].lightAmbient[2];
	}

	for (int i = 0; i < noLights; i++) {
		arraySpecular[3 * i] = lights[i].lightSpecular[0];
		arraySpecular[3 * i + 1] = lights[i].lightSpecular[1];
		arraySpecular[3 * i + 2] = lights[i].lightSpecular[2];
	}

    Resize(giWindowWidth,giWindowHeight);

}

//Function Resize
void Resize(int width, int height) {
   if (height == 0)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	perspectiveProjectionMatrix = perspective(45.0f,
		(GLfloat)width / (GLfloat)height,
		0.1f,
		100.0f);

}
//Function Display
void DisplayOpenGL(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (perVertexOrFragement == 0)
		glUseProgram(gShaderProgramObject_PV);
	else
		glUseProgram(gShaderProgramObject_PF);


	//Declaration of Matrices
	mat4 modelMatrix;
	mat4 viewMatrix;

	mat4 translationMatrix;
	mat4 rotationMatrix;
	mat4 scaleMatrix;

	//CUBE
	//Initialize matrices

	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	scaleMatrix = mat4::identity();

	translationMatrix = translate(0.0f, 0.0f, -3.0f);

	//Matrix Multiplication
	modelMatrix = translationMatrix * scaleMatrix * rotationMatrix;
	//perspectiveProjectionMatrix

	//Send necessary matrices to shader in resp. Uniforms

	glUniformMatrix4fv(mUniform,
		1,
		GL_FALSE,
		modelMatrix);
	glUniformMatrix4fv(vUniform,
		1,
		GL_FALSE,
		viewMatrix);
	glUniformMatrix4fv(pUniform,
		1,
		GL_FALSE,
		perspectiveProjectionMatrix);

	for (int i = 0; i < noLights; i++) {
		arrayPosition[4 * i] = lights[i].lightPosition[0];
		arrayPosition[4 * i + 1] = lights[i].lightPosition[1];
		arrayPosition[4 * i + 2] = lights[i].lightPosition[2];
		arrayPosition[4 * i + 3] = lights[i].lightPosition[3];
	}

	if (gb_Lighting) {
		glUniform1i(lKeyIsPressedUniform, gb_Lighting);

		glUniform3fv(ldUniform, noLights, arrayDiffuse);
		glUniform3fv(laUniform, noLights, arrayAmbient);
		glUniform3fv(lsUniform, noLights, arraySpecular);

		glUniform4fv(lightPositionUniform, noLights, arrayPosition);

		glUniform3fv(kaUniform, 1, material.materialAmbient);
		glUniform3fv(kdUniform, 1, material.materialDiffuse);
		glUniform3fv(ksUniform, 1, material.materialSpecular);
		glUniform1f(shininessUniform, material.materialShininess);
	}
	else {
		glUniform1i(lKeyIsPressedUniform, gb_Lighting);
	}

	// *** bind vao ***
	glBindVertexArray(vao_sphere);

	// *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	// *** unbind vao ***
	glBindVertexArray(0);

	glUseProgram(0);

	glXSwapBuffers(gpDisplay,gWindow);
}

float angle1 = 0.0f;

void Update(void)
{
	angle1 = angle1 + 0.005f;
	if (angle1 > 2 * 3.14159265)
		angle1 = 0.0f;

	lights[0].lightPosition[1] = 100 * sin(angle1);
	lights[0].lightPosition[2] = 100 * cos(angle1);

	lights[1].lightPosition[0] = 100 * sin(angle1);
	lights[1].lightPosition[2] = 100 * cos(angle1);

	lights[2].lightPosition[0] = 100 * sin(angle1);
	lights[2].lightPosition[1] = 100 * cos(angle1);
	
}

void getUniformLocation(GLint gShaderProgramObject) {
	//Retriving Uniform Location
	mUniform = glGetUniformLocation(gShaderProgramObject,
		"u_m_matrix");
	vUniform = glGetUniformLocation(gShaderProgramObject,
		"u_v_matrix");
	pUniform = glGetUniformLocation(gShaderProgramObject,
		"u_p_matrix");
	lKeyIsPressedUniform = glGetUniformLocation(gShaderProgramObject,
		"u_lkeyispressed");
	ldUniform = glGetUniformLocation(gShaderProgramObject,
		"u_ld");
	kdUniform = glGetUniformLocation(gShaderProgramObject,
		"u_kd");
	laUniform = glGetUniformLocation(gShaderProgramObject,
		"u_la");
	kaUniform = glGetUniformLocation(gShaderProgramObject,
		"u_ka");
	lsUniform = glGetUniformLocation(gShaderProgramObject,
		"u_ls");
	ksUniform = glGetUniformLocation(gShaderProgramObject,
		"u_ks");
	lightPositionUniform = glGetUniformLocation(gShaderProgramObject,
		"u_lightposition");
	shininessUniform = glGetUniformLocation(gShaderProgramObject,
		"u_shininess");
}
