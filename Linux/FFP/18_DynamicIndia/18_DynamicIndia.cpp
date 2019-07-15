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
#include<math.h>

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

GLfloat D_R_OrangeColor = 0.0f;
GLfloat D_G_OrangeColor = 0.0f;
GLfloat D_B_OrangeColor = 0.0f;
GLfloat D_R_GreenColor = 0.0f;
GLfloat D_G_GreenColor = 0.0f;
GLfloat D_B_GreenColor = 0.0f;
GLfloat Plane1_X_Position = -2.5f;
GLfloat angle = 0.0f;
GLfloat Line_X_Position = -1.28f;

GLfloat Line_Orange_R = 1.0f;
GLfloat Line_Orange_G = 0.6f;
GLfloat Line_Orange_B = 0.2f;
GLfloat Line_White_R = 1.00f;
GLfloat Line_White_G = 1.00f;
GLfloat Line_White_B = 1.00f;
GLfloat Line_Green_R = 0.074f;
GLfloat Line_Green_G = 0.533f;
GLfloat Line_Green_B = 0.2f;

void DrawLetter(char);
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
    
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    Resize(giWindowWidth,giWindowHeight);
    ToggleFullScreen();
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

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    static GLint stage = 1;
	static GLfloat I_1_XPosition = -2.25f;
	static GLfloat A_XPosition = 2.25f;
	static GLfloat N_YPosition = 2.25f;
	static GLfloat I_2_YPosition = -2.25f;
	//static GLfloat Plane1_X_Position = -2.5f;
	static GLfloat Plane1_Y_Position = 0.00f;
	static GLfloat Plane1_Z_Rotation_Angle = -90.0f;

	//glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	if (stage > 0) {
		glLoadIdentity();
		glTranslatef(I_1_XPosition, 0.0f, -3.0f);//glTranslatef(-1.5f, 0.0f, -3.0f);
		DrawLetter('I');
	}

	if (stage > 2) {
		glLoadIdentity();
		glTranslatef(-1.0f, N_YPosition, -3.0f);//glTranslatef(-1.0f, 0.0f, -3.0f);
		DrawLetter('N');
	}

	if (stage > 4) {
		glLoadIdentity();
		glTranslatef(-0.25f, 0.0f, -3.0f);
		DrawLetter('D');
	}

	if (stage > 3) {
		glLoadIdentity();
		glTranslatef(0.25f, I_2_YPosition, -3.0f);//glTranslatef(0.25f, 0.0f, -3.0f);
		DrawLetter('I');
	}
	if (stage > 1) {
		glLoadIdentity();
		glTranslatef(A_XPosition, 0.0f, -3.0f);////glTranslatef(0.75f, 0.0f, -3.0f);
		DrawLetter('A');
	}


	if (stage == 8) {
		glLoadIdentity();
		Plane1_Y_Position = ((Plane1_X_Position - 1.3)*(Plane1_X_Position - 1.3) / (4 * 0.15)) - 0.0f;
		glTranslatef(Plane1_X_Position, Plane1_Y_Position, -3.0f);//glTranslatef(-1.0f, 0.0f, -3.0f);	
		glRotatef(Plane1_Z_Rotation_Angle, 0.0f, 0.0f, 1.0f);
		DrawLetter('P');

		glLoadIdentity();
		glTranslatef(Plane1_X_Position, 0.0f, -3.0f);//glTranslatef(-1.0f, 0.0f, -3.0f);	
		DrawLetter('P');

		glLoadIdentity();
		glTranslatef(Plane1_X_Position, -Plane1_Y_Position, -3.0f);//glTranslatef(-1.0f, 0.0f, -3.0f);	
		glRotatef(-Plane1_Z_Rotation_Angle, 0.0f, 0.0f, 1.0f);
		DrawLetter('P');

	}

	if (stage == 7) {
		glLoadIdentity();
		glTranslatef(Plane1_X_Position, 0.0f, -3.0f);//glTranslatef(-1.0f, 0.0f, -3.0f);
		DrawLetter('P');
	}

	if ((stage == 7 || stage == 8) && Plane1_X_Position > -1.02f && Plane1_X_Position < 1.40f) {
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -3.0f);//glTranslatef(-1.0f, 0.0f, -3.0f);
		DrawLetter('L');
	}

	if (stage > 7 && Plane1_X_Position > 1.40f) {
		glLoadIdentity();
		glTranslatef(A_XPosition, 0.0f, -3.0f);////glTranslatef(0.75f, 0.0f, -3.0f);
		DrawLetter('T');
	}

	if (stage > 7 && Plane1_X_Position > 1.40f && Line_X_Position <= 1.10f) {
		Line_X_Position = Line_X_Position + 0.006f;

		if (Line_White_R > 0.0f)
			Line_White_R = Line_White_R - Line_White_R / 256;
		if (Line_White_G > 0.0f)
			Line_White_G = Line_White_G - Line_White_G / 256;
		if (Line_White_B > 0.0f)
			Line_White_B = Line_White_B - Line_White_B / 256;

		if (Line_Orange_R > 0.0f)
			Line_Orange_R = Line_Orange_R - Line_Orange_R / 256;
		if (Line_Orange_G > 0.0f)
			Line_Orange_G = Line_Orange_G - Line_Orange_G / 256;
		if (Line_Orange_B > 0.0f)
			Line_Orange_B = Line_Orange_B - Line_Orange_B / 256;

		if (Line_Green_R > 0.0f)
			Line_Green_R = Line_Green_R - Line_Green_R / 256;
		if (Line_Green_G > 0.0f)
			Line_Green_G = Line_Green_G - Line_Green_G / 256;
		if (Line_Green_B > 0.0f)
			Line_Green_B = Line_Green_B - Line_Green_B / 256;


		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -3.0f);//glTranslatef(-1.0f, 0.0f, -3.0f);
		DrawLetter('L');
	}

	if (stage > 7 && Plane1_X_Position > 1.40f) {
		glLoadIdentity();
		glTranslatef(A_XPosition, 0.0f, -3.0f);////glTranslatef(0.75f, 0.0f, -3.0f);
		DrawLetter('T');
	}


	if (stage == 6)
	{
		Plane1_Y_Position = ((Plane1_X_Position + 1.3)*(Plane1_X_Position + 1.3) / (4 * 0.15)) - 0.0f;
		glLoadIdentity();
		glTranslatef(Plane1_X_Position, Plane1_Y_Position, -3.0f);//glTranslatef(-1.0f, 0.0f, -3.0f);
		glRotatef(Plane1_Z_Rotation_Angle, 0.0f, 0.0f, 1.0f);
		DrawLetter('P');

		glLoadIdentity();
		glTranslatef(Plane1_X_Position, 0.0f, -3.0f);//glTranslatef(-1.0f, 0.0f, -3.0f);
		DrawLetter('P');

		glLoadIdentity();
		glTranslatef(Plane1_X_Position, -1 * (Plane1_Y_Position), -3.0f);//glTranslatef(-1.0f, 0.0f, -3.0f);
		glRotatef(-Plane1_Z_Rotation_Angle, 0.0f, 0.0f, 1.0f);

		DrawLetter('P');
	}

	glXSwapBuffers(gpDisplay,gWindow);

	switch (stage) {
	case 1:
		//Print I
		if (I_1_XPosition < -1.5f)
			//I_1_XPosition = I_1_XPosition + 0.00022f;
			I_1_XPosition = I_1_XPosition + 0.005f;
		else {
			I_1_XPosition = -1.5f;
			stage++;
		}
		break;
	case 2:
		//Print A
		if (A_XPosition > 0.75f)
			//A_XPosition = A_XPosition - 0.00022f;
			A_XPosition = A_XPosition - 0.005f;
		else {
			A_XPosition = 0.75f;
			stage++;
		}
		break;
	case 3:
		//Print N
		if (N_YPosition > 0.00f)
			//N_YPosition = N_YPosition - 0.00022f;
			N_YPosition = N_YPosition - 0.005f;
		else {
			N_YPosition = 0.00f;
			stage++;
		}
		break;
	case 4:
		//Print I
		if (I_2_YPosition < 0.00f)
			//I_2_YPosition = I_2_YPosition + 0.00022f;
			I_2_YPosition = I_2_YPosition + 0.005f;
		else {
			I_2_YPosition = 0.00f;
			stage++;
		}
		break;
	case 5:
		//Print D
		if (D_R_OrangeColor < 1.0f)
			//D_R_OrangeColor = D_R_OrangeColor + 1.0f / 5050;
			D_R_OrangeColor = D_R_OrangeColor + 1.0f / 1000;
		if (D_G_OrangeColor < 0.6f)
			D_G_OrangeColor = D_G_OrangeColor + 0.6f / 1000;
		if (D_B_OrangeColor < 0.2f)
			D_B_OrangeColor = D_B_OrangeColor + 0.2f / 1000;

		if (D_R_GreenColor < 0.074f)
			D_R_GreenColor = D_R_GreenColor + 0.074f / 1000;
		if (D_G_GreenColor < 0.533f)
			D_G_GreenColor = D_G_GreenColor + 0.533f / 1000;
		if (D_B_GreenColor < 0.2f)
			D_B_GreenColor = D_B_GreenColor + 0.2f / 1000;

		if (D_R_OrangeColor >= 1.0f && D_G_OrangeColor >= 0.6f && D_B_OrangeColor >= 0.2f && D_R_GreenColor >= 0.074f && D_G_GreenColor >= 0.533f && D_B_GreenColor >= 0.2f)
			stage++;
		break;
	case 6:
		if (Plane1_X_Position < -1.3f) {
			//Plane1_X_Position = Plane1_X_Position + 0.0008f;
			Plane1_X_Position = Plane1_X_Position + 0.001f;
			//Plane1_Z_Rotation_Angle = Plane1_Z_Rotation_Angle + 90.0f / (1.2 / 0.0008);//0.015f
			Plane1_Z_Rotation_Angle = Plane1_Z_Rotation_Angle + 90.0f / (1.2 / 0.001);//0.015f
		}
		else
			stage++;
		break;
	case 7:
		if (Plane1_X_Position < 1.3f)
			//Plane1_X_Position = Plane1_X_Position + 0.0008f;
			Plane1_X_Position = Plane1_X_Position + 0.001f;
		else
			stage++;
		break;
	case 8:
		if (Plane1_X_Position < 2.5f)
		{
			//Plane1_X_Position = Plane1_X_Position + 0.0008f;
			//Plane1_Z_Rotation_Angle = Plane1_Z_Rotation_Angle + 90.0f / (1.3 / 0.0008);
            Plane1_X_Position = Plane1_X_Position + 0.001f;
			Plane1_Z_Rotation_Angle = Plane1_Z_Rotation_Angle + 90.0f / (1.3 / 0.001);
		}
		else
			stage++;
		break;
	}



}
void Update(void)
{

}


