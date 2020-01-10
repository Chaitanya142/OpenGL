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
    
    GLuint vao;

    GLuint vbo_position;

    GLuint vbo_color;

    GLuint mvpUniform;
    
    GLint gVertexShaderObject;
    GLint gFragementShaderObject;
    GLint gShaderProgramObject;
    
    vmath::mat4 perspectiveProjectionMatrix;

    GLfloat D_R_OrangeColor;
    GLfloat D_G_OrangeColor ;
    GLfloat D_B_OrangeColor ;
    GLfloat D_R_GreenColor ;
    GLfloat D_G_GreenColor ;
    GLfloat D_B_GreenColor ;
    
    GLfloat rectangleVertices[12] ;
    GLfloat rectangleColorCoords[12] ;

    GLint stage;

    GLfloat I_1_XPosition ; //-2.25f;
    GLfloat A_XPosition;//2.25f;
    GLfloat N_YPosition;//2.25f;
    GLfloat I_2_YPosition;//-2.25f;
    
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
    
   /*  rectangleVertices[] = {
        0.275f, 1.0f, 0.0f,
        0.225f, 1.0f, 0.0f,
        0.225f, -1.0f, 0.0f,
        0.275f, -1.0f, 0.0f
    };

    GLfloat rectangleColorCoords[] = {
        1.0f, 0.6f, 0.2f,
        1.0f, 0.6f, 0.2f,
        0.074f, 0.533f, 0.2f,
        0.074f, 0.533f, 0.2f
    };*/

     stage = 5;

     I_1_XPosition = -1.5f; //-2.25f;
     A_XPosition = 0.75f;//2.25f;
     N_YPosition = 0.00f;//2.25f;
     I_2_YPosition = 0.00f;//-2.25f;
    
    

        glGenVertexArrays(1, &vao);

        glBindVertexArray(vao);

        //Generate Buffer
        glGenBuffers(1, &vbo_position);
        //Bind Generated Buffer
        glBindBuffer(GL_ARRAY_BUFFER,
            vbo_position);
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
        glGenBuffers(1, &vbo_color);
        //Bind Generated Buffer
        glBindBuffer(GL_ARRAY_BUFFER,
            vbo_color);
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

            glBindVertexArray(vao);

            //Bind with textures if any

            //Draw

            //DrawLetter('I');
            [self DrawLetter:'I'];

            //Unbind vao

            glBindVertexArray(0);
            
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

            glBindVertexArray(vao);

            //Bind with textures if any

            //Draw

            //DrawLetter('N');
            [self DrawLetter:'N'];

            //Unbind vao

            glBindVertexArray(0);
            
            }

        if(stage>2)
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

            glBindVertexArray(vao);

            //Bind with textures if any

            //Draw

            //DrawLetter('D');
            [self DrawLetter:'D'];
            //Unbind vao

            glBindVertexArray(0);
            
            }

        if(stage>2)
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

            glBindVertexArray(vao);

            //Bind with textures if any

            //Draw

            //DrawLetter('I');
            [self DrawLetter:'I'];
            //Unbind vao

            glBindVertexArray(0);
            
            }


        if(stage>2)
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

            glBindVertexArray(vao);

            //Bind with textures if any

            //Draw
            [self DrawLetter:'A'];
            [self DrawLetter:'T'];
            // DrawLetter('A');
            // DrawLetter('T');

            //Unbind vao

            glBindVertexArray(0);
            
            }

            glUseProgram(0);
    
CGLFlushDrawable((CGLContextObj)[[self openGLContext]CGLContextObj]);
CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);

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
  if (vbo_color) {
        glDeleteBuffers(1, &vbo_color);
        vbo_color = 0;
    }

    if (vbo_position) {
        glDeleteBuffers(1, &vbo_position);
        vbo_position = 0;
    }
    if (vao) {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
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
            vbo_position);
        glBufferData(GL_ARRAY_BUFFER,
            4 * 3 * sizeof(float),
            rectangleVertices,
            GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ARRAY_BUFFER,
            vbo_color);
        glBufferData(GL_ARRAY_BUFFER,
            4 * 3 * sizeof(float),
            rectangleColorCoords,
            GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLE_FAN,
            0,
            4);

        break;

    case 'N':

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
            vbo_position);
        glBufferData(GL_ARRAY_BUFFER,
            4 * 3 * sizeof(float),
            rectangleVertices,
            GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ARRAY_BUFFER,
            vbo_color);
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
            vbo_position);
        glBufferData(GL_ARRAY_BUFFER,
            4 * 3 * sizeof(float),
            rectangleVertices,
            GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ARRAY_BUFFER,
            vbo_color);
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
            vbo_position);
        glBufferData(GL_ARRAY_BUFFER,
            4 * 3 * sizeof(float),
            rectangleVertices,
            GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ARRAY_BUFFER,
            vbo_color);
        glBufferData(GL_ARRAY_BUFFER,
            4 * 3 * sizeof(float),
            rectangleColorCoords,
            GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLE_FAN,
            0,
            4);



        break;

    case 'D':

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
            vbo_position);
        glBufferData(GL_ARRAY_BUFFER,
            4 * 3 * sizeof(float),
            rectangleVertices,
            GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ARRAY_BUFFER,
            vbo_color);
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
            vbo_position);
        glBufferData(GL_ARRAY_BUFFER,
            4 * 3 * sizeof(float),
            rectangleVertices,
            GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ARRAY_BUFFER,
            vbo_color);
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
            vbo_position);
        glBufferData(GL_ARRAY_BUFFER,
            4 * 3 * sizeof(float),
            rectangleVertices,
            GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ARRAY_BUFFER,
            vbo_color);
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
            vbo_position);
        glBufferData(GL_ARRAY_BUFFER,
            4 * 3 * sizeof(float),
            rectangleVertices,
            GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ARRAY_BUFFER,
            vbo_color);
        glBufferData(GL_ARRAY_BUFFER,
            4 * 3 * sizeof(float),
            rectangleColorCoords,
            GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLE_FAN,
            0,
            4);

        break;
    case 'A':
        
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
            vbo_position);
        glBufferData(GL_ARRAY_BUFFER,
            4 * 3 * sizeof(float),
            rectangleVertices,
            GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ARRAY_BUFFER,
            vbo_color);
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
            vbo_position);
        glBufferData(GL_ARRAY_BUFFER,
            4 * 3 * sizeof(float),
            rectangleVertices,
            GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ARRAY_BUFFER,
            vbo_color);
        glBufferData(GL_ARRAY_BUFFER,
            4 * 3 * sizeof(float),
            rectangleColorCoords,
            GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLE_FAN,
            0,
            4);

    case 'T':

        
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
            vbo_position);
        glBufferData(GL_ARRAY_BUFFER,
            4 * 3 * sizeof(float),
            rectangleVertices,
            GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ARRAY_BUFFER,
            vbo_color);
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
            vbo_position);
        glBufferData(GL_ARRAY_BUFFER,
            4 * 3 * sizeof(float),
            rectangleVertices,
            GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ARRAY_BUFFER,
            vbo_color);
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
            vbo_position);
        glBufferData(GL_ARRAY_BUFFER,
            4 * 3 * sizeof(float),
            rectangleVertices,
            GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ARRAY_BUFFER,
            vbo_color);
        glBufferData(GL_ARRAY_BUFFER,
            4 * 3 * sizeof(float),
            rectangleColorCoords,
            GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLE_FAN,
            0,
            4);


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

