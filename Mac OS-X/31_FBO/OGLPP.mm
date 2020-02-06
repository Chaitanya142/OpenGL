//Headers

#import<Foundation/Foundation.h>
#import<Cocoa/Cocoa.h>

#import<QuartzCore/CVDisplayLink.h>

#import<OpenGL/gl3.h>
#import<OpenGL/gl3ext.h>

#import "vmath.h"

enum {
    AMC_ATTRIBUTE_POSITION = 0,
    AMC_ATTRIBUTE_COLOR,
    AMC_ATTRIBUTE_NORMAL,
    AMC_ATTRIBUTE_TEXCOORD0
};


//'C' style global functn declarations

CVReturn MyDisplayLinkCallback(CVDisplayLinkRef,const CVTimeStamp *,
const CVTimeStamp *,CVOptionFlags,CVOptionFlags *,void *);

//global variables
FILE *gpFile=NULL;

//Interface Declaration
@interface AppDelegate : NSObject <NSApplicationDelegate , NSWindowDelegate>

@end

@interface GLView : NSOpenGLView

@end

//Entry point functn
int main(int argc,const char * argv[])
{
	NSAutoreleasePool *pPool =[[NSAutoreleasePool alloc]init];
	
	NSApp=[NSApplication sharedApplication];
	
	[NSApp setDelegate:[[AppDelegate alloc] init]];
	
	[NSApp run];
	
	[pPool release];
	
	return(0);

}


//Interface Implementations

@implementation AppDelegate
{
@private

	NSWindow *window;
	GLView *view;
}

-(void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
//code

//log file
NSBundle *mainBundle=[NSBundle mainBundle];
NSString *appDirName=[mainBundle bundlePath];
NSString *parentDirPath=[appDirName stringByDeletingLastPathComponent];
NSString *logFieNameWithPath=[NSString stringWithFormat:@"%@/Log.txt",parentDirPath];

const char *pszLogFileNameWithPath=[logFieNameWithPath cStringUsingEncoding:NSASCIIStringEncoding];

gpFile=fopen(pszLogFileNameWithPath,"w");

if(gpFile==NULL){
printf("Can not create Log File.\nExitting ...\n");
[self release];
[NSApp terminate:self];
}
    
fprintf(gpFile,"Program Started Successfully\n");

//window

NSRect win_rect;
win_rect=NSMakeRect(0.0,0.0,800.0,600.0);

//Create simple window
window=[[NSWindow alloc]
		initWithContentRect	:	win_rect
				styleMask	:	NSWindowStyleMaskTitled |
								NSWindowStyleMaskClosable |
								NSWindowStyleMaskMiniaturizable |
								NSWindowStyleMaskResizable
				backing		: 	NSBackingStoreBuffered
				defer		:	NO	];

[window setTitle:@"macOS OpenGL Window"];

[window center];

view = [[GLView alloc]initWithFrame:win_rect];

[window setContentView:view];
[window setDelegate:self];
[window makeKeyAndOrderFront:self];

}

-(void) applicationWillTerminate:(NSNotification *)notification
{
//code

fprintf(gpFile,"Program is Terminated successfully\n");
if(gpFile){
fclose(gpFile);
gpFile=NULL;
}

}

-(void) windowWillClose:(NSNotification *)notification
{
//code
[NSApp terminate:self];
}

-(void)dealloc
{
//code
[view release];

[window release];

[super dealloc];
}

@end


@implementation GLView
{
@private
	CVDisplayLinkRef displayLink;
    
    GLfloat anglePyramid;
    GLfloat angleCube;

    GLuint texture_Kundali;
    //Shader Program Objects
    GLint gShaderProgramObject;

  //Addition
  GLint gShaderProgramObject_FBO;
  GLuint mvpUniform_FBO;
  GLuint samplerUniform_FBO;
  GLuint vbo_texCoord_Cube;
  GLuint FBO;
  GLuint texture_FBO;

    int windowWidth;
    int windowHeight;

    GLuint vao_cube;

    //GLuint vbo_position_pyramid;
    GLuint vbo_position_cube;

    //GLuint vbo_color_pyramid;
    GLuint vbo_color_cube;

    GLuint depthRenderBuffer;
    
    GLuint mvpUniform;
    
    vmath::mat4 perspectiveProjectionMatrix;
    
}

