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

#define checkImageWidth 1
#define checkImageHeight 1

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
GLint gShaderProgramObject;

enum {
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXCOORD0
};

GLuint texture;
GLubyte checkImage[checkImageWidth][checkImageHeight][4];
GLuint procTexture;

GLuint vao_rectangle;
GLuint vbo_position_rectangle;
//GLuint vbo_color_rectangle;
GLuint vbo_texCoord_rectangle;

GLuint mvpUniform;
GLuint samplerUniform;

mat4 perspectiveProjectionMatrix;

int PressedDigit = 0;

void Update(void);
bool LoadTexture(GLuint *texture ,const char *path);

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
                        
                        switch(event.xkey.keycode){
                            case 87:
                                PressedDigit = 1;
                                break;
                            case 88:
                                PressedDigit = 2;
                                break;
                            case 89:
                                PressedDigit = 3;
                                break;
                            case 83:
                                PressedDigit = 4;
                                break;
                            case 84:
                            case 85:
                            case 79:
                            case 80:
                            case 81:
                            case 90:
                                PressedDigit = 0;
                                break;
                        }

                        // static char buffer [60];
                        // static std::string s = std::to_string(keysym);
                        // static const char *cstr = s.c_str();
                        // fputs(cstr, filePointer) ; 
                        
                        switch(keysym)
                        {
                        case XK_Escape:
                        bDone=true;
                        break;
                        case 49:
                        //case XK_KP_1:
                            PressedDigit = 1;
                            break;
                        case 50:
                            PressedDigit = 2;
                            break;
                        case 51:
                            PressedDigit = 3;
                            break;
                        case 52:
                            PressedDigit = 4;
                            break;
                        case 48:
                        case 53:
                        case 54:
                        case 55:
                        case 56:
                        case 57:
                            PressedDigit = 0;
                            break;
                        default:
                        break;
                        }
                        XLookupString(&event.xkey,keys,sizeof(keys),NULL,NULL);
                        switch(keys[0])
                        {
                        case 'F':
                        case 'f':       
                                if(bFullScreen==false)
                                {
                                        ToggleFullScreen();
                                        bFullScreen=true;
                                }
                                else
                                {
                                        ToggleFullScreen();
                                        bFullScreen=false;
                                }
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

	if (vbo_texCoord_rectangle) {
		glDeleteBuffers(1, &vbo_texCoord_rectangle);
		vbo_texCoord_rectangle = 0;
	}
	if (vbo_position_rectangle) {
		glDeleteBuffers(1, &vbo_position_rectangle);
		vbo_position_rectangle = 0;
	}
	if (vao_rectangle) {
		glDeleteVertexArrays(1, &vao_rectangle);
		vao_rectangle = 0;
	}

	if (gShaderProgramObject) {
		GLsizei shaderCount;
		GLsizei shaderNumber;
		glUseProgram(gShaderProgramObject);

		glGetProgramiv(gShaderProgramObject,
			GL_ATTACHED_SHADERS,
			&shaderCount);

		GLuint * pShaders = (GLuint *)malloc(shaderCount * sizeof(GLuint));

		if (pShaders) {
			glGetAttachedShaders(gShaderProgramObject,
				shaderCount,
				&shaderCount,
				pShaders);

			for (shaderNumber = 0; shaderNumber < shaderCount; shaderNumber++) {
				glDetachShader(gShaderProgramObject, pShaders[shaderNumber]);
				glDeleteShader(pShaders[shaderNumber]);
				pShaders[shaderNumber] = 0;
			}
			free(pShaders);
		}
		glDeleteProgram(gShaderProgramObject);
		gShaderProgramObject = 0;
		glUseProgram(0);
	}

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
	if (texture) {
		glDeleteTextures(1, &texture);
	}
	if (procTexture) {
		glDeleteTextures(1, &procTexture);
	}

}

