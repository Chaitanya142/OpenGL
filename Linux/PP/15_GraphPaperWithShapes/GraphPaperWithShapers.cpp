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
GLuint vao_line;
GLuint vao_point;

GLuint vbo_position_lines;
GLuint vbo_color_lines;

GLuint vbo_position_point;
GLuint vbo_color_point;

GLuint mvpUniform;
mat4 perspectiveProjectionMatrix;

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
                        bDone=true;
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

	if (vbo_position_lines) {
		glDeleteBuffers(1, &vbo_position_lines);
		vbo_position_lines = 0;
	}

	if (vbo_color_lines) {
		glDeleteBuffers(1, &vbo_color_lines);
		vbo_color_lines = 0;
	}

	if (vbo_position_point) {
		glDeleteBuffers(1, &vbo_position_point);
		vbo_position_point = 0;
	}

	if (vbo_color_point) {
		glDeleteBuffers(1, &vbo_color_point);
		vbo_color_point = 0;
	}

	if (vao_line) {
		glDeleteVertexArrays(1, &vao_line);
		vao_line = 0;
	}

	if (vao_point) {
		glDeleteVertexArrays(1, &vao_point);
		vao_point = 0;
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
		"in vec4 vColor;" \
		"uniform mat4 u_mvp_matrix;" \
		"out vec4 out_color;" \
		"void main(void)" \
		"{" \
		"gl_Position=u_mvp_matrix * vPosition;" \
		"out_color=vColor;"\
		"} ";

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
		"out vec4 fragColor;"
		"in vec4 out_color;"\
		"void main(void)" \
		"{" \
		"fragColor=out_color;" \
		"} ";

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
		AMC_ATTRIBUTE_COLOR,
		"vColor");

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


	//Create vao_line
	//Save everying in single set
	glGenVertexArrays(1, &vao_line);

	glBindVertexArray(vao_line);


	//Line
	//Generate Buffer
	glGenBuffers(1, &vbo_position_lines);
	//Bind Generated Buffer
	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_position_lines);
	//Fill Buffer
	glBufferData(GL_ARRAY_BUFFER,
		6 * sizeof(float),
		NULL,
		GL_DYNAMIC_DRAW);
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

	glGenBuffers(1, &vbo_color_lines);
	//Bind Generated Buffer
	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_color_lines);
	//Fill Buffer
	glBufferData(GL_ARRAY_BUFFER,
		6 * sizeof(float),
		NULL,
		GL_DYNAMIC_DRAW);
	//Set Vertex Attrib Pointer
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		NULL);
	//Enable Vertex Attrib Array
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	//Unbind Buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Unbind array
	glBindVertexArray(0);

	//Create vao_point
	//Save everying in single set
	glGenVertexArrays(1, &vao_point);

	glBindVertexArray(vao_point);

	//POINT
	//Generate Buffer
	glGenBuffers(1, &vbo_position_point);
	//Bind Generated Buffer
	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_position_point);
	//Fill Buffer
	glBufferData(GL_ARRAY_BUFFER,
		3 * sizeof(float)*((2 * 3.1415) / 0.001),
		NULL,
		GL_DYNAMIC_DRAW);
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

	glGenBuffers(1, &vbo_color_point);
	//Bind Generated Buffer
	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_color_point);
	//Fill Buffer
	glBufferData(GL_ARRAY_BUFFER,
		3 * sizeof(float)*((2 * 3.1415) / 0.001),
		NULL,
		GL_DYNAMIC_DRAW);
	//Set Vertex Attrib Pointer
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		NULL);
	//Enable Vertex Attrib Array
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	//Unbind Buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	//Unbind array
	glBindVertexArray(0);



	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0f);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glEnable(GL_TEXTURE_2D);

	perspectiveProjectionMatrix = mat4::identity();

    Resize(giWindowWidth,giWindowHeight);

}

