#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>
#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/XKBlib.h>
#include<X11/keysym.h>

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

GLUquadric *quadric[24];

bool bLight = false;
GLfloat lightAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat lightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
//GLfloat lightSpecular[] = { 1.0f,0.0f,0.0f,1.0f };
GLfloat lightPosition[] = { 0.0f,0.0f,0.0f,1.0f };

GLfloat angleOfXRotation = 0.0f;
GLfloat angleOfYRotation = 0.0f;
GLfloat angleOfZRotation = 0.0f;
GLint keyPress = 0;

GLfloat light_Model_Ambient[] = { 0.2f,0.2f,0.2f,1.0f };
GLfloat light_Model_Local_Viewer[] = { 0.0f };

void Update(void);

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
                case KeyPress:
                        keysym=XkbKeycodeToKeysym(gpDisplay,event.xkey.keycode,0,0);
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
                        case 'l':
                        case 'L':
                            if (bLight == false) {
                                bLight = true;
                                glEnable(GL_LIGHTING);
                            }
                            else {
                                bLight = false;
                                glDisable(GL_LIGHTING);
                            }
                            break;
                        case 'x':
                        case 'X':
                            keyPress = 1;
                            lightPosition[0] = 0.0f;
                            lightPosition[1] = 0.0f;
                            lightPosition[2] = 0.0f;

                            angleOfXRotation = 0.0f;
                            break;
                        case 'y':
                        case 'Y':
                            keyPress = 2;
                            lightPosition[0] = 0.0f;
                            lightPosition[1] = 0.0f;
                            lightPosition[2] = 0.0f;

                            angleOfYRotation = 0.0f;
                            break;
                        case 'z':
                        case 'Z':
                            keyPress = 3;
                            lightPosition[0] = 0.0f;
                            lightPosition[1] = 0.0f;
                            lightPosition[2] = 0.0f;

                            angleOfZRotation = 0.0f;
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
GLX_DEPTH_SIZE,24,
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
//fputs("3\n", filePointer) ; 
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

for (int i = 0; i < 24; i++) {
	if (quadric[i])
		gluDeleteQuadric(quadric[i]);
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

	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);

	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_Model_Ambient);
	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, light_Model_Local_Viewer);

	glEnable(GL_LIGHT0);

	for (int i = 0; i < 24; i++) {
		quadric[i] = gluNewQuadric();
	}

    Resize(giWindowWidth,giWindowHeight);

}

