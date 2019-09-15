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
//#include</usr/include/SOIL/SOIL.h>
#include"vmath.h"
#include"Sphere.h"
#include"MatrixStack.h"

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

GLuint mvpUniform;
mat4 perspectiveProjectionMatrix;
GLuint samplerUniform;

int shoulder = 0;
int elbow = 0;

MatrixStack ms;

Sphere Elbow;
Sphere Shoulder;

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
						case 'S':
							shoulder = (shoulder + 3) % 360;
						break;
						case 's':	
							shoulder = (shoulder - 3) % 360;
						break;	
						case 'E':
							elbow = (elbow + 3) % 360;
						break;
						case 'e':
							elbow = (elbow - 3) % 360;
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

	Elbow.setTextureColor(0.5f*255.0f, 0.35f*255.0f, 0.05f * 255);
	Elbow.set(0.5f, 10, 10);

	Shoulder.setTextureColor(0.5f*255.0f, 0.35f*255.0f, 0.05f * 255);
	Shoulder.set(0.5f, 10, 10);

	//earth.extTexture = true;
	//LoadTexture(&texture_Stone, MAKEINTRESOURCE(IDBITMAP_STONE));
	//earth.txtImage = &texture_Stone;

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0f);

	//glEnable(GL_CULL_FACE);
	//glDisable(GL_CULL_FACE);

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

	//Pyramid
	//Initialize matrices

	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	//Transformation

	translationMatrix = translate(0.0f, 0.0f, -12.0f);

	//Matrix Multiplication
	modelViewMatrix = translationMatrix * rotationMatrix;

	//modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	ms.push(modelViewMatrix);

	rotationMatrix = rotate((float)shoulder, 0.0f, 0.0f, 1.0f);

	modelViewMatrix = modelViewMatrix * rotationMatrix;

	translationMatrix = mat4::identity();
	translationMatrix = translate(1.0f, 0.0f, 0.0f);

	modelViewMatrix = modelViewMatrix * translationMatrix;

	ms.push(modelViewMatrix);

	scaleMatrix = scale(2.0f, 0.5f, 1.0f);

	modelViewMatrix = modelViewMatrix * scaleMatrix;

	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(mvpUniform,
		1,
		GL_FALSE,
		modelViewProjectionMatrix);

	glUniform1i(samplerUniform, 0);

	Shoulder.draw();

	//earth.drawLines(color);

	modelViewMatrix = ms.pop();

	translationMatrix = mat4::identity();
	translationMatrix = translate(1.0f, 0.0f, 0.0f);

	modelViewMatrix = modelViewMatrix * translationMatrix;

	rotationMatrix = mat4::identity();
	rotationMatrix = rotate((float)elbow, 0.0f, 0.0f, 1.0f);

	modelViewMatrix = modelViewMatrix * rotationMatrix;

	translationMatrix = mat4::identity();
	translationMatrix = translate(1.0f, 0.0f, 0.0f);

	modelViewMatrix = modelViewMatrix * translationMatrix;

	ms.push(modelViewMatrix);

	scaleMatrix= mat4::identity();
	scaleMatrix = scale(2.0f, 0.5f, 1.0f);

	modelViewMatrix = modelViewMatrix * scaleMatrix;

	modelViewProjectionMatrix = mat4::identity();
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(mvpUniform,
		1,
		GL_FALSE,
		modelViewProjectionMatrix);

	glUniform1i(samplerUniform, 0);

	Elbow.draw();

	ms.pop();
	ms.pop();

	glUseProgram(0);

	glXSwapBuffers(gpDisplay,gWindow);
}
void Update(void)
{

}


//void PrintTime() {
//	struct tm newtime;
//	__time64_t long_time;
//	errno_t err;
//	_time64(&long_time);// Get time as 64-bit integer.
//	err = _localtime64_s(&newtime, &long_time);// Convert to local time.
//	if (!err && gpFile) {
//		fprintf_s(gpFile, "%i:%i:%i %i-%i-%i ",newtime.tm_mday,newtime.tm_mon+1,newtime.tm_year+1900,newtime.tm_hour,newtime.tm_min,newtime.tm_sec);
//	}
//}