//Function Resize
void Resize(int width, int height) {
   if (height == 0)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	//printf("%d %d \n",width,height);
	perspectiveProjectionMatrix = perspective(45.0f,
		(float)width / (float)height,
		0.1f,
		100.0f);

}
//Function Display
void DisplayOpenGL(void) {
	float count = -1;
	//Vertices

	void DrawTriangle(float);
	void DrawInCircle(float x1, float y1, float z1,
		float x2, float y2, float z2,
		float x3, float y3, float z3
	);
	void DrawGraph(GLfloat offset);
	void DrawCircle(GLfloat radius);
	void DrawRectangle(GLfloat width, GLfloat height);
	void DrawCircleAroundRectangle(GLfloat width, GLfloat height);

	//GLfloat lineVertices[6];

	//GLfloat lineColor[] = {
	//	0.0f,0.0f,1.0f,
	//	0.0f,0.0f,1.0f
	//};


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(gShaderProgramObject);

	//Declaration of Matrices
	mat4 modelViewMatrix;
	mat4 modelViewProjectionMatrix;
	mat4 translationMatrix;
	mat4 rotationMatrix;

	//Initialize matrices

	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	//Transformation

	translationMatrix = translate(0.0f, 0.0f, -3.0f);

	//Matrix Multiplication
	modelViewMatrix = translationMatrix * rotationMatrix;

	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	//Send necessary matrices to shader in resp. Uniforms

	glUniformMatrix4fv(mvpUniform,
		1,
		GL_FALSE,
		modelViewProjectionMatrix);

	//Bind with vao_line

	glBindVertexArray(vao_line);

	//Bind with textures if any

	DrawGraph(0.01f);
	
	DrawTriangle(0.5f);
	
	DrawRectangle(1.0f, 1.0f);
	//Unbind vao_line

	glBindVertexArray(0);

	glBindVertexArray(vao_point);

	DrawInCircle(0.0f, 0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f);

	DrawCircleAroundRectangle(1.0f, 1.0f);

	glBindVertexArray(0);

	glUseProgram(0);

	glXSwapBuffers(gpDisplay,gWindow);
}
void Update(void)
{

}

void DrawGraph(GLfloat offset) {

	float count;
	GLfloat lineVertices[6];

	GLfloat lineColor[] = {
		0.0f,0.0f,1.0f,
		0.0f,0.0f,1.0f
	};

	for (count = -1; count <= 1.01f; count = count + offset) {

		lineVertices[0] = count;
		lineVertices[1] = -1.0f;
		lineVertices[2] = 0.0f;
		lineVertices[3] = count;
		lineVertices[4] = 1.0f;
		lineVertices[5] = 0.0f;

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_position_lines);
		glBufferData(GL_ARRAY_BUFFER,
			3 * 2 * sizeof(float),
			lineVertices,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_color_lines);
		glBufferData(GL_ARRAY_BUFFER,
			3 * 2 * sizeof(float),
			lineColor,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//Draw

		glDrawArrays(GL_LINES,
			0,
			2);

		lineVertices[0] = -1.0f;
		lineVertices[1] = count;
		lineVertices[2] = 0.0f;
		lineVertices[3] = 1.0;
		lineVertices[4] = count;
		lineVertices[5] = 0.0f;

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_position_lines);
		glBufferData(GL_ARRAY_BUFFER,
			3 * 2 * sizeof(float),
			lineVertices,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_color_lines);
		glBufferData(GL_ARRAY_BUFFER,
			3 * 2 * sizeof(float),
			lineColor,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//Draw

		glDrawArrays(GL_LINES,
			0,
			2);
	}
}


void DrawTriangle(float offset)
{
	GLfloat lineVertices[6];

	GLfloat lineColor[] = {
		1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f
	};

	lineVertices[0] = 0.0f;
	lineVertices[1] = offset;
	lineVertices[2] = 0.0f;
	lineVertices[3] = -offset;
	lineVertices[4] = -offset;
	lineVertices[5] = 0.0f;

	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_position_lines);
	glBufferData(GL_ARRAY_BUFFER,
		3 * 2 * sizeof(float),
		lineVertices,
		GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_color_lines);
	glBufferData(GL_ARRAY_BUFFER,
		3 * 2 * sizeof(float),
		lineColor,
		GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Draw

	glDrawArrays(GL_LINES,
		0,
		2);

	lineVertices[0] = -offset;
	lineVertices[1] = -offset;
	lineVertices[2] = 0.0f;
	lineVertices[3] = offset;
	lineVertices[4] = -offset;
	lineVertices[5] = 0.0f;

	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_position_lines);
	glBufferData(GL_ARRAY_BUFFER,
		3 * 2 * sizeof(float),
		lineVertices,
		GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_color_lines);
	glBufferData(GL_ARRAY_BUFFER,
		3 * 2 * sizeof(float),
		lineColor,
		GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Draw

	glDrawArrays(GL_LINES,
		0,
		2);

	lineVertices[0] = offset;
	lineVertices[1] = -offset;
	lineVertices[2] = 0.0f;
	lineVertices[3] = 0.0f;
	lineVertices[4] = offset;
	lineVertices[5] = 0.0f;

	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_position_lines);
	glBufferData(GL_ARRAY_BUFFER,
		3 * 2 * sizeof(float),
		lineVertices,
		GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_color_lines);
	glBufferData(GL_ARRAY_BUFFER,
		3 * 2 * sizeof(float),
		lineColor,
		GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Draw

	glDrawArrays(GL_LINES,
		0,
		2);

}