-(id)initWithFrame:(NSRect)frame;
{
//code
self=[super initWithFrame:frame];

if(self)
{
[[self window]setContentView:self];

NSOpenGLPixelFormatAttribute attrs[]=
{
//Must specify the 4.1 core profile to use OpenGL4.1
NSOpenGLPFAOpenGLProfile,
NSOpenGLProfileVersion4_1Core,
//Specify the display ID to associate the GL context with (main dispaly for now)
NSOpenGLPFAScreenMask,
CGDisplayIDToOpenGLDisplayMask(kCGDirectMainDisplay),
NSOpenGLPFANoRecovery,
NSOpenGLPFAAccelerated,
NSOpenGLPFAColorSize,24,
NSOpenGLPFADepthSize,24,
NSOpenGLPFAAlphaSize,8,
NSOpenGLPFADoubleBuffer,
0//last 0 is must
};
NSOpenGLPixelFormat *pixelFormat=
[[[NSOpenGLPixelFormat alloc]initWithAttributes:attrs]autorelease];

if(pixelFormat==nil)
{
fprintf(gpFile,"No valid OpenGL pixel format is available.Exitting...");
    [self release];
[NSApp terminate:self];
}

NSOpenGLContext *glContext=[[[NSOpenGLContext alloc]initWithFormat:pixelFormat shareContext:nil]autorelease];

[self setPixelFormat:pixelFormat];

[self setOpenGLContext:glContext];
//it automatically releases the order context, if present , and sets the newer one

}
return (self);

}


-(CVReturn) getFrameForTime:(const CVTimeStamp *)pOutputTime
{
//code
NSAutoreleasePool *pool =[[NSAutoreleasePool alloc]init];

[self drawView];

[pool release];

return(kCVReturnSuccess);
}


