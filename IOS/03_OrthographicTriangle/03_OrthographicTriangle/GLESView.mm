//
//  MyView.m
//  01_Window
//
//  Created by Apple on 01/02/20.
//

#import "GLESView.h"
#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>

#import "vmath.h"

enum {
    AMC_ATTRIBUTE_POSITION = 0,
    AMC_ATTRIBUTE_COLOR,
    AMC_ATTRIBUTE_NORMAL,
    AMC_ATTRIBUTE_TEXCOORD0
};

@implementation GLESView
{
    EAGLContext *eaglContext;
    GLuint defaultFramebuffer;
    GLuint colorRenderbuffer;
    GLuint depthRenderbuffer;
    
    id displayLink;
    NSInteger animationFrameInterval;
    BOOL isAnimating;
    
    GLuint vao;
    GLuint vbo;
    GLuint mvpUniform;
    
    GLint gVertexShaderObject;
    GLint gFragementShaderObject;
    GLint gShaderProgramObject;
    
    vmath::mat4 orthographicProjectionMatrix;
    
}

-(id)initWithFrame:(CGRect)frame;
{
 
    self=[super initWithFrame:frame];
    
    if(self)
    {
        
        CAEAGLLayer *eaglLayer=(CAEAGLLayer *)super.layer;
        
        eaglLayer.opaque=YES;
        eaglLayer.drawableProperties=[NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:FALSE],
                                      kEAGLDrawablePropertyRetainedBacking,kEAGLColorFormatRGBA8,
                                      kEAGLDrawablePropertyColorFormat,nil
                                      ];
       
        eaglContext=[[EAGLContext alloc]initWithAPI:kEAGLRenderingAPIOpenGLES3];
        
        if(eaglContext==nil){
            [self release];
            return(nil);
        }
        
        [EAGLContext setCurrentContext:eaglContext];
        
        glGenFramebuffers(1,&defaultFramebuffer);
        glGenRenderbuffers(1, &colorRenderbuffer);
        
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
        
        glBindRenderbuffer(GL_RENDERBUFFER,colorRenderbuffer);
        
        [eaglContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer];
        
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_RENDERBUFFER,colorRenderbuffer);
        
        GLint backingWidth;
        GLint backingHeight;
        
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH,&backingWidth);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT,&backingHeight);

        glGenRenderbuffers(1, &depthRenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER_WIDTH, depthRenderbuffer);
        
        glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT16,backingWidth,backingHeight);
        
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
        
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE)
        {
            printf("FAILED To Create Complete FrameBuffer Object %x \n",glCheckFramebufferStatus(GL_FRAMEBUFFER));
            
            glDeleteFramebuffers(1,&defaultFramebuffer);
            glDeleteRenderbuffers(1,&colorRenderbuffer);
            glDeleteRenderbuffers(1,&depthRenderbuffer);
            
            return(nil);
        }
        
        printf("Renderer : %s, | GL Version : %s | GLES Version : %s \n", glGetString(GL_RENDERER),glGetString(GL_VERSION),glGetString(GL_SHADING_LANGUAGE_VERSION));
        
        isAnimating=NO;
        
        animationFrameInterval=60;
        
            //Vertex Shader
                //Define Shader Object
                gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
        
                //Vertex Shader Code
                const GLchar* vertextShaderSourceCode =
                    "#version 300 es" \
                    "\n " \
                    "in vec4 vPosition;" \
                    "uniform mat4 u_mvp_matrix;" \
                    "void main(void)" \
                    "{" \
                    "gl_Position=u_mvp_matrix * vPosition;" \
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
                            printf("Vertex Shader Compilation Log %s\n", szInfoLog);
                            free(szInfoLog);
                            [self release];
                         }
                    }
                }
        
                //Fragement Shader
                //Define Shader Object
                gFragementShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
        
                //Fragement Shader Code
                const GLchar* fragementShaderSourceCode =
                    "#version 300 es" \
                    "\n " \
                    "precision highp float;"\
                    "out vec4 fragColor;"\
                    "void main(void)" \
                    "{" \
                    "fragColor=vec4(1.0,1.0,0.0,1.0);" \
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
                            printf("Fragement Shader Compilation Log %s\n", szInfoLog);
                            free(szInfoLog);
                            [self release];
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
                            printf( "Program Link Log %s\n", szInfoLog);
                            free(szInfoLog);
                            [self release];
                        }
                    }
                }
        
                //Post Linking
                //Retriving Uniform Location
                mvpUniform = glGetUniformLocation(gShaderProgramObject,
                    "u_mvp_matrix");
        
        
                //Vertices
                const GLfloat triangleVertices[] = {
                    0.0f,50.0f,0.0f,
                    -50.0f,-50.0f,0.0f,
                    50.0f,-50.0f,0.0f
                };
        
                //Create vao
                //Save everying in single set
                glGenVertexArrays(1,&vao);
        
                glBindVertexArray(vao);
        
                //Generate Buffer
                glGenBuffers(1,&vbo);
                //Bind Generated Buffer
                glBindBuffer(GL_ARRAY_BUFFER,
                    vbo);
                //Fill Buffer
                glBufferData(GL_ARRAY_BUFFER,
                    sizeof(triangleVertices),
                    triangleVertices,
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
                glBindBuffer(GL_ARRAY_BUFFER,0);
                //Unbind array
                glBindVertexArray(0);
        
                glEnable(GL_DEPTH_TEST);
                glDepthFunc(GL_LEQUAL);
        
               
                glClearDepthf(1.0f);
                //set bg color
                glClearColor(0.0f,0.0f,1.0f,0.0f);
        
                orthographicProjectionMatrix = vmath::mat4::identity();
        
        UITapGestureRecognizer *singleTapGestureRecognizer=[[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(onSingleTap:)];
        [singleTapGestureRecognizer setNumberOfTapsRequired:1];
        [singleTapGestureRecognizer setNumberOfTouchesRequired:1];
        [singleTapGestureRecognizer setDelegate:self];
        [self addGestureRecognizer:singleTapGestureRecognizer];
        
        UITapGestureRecognizer *doubleTapGestureRecognizer=[[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(onDoubleTap:)];
        [doubleTapGestureRecognizer setNumberOfTapsRequired:2];
        [doubleTapGestureRecognizer setNumberOfTouchesRequired:1];
        [doubleTapGestureRecognizer setDelegate:self];
        [self addGestureRecognizer:doubleTapGestureRecognizer];

        [singleTapGestureRecognizer requireGestureRecognizerToFail:doubleTapGestureRecognizer];
        
        UISwipeGestureRecognizer *swipeGestureRecognizer=[[UISwipeGestureRecognizer alloc]initWithTarget:self action:@selector(onSwipe:)];
        [self addGestureRecognizer:swipeGestureRecognizer];
        
        UILongPressGestureRecognizer *longPressGestureRecognizer=[[UILongPressGestureRecognizer alloc]initWithTarget:self action:@selector(onLongPress:)];
                                                                  
        [self addGestureRecognizer:longPressGestureRecognizer];
        
    }
    
    return(self);
}


+(Class)layerClass{
    return ([CAEAGLLayer class]);
}


-(void)drawView:(id)sender{
    [EAGLContext setCurrentContext:eaglContext];
    
    glBindFramebuffer(GL_FRAMEBUFFER,defaultFramebuffer);
    
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    
    
        glUseProgram(gShaderProgramObject);
    
            //Initialize matrices
    
            vmath::mat4 modelViewMatrix = vmath::mat4::identity();
            vmath::mat4 modelViewProjectionMatrix = vmath::mat4::identity();
    
            //Transformation
    
            //Matrix Multiplication
    
            modelViewProjectionMatrix = orthographicProjectionMatrix * modelViewMatrix;
    
            //Send necessary matrices to shader in resp. Uniforms
    
            glUniformMatrix4fv(mvpUniform,
                1,
                GL_FALSE,
                modelViewProjectionMatrix);
    
            //Bind with vao
    
            glBindVertexArray(vao);
    
            //Bind with textures if any
    
            //Draw
    
            glDrawArrays(GL_TRIANGLES,
                0,
                3);
    
            //Unbind vao
    
            glBindVertexArray(0);
    
            glUseProgram(0);
    

    
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
    
    [eaglContext presentRenderbuffer:GL_RENDERBUFFER];
    
}

-(void)layoutSubviews
{
    GLint width;
    GLint height;
    
    glBindRenderbuffer(GL_RENDERBUFFER,colorRenderbuffer);
    
    [eaglContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)self.layer];
    
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH,&width);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT,&height);
    
    glGenRenderbuffers(1,&depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER,depthRenderbuffer);
    
    glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT16,width,height);
    
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,depthRenderbuffer);
    
    glViewport(0,0,width,height);
    
    if (width <= height) {
        orthographicProjectionMatrix=vmath::ortho(-100.0f,
            100.0f,
            (-100.0f*height / width),
            (100.0f*height / width),
            -100.0f,
            100.0f
        );
    }
    else
    {
        orthographicProjectionMatrix=vmath::ortho(
            (-100.0f*width/height),
            (100.0f*width/height),
            -100.0f,
            100.0f,
            -100.0f,
            100.0f);
    }
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        printf("Failed To Create Complete Framebuffer %x",glCheckFramebufferStatus(GL_FRAMEBUFFER));
        
    }
    
    [self drawView:nil];
    
}