void Initialize(void){
    //Function Declaration
        void Resize(int, int);
		void LoadTexture(GLuint *txtImage);
		bool LoadTexture(GLuint *texture ,const char *path);

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


	//Shader Objects
	GLint gVertexShaderObject;
	GLint gFragementShaderObject;

	//Vertex Shader
	//Define Shader Object
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	//Vertex Shader Code
	const GLchar* vertextShaderSourceCode =
		"#version 430 core " \
		"\n " \
		"in vec4 vPosition;" \
		"in vec2 vTexCoord;" \
		"uniform mat4 u_mvp_matrix;" \
		"out vec2 out_TexCoord;" \
		"void main(void)" \
		"{" \
		"gl_Position=u_mvp_matrix * vPosition;" \
		"out_TexCoord=vTexCoord;"\
		"} ";

	//"in vec4 vColor;" \
	//"out vec4 out_color;" \
		//"out_color=vColor;"\

	//Shader object and source code mapping
	glShaderSource(gVertexShaderObject, 1, &vertextShaderSourceCode, NULL);

	//Compile Shader
	glCompileShader(gVertexShaderObject);

	//Error Checking
	GLint iShaderCompileStatus = 0;
	GLint iInfoLogLength = 0;
	GLchar * szInfoLog = NULL;

	glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);

	if (iShaderCompileStatus == GL_FALSE) {
		glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0) {
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL) {
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject,
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
	gFragementShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	//Fragement Shader Code
	const GLchar* fragementShaderSourceCode =
		"#version 430 core " \
		"\n " \
		"in vec2 out_TexCoord;"
		"uniform sampler2D u_sampler;"
		"out vec4 fragColor;"
		"void main(void)" \
		"{" \
		"fragColor=texture(u_sampler,out_TexCoord);" \
		"} ";


	//"in vec4 out_color;"\
	//"fragColor=out_color;" \
		

	//Shader object and source code mapping
	glShaderSource(gFragementShaderObject, 1, &fragementShaderSourceCode, NULL);

	//Compile Shader
	glCompileShader(gFragementShaderObject);

	//Error Checking
	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(gFragementShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);

	if (iShaderCompileStatus == GL_FALSE) {
		glGetShaderiv(gFragementShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0) {
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL) {
				GLsizei written;
				glGetShaderInfoLog(gFragementShaderObject,
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
	gShaderProgramObject = glCreateProgram();
	//Attach Shaders
	glAttachShader(gShaderProgramObject, gVertexShaderObject);
	glAttachShader(gShaderProgramObject, gFragementShaderObject);

	//Prelinking
	//Binding to attribute
	glBindAttribLocation(gShaderProgramObject,
		AMC_ATTRIBUTE_POSITION,
		"vPosition");

	glBindAttribLocation(gShaderProgramObject,
		AMC_ATTRIBUTE_TEXCOORD0,
		"vTexCoord");
	/*glBindAttribLocation(gShaderProgramObject,
		AMC_ATTRIBUTE_COLOR,
		"vColor");*/

		//Link Shader Program
	glLinkProgram(gShaderProgramObject);

	//Error Checking
	GLint iShaderLinkStatus = 0;

	glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &iShaderLinkStatus);

	if (iShaderLinkStatus == GL_FALSE) {
		glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0) {
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL) {
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject,
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

	//Post Linking
	//Retriving Uniform Location
	mvpUniform = glGetUniformLocation(gShaderProgramObject,
		"u_mvp_matrix");

	samplerUniform = glGetUniformLocation(gShaderProgramObject,
		"u_sampler");

	//Vertices
	const GLfloat rectangleVertices[] = {
	1.0f, 1.0f, 0.0f ,
	-1.0f, 1.0f, 0.0f,
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f
	};

	//const GLfloat rectangleTexCoords[] = {
	//1.0f, 1.0f ,
	//0.0f, 1.0f,
	//0.0f, 0.0f,
	//1.0f, 0.0f
	//};

	//const GLfloat rectangleColors[] = {
	//	0.0f,0.0f,1.0f,
	//	0.0f,0.0f,1.0f,
	//	0.0f,0.0f,1.0f,
	//	0.0f,0.0f,1.0f
	//};


	//RECTANGLE
	glGenVertexArrays(1, &vao_rectangle);

	glBindVertexArray(vao_rectangle);

	//Generate Buffer
	glGenBuffers(1, &vbo_position_rectangle);
	//Bind Generated Buffer
	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_position_rectangle);
	//Fill Buffer
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(rectangleVertices),
		rectangleVertices,
		GL_STATIC_DRAW);
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

	////Generate Buffer
	//glGenBuffers(1, &vbo_color_rectangle);
	////Bind Generated Buffer
	//glBindBuffer(GL_ARRAY_BUFFER,
	//	vbo_color_rectangle);
	////Fill Buffer
	//glBufferData(GL_ARRAY_BUFFER,
	//	sizeof(rectangleColors),
	//	rectangleColors,
	//	GL_STATIC_DRAW);
	////Set Vertex Attrib Pointer
	//glVertexAttribPointer(AMC_ATTRIBUTE_COLOR,
	//	3,
	//	GL_FLOAT,
	//	GL_FALSE,
	//	0,
	//	NULL);
	////Enable Vertex Attrib Array
	//glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	////Unbind Buffer
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	//For Single Color
	//glVertexAttrib3f(AMC_ATTRIBUTE_COLOR,
	//	0.0f, 0.0f, 1.0f);

	//Generate Buffer
	glGenBuffers(1, &vbo_texCoord_rectangle);
	//Bind Generated Buffer
	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_texCoord_rectangle);
	//Fill Buffer
	//glBufferData(GL_ARRAY_BUFFER,
	//	sizeof(rectangleTexCoords),
	//	rectangleTexCoords,
	//	GL_STATIC_DRAW);

	glBufferData(GL_ARRAY_BUFFER,
		8 * sizeof(float),
		NULL,
		GL_DYNAMIC_DRAW);

	//Set Vertex Attrib Pointer
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD0,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		NULL);
	//Enable Vertex Attrib Array
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD0);
	//Unbind Buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Unbind array
	glBindVertexArray(0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0f);

	//glEnable(GL_CULL_FACE);
	//glDisable(GL_CULL_FACE);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glEnable(GL_TEXTURE_2D);

	LoadTexture(&procTexture);

	bool bResult=LoadTexture(&texture,"Smiley.bmp");

    if(bResult==false){
    printf("ERROR : Failed To Load Texture.\nExiting Now...\n");
    UnInitialize();
    exit(1);
    }

	perspectiveProjectionMatrix = mat4::identity();

    Resize(giWindowWidth,giWindowHeight);

}

