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
    
        GLuint vao_pyramid;
    GLuint vao_cube;

    GLuint vbo_position_pyramid;
    GLuint vbo_position_cube;

    GLuint vbo_color_pyramid;
    GLuint vbo_color_cube;

    GLuint mvpUniform;
    
    GLint gVertexShaderObject;
    GLint gFragementShaderObject;
    GLint gShaderProgramObject;
    
    vmath::mat4 perspectiveProjectionMatrix;
    
    GLfloat anglePyramid;
    GLfloat angleCube;
    
    
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


        //Vertices
        const GLfloat pyramidVertices[] = {
            0.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,
            0.0f, 1.0f, 0.0f,
            1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, -1.0f,
            0.0f, 1.0f, 0.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            0.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f
        };

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

        const GLfloat pyramidColors[] = {
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f
        };

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

        //Create vao
        //Save everying in single set
        glGenVertexArrays(1, &vao_pyramid);

        glBindVertexArray(vao_pyramid);


        //TRIANGLE
        //Generate Buffer
        glGenBuffers(1, &vbo_position_pyramid);
        //Bind Generated Buffer
        glBindBuffer(GL_ARRAY_BUFFER,
            vbo_position_pyramid);
        //Fill Buffer
        glBufferData(GL_ARRAY_BUFFER,
            sizeof(pyramidVertices),
            pyramidVertices,
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

        glGenBuffers(1, &vbo_color_pyramid);
        //Bind Generated Buffer
        glBindBuffer(GL_ARRAY_BUFFER,
            vbo_color_pyramid);
        //Fill Buffer
        glBufferData(GL_ARRAY_BUFFER,
            sizeof(pyramidColors),
            pyramidColors,
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

        //Unbind array
        glBindVertexArray(0);

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
        //Unbind array
        glBindVertexArray(0);


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
            vmath::mat4 scaleMatrix = vmath::mat4::identity();
    
            vmath::mat4 rotationMatrix = vmath::mat4::identity();
        //Transformation

            translationMatrix = vmath::translate(-1.5f, 0.0f, -6.0f);
            rotationMatrix = vmath::rotate(anglePyramid, 0.0f, 1.0f, 0.0f);

            //Matrix Multiplication
            modelViewMatrix = translationMatrix * rotationMatrix;

            modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

            //Send necessary matrices to shader in resp. Uniforms

            glUniformMatrix4fv(mvpUniform,
                1,
                GL_FALSE,
                modelViewProjectionMatrix);

            //Bind with vao

            glBindVertexArray(vao_pyramid);

            //Bind with textures if any

            //Draw

            glDrawArrays(GL_TRIANGLES,
                0,
                12);

            //Unbind vao

            glBindVertexArray(0);

            //CUBE
            //Initialize matrices

            modelViewMatrix = vmath::mat4::identity();
            modelViewProjectionMatrix = vmath::mat4::identity();
            translationMatrix = vmath::mat4::identity();
            rotationMatrix = vmath::mat4::identity();
            scaleMatrix = vmath::mat4::identity();

            //Transformation

            translationMatrix = vmath::translate(1.5f, 0.0f, -6.0f);
            scaleMatrix = vmath::scale(0.75f, 0.75f, 0.75f);
            rotationMatrix = vmath::rotate(angleCube,angleCube,angleCube);

            //Matrix Multiplication

            modelViewMatrix = translationMatrix * scaleMatrix * rotationMatrix;


            modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

            //Send necessary matrices to shader in resp. Uniforms

            glUniformMatrix4fv(mvpUniform,
                1,
                GL_FALSE,
                modelViewProjectionMatrix);

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

            glUseProgram(0);
            
            CGLFlushDrawable((CGLContextObj)[[self openGLContext]CGLContextObj]);
            CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
            
            anglePyramid = anglePyramid + 0.2f;
            if (anglePyramid > 360.0f)
                anglePyramid = 0.0f;
            angleCube = angleCube + 0.2f;
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
     if (vbo_position_pyramid) {
        glDeleteBuffers(1, &vbo_position_pyramid);
        vbo_position_pyramid = 0;
    }

    if (vbo_color_pyramid) {
        glDeleteBuffers(1, &vbo_color_pyramid);
        vbo_color_pyramid = 0;
    }

    if (vao_pyramid) {
        glDeleteVertexArrays(1, &vao_pyramid);
        vao_pyramid = 0;
    }
    if (vbo_color_cube) {
        glDeleteBuffers(1, &vbo_color_cube);
        vbo_color_cube = 0;
    }


    if (vbo_position_cube) {
        glDeleteBuffers(1, &vbo_position_cube);
        vbo_position_cube = 0;
    }
    if (vao_cube) {
        glDeleteVertexArrays(1, &vao_cube);
        vao_cube = 0;
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