-(void)startAnimation
{
if(!isAnimating)
{
    displayLink=[NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(drawView:)];
    
    [displayLink setPreferredFramesPerSecond:animationFrameInterval];
    [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    
    isAnimating=YES;
}
}


-(void)stopAnimation
{
    if(isAnimating){
        [displayLink invalidate];
        displayLink=nil;
        isAnimating=NO;
    }
}

-(BOOL)acceptsFirstResponde
{
    return(YES);
}

-(BOOL)acceptsFirstResponder
{
    return(YES);
}


-(void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    
}

-(void)onSingleTap:(UITapGestureRecognizer *)gr
{
    
}

-(void)onDoubleTap:(UITapGestureRecognizer *)gr
{
}

-(void)onSwipe:(UISwipeGestureRecognizer *)gr
{
    [self release];
    exit(0);
}

-(void)onLongPress:(UILongPressGestureRecognizer *)gr
{
    
}

-(void)dealloc
{
     if (vbo) {
            glDeleteBuffers(1,&vbo);
            vbo = 0;
        }
        if (vao) {
            glDeleteVertexArrays(1,&vao);
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

    
    
    if(depthRenderbuffer){
        glDeleteRenderbuffers(1,&depthRenderbuffer);
        depthRenderbuffer=0;
    }
    
    if(colorRenderbuffer){
        glDeleteRenderbuffers(1,&colorRenderbuffer);
        colorRenderbuffer=0;
    }

    if(defaultFramebuffer){
        glDeleteFramebuffers(1,&defaultFramebuffer);
        defaultFramebuffer=0;
    }
    
    if([EAGLContext currentContext]==eaglContext){
        [EAGLContext setCurrentContext:nil];
    }
    [eaglContext release];
    eaglContext=nil;
    
    [super dealloc];
    
}

@end