void DrawInCircle(float x1, float y1, float z1,
	float x2, float y2, float z2,
	float x3, float y3, float z3
)
{
	static float ArrInCirclePos[(unsigned int)((2 * 3.1415) / 0.001) * 3];
	static float ArrInCircleColor[(unsigned int)((2 * 3.1415) / 0.001) * 3];

	static int isArrInCircleInit = 0;

	if (isArrInCircleInit == 0) {
		
		float count = -1;
		int noCount = 2000;
		GLfloat angle = 0;

		float distP1P2 = sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1) + (z2 - z1)*(z2 - z1));
		float distP2P3 = sqrt((x3 - x2)*(x3 - x2) + (y3 - y2)*(y3 - y2) + (z3 - z2)*(z3 - z2));
		float distP3P1 = sqrt((x1 - x3)*(x1 - x3) + (y1 - y3)*(y1 - y3) + (z1 - z3)*(z1 - z3));

		float semiPerimeter = (distP1P2 + distP2P3 + distP3P1) / 2;

		float radius = sqrt((semiPerimeter - distP1P2)*(semiPerimeter - distP2P3)*(semiPerimeter - distP3P1) / semiPerimeter);

		float Ox = (x3 * distP1P2 + x1 * distP2P3 + x2 * distP3P1) / (semiPerimeter * 2);
		float Oy = (y3 * distP1P2 + y1 * distP2P3 + y2 * distP3P1) / (semiPerimeter * 2);
		float Oz = (z3 * distP1P2 + z1 * distP2P3 + z2 * distP3P1) / (semiPerimeter * 2);

		int indexPos = 0;
		int indexColor = 0;

		for (angle = 0.0f; angle < 2 * 3.14159265; angle = angle + 0.001f) {
			ArrInCirclePos[indexPos++] = cos(angle)*radius + Ox;
			ArrInCirclePos[indexPos++] = sin(angle)*radius + Oy;
			ArrInCirclePos[indexPos++] = 0.0f + Oz;

			ArrInCircleColor[indexColor++] = 1.0f;
			ArrInCircleColor[indexColor++] = 1.0f;
			ArrInCircleColor[indexColor++] = 0.0f;
		}
		isArrInCircleInit = 1;
	}


	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_color_point);
	glBufferData(GL_ARRAY_BUFFER,
		3 * sizeof(float)*((2 * 3.1415) / 0.001),
		ArrInCircleColor,
		GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_position_point);
	glBufferData(GL_ARRAY_BUFFER,
		3 * sizeof(float)*((2 * 3.1415) / 0.001) ,
		ArrInCirclePos,
		GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Draw

	glDrawArrays(GL_POINTS,
		0,
		((2 * 3.1415) / 0.001));


	//float count = -1;
	//int noCount = 2000;
	//GLfloat angle = 0;

	//float distP1P2 = sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1) + (z2 - z1)*(z2 - z1));
	//float distP2P3 = sqrt((x3 - x2)*(x3 - x2) + (y3 - y2)*(y3 - y2) + (z3 - z2)*(z3 - z2));
	//float distP3P1 = sqrt((x1 - x3)*(x1 - x3) + (y1 - y3)*(y1 - y3) + (z1 - z3)*(z1 - z3));

	//float semiPerimeter = (distP1P2 + distP2P3 + distP3P1) / 2;

	//float radius = sqrt((semiPerimeter - distP1P2)*(semiPerimeter - distP2P3)*(semiPerimeter - distP3P1) / semiPerimeter);

	//float Ox = (x3 * distP1P2 + x1 * distP2P3 + x2 * distP3P1) / (semiPerimeter * 2);
	//float Oy = (y3 * distP1P2 + y1 * distP2P3 + y2 * distP3P1) / (semiPerimeter * 2);
	//float Oz = (z3 * distP1P2 + z1 * distP2P3 + z2 * distP3P1) / (semiPerimeter * 2);

	//float pointVertices[3];
	//GLfloat pointColors[] = {
	//	1.0f,1.0f,0.0f
	//};

	//glBindBuffer(GL_ARRAY_BUFFER,
	//	vbo_color_point);
	//glBufferData(GL_ARRAY_BUFFER,
	//	3 * sizeof(float),
	//	pointColors,
	//	GL_DYNAMIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	//for (angle = 0.0f; angle < 2 * 3.14159265; angle = angle + 0.001f) {

	//	pointVertices[0] = cos(angle)*radius+Ox;
	//	pointVertices[1] = sin(angle)*radius+Oy;
	//	pointVertices[2] = 0.0f+Oz;

	//	glBindBuffer(GL_ARRAY_BUFFER,
	//		vbo_position_point);
	//	glBufferData(GL_ARRAY_BUFFER,
	//		3 * sizeof(float),
	//		pointVertices,
	//		GL_DYNAMIC_DRAW);
	//	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//	//Draw

	//	glDrawArrays(GL_POINTS,
	//		0,
	//		1);
	//}
}