-(void) prepareOpenGL
{
//code
//OpenGL info

fprintf(gpFile,"OpenGL version : %s\n",glGetString(GL_VERSION));
fprintf(gpFile,"GLSL Version : %s\n",glGetString(GL_SHADING_LANGUAGE_VERSION));

[[self openGLContext]makeCurrentContext];

GLint swapInt=1;
//PressedDigit=1;

[[self openGLContext]setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];


    
    //Addition
     GLint gVertexShaderObject_FBO;
     GLint gFragementShaderObject_FBO;
     
     gVertexShaderObject_FBO = glCreateShader(GL_VERTEX_SHADER);

     //Vertex Shader Code
     const GLchar* vertextShaderSourceCodeN =
         "#version 410 " \
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
     glShaderSource(gVertexShaderObject_FBO, 1, &vertextShaderSourceCodeN, NULL);

     //Compile Shader
     glCompileShader(gVertexShaderObject_FBO);

     //Error Checking
     GLint iShaderCompileStatusN = 0;
     GLint iInfoLogLengthN = 0;
     GLchar * szInfoLogN = NULL;

     glGetShaderiv(gVertexShaderObject_FBO, GL_COMPILE_STATUS, &iShaderCompileStatusN);

     if (iShaderCompileStatusN == GL_FALSE) {
         glGetShaderiv(gVertexShaderObject_FBO, GL_INFO_LOG_LENGTH, &iInfoLogLengthN);
         if (iInfoLogLengthN > 0) {
             szInfoLogN = (GLchar *)malloc(iInfoLogLengthN);
             if (szInfoLogN != NULL) {
                 GLsizei written;
                 glGetShaderInfoLog(gVertexShaderObject_FBO,
                     iInfoLogLengthN,
                     &written,
                     szInfoLogN);
                 fprintf(gpFile, "Vertex Shader Compilation Log %s\n", szInfoLogN);
                 free(szInfoLogN);
                 //UnInitialize();
             }
         }
     }

     //Fragement Shader
     //Define Shader Object
     gFragementShaderObject_FBO = glCreateShader(GL_FRAGMENT_SHADER);

     //Fragement Shader Code
     const GLchar* fragementShaderSourceCodeN =
          "#version 410 " \
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
     glShaderSource(gFragementShaderObject_FBO, 1, &fragementShaderSourceCodeN, NULL);

     //Compile Shader
     glCompileShader(gFragementShaderObject_FBO);

     //Error Checking
     iShaderCompileStatusN = 0;
     iInfoLogLengthN = 0;
     szInfoLogN = NULL;

     glGetShaderiv(gFragementShaderObject_FBO, GL_COMPILE_STATUS, &iShaderCompileStatusN);

     if (iShaderCompileStatusN == GL_FALSE) {
         glGetShaderiv(gFragementShaderObject_FBO, GL_INFO_LOG_LENGTH, &iInfoLogLengthN);
         if (iInfoLogLengthN > 0) {
             szInfoLogN = (GLchar *)malloc(iInfoLogLengthN);
             if (szInfoLogN != NULL) {
                 GLsizei written;
                 glGetShaderInfoLog(gFragementShaderObject_FBO,
                     iInfoLogLengthN,
                     &written,
                     szInfoLogN);
                 fprintf(gpFile, "Fragement Shader Compilation Log %s\n", szInfoLogN);
                 free(szInfoLogN);
                 //UnInitialize();

             }
         }
     }

     //Program Object
     gShaderProgramObject_FBO = glCreateProgram();
     //Attach Shaders
     glAttachShader(gShaderProgramObject_FBO, gVertexShaderObject_FBO);
     glAttachShader(gShaderProgramObject_FBO, gFragementShaderObject_FBO);

     //Prelinking
     //Binding to attribute
     glBindAttribLocation(gShaderProgramObject_FBO,
         AMC_ATTRIBUTE_POSITION,
         "vPosition");

     glBindAttribLocation(gShaderProgramObject_FBO,
         AMC_ATTRIBUTE_TEXCOORD0,
         "vTexCoord");
     /*glBindAttribLocation(gShaderProgramObject_FBO,
         AMC_ATTRIBUTE_COLOR,
         "vColor");*/

         //Link Shader Program
     glLinkProgram(gShaderProgramObject_FBO);

     //Error Checking
     GLint iShaderLinkStatusN = 0;

     glGetProgramiv(gShaderProgramObject_FBO, GL_LINK_STATUS, &iShaderLinkStatusN);

     if (iShaderLinkStatusN == GL_FALSE) {
         glGetProgramiv(gShaderProgramObject_FBO, GL_INFO_LOG_LENGTH, &iInfoLogLengthN);
         if (iInfoLogLengthN > 0) {
             szInfoLogN = (GLchar *)malloc(iInfoLogLengthN);
             if (szInfoLogN != NULL) {
                 GLsizei written;
                 glGetProgramInfoLog(gShaderProgramObject_FBO,
                     iInfoLogLengthN,
                     &written,
                     szInfoLogN);
                 fprintf(gpFile, "Program Link Log %s\n", szInfoLogN);
                 free(szInfoLogN);
                 //UnInitialize();

             }
         }
     }

     //Post Linking
     //Retriving Uniform Location
     mvpUniform_FBO = glGetUniformLocation(gShaderProgramObject_FBO,
         "u_mvp_matrix");

     samplerUniform_FBO = glGetUniformLocation(gShaderProgramObject_FBO,
         "u_sampler");





     //Shader Objects
     GLint gVertexShaderObject;
     GLint gFragementShaderObject;

     //Vertex Shader
     //Define Shader Object
     gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

     //Vertex Shader Code
     const GLchar* vertextShaderSourceCode =
          "#version 410 " \
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
                 //UnInitialize();

             }
         }
     }

     //Fragement Shader
     //Define Shader Object
     gFragementShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

     //Fragement Shader Code
     const GLchar* fragementShaderSourceCode =
          "#version 410 " \
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
                 //UnInitialize();

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
                 //UnInitialize();

             }
         }
     }

     //Post Linking
     //Retriving Uniform Location
     mvpUniform = glGetUniformLocation(gShaderProgramObject,
         "u_mvp_matrix");


     //Vertices
     //const GLfloat pyramidVertices[] = {
     //    0.0f, 1.0f, 0.0f,
     //    -1.0f, -1.0f, 1.0f,
     //    1.0f, -1.0f, 1.0f,
     //    0.0f, 1.0f, 0.0f,
     //    1.0f, -1.0f, 1.0f,
     //    1.0f, -1.0f, -1.0f,
     //    0.0f, 1.0f, 0.0f,
     //    1.0f, -1.0f, -1.0f,
     //    -1.0f, -1.0f, -1.0f,
     //    0.0f, 1.0f, 0.0f,
     //    -1.0f, -1.0f, -1.0f,
     //    -1.0f, -1.0f, 1.0f

     //    ,-1,-1,-1,
     //    -1,-1,1,
     //    1,-1,-1,

     //    1,-1,-1,
     //    1,-1,1,
     //    -1,-1,1

     //};

     const GLfloat cubeVertices[] = {
         1.0f, 1.0f, -1.0f,
         -1.0f, 1.0f, -1.0f,
         -1.0f, 1.0f, 1.0f,
         1.0f, 1.0f, 1.0f,
         1.0f, -1.0f, -1.0f,
         -1.0f, -1.0f, -1.0f,
         -1.0f, -1.0f, 1.0f,
         1.0f, -1.0f, 1.0f,
         1.0f, 1.0f, 1.0f,
         -1.0f, 1.0f, 1.0f,
         -1.0f, -1.0f, 1.0f,
         1.0f, -1.0f, 1.0f,
         1.0f, 1.0f, -1.0f,
         -1.0f, 1.0f, -1.0f,
         -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, 1.0f, -1.0f,
         1.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 1.0f,
         1.0f, -1.0f, -1.0f,
         -1.0f, 1.0f, -1.0f,
         -1.0f, 1.0f, 1.0f,
         -1.0f, -1.0f, 1.0f,
         -1.0f, -1.0f, -1.0f
     };

     //const GLfloat pyramidColors[] = {
     //    1.0f, 0.0f, 0.0f,
     //    0.0f, 1.0f, 0.0f,
     //    0.0f, 0.0f, 1.0f,
     //    1.0f, 0.0f, 0.0f,
     //    0.0f, 0.0f, 1.0f,
     //    0.0f, 1.0f, 0.0f,
     //    1.0f, 0.0f, 0.0f,
     //    0.0f, 1.0f, 0.0f,
     //    0.0f, 0.0f, 1.0f,
     //    1.0f, 0.0f, 0.0f,
     //    0.0f, 0.0f, 1.0f,
     //    0.0f, 1.0f, 0.0f

     //    ,1.0f,0.0f,0.0f,
     //    0.0f,1.0f,0.0f,
     //    0.0f,0.0f,1.0f,

     //    0.0f,0.0f,1.0f,
     //    1.0f,0.0f,0.0f,
     //    0.0f,1.0f,0.0f
     //};

     const GLfloat cubeColors[] = {
         1.0f, 0.0f, 0.0f,
         1.0f, 0.0f, 0.0f,
         1.0f, 0.0f, 0.0f,
         1.0f, 0.0f, 0.0f,
         0.0f, 1.0f, 0.0f,
         0.0f, 1.0f, 0.0f,
         0.0f, 1.0f, 0.0f,
         0.0f, 1.0f, 0.0f,
         0.0f, 0.0f, 1.0f,
         0.0f, 0.0f, 1.0f,
         0.0f, 0.0f, 1.0f,
         0.0f, 0.0f, 1.0f,
         0.0f, 1.0f, 1.0f,
         0.0f, 1.0f, 1.0f,
         0.0f, 1.0f, 1.0f,
         0.0f, 1.0f, 1.0f,
         1.0f, 1.0f, 0.0f,
         1.0f, 1.0f, 0.0f,
         1.0f, 1.0f, 0.0f,
         1.0f, 1.0f, 0.0f,
         1.0f, 0.0f, 1.0f,
         1.0f, 0.0f, 1.0f,
         1.0f, 0.0f, 1.0f,
         1.0f, 0.0f, 1.0f
     };

     const GLfloat cubeTexCoord[] =
     { 0.0f, 1.0f,
         0.0f, 0.0f,
         1.0f, 0.0f,
         1.0f, 1.0f,
         1.0f, 1.0f,
         0.0f, 1.0f,
         0.0f, 0.0f,
         1.0f, 0.0f,
         0.0f, 0.0f,
         1.0f, 0.0f,
         1.0f, 1.0f,
         0.0f, 1.0f,
         1.0f, 0.0f,
         1.0f, 1.0f,
         0.0f, 1.0f,
         0.0f, 0.0f,
         1.0f, 0.0f,
         1.0f, 1.0f,
         0.0f, 1.0f,
         0.0f, 0.0f,
         0.0f, 0.0f,
         1.0f, 0.0f,
         1.0f, 1.0f,
         0.0f, 1.0f };


     ////Create vao
     ////Save everying in single set
     //glGenVertexArrays(1, &vao_pyramid);

     //glBindVertexArray(vao_pyramid);


     ////TRIANGLE
     ////Generate Buffer
     //glGenBuffers(1, &vbo_position_pyramid);
     ////Bind Generated Buffer
     //glBindBuffer(GL_ARRAY_BUFFER,
     //    vbo_position_pyramid);
     ////Fill Buffer
     //glBufferData(GL_ARRAY_BUFFER,
     //    sizeof(pyramidVertices),
     //    pyramidVertices,
     //    GL_STATIC_DRAW);
     ////Set Vertex Attrib Pointer
     //glVertexAttribPointer(AMC_ATTRIBUTE_POSITION,
     //    3,
     //    GL_FLOAT,
     //    GL_FALSE,
     //    0,
     //    NULL);
     ////Enable Vertex Attrib Array
     //glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
     ////Unbind Buffer
     //glBindBuffer(GL_ARRAY_BUFFER, 0);

     //glGenBuffers(1, &vbo_color_pyramid);
     ////Bind Generated Buffer
     //glBindBuffer(GL_ARRAY_BUFFER,
     //    vbo_color_pyramid);
     ////Fill Buffer
     //glBufferData(GL_ARRAY_BUFFER,
     //    sizeof(pyramidColors),
     //    pyramidColors,
     //    GL_STATIC_DRAW);
     ////Set Vertex Attrib Pointer
     //glVertexAttribPointer(AMC_ATTRIBUTE_COLOR,
     //    3,
     //    GL_FLOAT,
     //    GL_FALSE,
     //    0,
     //    NULL);
     ////Enable Vertex Attrib Array
     //glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
     ////Unbind Buffer
     //glBindBuffer(GL_ARRAY_BUFFER, 0);

     ////Unbind array
     //glBindVertexArray(0);

     //RECTANGLE
     glGenVertexArrays(1, &vao_cube);

     glBindVertexArray(vao_cube);

     //Generate Buffer
     glGenBuffers(1, &vbo_position_cube);
     //Bind Generated Buffer
     glBindBuffer(GL_ARRAY_BUFFER,
         vbo_position_cube);
     //Fill Buffer
     glBufferData(GL_ARRAY_BUFFER,
         sizeof(cubeVertices),
         cubeVertices,
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

     //Generate Buffer
     glGenBuffers(1, &vbo_color_cube);
     //Bind Generated Buffer
     glBindBuffer(GL_ARRAY_BUFFER,
         vbo_color_cube);
     //Fill Buffer
     glBufferData(GL_ARRAY_BUFFER,
         sizeof(cubeColors),
         cubeColors,
         GL_STATIC_DRAW);
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


     //Generate Buffer
     glGenBuffers(1, &vbo_texCoord_Cube);
     //Bind Generated Buffer
     glBindBuffer(GL_ARRAY_BUFFER,
         vbo_texCoord_Cube);
     //Fill Buffer
     glBufferData(GL_ARRAY_BUFFER,
         sizeof(cubeTexCoord),
         cubeTexCoord,
         GL_STATIC_DRAW);
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

     //void createFrameBuffer(GLuint*);

     //LoadTexture(&texture_Kundali, MAKEINTRESOURCE(IDBITMAP_KUNDALI));
     //createFrameBuffer(&texture_Kundali);


     glGenFramebuffers(1,&FBO);
     glBindFramebuffer(GL_FRAMEBUFFER,FBO);

     glGenTextures(1, &texture_FBO);
     glBindTexture(GL_TEXTURE_2D, texture_FBO);

     glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 1024, 1024);

     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
     
     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_FBO, 0);

     glGenRenderbuffers(1, &depthRenderBuffer);
     glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);

     glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT16,1024,1024);
     glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);
     
     glBindTexture(GL_TEXTURE_2D, 0);

     glBindRenderbuffer(GL_RENDERBUFFER, 0);

     glBindFramebuffer(GL_FRAMEBUFFER, 0);

     if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
         fprintf(gpFile, "Done with framebuffer facility");
   
 

    glEnable(GL_TEXTURE_2D);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glClearDepth(1.0f);
    
    //set bg color
    glClearColor(0.0f,0.0f,0.0f,0.0f);

    texture_Kundali=[self loadTextureFromBMPFile:"Vijay_Kundali.bmp"];

    perspectiveProjectionMatrix = vmath::mat4::identity();


CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);

CVDisplayLinkSetOutputCallback(displayLink,&MyDisplayLinkCallback,self);

CGLContextObj cglContext=(CGLContextObj)[[self openGLContext]CGLContextObj];

CGLPixelFormatObj cglPixelFormat=(CGLPixelFormatObj)[[self pixelFormat]CGLPixelFormatObj];

CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink,cglContext,cglPixelFormat);

CVDisplayLinkStart(displayLink);

}


-(GLuint) loadTextureFromBMPFile:(const char *)texFileName
{
    NSBundle *mainBundle=[NSBundle mainBundle];
    NSString *appDirName=[mainBundle bundlePath];
    NSString *parentDirPath=[appDirName stringByDeletingLastPathComponent];
    NSString *texFileNameWithPath=[NSString stringWithFormat:@"%@/%s",parentDirPath,texFileName];

    NSImage *bmpImage=[[NSImage alloc]initWithContentsOfFile:texFileNameWithPath];

    if(!bmpImage){
        NSLog(@"can't find %@",texFileNameWithPath);
        return(0);
    }

    CGImageRef cgImage=[bmpImage CGImageForProposedRect:nil context:nil hints:nil];

    int w=(int)CGImageGetWidth(cgImage);
    int h=(int)CGImageGetHeight(cgImage);


    CFDataRef imageData=CGDataProviderCopyData(CGImageGetDataProvider(cgImage));

    void* pixels=(void *) CFDataGetBytePtr(imageData);

    GLuint bmpTexture;

    glGenTextures(1,&bmpTexture);

    glPixelStorei(GL_UNPACK_ALIGNMENT,1);

    glBindTexture(GL_TEXTURE_2D,bmpTexture);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);

    glTexImage2D(GL_TEXTURE_2D,
                    0,
                    GL_RGBA,
                    w,
                    h,
                    0,
                    GL_RGBA,
                    GL_UNSIGNED_BYTE,
                    pixels);

    glGenerateMipmap(GL_TEXTURE_2D);

    CFRelease(imageData);

    return(bmpTexture);
}


