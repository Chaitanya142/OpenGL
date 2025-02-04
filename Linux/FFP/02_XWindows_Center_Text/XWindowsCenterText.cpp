#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>
#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/XKBlib.h>
#include<X11/keysym.h>

using namespace std;
bool bFullScreen=false;
Display *gpDisplay=NULL;
XVisualInfo *gpXVisualInfo=NULL;
Colormap gColorMap;
Window gWindow;
int giWindowWidth=800;
int giWindowHeight=600;

//FILE *filePointer ; 

int main(void)
{
void CreateWindow(void);
void ToggleFullScreen(void);
void uninitialize();
//filePointer = fopen("ChaitanyaLog.c", "w+");
int winWidth=giWindowWidth;
int winHeight=giWindowHeight;
char keys[26];
static XFontStruct *pXFontStruct;
GC gc;
XGCValues gcValues;
XColor text_color;
char str[255]="Hellow World";
int strLength;
int strWidth,fontHeight;

CreateWindow();
XEvent event;
KeySym keysym;
while(1)
{
        XNextEvent(gpDisplay,&event);
        switch(event.type)
        {
                case MapNotify:
                pXFontStruct=XLoadQueryFont(gpDisplay,"fixed");
                break;
                case KeyPress:
                        keysym=XkbKeycodeToKeysym(gpDisplay,event.xkey.keycode,0,0);
                        switch(keysym)
                        {
                        case XK_Escape:
                                XFreeGC(gpDisplay,gc);
                                XUnloadFont(gpDisplay,pXFontStruct->fid);
                                uninitialize();
                                exit(0);
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
                break;
                case Expose:
                gc=XCreateGC(gpDisplay,gWindow,0,&gcValues);
                XSetFont(gpDisplay,gc,pXFontStruct->fid);
                XAllocNamedColor(gpDisplay,gColorMap,"green",&text_color,&text_color);
                XSetForeground(gpDisplay,gc,text_color.pixel);
                strLength=strlen(str);
                strWidth=XTextWidth(pXFontStruct,
                    str,
                    strLength);
                fontHeight=pXFontStruct->ascent+pXFontStruct->descent;
                XDrawString(gpDisplay,
                    gWindow,
                    gc,
                    (winWidth- strWidth)/2,
                    (winHeight- fontHeight)/2,
                    str,
                    strLength);  
                break;
                case DestroyNotify:
                break;
                case 33:
                        XFreeGC(gpDisplay,gc);
                        XUnloadFont(gpDisplay,pXFontStruct->fid);
                        uninitialize();
                        exit(0);
                default:
                break;
        }
}
//fclose(filePointer); 
uninitialize();
return(0);
}

void CreateWindow(void)
{
void uninitialize();
XSetWindowAttributes winAttribs;
int defaultScreen;
int defaultDepth;
int styleMask;
gpDisplay=XOpenDisplay(NULL);
if(gpDisplay == NULL){
        printf("ERROR : Unable To Open X Display.\nExitting Now...\n");
        uninitialize();
        exit(1);
}
defaultScreen=XDefaultScreen(gpDisplay);
defaultDepth=DefaultDepth(gpDisplay,defaultScreen);  
gpXVisualInfo=(XVisualInfo*)malloc(sizeof(XVisualInfo));
if(gpXVisualInfo==NULL)
{
        printf("ERROR : Unable To Allocate Memory For Visual Info.\nExitting Now...\n");
        uninitialize();
        exit(1);
}
XMatchVisualInfo(gpDisplay,defaultScreen,defaultDepth,TrueColor,gpXVisualInfo);
if(gpXVisualInfo==NULL){
        printf("ERROR : Unable To Get A Visual.\nExitting Now...\n");
        uninitialize();
        exit(1);
}
winAttribs.border_pixel=0;
winAttribs.background_pixmap=0;
winAttribs.colormap=XCreateColormap(gpDisplay,RootWindow(gpDisplay,gpXVisualInfo->screen),gpXVisualInfo->visual,AllocNone);
gColorMap=winAttribs.colormap;
winAttribs.background_pixel=BlackPixel(gpDisplay,defaultScreen);
winAttribs.event_mask=ExposureMask | VisibilityChangeMask | ButtonPressMask | KeyPressMask | PointerMotionMask | StructureNotifyMask;
winAttribs.border_pixmap=0;
styleMask= CWBorderPixel | CWBackPixel | CWEventMask | CWColormap;
gWindow=XCreateWindow(gpDisplay,RootWindow(gpDisplay,gpXVisualInfo->screen),0,0,giWindowWidth,giWindowHeight,0,gpXVisualInfo->depth,InputOutput,gpXVisualInfo->visual,styleMask,&winAttribs);
if(!gWindow)
{
        printf("ERROR : Failed To Create Main Window.\nExitting Now...\n");
        uninitialize();
        exit(1);
}
XStoreName(gpDisplay,gWindow,"First Window");
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

void uninitialize()
{
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