void DrawLetter(char letter) {
	switch (letter)
	{
	case 'I':

		glBegin(GL_QUADS);

		glColor3f(1.0f, 0.6f, 0.2f);
		glVertex3f(0.275f, 1.0f, 0.0f);
		glVertex3f(0.225f, 1.0f, 0.0f);

		glColor3f(0.074f, 0.533f, 0.2f);
		glVertex3f(0.225f, -1.0f, 0.0f);
		glVertex3f(0.275f, -1.0f, 0.0f);


		glEnd();
		break;

	case 'N':
		glBegin(GL_QUADS);

		glColor3f(1.0f, 0.6f, 0.2f);
		glVertex3f(0.05f, 1.0f, 0.0f);
		glVertex3f(0.0f, 1.0f, 0.0f);

		glColor3f(0.074f, 0.533f, 0.2f);
		glVertex3f(0.00f, -1.0f, 0.0f);
		glVertex3f(0.05f, -1.0f, 0.0f);

		glColor3f(1.0f, 0.6f, 0.2f);
		glVertex3f(0.05f, 1.0f, 0.0f);
		glVertex3f(0.00f, 1.0f, 0.0f);

		glColor3f(0.074f, 0.533f, 0.2f);
		glVertex3f(0.45f, -1.0f, 0.0f);
		glVertex3f(0.50f, -1.0f, 0.0f);

		glColor3f(1.0f, 0.6f, 0.2f);
		glVertex3f(0.50f, 1.0f, 0.0f);
		glVertex3f(0.45f, 1.0f, 0.0f);

		glColor3f(0.074f, 0.533f, 0.2f);
		glVertex3f(0.45f, -1.0f, 0.0f);
		glVertex3f(0.50f, -1.0f, 0.0f);


		glEnd();

		break;

	case 'D':

		glBegin(GL_QUADS);
		glColor3f(D_R_OrangeColor, D_G_OrangeColor, D_B_OrangeColor);
		glVertex3f(0.05f, 1.0f, 0.0f);
		glVertex3f(0.0f, 1.0f, 0.0f);

		glColor3f(D_R_GreenColor, D_G_GreenColor, D_B_GreenColor);
		glVertex3f(0.00f, -1.0f, 0.0f);
		glVertex3f(0.05f, -1.0f, 0.0f);


		glColor3f(D_R_OrangeColor, D_G_OrangeColor, D_B_OrangeColor);
		glVertex3f(0.5f, 1.0f, 0.0f);
		glVertex3f(-0.01f, 1.0f, 0.0f);

		glColor3f(D_R_OrangeColor, D_G_OrangeColor, D_B_OrangeColor);
		glVertex3f(-0.01f, 0.95f, 0.0f);
		glVertex3f(0.5f, 0.95f, 0.0f);


		glColor3f(D_R_OrangeColor, D_G_OrangeColor, D_B_OrangeColor);
		glVertex3f(0.50f, 1.0f, 0.0f);
		glVertex3f(0.45f, 1.0f, 0.0f);

		glColor3f(D_R_GreenColor, D_G_GreenColor, D_B_GreenColor);
		glVertex3f(0.45f, -1.0f, 0.0f);
		glVertex3f(0.50f, -1.0f, 0.0f);

		glColor3f(D_R_GreenColor, D_G_GreenColor, D_B_GreenColor);
		glVertex3f(0.50f, -0.95f, 0.0f);
		glVertex3f(-0.01f, -0.95f, 0.0f);

		glColor3f(D_R_GreenColor, D_G_GreenColor, D_B_GreenColor);
		glVertex3f(-0.01f, -1.0f, 0.0f);
		glVertex3f(0.50f, -1.0f, 0.0f);
		/*
		glColor3f(1.0f, 0.6f, 0.2f);
		glVertex3f(0.05f, 1.0f, 0.0f);
		glVertex3f(0.0f, 1.0f, 0.0f);

		glColor3f(0.074f, 0.533f, 0.2f);
		glVertex3f(0.00f, -1.0f, 0.0f);
		glVertex3f(0.05f, -1.0f, 0.0f);


		glColor3f(1.0f, 0.6f, 0.2f);
		glVertex3f(0.5f, 1.0f, 0.0f);
		glVertex3f(-0.01f, 1.0f, 0.0f);

		glColor3f(1.0f, 0.6f, 0.2f);
		glVertex3f(-0.01f, 0.95f, 0.0f);
		glVertex3f(0.5f, 0.95f, 0.0f);


		glColor3f(1.0f, 0.6f, 0.2f);
		glVertex3f(0.50f, 1.0f, 0.0f);
		glVertex3f(0.45f, 1.0f, 0.0f);

		glColor3f(0.074f, 0.533f, 0.2f);
		glVertex3f(0.45f, -1.0f, 0.0f);
		glVertex3f(0.50f, -1.0f, 0.0f);

		glColor3f(0.074f, 0.533f, 0.2f);
		glVertex3f(0.50f, -0.95f, 0.0f);
		glVertex3f(-0.01f, -0.95f, 0.0f);

		glColor3f(0.074f, 0.533f, 0.2f);
		glVertex3f(-0.01f, -1.0f, 0.0f);
		glVertex3f(0.50f, -1.0f, 0.0f);
		*/
		glEnd();

		break;
	case 'A':

		glBegin(GL_QUADS);

		glColor3f(1.0f, 0.6f, 0.2f);
		glVertex3f(0.275f, 1.0f, 0.0f);
		glVertex3f(0.225f, 1.0f, 0.0f);

		glColor3f(0.074f, 0.533f, 0.2f);
		glVertex3f(0.00f, -1.0f, 0.0f);
		glVertex3f(0.05f, -1.0f, 0.0f);

		glColor3f(1.0f, 0.6f, 0.2f);
		glVertex3f(0.275f, 1.0f, 0.0f);
		glVertex3f(0.225f, 1.0f, 0.0f);

		glColor3f(0.074f, 0.533f, 0.2f);
		glVertex3f(0.45f, -1.0f, 0.0f);
		glVertex3f(0.50f, -1.0f, 0.0f);


		/*glColor3f(1.0f, 0.6f, 0.2f);
		glVertex3f(0.375f, 0.075f, 0.0f);
		glVertex3f(0.125f, 0.075f, 0.0f);
		glVertex3f(0.125f, 0.025f, 0.0f);
		glVertex3f(0.375f, 0.025f, 0.0f);

		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.375f, 0.025f, 0.0f);
		glVertex3f(0.125f, 0.025f, 0.0f);
		glVertex3f(0.125f, -0.025f, 0.0f);
		glVertex3f(0.375f, -0.025f, 0.0f);

		glColor3f(0.074f, 0.533f, 0.2f);
		glVertex3f(0.375f, -0.025f, 0.0f);
		glVertex3f(0.125f, -0.025f, 0.0f);

		glVertex3f(0.125f, -0.075f, 0.0f);
		glVertex3f(0.375f, -0.075f, 0.0f);
*/

		glEnd();
		break;
	case 'P':

		glBegin(GL_QUADS);

		//Smoke R
		glColor3f(1.0f, 0.6f, 0.2f);
		glVertex3f(-0.24f, 0.06f, 0.0f);
		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3f(-0.40f, 0.06f, 0.0f);
		glVertex3f(-0.40f, 0.02f, 0.0f);
		glColor3f(1.0f, 0.6f, 0.2f);
		glVertex3f(-0.24f, 0.02f, 0.0f);

		//Smoke W
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(-0.24f, 0.02f, 0.0f);
		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3f(-0.40f, 0.02f, 0.0f);
		glVertex3f(-0.40f, -0.02f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(-0.24f, -0.02f, 0.0f);

		//Smoke G
		glColor3f(0.074f, 0.533f, 0.2f);
		glVertex3f(-0.24f, -0.02f, 0.0f);
		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3f(-0.40f, -0.02f, 0.0f);
		glVertex3f(-0.40f, -0.06f, 0.0f);
		glColor3f(0.074f, 0.533f, 0.2f);
		glVertex3f(-0.24f, -0.06f, 0.0f);


		glColor3f(0.729f, 0.886f, 0.933f);

		glVertex3f(0.20f, 0.07f, 0.0f);
		glVertex3f(-0.20f, 0.07f, 0.0f);
		glVertex3f(-0.20f, -0.07f, 0.0f);
		glVertex3f(0.20f, -0.07f, 0.0f);

		//Back Wings
		glVertex3f(-0.10f, 0.00f, 0.0f);
		glVertex3f(-0.27f, 0.12f, 0.0f);
		glVertex3f(-0.31f, 0.12f, 0.0f);
		glVertex3f(-0.24f, 0.00f, 0.0f);

		glVertex3f(-0.10f, 0.00f, 0.0f);
		glVertex3f(-0.27f, -0.12f, 0.0f);
		glVertex3f(-0.31f, -0.12f, 0.0f);
		glVertex3f(-0.24f, 0.00f, 0.0f);

		glEnd();

		glBegin(GL_TRIANGLES);
		/*glVertex3f(0.20f, 0.07f, 0.0f);
		glVertex3f(0.35f, 0.00f, 0.0f);
		glVertex3f(0.20f, -0.07f, 0.0f);
*/

//Upper Wing
		glVertex3f(-0.05f, 0.07f, 0.0f);
		glVertex3f(0.05f, 0.07f, 0.0f);
		glVertex3f(-0.15f, 0.22f, 0.0f);

		glVertex3f(-0.05f, -0.07f, 0.0f);
		glVertex3f(0.05f, -0.07f, 0.0f);
		glVertex3f(-0.15f, -0.22f, 0.0f);
		//Lower Wing
		glVertex3f(0.05f, 0.07f, 0.0f);
		glVertex3f(-0.10f, 0.22f, 0.0f);
		glVertex3f(-0.15f, 0.22f, 0.0f);

		glVertex3f(0.05f, -0.07f, 0.0f);
		glVertex3f(-0.10f, -0.22f, 0.0f);
		glVertex3f(-0.15f, -0.22f, 0.0f);

		glEnd();


		//Front Circle

		glBegin(GL_POINTS);

		//glColor3f(1.0f, 1.0f, 1.0f);

		for (float radius = 0.0f; radius <= 0.07f; radius = radius + 0.001f) {
			for (angle = 0.00f; angle < 2 * 3.14159265; angle = angle + 0.01f) {
				glVertex3f(cos(angle)*radius + 0.21f, sin(angle)*radius, 0.0f);
			}
		}
		glEnd();

		glBegin(GL_QUADS);

		//Letter I

		glColor3f(1.0f, 0.6f, 0.2f);
		glVertex3f(-0.05f, 0.05f, 0.0f);
		glVertex3f(-0.07f, 0.05f, 0.0f);
		glVertex3f(-0.07f, -0.05f, 0.0f);
		glVertex3f(-0.05f, -0.05f, 0.0f);

		//Letter A
		glColor3f(0.0f, 0.0f, 0.530f);

		glVertex3f(0.00f, 0.05f, 0.0f);
		glVertex3f(-0.02f, 0.05f, 0.0f);
		glVertex3f(-0.04f, -0.05f, 0.0f);
		glVertex3f(-0.02f, -0.05f, 0.0f);

		glVertex3f(0.00f, 0.05f, 0.0f);
		glVertex3f(0.02f, 0.05f, 0.0f);
		glVertex3f(0.04f, -0.05f, 0.0f);
		glVertex3f(0.02f, -0.05f, 0.0f);

		glVertex3f(0.02f, 0.01f, 0.0f);
		glVertex3f(-0.02f, 0.01f, 0.0f);
		glVertex3f(-0.02f, -0.01f, 0.0f);
		glVertex3f(0.02f, -0.01f, 0.0f);


		//Letter F
		glColor3f(0.074f, 0.533f, 0.2f);

		glVertex3f(0.06f, 0.05f, 0.0f);
		glVertex3f(0.04f, 0.05f, 0.0f);
		glVertex3f(0.04f, -0.05f, 0.0f);
		glVertex3f(0.06f, -0.05f, 0.0f);

		glVertex3f(0.09f, 0.05f, 0.0f);
		glVertex3f(0.06f, 0.05f, 0.0f);
		glVertex3f(0.06f, 0.035f, 0.0f);
		glVertex3f(0.09f, 0.035f, 0.0f);

		glVertex3f(0.09f, 0.020f, 0.0f);
		glVertex3f(0.06f, 0.020f, 0.0f);
		glVertex3f(0.06f, 0.005f, 0.0f);
		glVertex3f(0.09f, 0.005f, 0.0f);

		glEnd();


		break;
	case 'L':
		glBegin(GL_QUADS);
		if (Plane1_X_Position < 1.40) {
			glColor3f(1.0f, 0.6f, 0.2f);
			glVertex3f(Plane1_X_Position - 0.27f, 0.06f, 0.0f);
			glVertex3f(Line_X_Position, 0.06f, 0.0f);
			glVertex3f(Line_X_Position, 0.02f, 0.0f);
			glVertex3f(Plane1_X_Position - 0.27f, 0.02f, 0.0f);

			// W
			glColor3f(1.0f, 1.0f, 1.0f);
			glVertex3f(Plane1_X_Position - 0.27f, 0.02f, 0.0f);
			glVertex3f(Line_X_Position, 0.02f, 0.0f);
			glVertex3f(Line_X_Position, -0.02f, 0.0f);
			glVertex3f(Plane1_X_Position - 0.27f, -0.02f, 0.0f);

			// G
			glColor3f(0.074f, 0.533f, 0.2f);
			glVertex3f(Plane1_X_Position - 0.27f, -0.02f, 0.0f);
			glVertex3f(Line_X_Position, -0.02f, 0.0f);
			glVertex3f(Line_X_Position, -0.06f, 0.0f);
			glVertex3f(Plane1_X_Position - 0.27f, -0.06f, 0.0f);
		}
		else {
			glColor3f(Line_Orange_R, Line_Orange_G, Line_Orange_B);
			glVertex3f(1.1f, 0.06f, 0.0f);
			glVertex3f(Line_X_Position, 0.06f, 0.0f);
			glVertex3f(Line_X_Position, 0.02f, 0.0f);
			glVertex3f(1.1f, 0.02f, 0.0f);

			// W
			glColor3f(Line_White_R, Line_White_G, Line_White_B);
			glVertex3f(1.1f, 0.02f, 0.0f);
			glVertex3f(Line_X_Position, 0.02f, 0.0f);
			glVertex3f(Line_X_Position, -0.02f, 0.0f);
			glVertex3f(1.1f, -0.02f, 0.0f);

			// G
			glColor3f(Line_Green_R, Line_Green_G, Line_Green_B);
			glVertex3f(1.1f, -0.02f, 0.0f);
			glVertex3f(Line_X_Position, -0.02f, 0.0f);
			glVertex3f(Line_X_Position, -0.06f, 0.0f);
			glVertex3f(1.1f, -0.06f, 0.0f);

		}

		glEnd();

		break;
	case 'T'://Tricolor of A
		glBegin(GL_QUADS);

		//R
		glColor3f(1.0f, 0.6f, 0.2f);
		glVertex3f(0.380f, 0.06f, 0.0f);
		glVertex3f(0.12f, 0.06f, 0.0f);
		glVertex3f(0.12f, 0.02f, 0.0f);
		glVertex3f(0.380f, 0.02f, 0.0f);


		//W
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.380f, 0.02f, 0.0f);
		glVertex3f(0.12f, 0.02f, 0.0f);
		glVertex3f(0.12f, -0.02f, 0.0f);
		glVertex3f(0.380f, -0.02f, 0.0f);

		//G
		glColor3f(0.074f, 0.533f, 0.2f);
		glVertex3f(0.380f, -0.02f, 0.0f);
		glVertex3f(0.12f, -0.02f, 0.0f);
		glVertex3f(0.12f, -0.06f, 0.0f);
		glVertex3f(0.380f, -0.06f, 0.0f);

		glEnd();
		break;
	}
}