-(void) reshape
{
//code
CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);

NSRect rect=[self bounds];

GLfloat width=rect.size.width;
GLfloat height=rect.size.height;

if(height==0)
	height=1;
	
glViewport(0,0,(GLsizei)width,(GLsizei)height);


    perspectiveProjectionMatrix = vmath::perspective(45.0f,
            (GLfloat)width / (GLfloat)height,
            0.1f,
            100.0f);
    
CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);

}


-(void)drawRect:(NSRect)dirtyRect
{
//code
[self drawView];
}

-(void)drawView
{
//code
[[self openGLContext]makeCurrentContext];


CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);

//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    
    
    
    
    glClearColor(0.0, 0.0, 0.0, 0.0);
          glClearDepth(1.0f);
      
      glUseProgram(gShaderProgramObject_FBO);

     vmath::mat4 modelViewMatrix = vmath::mat4::identity();
       vmath::mat4 modelViewProjectionMatrix = vmath::mat4::identity();
       vmath::mat4 translationMatrix = vmath::mat4::identity();
       vmath::mat4 rotationMatrix = vmath::mat4::identity();
       vmath::mat4 scaleMatrix =vmath::mat4::identity();
       

      glBindFramebuffer(GL_FRAMEBUFFER, FBO);

      glViewport(0, 0, 1024, 1024);
      glClearColor(0.5, 0.5, 0.5, 0.0);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      translationMatrix = vmath::translate(0.0f, 0.0f, -5.0f);
      //scaleMatrix = scale(0.75f, 0.75f, 0.75f);
      rotationMatrix = vmath::rotate(anglePyramid, 0.0f, 1.0f, 0.0f);

      //Matrix Multiplication
      modelViewMatrix = translationMatrix * rotationMatrix;

      modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

      glBindTexture(GL_TEXTURE_2D, texture_Kundali);

      //Send necessary matrices to shader in resp. Uniforms

      glUniformMatrix4fv(mvpUniform,//Changed
          1,
          GL_FALSE,
          modelViewProjectionMatrix);

      glUniform1i(samplerUniform_FBO, 0);

      //Bind with vao

      glBindVertexArray(vao_cube);

      //Bind with textures if any

      //Draw

      glDrawArrays(GL_TRIANGLE_FAN,
          0,
          4);
      glDrawArrays(GL_TRIANGLE_FAN,
          4,
          4);
      glDrawArrays(GL_TRIANGLE_FAN,
          8,
          4);
      glDrawArrays(GL_TRIANGLE_FAN,
          12,
          4);
      glDrawArrays(GL_TRIANGLE_FAN,
          16,
          4);
      glDrawArrays(GL_TRIANGLE_FAN,
          20,
          4);

      //Unbind vao

      glBindVertexArray(0);

      //glBindRenderbuffer(GL_RENDERBUFFER, 0);

      glBindFramebuffer(GL_FRAMEBUFFER, 0);//Test

      glUseProgram(0);
      
      glClearColor(0.0, 0.0, 0.0, 0.0);
      glClearDepth(1.0f);

      glViewport(0, 0, (GLsizei)windowWidth, (GLsizei)windowHeight);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glUseProgram(gShaderProgramObject_FBO);
      //CUBE
      //Initialize matrices

      modelViewMatrix = vmath::mat4::identity();
      modelViewProjectionMatrix = vmath::mat4::identity();
      translationMatrix = vmath::mat4::identity();
      rotationMatrix = vmath::mat4::identity();
      scaleMatrix = vmath::mat4::identity();

      //Transformation

      translationMatrix = vmath::translate(0.0f, 0.0f, -5.0f);
      //scaleMatrix = scale(0.75f, 0.75f, 0.75f);
      rotationMatrix = vmath::rotate(angleCube, angleCube, angleCube);

      //Matrix Multiplication

      modelViewMatrix = translationMatrix * scaleMatrix * rotationMatrix;

      modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

      //Send necessary matrices to shader in resp. Uniforms

      glUniformMatrix4fv(mvpUniform,
          1,
          GL_FALSE,
          modelViewProjectionMatrix);

      glUniform1i(samplerUniform_FBO, 0);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, texture_FBO);
      //Bind with vao

      glBindVertexArray(vao_cube);

      //Bind with textures if any

      //Draw
      //6Faces
      glDrawArrays(GL_TRIANGLE_FAN,
          0,
          4);
      glDrawArrays(GL_TRIANGLE_FAN,
          4,
          4);
      glDrawArrays(GL_TRIANGLE_FAN,
          8,
          4);
      glDrawArrays(GL_TRIANGLE_FAN,
          12,
          4);
      glDrawArrays(GL_TRIANGLE_FAN,
          16,
          4);
      glDrawArrays(GL_TRIANGLE_FAN,
          20,
          4);
      //Unbind vao

      glBindVertexArray(0);

      glBindTexture(GL_TEXTURE_2D, 0);

      glUseProgram(0);
    
    
    /*
    
    glUseProgram(gShaderProgramObject);
    
   
    //Initialize matrices

    vmath::mat4 modelViewMatrix = vmath::mat4::identity();
    vmath::mat4 modelViewProjectionMatrix = vmath::mat4::identity();
    vmath::mat4 translationMatrix = vmath::mat4::identity();
    vmath::mat4 rotationMatrix = vmath::mat4::identity();
    vmath::mat4 scaleMatrix =vmath::mat4::identity();
    
    //Transformation

    translationMatrix = vmath::translate(0.0f, 0.0f, -6.0f);
    scaleMatrix=vmath::scale(1.0f,-1.0f,1.0f);
    //rotationMatrix = rotate(0.0f, 90.0f, 0.0f);

    //Matrix Multiplication
    modelViewMatrix = translationMatrix * rotationMatrix * scaleMatrix;

    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

    //Texture

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D,texture_smiley);

    //Send necessary matrices to shader in resp. Uniforms

    glUniformMatrix4fv(mvpUniform,
        1,
        GL_FALSE,
        modelViewProjectionMatrix);

    glUniform1i(samplerUniform,0);

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


    //Bind with vao

    glBindVertexArray(vao_rectangle);

    //Bind with textures if any
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

    glUseProgram(0);*/
    
    