//Function Resize
void Resize(int width, int height) {
   if (height == 0)
		height = 1;

	perspectiveProjectionMatrix = perspective(45.0f,
		(GLfloat)width / (GLfloat)height,
		0.1f,
		100.0f);

}
//Function Display
void DisplayOpenGL(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(gShaderProgramObject);

	//Declaration of Matrices
	mat4 modelViewMatrix;
	mat4 modelViewProjectionMatrix;
	mat4 translationMatrix;
	mat4 rotationMatrix;
	mat4 scaleMatrix;

	//RECTANGLE
	//Initialize matrices

	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	scaleMatrix= mat4::identity();
	//Transformation

	translationMatrix = translate(0.0f, 0.0f, -6.0f);
	scaleMatrix=scale(1.0f,-1.0f,1.0f);
	//rotationMatrix = rotate(angleRectangle, 1.0f, 0.0f, 0.0f);

	//Matrix Multiplication
	modelViewMatrix = translationMatrix * rotationMatrix*scaleMatrix;

	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	//Texture

	glActiveTexture(GL_TEXTURE0);

	if (PressedDigit == 1 || PressedDigit == 2 || PressedDigit == 3 || PressedDigit == 4)
		glBindTexture(GL_TEXTURE_2D, texture);
	else
		glBindTexture(GL_TEXTURE_2D, procTexture);

	//Send necessary matrices to shader in resp. Uniforms

	glUniformMatrix4fv(mvpUniform,
		1,
		GL_FALSE,
		modelViewProjectionMatrix);

	glUniform1i(samplerUniform, 0);

	//Bind with vao

	glBindVertexArray(vao_rectangle);

	//Bind with textures if any

	GLfloat rectangleTexCoords[] = {
	1.0f, 1.0f ,
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f
	};

	if (PressedDigit == 1) {
		rectangleTexCoords[0] = 0.5f;
		rectangleTexCoords[1] = 0.5f;
		rectangleTexCoords[2] = 0.0f;
		rectangleTexCoords[3] = 0.5f;
		rectangleTexCoords[4] = 0.0f;
		rectangleTexCoords[5] = 0.0f;
		rectangleTexCoords[6] = 0.5f;
		rectangleTexCoords[7] = 0.0f;
	}
	else if (PressedDigit == 2) {
		rectangleTexCoords[0] = 1.0f;
		rectangleTexCoords[1] = 1.0f;
		rectangleTexCoords[2] = 0.0f;
		rectangleTexCoords[3] = 1.0f;
		rectangleTexCoords[4] = 0.0f;
		rectangleTexCoords[5] = 0.0f;
		rectangleTexCoords[6] = 1.0f;
		rectangleTexCoords[7] = 0.0f;
	}
	else if (PressedDigit == 3) {
		rectangleTexCoords[0] = 2.0f;
		rectangleTexCoords[1] = 2.0f;
		rectangleTexCoords[2] = 0.0f;
		rectangleTexCoords[3] = 2.0f;
		rectangleTexCoords[4] = 0.0f;
		rectangleTexCoords[5] = 0.0f;
		rectangleTexCoords[6] = 2.0f;
		rectangleTexCoords[7] = 0.0f;
	}
	else if (PressedDigit == 4) {
		rectangleTexCoords[0] = 0.5f;
		rectangleTexCoords[1] = 0.5f;
		rectangleTexCoords[2] = 0.5f;
		rectangleTexCoords[3] = 0.5f;
		rectangleTexCoords[4] = 0.5f;
		rectangleTexCoords[5] = 0.5f;
		rectangleTexCoords[6] = 0.5f;
		rectangleTexCoords[7] = 0.5f;
	}
	else {
		rectangleTexCoords[0] = 1.0f;
		rectangleTexCoords[1] = 1.0f;
		rectangleTexCoords[2] = 0.0f;
		rectangleTexCoords[3] = 1.0f;
		rectangleTexCoords[4] = 0.0f;
		rectangleTexCoords[5] = 0.0f;
		rectangleTexCoords[6] = 1.0f;
		rectangleTexCoords[7] = 0.0f;
	}

	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_texCoord_rectangle);
	glBufferData(GL_ARRAY_BUFFER,
		8 * sizeof(float),
		rectangleTexCoords,
		GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Draw

	glDrawArrays(GL_TRIANGLE_FAN,
		0,
		4);

	//Unbind Texture
	glBindTexture(GL_TEXTURE_2D, 0);

	//Unbind vao
	glBindVertexArray(0);

	glUseProgram(0);
	
	glXSwapBuffers(gpDisplay,gWindow);
}
void Update(void)
{
	
}
bool LoadTexture(GLuint *texture ,const char *path){
    bool bResult=false;
    int imageWidth,imageHeight;
    unsigned char* imgData=SOIL_load_image(path,
    &imageWidth,
    &imageHeight,
    0,
    SOIL_LOAD_RGB);

    if(imgData==NULL){
        bResult=false;
        return bResult;
    }else{
        bResult=true;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, texture);
	glBindTexture(GL_TEXTURE_2D, *texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//gluBuild2DMipmaps(GL_TEXTURE_2D, 3 , imageWidth, imageHeight, GL_RGB, GL_UNSIGNED_BYTE, imgData);  

		glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGB,
		imageWidth,
		imageHeight,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		imgData);

	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

    SOIL_free_image_data(imgData);
    
    return bResult;
}

void LoadTexture(GLuint *txtImage) {
	void MakeCheckImage();
	MakeCheckImage();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, txtImage);
	glBindTexture(GL_TEXTURE_2D, *txtImage);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGBA,
		checkImageWidth,
		checkImageHeight,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		checkImage);

	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

}

void MakeCheckImage() {
	int i, j, c;
	for (i = 0; i < checkImageHeight; i++) {
		for (j = 0; j < checkImageWidth; j++) {
			c = 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = 255;

		}
	}
}
