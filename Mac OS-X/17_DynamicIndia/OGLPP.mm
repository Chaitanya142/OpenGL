//Headers

#import<Foundation/Foundation.h>
#import<Cocoa/Cocoa.h>

#import<QuartzCore/CVDisplayLink.h>

#import<OpenGL/gl3.h>
#import<OpenGL/gl3ext.h>

#import "vmath.h"
#import "Sphere.h"

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
    fprintf(gpFile,pszLogFileNameWithPath);
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
    
    
    GLuint vao_quad;
    GLuint vao_triangle;
    GLuint vao_point;

    GLuint vbo_position_quad;
    GLuint vbo_position_triangle;
    GLuint vbo_position_point;

    GLuint vbo_color_quad;
    GLuint vbo_color_triangle;
    GLuint vbo_color_point;

    GLuint mvpUniform;
    
    GLint gVertexShaderObject;
    GLint gFragementShaderObject;
    GLint gShaderProgramObject;
    
    vmath::mat4 perspectiveProjectionMatrix;

    GLfloat D_R_OrangeColor ;
    GLfloat D_G_OrangeColor ;
    GLfloat D_B_OrangeColor ;
    GLfloat D_R_GreenColor ;
    GLfloat D_G_GreenColor ;
    GLfloat D_B_GreenColor ;

    GLfloat Line_Orange_R;
    GLfloat Line_Orange_G ;
    GLfloat Line_Orange_B ;
    GLfloat Line_White_R;
    GLfloat Line_White_G ;
    GLfloat Line_White_B ;
    GLfloat Line_Green_R ;
    GLfloat Line_Green_G ;
    GLfloat Line_Green_B ;

    GLint stage ;
    GLfloat I_1_XPosition ;
    GLfloat A_XPosition ;
    GLfloat N_YPosition ;
    GLfloat I_2_YPosition ;
    GLfloat Plane1_Y_Position;
    GLfloat Plane1_Z_Rotation_Angle;

    GLfloat Plane1_X_Position ;
    GLfloat angle;
    GLfloat Line_X_Position;
    
    GLfloat rectangleVertices[12];

    GLfloat rectangleColorCoords[12];

    Sphere planeFrontSphere;
    
    GLuint vao_sphere;
    GLuint vbo_sphere;
    
    
    GLuint vaosphere;
    GLuint vbospherepos;
    GLuint vbospherecol;
    
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

[[self openGLContext]setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];


  
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

        //"in vec4 vColor;" \
        //"out vec4 out_color;" \
            //"out_color=vColor;"\

        //"in vec2 vTexCoord;" \
        //"out vec2 out_TexCoord;" \
        //"out_TexCoord=vTexCoord;"\


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
                    [self release];
                    [NSApp terminate:self];
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
            "in vec4 out_color;"\
            "out vec4 fragColor;"
            "void main(void)" \
            "{" \
            "fragColor=out_color;" \
            "} ";


        //"in vec4 out_color;"\
        //"fragColor=out_color;" \

        //"uniform sampler2D u_sampler;"
        //"in vec2 out_TexCoord;"
        //"fragColor=texture(u_sampler,out_TexCoord);" \


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
                    [self release];
                    [NSApp terminate:self];
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

        //glBindAttribLocation(gShaderProgramObject,
        //    AMC_ATTRIBUTE_TEXCOORD0,
        //    "vTexCoord");

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
                    [self release];
                    [NSApp terminate:self];
                }
            }
        }

        //Post Linking
        //Retriving Uniform Location
        mvpUniform = glGetUniformLocation(gShaderProgramObject,
            "u_mvp_matrix");

    
    
    
    D_R_OrangeColor = 1.0f;
    D_G_OrangeColor = 0.6f;
    D_B_OrangeColor = 0.2f;
    D_R_GreenColor = 0.074f;
    D_G_GreenColor = 0.533f;
    D_B_GreenColor = 0.2f;

    Line_Orange_R = 1.0f;
    Line_Orange_G = 0.6f;
    Line_Orange_B = 0.2f;
    Line_White_R = 1.00f;
    Line_White_G = 1.00f;
    Line_White_B = 1.00f;
    Line_Green_R = 0.074f;
    Line_Green_G = 0.533f;
    Line_Green_B = 0.2f;

    stage = 1;
    I_1_XPosition = -2.25f;
    A_XPosition = 2.25f;
    N_YPosition = 2.25f;
    I_2_YPosition = -2.25f;
    Plane1_Y_Position = 0.00f;
    Plane1_Z_Rotation_Angle = -90.0f;

    Plane1_X_Position = -2.5f;
    angle = 0.0f;
    Line_X_Position = -1.28f;


            //RECTANGLE
        glGenVertexArrays(1, &vao_quad);

        glBindVertexArray(vao_quad);

        //Generate Buffer
        glGenBuffers(1, &vbo_position_quad);
        //Bind Generated Buffer
        glBindBuffer(GL_ARRAY_BUFFER,
            vbo_position_quad);
        //Fill Buffer
        //glBufferData(GL_ARRAY_BUFFER,
        //    sizeof(rectangleVertices),
        //    rectangleVertices,
        //    GL_STATIC_DRAW);
        glBufferData(GL_ARRAY_BUFFER,
            4 * 3 * sizeof(float),
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

        //COLOR Generate Buffer
        glGenBuffers(1, &vbo_color_quad);
        //Bind Generated Buffer
        glBindBuffer(GL_ARRAY_BUFFER,
            vbo_color_quad);
        //Fill Buffer
        //glBufferData(GL_ARRAY_BUFFER,
        //    sizeof(rectangleColors),
        //    rectangleColors,
        //    GL_STATIC_DRAW);
        glBufferData(GL_ARRAY_BUFFER,
            4 * 3 * sizeof(float),
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



        //Create vao
        //Save everying in single set
        glGenVertexArrays(1, &vao_triangle);

        glBindVertexArray(vao_triangle);


        //TRIANGLE
        //Generate Buffer
        glGenBuffers(1, &vbo_position_triangle);
        //Bind Generated Buffer
        glBindBuffer(GL_ARRAY_BUFFER,
            vbo_position_triangle);
        //Fill Buffer
        glBufferData(GL_ARRAY_BUFFER,
            3 * 3 * sizeof(float),
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

        glGenBuffers(1, &vbo_color_triangle);
        //Bind Generated Buffer
        glBindBuffer(GL_ARRAY_BUFFER,
            vbo_color_triangle);
        //Fill Buffer
        glBufferData(GL_ARRAY_BUFFER,
            3 * 3 * sizeof(float),
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

        planeFrontSphere.setCenter(0.21f,0.0f);
       // planeFrontSphere.setTextureColor(0.729f , 0.886f , 0.933f);//(1.0f*255.0f, 1.0f*255.0f, 1.0f * 255);
        planeFrontSphere.set(0.07f, 90, 90);

    ////////sphere 2 vbo
    
    
     glGenVertexArrays(1, &vaosphere);

     glBindVertexArray(vaosphere);

    float arrVer[planeFrontSphere.vertices.size()];
    for(int i=0;i<planeFrontSphere.vertices.size();i++){
        arrVer[i]=planeFrontSphere.vertices.at(i);
        //fprintf(gpFile,"%f ",planeFrontSphere.vertices.at(i));
    }
    
    //fprintf(gpFile,"Colors \n ");
    
    float arrCol[planeFrontSphere.colorCoords.size()];
    for(int i=0;i<planeFrontSphere.colorCoords.size();i++){
        arrCol[i]=planeFrontSphere.colorCoords.at(i);
        //fprintf(gpFile,"%f ",planeFrontSphere.colorCoords.at(i));
    }
    
    
    //Generate Buffer
     glGenBuffers(1, &vbospherepos);
     //Bind Generated Buffer
     glBindBuffer(GL_ARRAY_BUFFER,
         vbospherepos);
     //Fill Buffer
     //glBufferData(GL_ARRAY_BUFFER,
     //    sizeof(rectangleVertices),
     //    rectangleVertices,
     //    GL_STATIC_DRAW);
     glBufferData(GL_ARRAY_BUFFER,
         planeFrontSphere.vertices.size() * sizeof(float),
         arrVer,
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

     //COLOR Generate Buffer
     glGenBuffers(1, &vbospherecol);
     //Bind Generated Buffer
     glBindBuffer(GL_ARRAY_BUFFER,
         vbospherecol);
     //Fill Buffer
     //glBufferData(GL_ARRAY_BUFFER,
     //    sizeof(rectangleColors),
     //    rectangleColors,
     //    GL_STATIC_DRAW);
     glBufferData(GL_ARRAY_BUFFER,
         planeFrontSphere.colorCoords.size() * sizeof(float),
         arrCol,
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

    ///
    
    
    //////////////////////////////Added
    
    
    //Create vao
    //Save everying in single set
    glGenVertexArrays(1, &vao_sphere);

    glBindVertexArray(vao_sphere);

    //Generate Buffer
    glGenBuffers(1, &vbo_sphere);
    //Bind Generated Buffer
    glBindBuffer(GL_ARRAY_BUFFER,
        vbo_sphere);
    //Fill Buffer
   /* glBufferData(GL_ARRAY_BUFFER,
        //sizeof(interleavedVertices),
        //interleavedVertices,
        sizeof(planeFrontSphere.interleavedVertices[0]) * planeFrontSphere.interleavedVertices.size(),
        reinterpret_cast<GLvoid*>(&planeFrontSphere.interleavedVertices[0]),
        GL_STATIC_DRAW);*/
    
    float arr[planeFrontSphere.interleavedVertices.size()];
    for(int i=0;i<planeFrontSphere.interleavedVertices.size();i++){
        arr[i]=planeFrontSphere.interleavedVertices.at(i);
        //fprintf(gpFile,"%f ",planeFrontSphere.interleavedVertices.at(i));
    }
    
    glBufferData(GL_ARRAY_BUFFER,
    sizeof(arr[0]) * planeFrontSphere.interleavedVertices.size(),
    arr,
    GL_STATIC_DRAW);
    
    //float arr[planeFrontSphere.interleavedVertices.size()];
    //for(int i=0;i<planeFrontSphere.interleavedVertices.size();i++)
      //  fprintf(gpFile,"%d %d",i,planeFrontSphere.interleavedVertices[i]);
    
    //Set Vertex Attrib Pointer
    glVertexAttribPointer(AMC_ATTRIBUTE_POSITION,
        3,
        GL_FLOAT,
        GL_FALSE,
        11 * sizeof(GL_FLOAT),
        (GLvoid*)(0 * sizeof(GL_FLOAT)));

    //Enable Vertex Attrib Array
    glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
    
    glVertexAttribPointer(AMC_ATTRIBUTE_COLOR,
        3,
        GL_FLOAT,
        GL_FALSE,
        11 * sizeof(GL_FLOAT),
        (GLvoid*)(3 * sizeof(GL_FLOAT)));

    //Enable Vertex Attrib Array
    glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
    
    //Set Vertex Attrib Pointer
    glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL,
        3,
        GL_FLOAT,
        GL_FALSE,
        11 * sizeof(GL_FLOAT),
        (GLvoid*)(6 * sizeof(GL_FLOAT)));
    //Enable Vertex Attrib Array
    glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);

    //Set Vertex Attrib Pointer
    glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD0,
        2,
        GL_FLOAT,
        GL_FALSE,
        11 * sizeof(GL_FLOAT),
        (GLvoid*)(9 * sizeof(GL_FLOAT)));
    //Enable Vertex Attrib Array
    glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD0);

    //Unbind Buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);
     
    //Unbind array
    glBindVertexArray(0);

    
    
///////////////////////////////////////
    
    
    
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glClearDepth(1.0f);
        
        //set bg color
        glClearColor(0.0f,0.0f,0.0f,0.0f);

        perspectiveProjectionMatrix = vmath::mat4::identity();
    
    

CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);

CVDisplayLinkSetOutputCallback(displayLink,&MyDisplayLinkCallback,self);

CGLContextObj cglContext=(CGLContextObj)[[self openGLContext]CGLContextObj];

CGLPixelFormatObj cglPixelFormat=(CGLPixelFormatObj)[[self pixelFormat]CGLPixelFormatObj];

CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink,cglContext,cglPixelFormat);