//Function Resize
void Resize(int width, int height) {
	if (height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (width <= height) {
		glOrtho(0.0f,
			15.0f,
			0.0f,
			(15.0f*(GLfloat)height / (GLfloat)width),
			-10.0f,
			10.0f
		);
	}
	else
	{
		glOrtho(
			0.0f,
			(15.0f*(GLfloat)width / (GLfloat)height),
			0.0f,
			15.5f,
			-10.0f,
			10.0f);
	}    
}

//Function Display
void DisplayOpenGL(void) {
	void draw24Sphere();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (keyPress == 1) {
		glRotatef(angleOfXRotation, 0.0f, 1.0f, 0.0f);
		lightPosition[1] = angleOfXRotation;
		angleOfXRotation = angleOfXRotation + 0.5f;
		if (angleOfXRotation > 360.0f)
			angleOfXRotation = 0.0f;
	}
	if (keyPress == 2) {
		glRotatef(angleOfYRotation, 0.0f, 0.0f, 1.0f);
		lightPosition[2] = angleOfYRotation;
		angleOfYRotation = angleOfYRotation + 0.5f;
		if (angleOfYRotation > 360.0f)
			angleOfYRotation = 0.0f;
	}
	if (keyPress == 3) {
		glRotatef(angleOfZRotation, 1.0f, 0.0f, 0.0f);
		lightPosition[0] = angleOfZRotation;

		angleOfZRotation = angleOfZRotation + 0.5f;
		if (angleOfZRotation > 360.0f)
			angleOfZRotation = 0.0f;
	}

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	draw24Sphere();

	glXSwapBuffers(gpDisplay,gWindow);
}

void Update(void)
{

}

void draw24Sphere()
{
	GLfloat materialAmbient[4];
	GLfloat materialDiffuse[4];
	GLfloat materialSpecular[4];
	GLfloat materialShininess[1];
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// ***** 1st sphere on 1st column, emerald *****
	// ambient material
	materialAmbient[0] = 0.0215; // r
	materialAmbient[1] = 0.1745; // g
	materialAmbient[2] = 0.0215; // b
	materialAmbient[3] = 1.0f;   // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// diffuse material
	materialDiffuse[0] = 0.07568; // r
	materialDiffuse[1] = 0.61424; // g
	materialDiffuse[2] = 0.07568; // b
	materialDiffuse[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// specular material
	materialSpecular[0] = 0.633;    // r
	materialSpecular[1] = 0.727811; // g
	materialSpecular[2] = 0.633;    // b
	materialSpecular[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	// shininess
	materialShininess[0] = 0.6 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(1.5f, 14.0f, 0.0f);
	gluSphere(quadric[0], 1.0f, 30, 30);
	// ***** 2nd sphere on 1st column, jade *****
	// ambient material
	materialAmbient[0] = 0.135;  // r
	materialAmbient[1] = 0.2225; // g
	materialAmbient[2] = 0.1575; // b
	materialAmbient[3] = 1.0f;   // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// diffuse material
	materialDiffuse[0] = 0.54; // r
	materialDiffuse[1] = 0.89; // g
	materialDiffuse[2] = 0.63; // b
	materialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// specular material
	materialSpecular[0] = 0.316228; // r
	materialSpecular[1] = 0.316228; // g
	materialSpecular[2] = 0.316228; // b
	materialSpecular[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	// shininess
	materialShininess[0] = 0.1 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(1.5f, 11.5f, 0.0f);
	gluSphere(quadric[1], 1.0f, 30, 30);
	// ***** 3rd sphere on 1st column, obsidian *****
	// ambient material
	materialAmbient[0] = 0.05375; // r
	materialAmbient[1] = 0.05;    // g
	materialAmbient[2] = 0.06625; // b
	materialAmbient[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// diffuse material
	materialDiffuse[0] = 0.18275; // r
	materialDiffuse[1] = 0.17;    // g
	materialDiffuse[2] = 0.22525; // b
	materialDiffuse[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// specular material
	materialSpecular[0] = 0.332741; // r
	materialSpecular[1] = 0.328634; // g
	materialSpecular[2] = 0.346435; // b
	materialSpecular[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	// shininess
	materialShininess[0] = 0.3 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(1.5f, 9.0f, 0.0f);
	gluSphere(quadric[2], 1.0f, 30, 30);
	// ***** 4th sphere on 1st column, pearl *****
	// ambient material
	materialAmbient[0] = 0.25;    // r
	materialAmbient[1] = 0.20725; // g
	materialAmbient[2] = 0.20725; // b
	materialAmbient[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// diffuse material
	materialDiffuse[0] = 1.0;   // r
	materialDiffuse[1] = 0.829; // g
	materialDiffuse[2] = 0.829; // b
	materialDiffuse[3] = 1.0f;  // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// specular material
	materialSpecular[0] = 0.296648; // r
	materialSpecular[1] = 0.296648; // g
	materialSpecular[2] = 0.296648; // b
	materialSpecular[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	// shininess
	materialShininess[0] = 0.088 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(1.5f, 6.5f, 0.0f);
	gluSphere(quadric[3], 1.0f, 30, 30);
	// ***** 5th sphere on 1st column, ruby *****
	// ambient material
	materialAmbient[0] = 0.1745;  // r
	materialAmbient[1] = 0.01175; // g
	materialAmbient[2] = 0.01175; // b
	materialAmbient[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// diffuse material
	materialDiffuse[0] = 0.61424; // r
	materialDiffuse[1] = 0.04136; // g
	materialDiffuse[2] = 0.04136; // b
	materialDiffuse[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// specular material
	materialSpecular[0] = 0.727811; // r
	materialSpecular[1] = 0.626959; // g
	materialSpecular[2] = 0.626959; // b
	materialSpecular[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	// shininess
	materialShininess[0] = 0.6 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(1.5f, 4.0f, 0.0f);
	gluSphere(quadric[4], 1.0f, 30, 30);
	// ***** 6th sphere on 1st column, turquoise *****
	// ambient material
	materialAmbient[0] = 0.1;     // r
	materialAmbient[1] = 0.18725; // g
	materialAmbient[2] = 0.1745;  // b
	materialAmbient[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// diffuse material
	materialDiffuse[0] = 0.396;   // r
	materialDiffuse[1] = 0.74151; // g
	materialDiffuse[2] = 0.69102; // b
	materialDiffuse[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// specular material
	materialSpecular[0] = 0.297254; // r
	materialSpecular[1] = 0.30829;  // g
	materialSpecular[2] = 0.306678; // b
	materialSpecular[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	// shininess
	materialShininess[0] = 0.1 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(1.5f, 1.5f, 0.0f);
	gluSphere(quadric[5], 1.0f, 30, 30);
	// ***** 1st sphere on 2nd column, brass *****
	// ambient material
	materialAmbient[0] = 0.329412; // r
	materialAmbient[1] = 0.223529; // g
	materialAmbient[2] = 0.027451; // b
	materialAmbient[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// diffuse material
	materialDiffuse[0] = 0.780392; // r
	materialDiffuse[1] = 0.568627; // g
	materialDiffuse[2] = 0.113725; // b
	materialDiffuse[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// specular material
	materialSpecular[0] = 0.992157; // r
	materialSpecular[1] = 0.941176; // g
	materialSpecular[2] = 0.807843; // b
	materialSpecular[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	// shininess
	materialShininess[0] = 0.21794872 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(6.0f, 14.0f, 0.0f);
	gluSphere(quadric[6], 1.0f, 30, 30);
	// ***** 2nd sphere on 2nd column, bronze *****
	// ambient material
	materialAmbient[0] = 0.2125; // r
	materialAmbient[1] = 0.1275; // g
	materialAmbient[2] = 0.054;  // b
	materialAmbient[3] = 1.0f;   // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// diffuse material
	materialDiffuse[0] = 0.714;   // r
	materialDiffuse[1] = 0.4284;  // g
	materialDiffuse[2] = 0.18144; // b
	materialDiffuse[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// specular material
	materialSpecular[0] = 0.393548; // r
	materialSpecular[1] = 0.271906; // g
	materialSpecular[2] = 0.166721; // b
	materialSpecular[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	// shininess
	materialShininess[0] = 0.2 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(6.0f, 11.5f, 0.0f);
	gluSphere(quadric[7], 1.0f, 30, 30);
	// ***** 3rd sphere on 2nd column, chrome *****
	// ambient material
	materialAmbient[0] = 0.25; // r
	materialAmbient[1] = 0.25; // g
	materialAmbient[2] = 0.25; // b
	materialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// diffuse material
	materialDiffuse[0] = 0.4;  // r
	materialDiffuse[1] = 0.4;  // g
	materialDiffuse[2] = 0.4;  // b
	materialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// specular material
	materialSpecular[0] = 0.774597; // r
	materialSpecular[1] = 0.774597; // g
	materialSpecular[2] = 0.774597; // b
	materialSpecular[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	// shininess
	materialShininess[0] = 0.6 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(6.0f, 9.0f, 0.0f);
	gluSphere(quadric[8], 1.0f, 30, 30);
	// ***** 4th sphere on 2nd column, copper *****
	// ambient material
	materialAmbient[0] = 0.19125; // r
	materialAmbient[1] = 0.0735;  // g
	materialAmbient[2] = 0.0225;  // b
	materialAmbient[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// diffuse material
	materialDiffuse[0] = 0.7038;  // r
	materialDiffuse[1] = 0.27048; // g
	materialDiffuse[2] = 0.0828;  // b
	materialDiffuse[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// specular material
	materialSpecular[0] = 0.256777; // r
	materialSpecular[1] = 0.137622; // g
	materialSpecular[2] = 0.086014; // b
	materialSpecular[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	// shininess
	materialShininess[0] = 0.1 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(6.0f, 6.5f, 0.0f);
	gluSphere(quadric[9], 1.0f, 30, 30);
	// ***** 5th sphere on 2nd column, gold *****
	// ambient material
	materialAmbient[0] = 0.24725; // r
	materialAmbient[1] = 0.1995;  // g
	materialAmbient[2] = 0.0745;  // b
	materialAmbient[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// diffuse material
	materialDiffuse[0] = 0.75164; // r
	materialDiffuse[1] = 0.60648; // g
	materialDiffuse[2] = 0.22648; // b
	materialDiffuse[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// specular material
	materialSpecular[0] = 0.628281; // r
	materialSpecular[1] = 0.555802; // g
	materialSpecular[2] = 0.366065; // b
	materialSpecular[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	// shininess
	materialShininess[0] = 0.4 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(6.0f, 4.0f, 0.0f);
	gluSphere(quadric[10], 1.0f, 30, 30);
	// ***** 6th sphere on 2nd column, silver *****
	// ambient material
	materialAmbient[0] = 0.19225; // r
	materialAmbient[1] = 0.19225; // g
	materialAmbient[2] = 0.19225; // b
	materialAmbient[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// diffuse material
	materialDiffuse[0] = 0.50754; // r
	materialDiffuse[1] = 0.50754; // g
	materialDiffuse[2] = 0.50754; // b
	materialDiffuse[3] = 1.0f;    // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// specular material
	materialSpecular[0] = 0.508273; // r
	materialSpecular[1] = 0.508273; // g
	materialSpecular[2] = 0.508273; // b
	materialSpecular[3] = 1.0f;     // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	// shininess
	materialShininess[0] = 0.4 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(6.0f, 1.5f, 0.0f);
	gluSphere(quadric[11], 1.0f, 30, 30);
	// ***** 1st sphere on 3rd column, black *****
	// ambient material
	materialAmbient[0] = 0.0;  // r
	materialAmbient[1] = 0.0;  // g
	materialAmbient[2] = 0.0;  // b
	materialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// diffuse material
	materialDiffuse[0] = 0.01; // r
	materialDiffuse[1] = 0.01; // g
	materialDiffuse[2] = 0.01; // b
	materialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// specular material
	materialSpecular[0] = 0.50; // r
	materialSpecular[1] = 0.50; // g
	materialSpecular[2] = 0.50; // b
	materialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	// shininess
	materialShininess[0] = 0.25 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(10.5f, 14.0f, 0.0f);
	gluSphere(quadric[12], 1.0f, 30, 30);
	// ***** 2nd sphere on 3rd column, cyan *****
	// ambient material
	materialAmbient[0] = 0.0;  // r
	materialAmbient[1] = 0.1;  // g
	materialAmbient[2] = 0.06; // b
	materialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// diffuse material
	materialDiffuse[0] = 0.0;        // r
	materialDiffuse[1] = 0.50980392; // g
	materialDiffuse[2] = 0.50980392; // b
	materialDiffuse[3] = 1.0f;       // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// specular material
	materialSpecular[0] = 0.50196078; // r
	materialSpecular[1] = 0.50196078; // g
	materialSpecular[2] = 0.50196078; // b
	materialSpecular[3] = 1.0f;       // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	// shininess
	materialShininess[0] = 0.25 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(10.5f, 11.5f, 0.0f);
	gluSphere(quadric[13], 1.0f, 30, 30);
	// ***** 3rd sphere on 2nd column, green *****
	// ambient material
	materialAmbient[0] = 0.0;  // r
	materialAmbient[1] = 0.0;  // g
	materialAmbient[2] = 0.0;  // b
	materialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// diffuse material
	materialDiffuse[0] = 0.1;  // r
	materialDiffuse[1] = 0.35; // g
	materialDiffuse[2] = 0.1;  // b
	materialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// specular material
	materialSpecular[0] = 0.45; // r
	materialSpecular[1] = 0.55; // g
	materialSpecular[2] = 0.45; // b
	materialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	// shininess
	materialShininess[0] = 0.25 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(10.5f, 9.0f, 0.0f);
	gluSphere(quadric[14], 1.0f, 30, 30);
	// ***** 4th sphere on 3rd column, red *****
	// ambient material
	materialAmbient[0] = 0.0;  // r
	materialAmbient[1] = 0.0;  // g
	materialAmbient[2] = 0.0;  // b
	materialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// diffuse material
	materialDiffuse[0] = 0.5;  // r
	materialDiffuse[1] = 0.0;  // g
	materialDiffuse[2] = 0.0;  // b
	materialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// specular material
	materialSpecular[0] = 0.7;  // r
	materialSpecular[1] = 0.6;  // g
	materialSpecular[2] = 0.6;  // b
	materialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	// shininess
	materialShininess[0] = 0.25 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(10.5f, 6.5f, 0.0f);
	gluSphere(quadric[15], 1.0f, 30, 30);
	// ***** 5th sphere on 3rd column, white *****
	// ambient material
	materialAmbient[0] = 0.0;  // r
	materialAmbient[1] = 0.0;  // g
	materialAmbient[2] = 0.0;  // b
	materialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// diffuse material
	materialDiffuse[0] = 0.55; // r
	materialDiffuse[1] = 0.55; // g
	materialDiffuse[2] = 0.55; // b
	materialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// specular material
	materialSpecular[0] = 0.70; // r
	materialSpecular[1] = 0.70; // g
	materialSpecular[2] = 0.70; // b
	materialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	// shininess
	materialShininess[0] = 0.25 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(10.5f, 4.0f, 0.0f);
	gluSphere(quadric[16], 1.0f, 30, 30);
	// ***** 6th sphere on 3rd column, yellow plastic *****
	// ambient material
	materialAmbient[0] = 0.0;  // r
	materialAmbient[1] = 0.0;  // g
	materialAmbient[2] = 0.0;  // b
	materialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// diffuse material
	materialDiffuse[0] = 0.5;  // r
	materialDiffuse[1] = 0.5;  // g
	materialDiffuse[2] = 0.0;  // b
	materialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// specular material
	materialSpecular[0] = 0.60; // r
	materialSpecular[1] = 0.60; // g
	materialSpecular[2] = 0.50; // b
	materialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	// shininess
	materialShininess[0] = 0.25 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(10.5f, 1.5f, 0.0f);
	gluSphere(quadric[17], 1.0f, 30, 30);
	// ***** 1st sphere on 4th column, black *****
	// ambient material
	materialAmbient[0] = 0.02; // r
	materialAmbient[1] = 0.02; // g
	materialAmbient[2] = 0.02; // b
	materialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// diffuse material
	materialDiffuse[0] = 0.01; // r
	materialDiffuse[1] = 0.01; // g
	materialDiffuse[2] = 0.01; // b
	materialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// specular material
	materialSpecular[0] = 0.4;  // r
	materialSpecular[1] = 0.4;  // g
	materialSpecular[2] = 0.4;  // b
	materialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	// shininess
	materialShininess[0] = 0.078125 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(15.0f, 14.0f, 0.0f);
	gluSphere(quadric[18], 1.0f, 30, 30);
	// ***** 2nd sphere on 4th column, cyan *****
	// ambient material
	materialAmbient[0] = 0.0;  // r
	materialAmbient[1] = 0.05; // g
	materialAmbient[2] = 0.05; // b
	materialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// diffuse material
	materialDiffuse[0] = 0.4;  // r
	materialDiffuse[1] = 0.5;  // g
	materialDiffuse[2] = 0.5;  // b
	materialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// specular material
	materialSpecular[0] = 0.04; // r
	materialSpecular[1] = 0.7;  // g
	materialSpecular[2] = 0.7;  // b
	materialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	// shininess
	materialShininess[0] = 0.078125 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(15.0f, 11.5f, 0.0f);
	gluSphere(quadric[19], 1.0f, 30, 30);
	// ***** 3rd sphere on 4th column, green *****
	// ambient material
	materialAmbient[0] = 0.0;  // r
	materialAmbient[1] = 0.05; // g
	materialAmbient[2] = 0.0;  // b
	materialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// diffuse material
	materialDiffuse[0] = 0.4;  // r
	materialDiffuse[1] = 0.5;  // g
	materialDiffuse[2] = 0.4;  // b
	materialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// specular material
	materialSpecular[0] = 0.04; // r
	materialSpecular[1] = 0.7;  // g
	materialSpecular[2] = 0.04; // b
	materialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	// shininess
	materialShininess[0] = 0.078125 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(15.0f, 9.0f, 0.0f);
	gluSphere(quadric[20], 1.0f, 30, 30);
	// ***** 4th sphere on 4th column, red *****
	// ambient material
	materialAmbient[0] = 0.05; // r
	materialAmbient[1] = 0.0;  // g
	materialAmbient[2] = 0.0;  // b
	materialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// diffuse material
	materialDiffuse[0] = 0.5;  // r
	materialDiffuse[1] = 0.4;  // g
	materialDiffuse[2] = 0.4;  // b
	materialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// specular material
	materialSpecular[0] = 0.7;  // r
	materialSpecular[1] = 0.04; // g
	materialSpecular[2] = 0.04; // b
	materialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	// shininess
	materialShininess[0] = 0.078125 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(15.0f, 6.5f, 0.0f);
	gluSphere(quadric[21], 1.0f, 30, 30);
	// ***** 5th sphere on 4th column, white *****
	// ambient material
	materialAmbient[0] = 0.05; // r
	materialAmbient[1] = 0.05; // g
	materialAmbient[2] = 0.05; // b
	materialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// diffuse material
	materialDiffuse[0] = 0.5;  // r
	materialDiffuse[1] = 0.5;  // g
	materialDiffuse[2] = 0.5;  // b
	materialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// specular material
	materialSpecular[0] = 0.7;  // r
	materialSpecular[1] = 0.7;  // g
	materialSpecular[2] = 0.7;  // b
	materialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	// shininess
	materialShininess[0] = 0.078125 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(15.0f, 4.0f, 0.0f);
	gluSphere(quadric[22], 1.0f, 30, 30);
	// ***** 6th sphere on 4th column, yellow rubber *****
	// ambient material
	materialAmbient[0] = 0.05; // r
	materialAmbient[1] = 0.05; // g
	materialAmbient[2] = 0.0;  // b
	materialAmbient[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	// diffuse material
	materialDiffuse[0] = 0.5;  // r
	materialDiffuse[1] = 0.5;  // g
	materialDiffuse[2] = 0.4;  // b
	materialDiffuse[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	// specular material
	materialSpecular[0] = 0.7;  // r
	materialSpecular[1] = 0.7;  // g
	materialSpecular[2] = 0.04; // b
	materialSpecular[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	// shininess
	materialShininess[0] = 0.078125 * 128;
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(15.0f, 1.5f, 0.0f);
	gluSphere(quadric[23], 1.0f, 30, 30);
}