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

#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>
#include</usr/include/SOIL/SOIL.h>

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

GLfloat angle_Cube = 0.0f;
GLuint texture_kundali;
GLfloat angle_Trianlge = 0.0f;
GLuint texture_stone;

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
if (texture_kundali) {
	glDeleteTextures(1, &texture_kundali);
}
if (texture_stone) {
	glDeleteTextures(1, &texture_stone);
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


    glEnable(GL_TEXTURE_2D);
	
    bool bResult=LoadTexture(&texture_stone,"Stone.bmp");
    if(bResult==false){
    printf("ERROR : Failed To Load Texture.\nExiting Now...\n");
    UnInitialize();
    exit(1);
    }

	bResult=LoadTexture(&texture_kundali,"Vijay_Kundali.bmp");
    if(bResult==false){
    printf("ERROR : Failed To Load Texture.\nExiting Now...\n");
    UnInitialize();
    exit(1);
    }
    
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

   
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
		(GLfloat)width/(GLfloat)height,
		0.1f,
		100.0f);
    
}
//Function Display
void DisplayOpenGL(void) {
	void DrawCube(float offset);
	void DrawPyramid(float offset);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	glTranslatef(2.0f, 0.0f, -6.0f);
	glRotatef(angle_Cube, 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D,texture_kundali);

	DrawCube(1.0f);

	glLoadIdentity();
	glTranslatef(-2.0f, 0.0f, -6.0f);
	glBindTexture(GL_TEXTURE_2D, texture_stone);
	glRotatef(angle_Trianlge, 0.0f, 1.0f, 0.0f);

	DrawPyramid(1.0f);

	glXSwapBuffers(gpDisplay,gWindow);
}
void Update(void)
{
    angle_Cube = angle_Cube + 0.15f;
	if (angle_Cube > 360.0f)
		angle_Cube = 0.0f;
	angle_Trianlge = angle_Trianlge + 0.15f;
	if (angle_Trianlge > 360.0f)
		angle_Trianlge = 0.0f;

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
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3 , imageWidth, imageHeight, GL_RGB, GL_UNSIGNED_BYTE, imgData);  

    SOIL_free_image_data(imgData);
    
    return bResult;
}


void DrawCube(float offset)
{
	glBegin(GL_QUADS);

	//TOP
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(offset, offset, -offset);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-offset, offset, -offset);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-offset, offset, offset);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(offset, offset, offset);


	//Bottom
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(offset, -offset, -offset);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-offset, -offset, -offset);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-offset, -offset, offset);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(offset, -offset, offset);

	//Front
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(offset, offset, offset);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-offset, offset, offset);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-offset, -offset, offset);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(offset, -offset, offset);

	//Back
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(offset, offset, -offset);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-offset, offset, -offset);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-offset, -offset, -offset);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(offset, -offset, -offset);

	//Right
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(offset, offset, -offset);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(offset, offset, offset);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(offset, -offset, offset);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(offset, -offset, -offset);

	//Left
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-offset, offset, -offset);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-offset, offset, offset);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-offset, -offset, offset);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-offset, -offset, -offset);

	glEnd();
}


void DrawPyramid(float offset)
{
	glBegin(GL_TRIANGLES);

	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.0f, offset, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-offset, -offset, offset);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(offset, -offset, offset);

	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.0f, offset, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(offset, -offset, offset);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(offset, -offset, -offset);


	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.0f, offset, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(offset, -offset, -offset);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-offset, -offset, -offset);

	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.0f, offset, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-offset, -offset, -offset);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-offset, -offset, offset);

	glEnd();
}