CGLFlushDrawable((CGLContextObj)[[self openGLContext]CGLContextObj]);
CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);

    
    anglePyramid = anglePyramid + 0.01f;
    if (anglePyramid > 360.0f)
        anglePyramid = 0.0f;
    angleCube = angleCube + 0.01f;
    if (angleCube > 360.0f)
        angleCube = 0.0f;
    
}


-(BOOL) acceptsFirstResponder
{
//code
[[self window]makeFirstResponder:self];
return(YES);
}



-(void)keyDown:(NSEvent *)theEvent
{
//code
int key=(int)[[theEvent characters]characterAtIndex:0];

switch(key)
{
case 27:
		[self release];
		[NSApp terminate:self];
		break;
case 'F':
case 'f':
	
	[[self window]toggleFullScreen:self];//repainting occurs automatically
	break;
default:
	break;
}

}

-(void) mouseDown:(NSEvent *)theEvent
{
//code

}

-(void)mouseDragged:(NSEvent *)theEvent
{
//code
}

-(void)rightMouseDown:(NSEvent *)theEvent
{
//code
}

-(void) dealloc
{
//code
    
    if (vbo_texCoord_Cube) {
        glDeleteBuffers(1, &vbo_texCoord_Cube);
        vbo_texCoord_Cube = 0;
    }


    if (vbo_position_cube) {
        glDeleteBuffers(1, &vbo_position_cube);
        vbo_position_cube = 0;
    }
    if (vao_cube) {
        glDeleteVertexArrays(1, &vao_cube);
        vao_cube = 0;
    }


    if (texture_FBO) {
        glDeleteTextures(1, &texture_FBO);
        texture_FBO = 0;
    }

    if (FBO) {
        glDeleteFramebuffers(1, &FBO);
        FBO = 0;
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

    
    
CVDisplayLinkStop(displayLink);
CVDisplayLinkRelease(displayLink);

[super dealloc];
}

@end


CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink,
const CVTimeStamp *pNow,
const CVTimeStamp *pOutputTime,
CVOptionFlags flagsIn,
CVOptionFlags *pFlagsOut,
void *pDisplayLinkContext
)
{
CVReturn result=[(GLView *)pDisplayLinkContext getFrameForTime:pOutputTime];
return(result);
}