void DrawCircle(GLfloat radius) {
	GLfloat angle = 0;

	static float ArrInCirclePos[(unsigned int)((2 * 3.1415) / 0.001) * 3];
	static float ArrInCircleColor[(unsigned int)((2 * 3.1415) / 0.001) * 3];

	static int isCircleInit = 0;

	if (isCircleInit == 0) {
		int indexPos = 0;
		int indexColor = 0;

		for (angle = 0.0f; angle < 2 * 3.14159265; angle = angle + 0.001f) {
			ArrInCirclePos[indexPos++] = cos(angle)*radius;
			ArrInCirclePos[indexPos++] = sin(angle)*radius;
			ArrInCirclePos[indexPos++] = 0.0f;

			ArrInCircleColor[indexColor++] = 1.0f;
			ArrInCircleColor[indexColor++] = 1.0f;
			ArrInCircleColor[indexColor++] = 0.0f;

			int isCircleInit = 1;
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_color_point);
	glBufferData(GL_ARRAY_BUFFER,
		3 * sizeof(float)*((2 * 3.1415) / 0.001),
		ArrInCircleColor,
		GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_position_point);
	glBufferData(GL_ARRAY_BUFFER,
		3 * sizeof(float)*((2 * 3.1415) / 0.001),
		ArrInCirclePos,
		GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Draw
	glDrawArrays(GL_POINTS,
		0,
		((2 * 3.1415) / 0.001));

}

void DrawRectangle(GLfloat width, GLfloat height) {

	GLfloat lineVertices[6];

	GLfloat lineColor[] = {
		1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f
	};

	lineVertices[0] = width / 2;
	lineVertices[1] = height / 2;
	lineVertices[2] = 0.0f;
	lineVertices[3] = -width / 2;
	lineVertices[4] = height / 2;
	lineVertices[5] = 0.0f;

	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_position_lines);
	glBufferData(GL_ARRAY_BUFFER,
		3 * 2 * sizeof(float),
		lineVertices,
		GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_color_lines);
	glBufferData(GL_ARRAY_BUFFER,
		3 * 2 * sizeof(float),
		lineColor,
		GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Draw

	glDrawArrays(GL_LINES,
		0,
		2);

	lineVertices[0] = -width / 2;
	lineVertices[1] = height / 2;
	lineVertices[2] = 0.0f;
	lineVertices[3] = -width / 2;
	lineVertices[4] = -height / 2;
	lineVertices[5] = 0.0f;

	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_position_lines);
	glBufferData(GL_ARRAY_BUFFER,
		3 * 2 * sizeof(float),
		lineVertices,
		GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_color_lines);
	glBufferData(GL_ARRAY_BUFFER,
		3 * 2 * sizeof(float),
		lineColor,
		GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Draw

	glDrawArrays(GL_LINES,
		0,
		2);

	lineVertices[0] = -width / 2;
	lineVertices[1] = -height / 2;
	lineVertices[2] = 0.0f;
	lineVertices[3] = width / 2;
	lineVertices[4] = -height / 2;
	lineVertices[5] = 0.0f;

	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_position_lines);
	glBufferData(GL_ARRAY_BUFFER,
		3 * 2 * sizeof(float),
		lineVertices,
		GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_color_lines);
	glBufferData(GL_ARRAY_BUFFER,
		3 * 2 * sizeof(float),
		lineColor,
		GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Draw

	glDrawArrays(GL_LINES,
		0,
		2);


	lineVertices[0] = width / 2;
	lineVertices[1] = -height / 2;
	lineVertices[2] = 0.0f;
	lineVertices[3] = width / 2;
	lineVertices[4] = height / 2;
	lineVertices[5] = 0.0f;

	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_position_lines);
	glBufferData(GL_ARRAY_BUFFER,
		3 * 2 * sizeof(float),
		lineVertices,
		GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_color_lines);
	glBufferData(GL_ARRAY_BUFFER,
		3 * 2 * sizeof(float),
		lineColor,
		GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Draw

	glDrawArrays(GL_LINES,
		0,
		2);

}

void DrawCircleAroundRectangle(GLfloat width, GLfloat height) {

	float radius = sqrt(width / 2 * width / 2 + height / 2 * height / 2);
	DrawCircle(radius);

}