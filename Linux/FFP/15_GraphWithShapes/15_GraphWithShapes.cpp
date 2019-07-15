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

#define _USE_MATH_DEFINES 1
#include<math.h>

#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>

using namespace std;
bool bFullScreen=false;
Display *gpDisplay=NULL;
XVisualInfo *gpXVisualInfo=NULL;
Colormap gColorMap;
Window gWindow;
int giWindowWidth=800;
int giWindowHeight=600;
static GLXContext gGLXContext;
bool bDone=false;
//FILE *filePointer ; 

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
//filePointer = fopen("ChaitanyaLog.c", "w+");
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
static int FrameBufferAttributes[]={
GLX_RGBA,
GLX_DOUBLEBUFFER ,True,
GLX_RED_SIZE,8,
GLX_GREEN_SIZE,8,
GLX_BLUE_SIZE,8,
GLX_ALPHA_SIZE,8,
//GLX_DEPTH_SIZE,24,
None    
};

gpDisplay=XOpenDisplay(NULL);
if(gpDisplay == NULL){
        printf("ERROR : Unable To Open X Display.\nExitting Now...\n");
        UnInitialize();
        exit(1);
}
defaultScreen=XDefaultScreen(gpDisplay);
defaultDepth=DefaultDepth(gpDisplay,defaultScreen);  

gpXVisualInfo=glXChooseVisual(gpDisplay,
    defaultScreen,
    FrameBufferAttributes);

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

}
void Initialize(void){
    //Function Declaration
    void Resize(int, int);

    gGLXContext=glXCreateContext(gpDisplay,
                                gpXVisualInfo,
                                NULL,
                                GL_TRUE);
    glXMakeCurrent(gpDisplay,
        gWindow,
        gGLXContext);

	//glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	// glClearDepth(1.0f);
	// glEnable(GL_DEPTH_TEST);
	// glDepthFunc(GL_LEQUAL);
	// glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glEnable(GL_TEXTURE_2D);

    Resize(giWindowWidth,giWindowHeight);

}

//Function Resize
void Resize(int width, int height) {
	if (height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,
		(GLfloat)width / (GLfloat)height,
		0.1f,
		100.0f);

}
//Function Display
void DisplayOpenGL(void) {
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	void DrawTriangle(float);
	void DrawInCircle(float x1, float y1, float z1,
		float x2, float y2, float z2,
		float x3, float y3, float z3
	);
	void DrawGraph(GLfloat offset);
	void DrawCircle(GLfloat radius);
	void DrawRectangle(GLfloat width, GLfloat height);
	void DrawCircleAroundRectangle(GLfloat width, GLfloat height);

	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -3.0f);

	DrawGraph(0.01f);
	DrawTriangle(0.5f);
	DrawInCircle(0.0f, 0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f
	);
	DrawRectangle(1.0f,1.0f);
	DrawCircleAroundRectangle(1.0f, 1.0f);

	glXSwapBuffers(gpDisplay,gWindow);
}
void Update(void)
{

}

void DrawTriangle(float offset)
{
	float count = -1;
	int noCount = 2000;
	GLfloat angle = 0;
	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 0.0f);
	
	glVertex3f(0.0f, offset, 0.0f);
	glVertex3f(-offset, -offset, 0.0f);

	glVertex3f(-offset, -offset, 0.0f);
	glVertex3f(offset, -offset, 0.0f);

	glVertex3f(offset, -offset, 0.0f);
	glVertex3f(0.0f, offset, 0.0f);

	glEnd();
}

void DrawInCircle(float x1, float y1, float z1,
	float x2, float y2, float z2,
	float x3, float y3, float z3
)
{
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

	glBegin(GL_LINE_LOOP);
	glColor3f(1.0f,1.0f,0.0f);
	for (count = 0; count <= noCount; count++) {
		angle = 2 * M_PI*count / noCount;
		glVertex3f(cos(angle)*radius + Ox, sin(angle)*radius + Oy, 0.0f + Oz);
	}

	glEnd();

}

void DrawGraph(GLfloat offset) {

	glBegin(GL_LINES);

	glColor3f(0.0, 0.0f, 1.0f);
		for (GLfloat count = -1.00f; count <= 1.00f; count = count + offset)
		{
			glVertex3f(count,-1.0f,0.0f);
			glVertex3f(count, 1.0f, 0.0f);

			glVertex3f(-1.0f, count,  0.0f);
			glVertex3f(1.0f, count, 0.0f);

		}
		
	glEnd();

}

void DrawCircle(GLfloat radius) {
	float count = -1;
	int noCount = 2000;
	GLfloat angle = 0;

	glBegin(GL_LINE_LOOP);
	glColor3f(1.0f,1.0f,0.0f);
	for (count = 0; count <= noCount; count++) {
		angle = 2 * M_PI*count / noCount;
		glVertex3f(cos(angle)*radius , sin(angle)*radius, 0.0f );
	}

	glEnd();

}

void DrawRectangle(GLfloat width, GLfloat height) {
	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 0.0f);

	glVertex3f(width/2, height/2, 0.0f);
	glVertex3f(-width / 2, height / 2, 0.0f);

	glVertex3f(-width / 2, height / 2, 0.0f);
	glVertex3f(-width / 2, -height / 2, 0.0f);

	glVertex3f(-width / 2, -height / 2, 0.0f);
	glVertex3f(width / 2, -height / 2, 0.0f);

	glVertex3f(width / 2, -height / 2, 0.0f);
	glVertex3f(width / 2, height / 2, 0.0f);

	glEnd();
}

void DrawCircleAroundRectangle(GLfloat width, GLfloat height) {
	float count = -1;
	int noCount = 2000;
	GLfloat angle = 0;
	float radius = sqrt(width / 2 * width / 2 + height / 2 * height / 2);

	glBegin(GL_LINE_LOOP);
	glColor3f(1.0f, 1.0f, 0.0f);
	for (count = 0; count <= noCount; count++) {
		angle = 2 * M_PI*count / noCount;
		glVertex3f(cos(angle)*radius, sin(angle)*radius, 0.0f);
	}

	glEnd();
}