CVDisplayLinkStart(displayLink);

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

glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glUseProgram(gShaderProgramObject);

  //Initialize matrices

            vmath::mat4 modelViewMatrix = vmath::mat4::identity();
            vmath::mat4 modelViewProjectionMatrix = vmath::mat4::identity();
            vmath::mat4 translationMatrix = vmath::mat4::identity();
            vmath::mat4 rotationMatrix = vmath::mat4::identity();
            
        if(stage>0)
                {
                modelViewMatrix = vmath::mat4::identity();
                modelViewProjectionMatrix = vmath::mat4::identity();
                translationMatrix = vmath::mat4::identity();
                rotationMatrix = vmath::mat4::identity();
                
                //Transformation

                translationMatrix = vmath::translate(I_1_XPosition, 0.0f, -3.0f);
                //rotationMatrix = vmath::rotate(anglePyramid, 0.0f, 1.0f, 0.0f);

                //Matrix Multiplication
                modelViewMatrix = translationMatrix * rotationMatrix;

                modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

                //Send necessary matrices to shader in resp. Uniforms

                glUniformMatrix4fv(mvpUniform,
                    1,
                    GL_FALSE,
                    modelViewProjectionMatrix);

                //Bind with vao

                //glBindVertexArray(vao);

                //Bind with textures if any

                //Draw

                //DrawLetter('I');
                [self DrawLetter:'I'];

                //Unbind vao

                //glBindVertexArray(0);
                
                }
                
                if(stage>2)
                {
                modelViewMatrix = vmath::mat4::identity();
                modelViewProjectionMatrix = vmath::mat4::identity();
                translationMatrix = vmath::mat4::identity();
                rotationMatrix = vmath::mat4::identity();
                
                //Transformation

                translationMatrix = vmath::translate(-1.0f, N_YPosition, -3.0f);
                //rotationMatrix = vmath::rotate(anglePyramid, 0.0f, 1.0f, 0.0f);

                //Matrix Multiplication
                modelViewMatrix = translationMatrix * rotationMatrix;

                modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

                //Send necessary matrices to shader in resp. Uniforms

                glUniformMatrix4fv(mvpUniform,
                    1,
                    GL_FALSE,
                    modelViewProjectionMatrix);

                //Bind with vao

                //glBindVertexArray(vao);

                //Bind with textures if any

                //Draw

                //DrawLetter('N');
                [self DrawLetter:'N'];

                //Unbind vao

                //glBindVertexArray(0);
                
                }

            if(stage>4)
                {
                modelViewMatrix = vmath::mat4::identity();
                modelViewProjectionMatrix = vmath::mat4::identity();
                translationMatrix = vmath::mat4::identity();
                rotationMatrix = vmath::mat4::identity();
                
                //Transformation

                translationMatrix = vmath::translate(-0.25f, 0.0f, -3.0f);
                //rotationMatrix = vmath::rotate(anglePyramid, 0.0f, 1.0f, 0.0f);

                //Matrix Multiplication
                modelViewMatrix = translationMatrix * rotationMatrix;

                modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

                //Send necessary matrices to shader in resp. Uniforms

                glUniformMatrix4fv(mvpUniform,
                    1,
                    GL_FALSE,
                    modelViewProjectionMatrix);

                //Bind with vao

                //glBindVertexArray(vao);

                //Bind with textures if any

                //Draw

                //DrawLetter('D');
                [self DrawLetter:'D'];
                //Unbind vao

                //glBindVertexArray(0);
                
                }

            if(stage>3)
                {
                modelViewMatrix = vmath::mat4::identity();
                modelViewProjectionMatrix = vmath::mat4::identity();
                translationMatrix = vmath::mat4::identity();
                rotationMatrix = vmath::mat4::identity();
                
                //Transformation

                translationMatrix = vmath::translate(0.25f, I_2_YPosition, -3.0f);
                //rotationMatrix = vmath::rotate(anglePyramid, 0.0f, 1.0f, 0.0f);

                //Matrix Multiplication
                modelViewMatrix = translationMatrix * rotationMatrix;

                modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

                //Send necessary matrices to shader in resp. Uniforms

                glUniformMatrix4fv(mvpUniform,
                    1,
                    GL_FALSE,
                    modelViewProjectionMatrix);

                //Bind with vao

                //glBindVertexArray(vao);

                //Bind with textures if any

                //Draw

                //DrawLetter('I');
                [self DrawLetter:'I'];
                //Unbind vao

                //glBindVertexArray(0);
                
                }


            if(stage>1)
                {
                modelViewMatrix = vmath::mat4::identity();
                modelViewProjectionMatrix = vmath::mat4::identity();
                translationMatrix = vmath::mat4::identity();
                rotationMatrix = vmath::mat4::identity();
                
                //Transformation

                translationMatrix = vmath::translate(A_XPosition, 0.0f, -3.0f);
                //rotationMatrix = vmath::rotate(anglePyramid, 0.0f, 1.0f, 0.0f);

                //Matrix Multiplication
                modelViewMatrix = translationMatrix * rotationMatrix;

                modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

                //Send necessary matrices to shader in resp. Uniforms

                glUniformMatrix4fv(mvpUniform,
                    1,
                    GL_FALSE,
                    modelViewProjectionMatrix);

                //Bind with vao

                //glBindVertexArray(vao);

                //Bind with textures if any

                //Draw
                [self DrawLetter:'A'];
                //[self DrawLetter:'T'];
                // DrawLetter('A');
                // DrawLetter('T');

                //Unbind vao

                //glBindVertexArray(0);
                
                }


                
                if(stage==8)
                {

                Plane1_Y_Position = ((Plane1_X_Position - 1.3)*(Plane1_X_Position - 1.3) / (4 * 0.15)) - 0.0f;

                modelViewMatrix = vmath::mat4::identity();
                modelViewProjectionMatrix = vmath::mat4::identity();
                translationMatrix = vmath::mat4::identity();
                rotationMatrix = vmath::mat4::identity();
                
                //Transformation

                translationMatrix = vmath::translate(Plane1_X_Position, Plane1_Y_Position, -3.0f);
                rotationMatrix = vmath::rotate(Plane1_Z_Rotation_Angle, 0.0f, 0.0f, 1.0f);

                //Matrix Multiplication
                modelViewMatrix = translationMatrix * rotationMatrix;

                modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

                //Send necessary matrices to shader in resp. Uniforms

                glUniformMatrix4fv(mvpUniform,
                    1,
                    GL_FALSE,
                    modelViewProjectionMatrix);

                //Bind with vao

                //glBindVertexArray(vao);

                //Bind with textures if any

                //Draw
                [self DrawLetter:'P'];
                // DrawLetter('P');

                //Unbind vao

                //glBindVertexArray(0);
                


                modelViewMatrix = vmath::mat4::identity();
                modelViewProjectionMatrix = vmath::mat4::identity();
                translationMatrix = vmath::mat4::identity();
                rotationMatrix = vmath::mat4::identity();
                
                //Transformation

                translationMatrix = vmath::translate(Plane1_X_Position, 0.0f, -3.0f);
                //rotationMatrix = vmath::rotate(Plane1_Z_Rotation_Angle, 0.0f, 0.0f, 1.0f);

                //Matrix Multiplication
                modelViewMatrix = translationMatrix * rotationMatrix;

                modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

                //Send necessary matrices to shader in resp. Uniforms

                glUniformMatrix4fv(mvpUniform,
                    1,
                    GL_FALSE,
                    modelViewProjectionMatrix);

                //Bind with vao

                //glBindVertexArray(vao);

                //Bind with textures if any

                //Draw
                [self DrawLetter:'P'];
                // DrawLetter('P');

                //Unbind vao

                //glBindVertexArray(0);
                
                
                modelViewMatrix = vmath::mat4::identity();
                modelViewProjectionMatrix = vmath::mat4::identity();
                translationMatrix = vmath::mat4::identity();
                rotationMatrix = vmath::mat4::identity();
                
                //Transformation

                translationMatrix = vmath::translate(Plane1_X_Position, -Plane1_Y_Position, -3.0f);
                rotationMatrix = vmath::rotate(-Plane1_Z_Rotation_Angle, 0.0f, 0.0f, 1.0f);

                //Matrix Multiplication
                modelViewMatrix = translationMatrix * rotationMatrix;

                modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

                //Send necessary matrices to shader in resp. Uniforms

                glUniformMatrix4fv(mvpUniform,
                    1,
                    GL_FALSE,
                    modelViewProjectionMatrix);

                //Bind with vao

                //glBindVertexArray(vao);

                //Bind with textures if any

                //Draw
                [self DrawLetter:'P'];
                // DrawLetter('P');

                //Unbind vao

                //glBindVertexArray(0);
                

                }

            if(stage==7)
                {
                modelViewMatrix = vmath::mat4::identity();
                modelViewProjectionMatrix = vmath::mat4::identity();
                translationMatrix = vmath::mat4::identity();
                rotationMatrix = vmath::mat4::identity();
                
                //Transformation

                translationMatrix = vmath::translate(Plane1_X_Position, 0.0f, -3.0f);
                rotationMatrix = vmath::rotate(-Plane1_Z_Rotation_Angle, 0.0f, 0.0f, 1.0f);

                //Matrix Multiplication
                modelViewMatrix = translationMatrix * rotationMatrix;

                modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

                //Send necessary matrices to shader in resp. Uniforms

                glUniformMatrix4fv(mvpUniform,
                    1,
                    GL_FALSE,
                    modelViewProjectionMatrix);

                //Bind with vao

                //glBindVertexArray(vao);

                //Bind with textures if any

                //Draw
                [self DrawLetter:'P'];

                //Unbind vao

                //glBindVertexArray(0);
                
                }

            if ((stage == 7 || stage == 8) && Plane1_X_Position > -1.02f && Plane1_X_Position < 1.40f)
            {
                modelViewMatrix = vmath::mat4::identity();
                modelViewProjectionMatrix = vmath::mat4::identity();
                translationMatrix = vmath::mat4::identity();
                rotationMatrix = vmath::mat4::identity();
                
                //Transformation

                translationMatrix = vmath::translate(0.0f, 0.0f, -3.0f);
                //rotationMatrix = vmath::rotate(-Plane1_Z_Rotation_Angle, 0.0f, 0.0f, 1.0f);

                //Matrix Multiplication
                modelViewMatrix = translationMatrix * rotationMatrix;

                modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

                //Send necessary matrices to shader in resp. Uniforms

                glUniformMatrix4fv(mvpUniform,
                    1,
                    GL_FALSE,
                    modelViewProjectionMatrix);

                //Bind with vao

                //glBindVertexArray(vao);

                //Bind with textures if any

                //Draw
                [self DrawLetter:'L'];

                //Unbind vao

                //glBindVertexArray(0);
                
                }

                if (stage > 7 && Plane1_X_Position > 1.40f)
                {
                
                modelViewMatrix = vmath::mat4::identity();
                modelViewProjectionMatrix = vmath::mat4::identity();
                translationMatrix = vmath::mat4::identity();
                rotationMatrix = vmath::mat4::identity();
                
                //Transformation

                translationMatrix = vmath::translate(A_XPosition, 0.0f, -3.0f);
                //rotationMatrix = vmath::rotate(-Plane1_Z_Rotation_Angle, 0.0f, 0.0f, 1.0f);

                //Matrix Multiplication
                modelViewMatrix = translationMatrix * rotationMatrix;

                modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

                //Send necessary matrices to shader in resp. Uniforms

                glUniformMatrix4fv(mvpUniform,
                    1,
                    GL_FALSE,
                    modelViewProjectionMatrix);

                //Bind with vao

                //glBindVertexArray(vao);

                //Bind with textures if any

                //Draw
                [self DrawLetter:'T'];

                //Unbind vao

                //glBindVertexArray(0);
                
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

                modelViewMatrix = vmath::mat4::identity();
                modelViewProjectionMatrix = vmath::mat4::identity();
                translationMatrix = vmath::mat4::identity();
                rotationMatrix = vmath::mat4::identity();
                
                //Transformation

                translationMatrix = vmath::translate(0.0f, 0.0f, -3.0f);
                //rotationMatrix = vmath::rotate(-Plane1_Z_Rotation_Angle, 0.0f, 0.0f, 1.0f);

                //Matrix Multiplication
                modelViewMatrix = translationMatrix * rotationMatrix;

                modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

                //Send necessary matrices to shader in resp. Uniforms

                glUniformMatrix4fv(mvpUniform,
                    1,
                    GL_FALSE,
                    modelViewProjectionMatrix);

                //Bind with vao

                //glBindVertexArray(vao);

                //Bind with textures if any

                //Draw
                [self DrawLetter:'L'];

                //Unbind vao

                //glBindVertexArray(0);

                }

                if (stage == 6)
                {
                   Plane1_Y_Position = ((Plane1_X_Position + 1.3)*(Plane1_X_Position + 1.3) / (4 * 0.15)) - 0.0f;

                modelViewMatrix = vmath::mat4::identity();
                modelViewProjectionMatrix = vmath::mat4::identity();
                translationMatrix = vmath::mat4::identity();
                rotationMatrix = vmath::mat4::identity();
                
                //Transformation

                translationMatrix = vmath::translate(Plane1_X_Position, Plane1_Y_Position, -3.0f);
                rotationMatrix = vmath::rotate(Plane1_Z_Rotation_Angle, 0.0f, 0.0f, 1.0f);

                //Matrix Multiplication
                modelViewMatrix = translationMatrix * rotationMatrix;

                modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

                //Send necessary matrices to shader in resp. Uniforms

                glUniformMatrix4fv(mvpUniform,
                    1,
                    GL_FALSE,
                    modelViewProjectionMatrix);

                //Bind with vao

                //glBindVertexArray(vao);

                //Bind with textures if any

                //Draw
                [self DrawLetter:'P'];

                //Unbind vao

                //glBindVertexArray(0);
                
                modelViewMatrix = vmath::mat4::identity();
                modelViewProjectionMatrix = vmath::mat4::identity();
                translationMatrix = vmath::mat4::identity();
                rotationMatrix = vmath::mat4::identity();
                
                //Transformation

                translationMatrix = vmath::translate(Plane1_X_Position, 0.0f,-3.0f);
                //rotationMatrix = vmath::rotate(Plane1_Z_Rotation_Angle, 0.0f, 0.0f, 1.0f);

                //Matrix Multiplication
                modelViewMatrix = translationMatrix * rotationMatrix;

                modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

                //Send necessary matrices to shader in resp. Uniforms

                glUniformMatrix4fv(mvpUniform,
                    1,
                    GL_FALSE,
                    modelViewProjectionMatrix);

                //Bind with vao

                //glBindVertexArray(vao);

                //Bind with textures if any

                //Draw
                [self DrawLetter:'P'];

                //Unbind vao

                //glBindVertexArray(0);
                

                modelViewMatrix = vmath::mat4::identity();
                modelViewProjectionMatrix = vmath::mat4::identity();
                translationMatrix = vmath::mat4::identity();
                rotationMatrix = vmath::mat4::identity();
                
                //Transformation

                translationMatrix = vmath::translate(Plane1_X_Position, -1 * (Plane1_Y_Position), -3.0f);
                rotationMatrix = vmath::rotate(-Plane1_Z_Rotation_Angle, 0.0f, 0.0f, 1.0f);

                //Matrix Multiplication
                modelViewMatrix = translationMatrix * rotationMatrix;

                modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

                //Send necessary matrices to shader in resp. Uniforms

                glUniformMatrix4fv(mvpUniform,
                    1,
                    GL_FALSE,
                    modelViewProjectionMatrix);

                //Bind with vao

                //glBindVertexArray(vao);

                //Bind with textures if any

                //Draw
                [self DrawLetter:'P'];

                //Unbind vao

                //glBindVertexArray(0);

                }

                glUseProgram(0);
                
                CGLFlushDrawable((CGLContextObj)[[self openGLContext]CGLContextObj]);
                CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);


                switch (stage)
                {
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
 if (vbo_color_quad) {
      glDeleteBuffers(1, &vbo_color_quad);
      vbo_color_quad = 0;
  }

  if (vbo_position_quad) {
      glDeleteBuffers(1, &vbo_position_quad);
      vbo_position_quad = 0;
  }
  if (vao_quad) {
      glDeleteVertexArrays(1, &vao_quad);
      vao_quad = 0;
  }
  if (vao_point) {
      glDeleteVertexArrays(1, &vao_point);
      vao_point = 0;
  }
  if (vbo_position_point) {
      glDeleteBuffers(1, &vbo_position_point);
      vbo_position_point = 0;
  }

  if (vbo_color_point) {
      glDeleteBuffers(1, &vbo_color_point);
      vbo_color_point = 0;
  }

  if (vbo_position_triangle) {
      glDeleteBuffers(1, &vbo_position_triangle);
      vbo_position_triangle = 0;
  }

  if (vbo_color_triangle) {
      glDeleteBuffers(1, &vbo_color_triangle);
      vbo_color_triangle = 0;
  }

  if (vao_triangle) {
      glDeleteVertexArrays(1, &vao_triangle);
      vao_triangle = 0;
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



-(void) DrawLetter:( char )letter
{
    switch (letter)
        {
        case 'I':
            glBindVertexArray(vao_quad);

            rectangleVertices[0] = 0.275f;
            rectangleVertices[1] = 1.0f;
            rectangleVertices[2] = 0.0f;
            rectangleVertices[3] = 0.225f;
            rectangleVertices[4] = 1.0f;
            rectangleVertices[5] = 0.0f;
            rectangleVertices[6] = 0.225f;
            rectangleVertices[7] = -1.0f;
            rectangleVertices[8] = 0.0f;
            rectangleVertices[9] = 0.275f;
            rectangleVertices[10] = -1.0f;
            rectangleVertices[11] = 0.0f;

            rectangleColorCoords[0] = 1.0f;
            rectangleColorCoords[1] = 0.6f;
            rectangleColorCoords[2] = 0.2f;
            rectangleColorCoords[3] = 1.0f;
            rectangleColorCoords[4] = 0.6f;
            rectangleColorCoords[5] = 0.2f;
            rectangleColorCoords[6] = 0.074f;
            rectangleColorCoords[7] = 0.533f;
            rectangleColorCoords[8] = 0.2f;
            rectangleColorCoords[9] = 0.074f;
            rectangleColorCoords[10] = 0.533f;
            rectangleColorCoords[11] = 0.2f;



            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_position_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleVertices,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_color_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleColorCoords,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLE_FAN,
                0,
                4);

            glBindVertexArray(0);

            break;

        case 'N':
            glBindVertexArray(vao_quad);

            //Left side of N
            rectangleVertices[0] = 0.05f;
            rectangleVertices[1] = 1.0f;
            rectangleVertices[2] = 0.0f;
            rectangleVertices[3] = 0.0f;
            rectangleVertices[4] = 1.0f;
            rectangleVertices[5] = 0.0f;
            rectangleVertices[6] = 0.00f;
            rectangleVertices[7] = -1.0f;
            rectangleVertices[8] = 0.0f;
            rectangleVertices[9] = 0.05f;
            rectangleVertices[10] = -1.0f;
            rectangleVertices[11] = 0.0f;

            rectangleColorCoords[0] = 1.0f;
            rectangleColorCoords[1] = 0.6f;
            rectangleColorCoords[2] = 0.2f;
            rectangleColorCoords[3] = 1.0f;
            rectangleColorCoords[4] = 0.6f;
            rectangleColorCoords[5] = 0.2f;
            rectangleColorCoords[6] = 0.074f;
            rectangleColorCoords[7] = 0.533f;
            rectangleColorCoords[8] = 0.2f;
            rectangleColorCoords[9] = 0.074f;
            rectangleColorCoords[10] = 0.533f;
            rectangleColorCoords[11] = 0.2f;


            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_position_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleVertices,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_color_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleColorCoords,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLE_FAN,
                0,
                4);

            //Middle part of N
            rectangleVertices[0] = 0.05f;
            rectangleVertices[1] = 1.0f;
            rectangleVertices[2] = 0.0f;
            rectangleVertices[3] = 0.00f;
            rectangleVertices[4] = 1.0f;
            rectangleVertices[5] = 0.0f;
            rectangleVertices[6] = 0.45f;
            rectangleVertices[7] = -1.0f;
            rectangleVertices[8] = 0.0f;
            rectangleVertices[9] = 0.50f;
            rectangleVertices[10] = -1.0f;
            rectangleVertices[11] = 0.0f;

            rectangleColorCoords[0] = 1.0f;
            rectangleColorCoords[1] = 0.6f;
            rectangleColorCoords[2] = 0.2f;
            rectangleColorCoords[3] = 1.0f;
            rectangleColorCoords[4] = 0.6f;
            rectangleColorCoords[5] = 0.2f;
            rectangleColorCoords[6] = 0.074f;
            rectangleColorCoords[7] = 0.533f;
            rectangleColorCoords[8] = 0.2f;
            rectangleColorCoords[9] = 0.074f;
            rectangleColorCoords[10] = 0.533f;
            rectangleColorCoords[11] = 0.2f;

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_position_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleVertices,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_color_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleColorCoords,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLE_FAN,
                0,
                4);

            //Right Side of N
            rectangleVertices[0] = 0.50f;
            rectangleVertices[1] = 1.0f;
            rectangleVertices[2] = 0.0f;
            rectangleVertices[3] = 0.45f;
            rectangleVertices[4] = 1.0f;
            rectangleVertices[5] = 0.0f;
            rectangleVertices[6] = 0.45f;
            rectangleVertices[7] = -1.0f;
            rectangleVertices[8] = 0.0f;
            rectangleVertices[9] = 0.50f;
            rectangleVertices[10] = -1.0f;
            rectangleVertices[11] = 0.0f;

            rectangleColorCoords[0] = 1.0f;
            rectangleColorCoords[1] = 0.6f;
            rectangleColorCoords[2] = 0.2f;
            rectangleColorCoords[3] = 1.0f;
            rectangleColorCoords[4] = 0.6f;
            rectangleColorCoords[5] = 0.2f;
            rectangleColorCoords[6] = 0.074f;
            rectangleColorCoords[7] = 0.533f;
            rectangleColorCoords[8] = 0.2f;
            rectangleColorCoords[9] = 0.074f;
            rectangleColorCoords[10] = 0.533f;
            rectangleColorCoords[11] = 0.2f;


            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_position_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleVertices,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_color_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleColorCoords,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLE_FAN,
                0,
                4);

            glBindVertexArray(0);

            break;

        case 'D':
            glBindVertexArray(vao_quad);

            rectangleVertices[0] = 0.05f;
            rectangleVertices[1] = 1.0f;
            rectangleVertices[2] = 0.0f;
            rectangleVertices[3] = 0.0f;
            rectangleVertices[4] = 1.0f;
            rectangleVertices[5] = 0.0f;
            rectangleVertices[6] = 0.00f;
            rectangleVertices[7] = -1.0f;
            rectangleVertices[8] = 0.0f;
            rectangleVertices[9] = 0.05f;
            rectangleVertices[10] = -1.0f;
            rectangleVertices[11] = 0.0f;

            rectangleColorCoords[0] = D_R_OrangeColor;
            rectangleColorCoords[1] = D_G_OrangeColor;
            rectangleColorCoords[2] = D_B_OrangeColor;
            rectangleColorCoords[3] = D_R_OrangeColor;
            rectangleColorCoords[4] = D_G_OrangeColor;
            rectangleColorCoords[5] = D_B_OrangeColor;
            rectangleColorCoords[6] = D_R_GreenColor;
            rectangleColorCoords[7] = D_G_GreenColor;
            rectangleColorCoords[8] = D_B_GreenColor;
            rectangleColorCoords[9] = D_R_GreenColor;
            rectangleColorCoords[10] = D_G_GreenColor;
            rectangleColorCoords[11] = D_B_GreenColor;




            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_position_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleVertices,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_color_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleColorCoords,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLE_FAN,
                0,
                4);


            rectangleVertices[0] = 0.5f;
            rectangleVertices[1] = 1.0f;
            rectangleVertices[2] = 0.0f;
            rectangleVertices[3] = -0.01f;
            rectangleVertices[4] = 1.0f;
            rectangleVertices[5] = 0.0f;
            rectangleVertices[6] = -0.01f;
            rectangleVertices[7] = 0.95f;
            rectangleVertices[8] = 0.0f;
            rectangleVertices[9] = 0.5f;
            rectangleVertices[10] = 0.95f;
            rectangleVertices[11] = 0.0f;

            rectangleColorCoords[0] = D_R_OrangeColor;
            rectangleColorCoords[1] = D_G_OrangeColor;
            rectangleColorCoords[2] = D_B_OrangeColor;
            rectangleColorCoords[3] = D_R_OrangeColor;
            rectangleColorCoords[4] = D_G_OrangeColor;
            rectangleColorCoords[5] = D_B_OrangeColor;
            rectangleColorCoords[6] = D_R_OrangeColor;
            rectangleColorCoords[7] = D_G_OrangeColor;
            rectangleColorCoords[8] = D_B_OrangeColor;
            rectangleColorCoords[9] = D_R_OrangeColor;
            rectangleColorCoords[10] = D_G_OrangeColor;
            rectangleColorCoords[11] = D_B_OrangeColor;


            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_position_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleVertices,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_color_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleColorCoords,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLE_FAN,
                0,
                4);

            rectangleVertices[0] = 0.50f;
            rectangleVertices[1] = 1.0f;
            rectangleVertices[2] = 0.0f;
            rectangleVertices[3] = 0.45f;
            rectangleVertices[4] = 1.0f;
            rectangleVertices[5] = 0.0f;
            rectangleVertices[6] = 0.45f;
            rectangleVertices[7] = -1.0f;
            rectangleVertices[8] = 0.0f;
            rectangleVertices[9] = 0.50f;
            rectangleVertices[10] = -1.0f;
            rectangleVertices[11] = 0.0f;

            rectangleColorCoords[0] = D_R_OrangeColor;
            rectangleColorCoords[1] = D_G_OrangeColor;
            rectangleColorCoords[2] = D_B_OrangeColor;
            rectangleColorCoords[3] = D_R_OrangeColor;
            rectangleColorCoords[4] = D_G_OrangeColor;
            rectangleColorCoords[5] = D_B_OrangeColor;
            rectangleColorCoords[6] = D_R_GreenColor;
            rectangleColorCoords[7] = D_G_GreenColor;
            rectangleColorCoords[8] = D_B_GreenColor;
            rectangleColorCoords[9] = D_R_GreenColor;
            rectangleColorCoords[10] = D_G_GreenColor;
            rectangleColorCoords[11] = D_B_GreenColor;


            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_position_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleVertices,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_color_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleColorCoords,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLE_FAN,
                0,
                4);

            rectangleVertices[0] = 0.50f;
            rectangleVertices[1] = -0.95f;
            rectangleVertices[2] = 0.0f;
            rectangleVertices[3] = -0.01f;
            rectangleVertices[4] = -0.95f;
            rectangleVertices[5] = 0.0f;
            rectangleVertices[6] = -0.01f;
            rectangleVertices[7] = -1.0f;
            rectangleVertices[8] = 0.0f;
            rectangleVertices[9] = 0.50f;
            rectangleVertices[10] = -1.0f;
            rectangleVertices[11] = 0.0f;

            rectangleColorCoords[0] = D_R_GreenColor;
            rectangleColorCoords[1] = D_G_GreenColor;
            rectangleColorCoords[2] = D_B_GreenColor;
            rectangleColorCoords[3] = D_R_GreenColor;
            rectangleColorCoords[4] = D_G_GreenColor;
            rectangleColorCoords[5] = D_B_GreenColor;
            rectangleColorCoords[6] = D_R_GreenColor;
            rectangleColorCoords[7] = D_G_GreenColor;
            rectangleColorCoords[8] = D_B_GreenColor;
            rectangleColorCoords[9] = D_R_GreenColor;
            rectangleColorCoords[10] = D_G_GreenColor;
            rectangleColorCoords[11] = D_B_GreenColor;

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_position_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleVertices,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_color_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleColorCoords,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLE_FAN,
                0,
                4);

            glBindVertexArray(0);

            break;
        case 'A':
            glBindVertexArray(vao_quad);

            rectangleVertices[0] = 0.275f;
            rectangleVertices[1] = 1.0f;
            rectangleVertices[2] = 0.0f;
            rectangleVertices[3] = 0.225f;
            rectangleVertices[4] = 1.0f;
            rectangleVertices[5] = 0.0f;
            rectangleVertices[6] = 0.00f;
            rectangleVertices[7] = -1.0f;
            rectangleVertices[8] = 0.0f;
            rectangleVertices[9] = 0.05f;
            rectangleVertices[10] = -1.0f;
            rectangleVertices[11] = 0.0f;

            rectangleColorCoords[0] = 1.0f;
            rectangleColorCoords[1] = 0.6f;
            rectangleColorCoords[2] = 0.2f;
            rectangleColorCoords[3] = 1.0f;
            rectangleColorCoords[4] = 0.6f;
            rectangleColorCoords[5] = 0.2f;
            rectangleColorCoords[6] = 0.074f;
            rectangleColorCoords[7] = 0.533f;
            rectangleColorCoords[8] = 0.2f;
            rectangleColorCoords[9] = 0.074f;
            rectangleColorCoords[10] = 0.533f;
            rectangleColorCoords[11] = 0.2f;

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_position_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleVertices,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_color_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleColorCoords,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLE_FAN,
                0,
                4);


            rectangleVertices[0] = 0.275f;
            rectangleVertices[1] = 1.0f;
            rectangleVertices[2] = 0.0f;
            rectangleVertices[3] = 0.225f;
            rectangleVertices[4] = 1.0f;
            rectangleVertices[5] = 0.0f;
            rectangleVertices[6] = 0.45f;
            rectangleVertices[7] = -1.0f;
            rectangleVertices[8] = 0.0f;
            rectangleVertices[9] = 0.50f;
            rectangleVertices[10] = -1.0f;
            rectangleVertices[11] = 0.0f;

            rectangleColorCoords[0] = 1.0f;
            rectangleColorCoords[1] = 0.6f;
            rectangleColorCoords[2] = 0.2f;
            rectangleColorCoords[3] = 1.0f;
            rectangleColorCoords[4] = 0.6f;
            rectangleColorCoords[5] = 0.2f;
            rectangleColorCoords[6] = 0.074f;
            rectangleColorCoords[7] = 0.533f;
            rectangleColorCoords[8] = 0.2f;
            rectangleColorCoords[9] = 0.074f;
            rectangleColorCoords[10] = 0.533f;
            rectangleColorCoords[11] = 0.2f;

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_position_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleVertices,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_color_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleColorCoords,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLE_FAN,
                0,
                4);
            break;

            glBindVertexArray(0);

        case 'T':
            glBindVertexArray(vao_quad);

            //Tricolor of A
            rectangleVertices[0] = 0.380f;
            rectangleVertices[1] = 0.06f;
            rectangleVertices[2] = 0.0f;
            rectangleVertices[3] = 0.12f;
            rectangleVertices[4] = 0.06f;
            rectangleVertices[5] = 0.0f;
            rectangleVertices[6] = 0.12f;
            rectangleVertices[7] = 0.02f;
            rectangleVertices[8] = 0.0f;
            rectangleVertices[9] = 0.380f;
            rectangleVertices[10] = 0.02f;
            rectangleVertices[11] = 0.0f;

            rectangleColorCoords[0] = 1.0f;
            rectangleColorCoords[1] = 0.6f;
            rectangleColorCoords[2] = 0.2f;
            rectangleColorCoords[3] = 1.0f;
            rectangleColorCoords[4] = 0.6f;
            rectangleColorCoords[5] = 0.2f;
            rectangleColorCoords[6] = 1.0f;
            rectangleColorCoords[7] = 0.6f;
            rectangleColorCoords[8] = 0.2f;
            rectangleColorCoords[9] = 1.0f;
            rectangleColorCoords[10] = 0.6f;
            rectangleColorCoords[11] = 0.2f;

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_position_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleVertices,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_color_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleColorCoords,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLE_FAN,
                0,
                4);

            rectangleVertices[0] = 0.380f;
            rectangleVertices[1] = 0.02f;
            rectangleVertices[2] = 0.0f;
            rectangleVertices[3] = 0.12f;
            rectangleVertices[4] = 0.02f;
            rectangleVertices[5] = 0.0f;
            rectangleVertices[6] = 0.12f;
            rectangleVertices[7] = -0.02f;
            rectangleVertices[8] = 0.0f;
            rectangleVertices[9] = 0.380f;
            rectangleVertices[10] = -0.02f;
            rectangleVertices[11] = 0.0f;

            rectangleColorCoords[0] = 1.0f;
            rectangleColorCoords[1] = 1.0f;
            rectangleColorCoords[2] = 1.0f;
            rectangleColorCoords[3] = 1.0f;
            rectangleColorCoords[4] = 1.0f;
            rectangleColorCoords[5] = 1.0f;
            rectangleColorCoords[6] = 1.0f;
            rectangleColorCoords[7] = 1.0f;
            rectangleColorCoords[8] = 1.0f;
            rectangleColorCoords[9] = 1.0f;
            rectangleColorCoords[10] = 1.0f;
            rectangleColorCoords[11] = 1.0f;

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_position_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleVertices,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_color_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleColorCoords,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLE_FAN,
                0,
                4);

            rectangleVertices[0] = 0.380f;
            rectangleVertices[1] = -0.02f;
            rectangleVertices[2] = 0.0f;
            rectangleVertices[3] = 0.12f;
            rectangleVertices[4] = -0.02f;
            rectangleVertices[5] = 0.0f;
            rectangleVertices[6] = 0.12f;
            rectangleVertices[7] = -0.06f;
            rectangleVertices[8] = 0.0f;
            rectangleVertices[9] = 0.380f;
            rectangleVertices[10] = -0.06f;
            rectangleVertices[11] = 0.0f;

            rectangleColorCoords[0] = 0.074f;
            rectangleColorCoords[1] = 0.533f;
            rectangleColorCoords[2] = 0.2f;
            rectangleColorCoords[3] = 0.074f;
            rectangleColorCoords[4] = 0.533f;
            rectangleColorCoords[5] = 0.2f;
            rectangleColorCoords[6] = 0.074f;
            rectangleColorCoords[7] = 0.533f;
            rectangleColorCoords[8] = 0.2f;
            rectangleColorCoords[9] = 0.074f;
            rectangleColorCoords[10] = 0.533f;
            rectangleColorCoords[11] = 0.2f;

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_position_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleVertices,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_color_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleColorCoords,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLE_FAN,
                0,
                4);

            glBindVertexArray(0);

            break;

        case 'L':
            glBindVertexArray(vao_quad);

            if (Plane1_X_Position < 1.40) {
                //Line Red
                rectangleVertices[0] = Plane1_X_Position - 0.27f;
                rectangleVertices[1] = 0.06f;
                rectangleVertices[2] = 0.0f;
                rectangleVertices[3] = Line_X_Position;
                rectangleVertices[4] = 0.06f;
                rectangleVertices[5] = 0.0f;
                rectangleVertices[6] = Line_X_Position;
                rectangleVertices[7] = 0.02f;
                rectangleVertices[8] = 0.0f;
                rectangleVertices[9] = Plane1_X_Position - 0.27f;
                rectangleVertices[10] = 0.02f;
                rectangleVertices[11] = 0.0f;

                rectangleColorCoords[0] = 1.0f;
                rectangleColorCoords[1] = 0.6f;
                rectangleColorCoords[2] = 0.2f;
                rectangleColorCoords[3] = 1.0f;
                rectangleColorCoords[4] = 0.6f;
                rectangleColorCoords[5] = 0.2f;
                rectangleColorCoords[6] = 1.0f;
                rectangleColorCoords[7] = 0.6f;
                rectangleColorCoords[8] = 0.2f;
                rectangleColorCoords[9] = 1.0f;
                rectangleColorCoords[10] = 0.6f;
                rectangleColorCoords[11] = 0.2f;

                glBindBuffer(GL_ARRAY_BUFFER,
                    vbo_position_quad);
                glBufferData(GL_ARRAY_BUFFER,
                    4 * 3 * sizeof(float),
                    rectangleVertices,
                    GL_DYNAMIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                glBindBuffer(GL_ARRAY_BUFFER,
                    vbo_color_quad);
                glBufferData(GL_ARRAY_BUFFER,
                    4 * 3 * sizeof(float),
                    rectangleColorCoords,
                    GL_DYNAMIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                glDrawArrays(GL_TRIANGLE_FAN,
                    0,
                    4);

                // W
                rectangleVertices[0] = Plane1_X_Position - 0.27f;
                rectangleVertices[1] = 0.02f;
                rectangleVertices[2] = 0.0f;
                rectangleVertices[3] = Line_X_Position;
                rectangleVertices[4] = 0.02f;
                rectangleVertices[5] = 0.0f;
                rectangleVertices[6] = Line_X_Position;
                rectangleVertices[7] = -0.02f;
                rectangleVertices[8] = 0.0f;
                rectangleVertices[9] = Plane1_X_Position - 0.27f;
                rectangleVertices[10] = -0.02f;
                rectangleVertices[11] = 0.0f;

                rectangleColorCoords[0] = 1.0f;
                rectangleColorCoords[1] = 1.0f;
                rectangleColorCoords[2] = 1.0f;
                rectangleColorCoords[3] = 1.0f;
                rectangleColorCoords[4] = 1.0f;
                rectangleColorCoords[5] = 1.0f;
                rectangleColorCoords[6] = 1.0f;
                rectangleColorCoords[7] = 1.0f;
                rectangleColorCoords[8] = 1.0f;
                rectangleColorCoords[9] = 1.0f;
                rectangleColorCoords[10] = 1.0f;
                rectangleColorCoords[11] = 1.0f;

                glBindBuffer(GL_ARRAY_BUFFER,
                    vbo_position_quad);
                glBufferData(GL_ARRAY_BUFFER,
                    4 * 3 * sizeof(float),
                    rectangleVertices,
                    GL_DYNAMIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                glBindBuffer(GL_ARRAY_BUFFER,
                    vbo_color_quad);
                glBufferData(GL_ARRAY_BUFFER,
                    4 * 3 * sizeof(float),
                    rectangleColorCoords,
                    GL_DYNAMIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                glDrawArrays(GL_TRIANGLE_FAN,
                    0,
                    4);


                // G
                rectangleVertices[0] = Plane1_X_Position - 0.27f;
                rectangleVertices[1] = -0.02f;
                rectangleVertices[2] = 0.0f;
                rectangleVertices[3] = Line_X_Position;
                rectangleVertices[4] = -0.02f;
                rectangleVertices[5] = 0.0f;
                rectangleVertices[6] = Line_X_Position;
                rectangleVertices[7] = -0.06f;
                rectangleVertices[8] = 0.0f;
                rectangleVertices[9] = Plane1_X_Position - 0.27f;
                rectangleVertices[10] = -0.06f;
                rectangleVertices[11] = 0.0f;

                rectangleColorCoords[0] = 0.074f;
                rectangleColorCoords[1] = 0.533f;
                rectangleColorCoords[2] = 0.2f;
                rectangleColorCoords[3] = 0.074f;
                rectangleColorCoords[4] = 0.533f;
                rectangleColorCoords[5] = 0.2f;
                rectangleColorCoords[6] = 0.074f;
                rectangleColorCoords[7] = 0.533f;
                rectangleColorCoords[8] = 0.2f;
                rectangleColorCoords[9] = 0.074f;
                rectangleColorCoords[10] = 0.533f;
                rectangleColorCoords[11] = 0.2f;

                glBindBuffer(GL_ARRAY_BUFFER,
                    vbo_position_quad);
                glBufferData(GL_ARRAY_BUFFER,
                    4 * 3 * sizeof(float),
                    rectangleVertices,
                    GL_DYNAMIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                glBindBuffer(GL_ARRAY_BUFFER,
                    vbo_color_quad);
                glBufferData(GL_ARRAY_BUFFER,
                    4 * 3 * sizeof(float),
                    rectangleColorCoords,
                    GL_DYNAMIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                glDrawArrays(GL_TRIANGLE_FAN,
                    0,
                    4);

            }
            else {

                //R
                rectangleVertices[0] = 1.1f;
                rectangleVertices[1] = 0.06f;
                rectangleVertices[2] = 0.0f;
                rectangleVertices[3] = Line_X_Position;
                rectangleVertices[4] = 0.06f;
                rectangleVertices[5] = 0.0f;
                rectangleVertices[6] = Line_X_Position;
                rectangleVertices[7] = 0.02f;
                rectangleVertices[8] = 0.0f;
                rectangleVertices[9] = 1.1f;
                rectangleVertices[10] = 0.02f;
                rectangleVertices[11] = 0.0f;

                rectangleColorCoords[0] = 1.0f;
                rectangleColorCoords[1] = 0.6f;
                rectangleColorCoords[2] = 0.2f;
                rectangleColorCoords[3] = 1.0f;
                rectangleColorCoords[4] = 0.6f;
                rectangleColorCoords[5] = 0.2f;
                rectangleColorCoords[6] = 1.0f;
                rectangleColorCoords[7] = 0.6f;
                rectangleColorCoords[8] = 0.2f;
                rectangleColorCoords[9] = 1.0f;
                rectangleColorCoords[10] = 0.6f;
                rectangleColorCoords[11] = 0.2f;

                glBindBuffer(GL_ARRAY_BUFFER,
                    vbo_position_quad);
                glBufferData(GL_ARRAY_BUFFER,
                    4 * 3 * sizeof(float),
                    rectangleVertices,
                    GL_DYNAMIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                glBindBuffer(GL_ARRAY_BUFFER,
                    vbo_color_quad);
                glBufferData(GL_ARRAY_BUFFER,
                    4 * 3 * sizeof(float),
                    rectangleColorCoords,
                    GL_DYNAMIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                glDrawArrays(GL_TRIANGLE_FAN,
                    0,
                    4);

                // W
                rectangleVertices[0] = 1.1f;
                rectangleVertices[1] = 0.02f;
                rectangleVertices[2] = 0.0f;
                rectangleVertices[3] = Line_X_Position;
                rectangleVertices[4] = 0.02f;
                rectangleVertices[5] = 0.0f;
                rectangleVertices[6] = Line_X_Position;
                rectangleVertices[7] = -0.02f;
                rectangleVertices[8] = 0.0f;
                rectangleVertices[9] = 1.1f;
                rectangleVertices[10] = -0.02f;
                rectangleVertices[11] = 0.0f;

                rectangleColorCoords[0] = 1.0f;
                rectangleColorCoords[1] = 1.0f;
                rectangleColorCoords[2] = 1.0f;
                rectangleColorCoords[3] = 1.0f;
                rectangleColorCoords[4] = 1.0f;
                rectangleColorCoords[5] = 1.0f;
                rectangleColorCoords[6] = 1.0f;
                rectangleColorCoords[7] = 1.0f;
                rectangleColorCoords[8] = 1.0f;
                rectangleColorCoords[9] = 1.0f;
                rectangleColorCoords[10] = 1.0f;
                rectangleColorCoords[11] = 1.0f;

                glBindBuffer(GL_ARRAY_BUFFER,
                    vbo_position_quad);
                glBufferData(GL_ARRAY_BUFFER,
                    4 * 3 * sizeof(float),
                    rectangleVertices,
                    GL_DYNAMIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                glBindBuffer(GL_ARRAY_BUFFER,
                    vbo_color_quad);
                glBufferData(GL_ARRAY_BUFFER,
                    4 * 3 * sizeof(float),
                    rectangleColorCoords,
                    GL_DYNAMIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                glDrawArrays(GL_TRIANGLE_FAN,
                    0,
                    4);

                // G
                rectangleVertices[0] = 1.1f;
                rectangleVertices[1] = -0.02f;
                rectangleVertices[2] = 0.0f;
                rectangleVertices[3] = Line_X_Position;
                rectangleVertices[4] = -0.02f;
                rectangleVertices[5] = 0.0f;
                rectangleVertices[6] = Line_X_Position;
                rectangleVertices[7] = -0.06f;
                rectangleVertices[8] = 0.0f;
                rectangleVertices[9] = 1.1f;
                rectangleVertices[10] = -0.06f;
                rectangleVertices[11] = 0.0f;

                rectangleColorCoords[0] = 0.074f;
                rectangleColorCoords[1] = 0.533f;
                rectangleColorCoords[2] = 0.2f;
                rectangleColorCoords[3] = 0.074f;
                rectangleColorCoords[4] = 0.533f;
                rectangleColorCoords[5] = 0.2f;
                rectangleColorCoords[6] = 0.074f;
                rectangleColorCoords[7] = 0.533f;
                rectangleColorCoords[8] = 0.2f;
                rectangleColorCoords[9] = 0.074f;
                rectangleColorCoords[10] = 0.533f;
                rectangleColorCoords[11] = 0.2f;

                glBindBuffer(GL_ARRAY_BUFFER,
                    vbo_position_quad);
                glBufferData(GL_ARRAY_BUFFER,
                    4 * 3 * sizeof(float),
                    rectangleVertices,
                    GL_DYNAMIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                glBindBuffer(GL_ARRAY_BUFFER,
                    vbo_color_quad);
                glBufferData(GL_ARRAY_BUFFER,
                    4 * 3 * sizeof(float),
                    rectangleColorCoords,
                    GL_DYNAMIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                glDrawArrays(GL_TRIANGLE_FAN,
                    0,
                    4);
            }
            glBindVertexArray(0);

            break;
            //Plane
        case 'P':
            glBindVertexArray(vao_quad);

            //Smoke R
            rectangleVertices[0] = -0.24f;
            rectangleVertices[1] = 0.06f;
            rectangleVertices[2] = 0.0f;
            rectangleVertices[3] = -0.40f;
            rectangleVertices[4] = 0.06f;
            rectangleVertices[5] = 0.0f;
            rectangleVertices[6] = -0.40;
            rectangleVertices[7] = -0.02f;
            rectangleVertices[8] = 0.0f;
            rectangleVertices[9] = -0.24;
            rectangleVertices[10] = -0.02f;
            rectangleVertices[11] = 0.0f;

            rectangleColorCoords[0] = 1.0f;
            rectangleColorCoords[1] = 0.6f;
            rectangleColorCoords[2] = 0.2f;
            rectangleColorCoords[3] = 0.0f;
            rectangleColorCoords[4] = 0.0f;
            rectangleColorCoords[5] = 0.0f;
            rectangleColorCoords[6] = 0.0f;
            rectangleColorCoords[7] = 0.0f;
            rectangleColorCoords[8] = 0.0f;
            rectangleColorCoords[9] = 1.0f;
            rectangleColorCoords[10] = 0.6f;
            rectangleColorCoords[11] = 0.2f;

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_position_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleVertices,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_color_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleColorCoords,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLE_FAN,
                0,
                4);

            //Smoke W

            rectangleVertices[0] = -0.24f;
            rectangleVertices[1] = 0.02f;
            rectangleVertices[2] = 0.0f;
            rectangleVertices[3] = -0.40f;
            rectangleVertices[4] = 0.02f;
            rectangleVertices[5] = 0.0f;
            rectangleVertices[6] = -0.40;
            rectangleVertices[7] = -0.02f;
            rectangleVertices[8] = 0.0f;
            rectangleVertices[9] = -0.24;
            rectangleVertices[10] = -0.02f;
            rectangleVertices[11] = 0.0f;

            rectangleColorCoords[0] = 1.0f;
            rectangleColorCoords[1] = 1.0f;
            rectangleColorCoords[2] = 1.0f;
            rectangleColorCoords[3] = 0.0f;
            rectangleColorCoords[4] = 0.0f;
            rectangleColorCoords[5] = 0.0f;
            rectangleColorCoords[6] = 0.0f;
            rectangleColorCoords[7] = 0.0f;
            rectangleColorCoords[8] = 0.0f;
            rectangleColorCoords[9] = 1.0f;
            rectangleColorCoords[10] = 1.0f;
            rectangleColorCoords[11] = 1.0f;

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_position_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleVertices,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_color_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleColorCoords,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLE_FAN,
                0,
                4);


            //Smoke G

            rectangleVertices[0] = -0.24f;
            rectangleVertices[1] = -0.02f;
            rectangleVertices[2] = 0.0f;
            rectangleVertices[3] = -0.40f;
            rectangleVertices[4] = -0.02f;
            rectangleVertices[5] = 0.0f;
            rectangleVertices[6] = -0.40;
            rectangleVertices[7] = -0.06f;
            rectangleVertices[8] = 0.0f;
            rectangleVertices[9] = -0.24;
            rectangleVertices[10] = -0.06f;
            rectangleVertices[11] = 0.0f;

            rectangleColorCoords[0] = 0.074f;
            rectangleColorCoords[1] = 0.533f;
            rectangleColorCoords[2] = 0.2f;
            rectangleColorCoords[3] = 0.0f;
            rectangleColorCoords[4] = 0.0f;
            rectangleColorCoords[5] = 0.0f;
            rectangleColorCoords[6] = 0.0f;
            rectangleColorCoords[7] = 0.0f;
            rectangleColorCoords[8] = 0.0f;
            rectangleColorCoords[9] = 0.074f;
            rectangleColorCoords[10] = 0.533f;
            rectangleColorCoords[11] = 0.2f;

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_position_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleVertices,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_color_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleColorCoords,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLE_FAN,
                0,
                4);





            rectangleVertices[0] = 0.20f;
            rectangleVertices[1] = 0.07f;
            rectangleVertices[2] = 0.0f;
            rectangleVertices[3] = -0.20f;
            rectangleVertices[4] = 0.07f;
            rectangleVertices[5] = 0.0f;
            rectangleVertices[6] = -0.20f;
            rectangleVertices[7] = -0.07f;
            rectangleVertices[8] = 0.0f;
            rectangleVertices[9] = 0.20;
            rectangleVertices[10] = -0.07f;
            rectangleVertices[11] = 0.0f;

            rectangleColorCoords[0] = 0.729f;
            rectangleColorCoords[1] = 0.886f;
            rectangleColorCoords[2] = 0.933f;
            rectangleColorCoords[3] = 0.729f;
            rectangleColorCoords[4] = 0.886f;
            rectangleColorCoords[5] = 0.933f;
            rectangleColorCoords[6] = 0.729f;
            rectangleColorCoords[7] = 0.886f;
            rectangleColorCoords[8] = 0.933f;
            rectangleColorCoords[9] = 0.729f;
            rectangleColorCoords[10] = 0.886f;
            rectangleColorCoords[11] = 0.933f;

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_position_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleVertices,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_color_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleColorCoords,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLE_FAN,
                0,
                4);


            //Back Wings

            rectangleVertices[0] = -0.10f;
            rectangleVertices[1] = 0.0f;
            rectangleVertices[2] = 0.0f;
            rectangleVertices[3] = -0.27f;
            rectangleVertices[4] = 0.12f;
            rectangleVertices[5] = 0.0f;
            rectangleVertices[6] = -0.31f;
            rectangleVertices[7] = 0.12f;
            rectangleVertices[8] = 0.0f;
            rectangleVertices[9] = -0.24f;
            rectangleVertices[10] = 0.0f;
            rectangleVertices[11] = 0.0f;

            rectangleColorCoords[0] = 0.729f;
            rectangleColorCoords[1] = 0.886f;
            rectangleColorCoords[2] = 0.933f;
            rectangleColorCoords[3] = 0.729f;
            rectangleColorCoords[4] = 0.886f;
            rectangleColorCoords[5] = 0.933f;
            rectangleColorCoords[6] = 0.729f;
            rectangleColorCoords[7] = 0.886f;
            rectangleColorCoords[8] = 0.933f;
            rectangleColorCoords[9] = 0.729f;
            rectangleColorCoords[10] = 0.886f;
            rectangleColorCoords[11] = 0.933f;

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_position_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleVertices,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_color_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleColorCoords,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLE_FAN,
                0,
                4);

            rectangleVertices[0] = -0.10f;
            rectangleVertices[1] = 0.0f;
            rectangleVertices[2] = 0.0f;
            rectangleVertices[3] = -0.27f;
            rectangleVertices[4] = -0.12f;
            rectangleVertices[5] = 0.0f;
            rectangleVertices[6] = -0.31f;
            rectangleVertices[7] = -0.12f;
            rectangleVertices[8] = 0.0f;
            rectangleVertices[9] = -0.24f;
            rectangleVertices[10] = 0.0f;
            rectangleVertices[11] = 0.0f;

            rectangleColorCoords[0] = 0.729f;
            rectangleColorCoords[1] = 0.886f;
            rectangleColorCoords[2] = 0.933f;
            rectangleColorCoords[3] = 0.729f;
            rectangleColorCoords[4] = 0.886f;
            rectangleColorCoords[5] = 0.933f;
            rectangleColorCoords[6] = 0.729f;
            rectangleColorCoords[7] = 0.886f;
            rectangleColorCoords[8] = 0.933f;
            rectangleColorCoords[9] = 0.729f;
            rectangleColorCoords[10] = 0.886f;
            rectangleColorCoords[11] = 0.933f;

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_position_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleVertices,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_color_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleColorCoords,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLE_FAN,
                0,
                4);

            //Letter I
            rectangleVertices[0] = -0.05f;
            rectangleVertices[1] = 0.05f;
            rectangleVertices[2] = 0.0f;
            rectangleVertices[3] = -0.07f;
            rectangleVertices[4] = 0.05f;
            rectangleVertices[5] = 0.0f;
            rectangleVertices[6] = -0.07f;
            rectangleVertices[7] = -0.05f;
            rectangleVertices[8] = 0.0f;
            rectangleVertices[9] = -0.05f;
            rectangleVertices[10] = -0.05f;
            rectangleVertices[11] = 0.0f;

            rectangleColorCoords[0] = 1.0f;
            rectangleColorCoords[1] = 0.6f;
            rectangleColorCoords[2] = 0.2f;
            rectangleColorCoords[3] = 1.0f;
            rectangleColorCoords[4] = 0.6f;
            rectangleColorCoords[5] = 0.2f;
            rectangleColorCoords[6] = 0.074f;
            rectangleColorCoords[7] = 0.533f;
            rectangleColorCoords[8] = 0.2f;
            rectangleColorCoords[9] = 0.074f;
            rectangleColorCoords[10] = 0.533f;
            rectangleColorCoords[11] = 0.2f;

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_position_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleVertices,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_color_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleColorCoords,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLE_FAN,
                0,
                4);


            //Letter A
            rectangleVertices[0] = 0.00f;
            rectangleVertices[1] = 0.05f;
            rectangleVertices[2] = 0.0f;
            rectangleVertices[3] = -0.02f;
            rectangleVertices[4] = 0.05f;
            rectangleVertices[5] = 0.0f;
            rectangleVertices[6] = -0.04f;
            rectangleVertices[7] = -0.05f;
            rectangleVertices[8] = 0.0f;
            rectangleVertices[9] = -0.02f;
            rectangleVertices[10] = -0.05f;
            rectangleVertices[11] = 0.0f;

            rectangleColorCoords[0] = 0.0f;
            rectangleColorCoords[1] = 0.0f;
            rectangleColorCoords[2] = 0.530f;
            rectangleColorCoords[3] = 0.0f;
            rectangleColorCoords[4] = 0.0f;
            rectangleColorCoords[5] = 0.530f;
            rectangleColorCoords[6] = 0.0f;
            rectangleColorCoords[7] = 0.0f;
            rectangleColorCoords[8] = 0.530f;
            rectangleColorCoords[9] = 0.0f;
            rectangleColorCoords[10] = 0.0f;
            rectangleColorCoords[11] = 0.530f;

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_position_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleVertices,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_color_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleColorCoords,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLE_FAN,
                0,
                4);

            rectangleVertices[0] = 0.00f;
            rectangleVertices[1] = 0.05f;
            rectangleVertices[2] = 0.0f;
            rectangleVertices[3] = 0.02f;
            rectangleVertices[4] = 0.05f;
            rectangleVertices[5] = 0.0f;
            rectangleVertices[6] = 0.04f;
            rectangleVertices[7] = -0.05f;
            rectangleVertices[8] = 0.0f;
            rectangleVertices[9] = 0.02f;
            rectangleVertices[10] = -0.05f;
            rectangleVertices[11] = 0.0f;

            rectangleColorCoords[0] = 0.0f;
            rectangleColorCoords[1] = 0.0f;
            rectangleColorCoords[2] = 0.530f;
            rectangleColorCoords[3] = 0.0f;
            rectangleColorCoords[4] = 0.0f;
            rectangleColorCoords[5] = 0.530f;
            rectangleColorCoords[6] = 0.0f;
            rectangleColorCoords[7] = 0.0f;
            rectangleColorCoords[8] = 0.530f;
            rectangleColorCoords[9] = 0.0f;
            rectangleColorCoords[10] = 0.0f;
            rectangleColorCoords[11] = 0.530f;

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_position_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleVertices,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_color_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleColorCoords,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLE_FAN,
                0,
                4);


            rectangleVertices[0] = 0.02f;
            rectangleVertices[1] = 0.01f;
            rectangleVertices[2] = 0.0f;
            rectangleVertices[3] = -0.02f;
            rectangleVertices[4] = 0.01f;
            rectangleVertices[5] = 0.0f;
            rectangleVertices[6] = -0.02f;
            rectangleVertices[7] = -0.01f;
            rectangleVertices[8] = 0.0f;
            rectangleVertices[9] = 0.02f;
            rectangleVertices[10] = -0.01f;
            rectangleVertices[11] = 0.0f;

            rectangleColorCoords[0] = 0.0f;
            rectangleColorCoords[1] = 0.0f;
            rectangleColorCoords[2] = 0.530f;
            rectangleColorCoords[3] = 0.0f;
            rectangleColorCoords[4] = 0.0f;
            rectangleColorCoords[5] = 0.530f;
            rectangleColorCoords[6] = 0.0f;
            rectangleColorCoords[7] = 0.0f;
            rectangleColorCoords[8] = 0.530f;
            rectangleColorCoords[9] = 0.0f;
            rectangleColorCoords[10] = 0.0f;
            rectangleColorCoords[11] = 0.530f;

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_position_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleVertices,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_color_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleColorCoords,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLE_FAN,
                0,
                4);


            //Letter F

            rectangleVertices[0] = 0.06f;
            rectangleVertices[1] = 0.05f;
            rectangleVertices[2] = 0.0f;
            rectangleVertices[3] = 0.04f;
            rectangleVertices[4] = 0.05f;
            rectangleVertices[5] = 0.0f;
            rectangleVertices[6] = 0.04f;
            rectangleVertices[7] = -0.05f;
            rectangleVertices[8] = 0.0f;
            rectangleVertices[9] = 0.06f;
            rectangleVertices[10] = -0.05f;
            rectangleVertices[11] = 0.0f;

            rectangleColorCoords[0] = 0.074f;
            rectangleColorCoords[1] = 0.533f;
            rectangleColorCoords[2] = 0.2f;
            rectangleColorCoords[3] = 0.074f;
            rectangleColorCoords[4] = 0.533f;
            rectangleColorCoords[5] = 0.2f;
            rectangleColorCoords[6] = 0.074f;
            rectangleColorCoords[7] = 0.533f;
            rectangleColorCoords[8] = 0.2f;
            rectangleColorCoords[9] = 0.074f;
            rectangleColorCoords[10] = 0.533f;
            rectangleColorCoords[11] = 0.2f;

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_position_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleVertices,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_color_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleColorCoords,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLE_FAN,
                0,
                4);

            rectangleVertices[0] = 0.09f;
            rectangleVertices[1] = 0.05f;
            rectangleVertices[2] = 0.0f;
            rectangleVertices[3] = 0.06f;
            rectangleVertices[4] = 0.05f;
            rectangleVertices[5] = 0.0f;
            rectangleVertices[6] = 0.06f;
            rectangleVertices[7] = 0.035f;
            rectangleVertices[8] = 0.0f;
            rectangleVertices[9] = 0.09f;
            rectangleVertices[10] = 0.035;
            rectangleVertices[11] = 0.0f;

            rectangleColorCoords[0] = 0.074f;
            rectangleColorCoords[1] = 0.533f;
            rectangleColorCoords[2] = 0.2f;
            rectangleColorCoords[3] = 0.074f;
            rectangleColorCoords[4] = 0.533f;
            rectangleColorCoords[5] = 0.2f;
            rectangleColorCoords[6] = 0.074f;
            rectangleColorCoords[7] = 0.533f;
            rectangleColorCoords[8] = 0.2f;
            rectangleColorCoords[9] = 0.074f;
            rectangleColorCoords[10] = 0.533f;
            rectangleColorCoords[11] = 0.2f;

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_position_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleVertices,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_color_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleColorCoords,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLE_FAN,
                0,
                4);

            rectangleVertices[0] = 0.09f;
            rectangleVertices[1] = 0.02f;
            rectangleVertices[2] = 0.0f;
            rectangleVertices[3] = 0.06f;
            rectangleVertices[4] = 0.02f;
            rectangleVertices[5] = 0.0f;
            rectangleVertices[6] = 0.06f;
            rectangleVertices[7] = 0.005f;
            rectangleVertices[8] = 0.0f;
            rectangleVertices[9] = 0.09f;
            rectangleVertices[10] = 0.005;
            rectangleVertices[11] = 0.0f;

            rectangleColorCoords[0] = 0.074f;
            rectangleColorCoords[1] = 0.533f;
            rectangleColorCoords[2] = 0.2f;
            rectangleColorCoords[3] = 0.074f;
            rectangleColorCoords[4] = 0.533f;
            rectangleColorCoords[5] = 0.2f;
            rectangleColorCoords[6] = 0.074f;
            rectangleColorCoords[7] = 0.533f;
            rectangleColorCoords[8] = 0.2f;
            rectangleColorCoords[9] = 0.074f;
            rectangleColorCoords[10] = 0.533f;
            rectangleColorCoords[11] = 0.2f;

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_position_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleVertices,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_color_quad);
            glBufferData(GL_ARRAY_BUFFER,
                4 * 3 * sizeof(float),
                rectangleColorCoords,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLE_FAN,
                0,
                4);

            glBindVertexArray(0);

            GLfloat triangleVertices[] = {
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            };

            GLfloat triangleColorCoords[] = {
                0.729f,
                0.886f,
                0.933f,
                0.729f,
                0.886f,
                0.933f,
                0.729f,
                0.886f,
                0.933f
            };


            glBindVertexArray(vao_triangle);
            //Upper Wing
            triangleVertices[0] = -0.05f;
            triangleVertices[1] = 0.07f;
            triangleVertices[2] = 0.0f;
            triangleVertices[3] = 0.05f;
            triangleVertices[4] = 0.07f;
            triangleVertices[5] = 0.0f;
            triangleVertices[6] = -0.15f;
            triangleVertices[7] = 0.22f;
            triangleVertices[8] = 0.0f;

            /*triangleColorCoords[0] = 0.729f;
            triangleColorCoords[1] = 0.886f;
            triangleColorCoords[2] = 0.933f;
            triangleColorCoords[3] = 1.0f;
            triangleColorCoords[4] = 0.6f;
            triangleColorCoords[5] = 0.2f;
            triangleColorCoords[6] = 0.074f;
            triangleColorCoords[7] = 0.533f;
            triangleColorCoords[8] = 0.2f;*/

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_position_triangle);
            glBufferData(GL_ARRAY_BUFFER,
                3 * 3 * sizeof(float),
                triangleVertices,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_color_triangle);
            glBufferData(GL_ARRAY_BUFFER,
                3 * 3 * sizeof(float),
                triangleColorCoords,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLES,
                0,
                3);

            glBindVertexArray(0);


            glBindVertexArray(vao_triangle);
            triangleVertices[0] = -0.05f;
            triangleVertices[1] = -0.07f;
            triangleVertices[2] = 0.0f;
            triangleVertices[3] = 0.05f;
            triangleVertices[4] = -0.07f;
            triangleVertices[5] = 0.0f;
            triangleVertices[6] = -0.15f;
            triangleVertices[7] = -0.22f;
            triangleVertices[8] = 0.0f;

            /*triangleColorCoords[0] = 0.729f;
            triangleColorCoords[1] = 0.886f;
            triangleColorCoords[2] = 0.933f;
            triangleColorCoords[3] = 1.0f;
            triangleColorCoords[4] = 0.6f;
            triangleColorCoords[5] = 0.2f;
            triangleColorCoords[6] = 0.074f;
            triangleColorCoords[7] = 0.533f;
            triangleColorCoords[8] = 0.2f;*/

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_position_triangle);
            glBufferData(GL_ARRAY_BUFFER,
                3 * 3 * sizeof(float),
                triangleVertices,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_color_triangle);
            glBufferData(GL_ARRAY_BUFFER,
                3 * 3 * sizeof(float),
                triangleColorCoords,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLES,
                0,
                3);

            glBindVertexArray(0);

            //Lower Wing

            glBindVertexArray(vao_triangle);
            triangleVertices[0] = 0.05f;
            triangleVertices[1] = 0.07f;
            triangleVertices[2] = 0.0f;
            triangleVertices[3] = -0.10f;
            triangleVertices[4] = 0.22f;
            triangleVertices[5] = 0.0f;
            triangleVertices[6] = -0.15f;
            triangleVertices[7] = 0.22f;
            triangleVertices[8] = 0.0f;

            /*triangleColorCoords[0] = 0.729f;
            triangleColorCoords[1] = 0.886f;
            triangleColorCoords[2] = 0.933f;
            triangleColorCoords[3] = 1.0f;
            triangleColorCoords[4] = 0.6f;
            triangleColorCoords[5] = 0.2f;
            triangleColorCoords[6] = 0.074f;
            triangleColorCoords[7] = 0.533f;
            triangleColorCoords[8] = 0.2f;*/

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_position_triangle);
            glBufferData(GL_ARRAY_BUFFER,
                3 * 3 * sizeof(float),
                triangleVertices,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_color_triangle);
            glBufferData(GL_ARRAY_BUFFER,
                3 * 3 * sizeof(float),
                triangleColorCoords,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLES,
                0,
                3);

            glBindVertexArray(0);


            glBindVertexArray(vao_triangle);
            triangleVertices[0] = 0.05f;
            triangleVertices[1] = -0.07f;
            triangleVertices[2] = 0.0f;
            triangleVertices[3] = -0.10f;
            triangleVertices[4] = -0.22f;
            triangleVertices[5] = 0.0f;
            triangleVertices[6] = -0.15f;
            triangleVertices[7] = -0.22f;
            triangleVertices[8] = 0.0f;

            /*triangleColorCoords[0] = 0.729f;
            triangleColorCoords[1] = 0.886f;
            triangleColorCoords[2] = 0.933f;
            triangleColorCoords[3] = 1.0f;
            triangleColorCoords[4] = 0.6f;
            triangleColorCoords[5] = 0.2f;
            triangleColorCoords[6] = 0.074f;
            triangleColorCoords[7] = 0.533f;
            triangleColorCoords[8] = 0.2f;*/

                glBindBuffer(GL_ARRAY_BUFFER,
                             vbo_position_triangle);
                glBufferData(GL_ARRAY_BUFFER,
                             3 * 3 * sizeof(float),
                             triangleVertices,
                             GL_DYNAMIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                
                glBindBuffer(GL_ARRAY_BUFFER,
                vbo_color_triangle);
            glBufferData(GL_ARRAY_BUFFER,
                3 * 3 * sizeof(float),
                triangleColorCoords,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLES,
                0,
                3);

            glBindVertexArray(0);



            //glBegin(GL_TRIANGLES);

    ////Upper Wing
    //glVertex3f(-0.05f, 0.07f, 0.0f);
    //glVertex3f(0.05f, 0.07f, 0.0f);
    //glVertex3f(-0.15f, 0.22f, 0.0f);

    //glVertex3f(-0.05f, -0.07f, 0.0f);
    //glVertex3f(0.05f, -0.07f, 0.0f);
    //glVertex3f(-0.15f, -0.22f, 0.0f);
    ////Lower Wing
    //glVertex3f(0.05f, 0.07f, 0.0f);
    //glVertex3f(-0.10f, 0.22f, 0.0f);
    //glVertex3f(-0.15f, 0.22f, 0.0f);

    //glVertex3f(0.05f, -0.07f, 0.0f);
    //glVertex3f(-0.10f, -0.22f, 0.0f);
    //glVertex3f(-0.15f, -0.22f, 0.0f);

    //glEnd();


    ////Front Circle

    //glBegin(GL_POINTS);

    ////glColor3f(1.0f, 1.0f, 1.0f);

    //for (float radius = 0.0f; radius <= 0.07f; radius = radius + 0.001f) {
    //    for (angle = 0.00f; angle < 2 * 3.14159265; angle = angle + 0.01f) {
    //        glVertex3f(cos(angle)*radius + 0.21f, sin(angle)*radius, 0.0f);
    //    }
    //}
    //glEnd();

            /*glColor3f(0.729f, 0.886f, 0.933f);

            if (!initCircle) {
                for (float radius = 0.0f; radius <= 0.07f; radius = radius + 0.001f) {
                    for (angle = 0.00f; angle < 2 * 3.14159265; angle = angle + 0.01f) {
                        glVertex3f(cos(angle)*radius + 0.21f, sin(angle)*radius, 0.0f);
                    }
                }
            }

            glBindVertexArray(vao_point);

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_position_point);
            glBufferData(GL_ARRAY_BUFFER,
                3 * 3 * sizeof(float),
                triangleVertices,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_color_point);
            glBufferData(GL_ARRAY_BUFFER,
                3 * 3 * sizeof(float),
                triangleColorCoords,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLES,
                0,
                3);

            glBindVertexArray(0);
    */

            //planeFrontSphere.drawWithColor();

                
                
                
                ///////////////////////////
                
                
           /*     glBindVertexArray(vao_sphere);
    
                unsigned int ind[planeFrontSphere.indices.size()];
                for(int i=0;i<planeFrontSphere.indices.size();i++){
                    ind[i]=planeFrontSphere.indices.at(i);
                }
                
                
                //glDrawElements(GL_TRIANGLES, (unsigned int)planeFrontSphere.indices.size(), GL_UNSIGNED_INT, planeFrontSphere.indices.data());
                glDrawElements(GL_TRIANGLES, (unsigned int)planeFrontSphere.indices.size(), GL_UNSIGNED_INT, ind);
                glBindVertexArray(0);

              ////////////////////
             */
                glBindVertexArray(vaosphere);
                   glDrawArrays(GL_TRIANGLE_FAN,
                   0,
                   (int)(planeFrontSphere.vertices.size()/3));
                glBindVertexArray(0);
                   
                
            break;

        }
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

