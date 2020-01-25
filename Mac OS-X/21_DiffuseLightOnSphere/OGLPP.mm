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
   
    Sphere planeFrontSphere;
    

    GLuint mvUniform;
    GLuint pUniform;
    GLuint lKeyIsPressedUniform;
    GLuint ldUniform;
    GLuint kdUniform;
    GLuint lightPositionUniform;

    
    vmath::mat4 perspectiveProjectionMatrix;
    GLuint gb_Animation;
    GLuint gb_Lighting;

    GLint gShaderProgramObject;
    GLuint gVertexShaderObject;
    GLuint gFragementShaderObject;
    
    GLfloat angleCube;
    
    
    GLuint vaosphere;
    GLuint vbospherepos;
    GLuint vbospherenor;
    GLuint vbo_sphere_element;
    
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
    
    gb_Animation=0;
    gb_Lighting=0;
    angleCube=0.0f;

[[self openGLContext]setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];

    //Vertex Shader
    //Define Shader Object
    gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

    //Vertex Shader Code
    const GLchar* vertextShaderSourceCode =
        "#version 410 " \
        "\n " \
       "in vec4 vPosition;" \
        "in vec3 vNormal;" \
        "uniform mat4 u_mv_matrix;" \
        "uniform mat4 u_p_matrix;" \
        "uniform int u_lkeyispressed;" \
        "uniform vec3 u_ld;" \
        "uniform vec3 u_kd;" \
        "uniform vec4 u_lightposition;" \
        "out vec3 diffusecolor;" \
        "void main(void)" \
        "{" \
        "gl_Position=u_p_matrix*u_mv_matrix * vPosition;" \
        "if(u_lkeyispressed==1)"\
        "{"\
        "vec4 eyecoordinates=u_mv_matrix*vPosition;"\
        "mat3 normalmatrix=mat3(transpose(inverse(u_mv_matrix)));"\
        "vec3 tnorm=normalize(normalmatrix*vNormal);"\
        "vec3 s=normalize(vec3(u_lightposition)-eyecoordinates.xyz);"\
        "diffusecolor=u_ld*u_kd*max(dot(s,tnorm),0.0);"\
        "};"\
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
        "in vec3 diffusecolor;" \
        "uniform int u_lkeyispressed;" \
        "void main(void)" \
        "{" \
        "if(u_lkeyispressed==1)"\
        "{"\
        "fragColor=vec4(diffusecolor,1.0f);"\
        "}"\
        "else"\
        "{"\
        "fragColor=vec4(1.0f,1.0f,1.0f,1.0f);"
        "}"\
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
    AMC_ATTRIBUTE_NORMAL,
    "vNormal");

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

    mvUniform = glGetUniformLocation(gShaderProgramObject,
        "u_mv_matrix");
    pUniform = glGetUniformLocation(gShaderProgramObject,
        "u_p_matrix");
    lKeyIsPressedUniform = glGetUniformLocation(gShaderProgramObject,
        "u_lkeyispressed");
    ldUniform = glGetUniformLocation(gShaderProgramObject,
        "u_ld");
    kdUniform = glGetUniformLocation(gShaderProgramObject,
        "u_kd");
    lightPositionUniform = glGetUniformLocation(gShaderProgramObject,
        "u_lightposition");


    planeFrontSphere.setCenter(0.0f,0.0f);
    // planeFrontSphere.setTextureColor(0.729f , 0.886f , 0.933f);//(1.0f*255.0f, 1.0f*255.0f, 1.0f * 255);
    planeFrontSphere.set(1.0f, 90, 90);
    
    
    ////////sphere 2 vbo
    
    
     glGenVertexArrays(1, &vaosphere);

     glBindVertexArray(vaosphere);

    float arrVer[planeFrontSphere.vertices.size()];
    for(int i=0;i<planeFrontSphere.vertices.size();i++){
        arrVer[i]=planeFrontSphere.vertices.at(i);
        //fprintf(gpFile,"%f ",planeFrontSphere.vertices.at(i));
    }
    
    //fprintf(gpFile,"Colors \n ");
    
    float arrNor[planeFrontSphere.normals.size()];
    for(int i=0;i<planeFrontSphere.normals.size();i++){
        arrNor[i]=planeFrontSphere.normals.at(i);
        //fprintf(gpFile,"%f ",planeFrontSphere.colorCoords.at(i));
    }
    
     unsigned int arrIndices[planeFrontSphere.indices.size()];
    
    //fprintf(gpFile,"Indices Size %u ",planeFrontSphere.indices.size());
    
       for(unsigned int i=0;i<planeFrontSphere.indices.size();i++){
           arrIndices[i]=planeFrontSphere.indices.at(i);
           //fprintf(gpFile,"Indices %u ",planeFrontSphere.indices.at(i));
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

    
     glGenBuffers(1, &vbospherenor);
     //Bind Generated Buffer
     glBindBuffer(GL_ARRAY_BUFFER,
         vbospherenor);
     //Fill Buffer
     //glBufferData(GL_ARRAY_BUFFER,
     //    sizeof(rectangleColors),
     //    rectangleColors,
     //    GL_STATIC_DRAW);
     glBufferData(GL_ARRAY_BUFFER,
         planeFrontSphere.normals.size() * sizeof(float),
         arrNor,
         GL_STATIC_DRAW);
     //Set Vertex Attrib Pointer
     glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL,
         3,
         GL_FLOAT,
         GL_FALSE,
         0,
         NULL);
     //Enable Vertex Attrib Array
     glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
    

    glGenBuffers(1, &vbo_sphere_element);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, planeFrontSphere.indices.size()*sizeof(unsigned int), arrIndices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    
    
     //Unbind Buffer
     glBindBuffer(GL_ARRAY_BUFFER, 0);

    ///
    
    
    //////////////////////////////Added
    

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
    vmath::mat4 scaleMatrix = vmath::mat4::identity();

    //Transformation

    translationMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
    //rotationMatrix = vmath::rotate(angleCube,angleCube,angleCube);
    //scaleMatrix=vmath::scale(0.75f,0.75f,0.75f);
    //Matrix Multiplication
    modelViewMatrix = translationMatrix * scaleMatrix;
    modelViewMatrix=modelViewMatrix*rotationMatrix;
    
    
    
    //modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

    //Send necessary matrices to shader in resp. Uniforms

    glUniformMatrix4fv(mvUniform,
        1,
        GL_FALSE,
        modelViewMatrix);
    glUniformMatrix4fv(pUniform,
        1,
        GL_FALSE,
        perspectiveProjectionMatrix);

    if (gb_Lighting) {
        glUniform1i(lKeyIsPressedUniform, gb_Lighting);
        glUniform3f(ldUniform, 1.0f, 1.0f, 1.0f);
        glUniform3f(kdUniform, 0.5f, 0.5, 0.5f);
        glUniform4f(lightPositionUniform, 0.0f, 0.0, 2.0, 1.0f);
    }
    else {
        glUniform1i(lKeyIsPressedUniform, gb_Lighting);
    }
    
    glBindVertexArray(vaosphere);
    
           //     glDrawArrays(GL_TRIANGLE_FAN,
            //    0,
            //    (int)(planeFrontSphere.vertices.size()/3));
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
    glDrawElements(GL_TRIANGLES, planeFrontSphere.indices.size(), GL_UNSIGNED_INT, 0);

    
    glBindVertexArray(0);
                
             
    
    
    //planeFrontSphere.draw();
    
    //Bind with vao

    /*glBindVertexArray(vao_cube);

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

    glBindVertexArray(0);*/
    
    glUseProgram(0);

    CGLFlushDrawable((CGLContextObj)[[self openGLContext]CGLContextObj]);
    CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);

   if (gb_Animation == 1) {
        angleCube = angleCube + 0.1f;
        if (angleCube > 360.0f)
            angleCube = 0.0f;
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
case 'l':
case 'L':
        if (gb_Lighting == 0)
            gb_Lighting = 1;
        else
            gb_Lighting = 0;
        break;
case 'a':
case 'A':
        if (gb_Animation == 0)
            gb_Animation = 1;
        else
            gb_Animation = 0;
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

