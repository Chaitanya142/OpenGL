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
#import "Sphere.h"

enum {
    AMC_ATTRIBUTE_POSITION = 0,
    AMC_ATTRIBUTE_COLOR,
    AMC_ATTRIBUTE_NORMAL,
    AMC_ATTRIBUTE_TEXCOORD0
};

@implementation GLESView
{
    
    ///////////// Fix Block
    EAGLContext *eaglContext;
    GLuint defaultFramebuffer;
    GLuint colorRenderbuffer;
    GLuint depthRenderbuffer;
    
    id displayLink;
    NSInteger animationFrameInterval;
    BOOL isAnimating;
    ///////////////////Fix Block
    
    GLuint vao_cube;
    
    GLuint vbo_position_cube;
    GLuint vbo_normal_cube;
    
    //GLuint vbo_color_cube;
    
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
        
        gb_Lighting=0;
        angleCube=0.0f;
        
        
        //Vertex Shader
        //Define Shader Object
        gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
        
        //Vertex Shader Code
        const GLchar* vertextShaderSourceCode =
        "#version 300 es" \
        "\n " \
        "precision highp float;"\
        "precision highp int;" \
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
        "precision highp int;"
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
                    printf( "Program Link Log %s\n", szInfoLog);
                    free(szInfoLog);
                    [self release];
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
        
        //const GLfloat cubeColors[] = {
        //    1.0f, 0.0f, 0.0f,
        //    1.0f, 0.0f, 0.0f,
        //    1.0f, 0.0f, 0.0f,
        //    1.0f, 0.0f, 0.0f,
        //    0.0f, 1.0f, 0.0f,
        //    0.0f, 1.0f, 0.0f,
        //    0.0f, 1.0f, 0.0f,
        //    0.0f, 1.0f, 0.0f,
        //    0.0f, 0.0f, 1.0f,
        //    0.0f, 0.0f, 1.0f,
        //    0.0f, 0.0f, 1.0f,
        //    0.0f, 0.0f, 1.0f,
        //    0.0f, 1.0f, 1.0f,
        //    0.0f, 1.0f, 1.0f,
        //    0.0f, 1.0f, 1.0f,
        //    0.0f, 1.0f, 1.0f,
        //    1.0f, 1.0f, 0.0f,
        //    1.0f, 1.0f, 0.0f,
        //    1.0f, 1.0f, 0.0f,
        //    1.0f, 1.0f, 0.0f,
        //    1.0f, 0.0f, 1.0f,
        //    1.0f, 0.0f, 1.0f,
        //    1.0f, 0.0f, 1.0f,
        //    1.0f, 0.0f, 1.0f
        //};
        
        const GLfloat cubeNormals[] = {
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            
            0.0f, -1.0f, 0.0f,
            0.0f, -1.0f, 0.0f,
            0.0f, -1.0f, 0.0f,
            0.0f, -1.0f, 0.0f,
            
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            
            0.0f, 0.0f, -1.0f,
            0.0f, 0.0f, -1.0f,
            0.0f, 0.0f, -1.0f,
            0.0f, 0.0f, -1.0f,
            
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            
            -1.0f, 0.0f, 0.0f,
            -1.0f, 0.0f, 0.0f,
            -1.0f, 0.0f, 0.0f,
            -1.0f, 0.0f, 0.0f
        };
        
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
        
        ////Generate Buffer
        //glGenBuffers(1, &vbo_color_cube);
        ////Bind Generated Buffer
        //glBindBuffer(GL_ARRAY_BUFFER,
        //    vbo_color_cube);
        ////Fill Buffer
        //glBufferData(GL_ARRAY_BUFFER,
        //    sizeof(cubeColors),
        //    cubeColors,
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
        //Unbind array
        
        //Generate Buffer
        glGenBuffers(1, &vbo_normal_cube);
        //Bind Generated Buffer
        glBindBuffer(GL_ARRAY_BUFFER,
                     vbo_normal_cube);
        //Fill Buffer
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(cubeNormals),
                     cubeNormals,
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
        //Unbind Buffer
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        //Unbind array
        glBindVertexArray(0);
        
        
        glEnable(GL_TEXTURE_2D);
        
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        
        glClearDepthf(1.0f);
        //set bg color
        glClearColor(0.0f,0.0f,0.0f,0.0f);
        
        perspectiveProjectionMatrix = vmath::mat4::identity();
        
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
    vmath::mat4 translationMatrix = vmath::mat4::identity();
    vmath::mat4 rotationMatrix = vmath::mat4::identity();
    vmath::mat4 scaleMatrix = vmath::mat4::identity();
    
    //Transformation
    
    translationMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
    rotationMatrix = vmath::rotate(angleCube,angleCube,angleCube);
    scaleMatrix=vmath::scale(0.75f,0.75f,0.75f);
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
    
    
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
    
    [eaglContext presentRenderbuffer:GL_RENDERBUFFER];
    
    if (gb_Animation == 1) {
        angleCube = angleCube + 0.5f;
        if (angleCube > 360.0f)
            angleCube = 0.0f;
    }
    
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
    
    
    perspectiveProjectionMatrix = vmath::perspective(45.0f,
                                                     (GLfloat)width / (GLfloat)height,
                                                     0.1f,
                                                     100.0f);
    
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
    if (gb_Lighting == 0)
        gb_Lighting = 1;
    else
        gb_Lighting = 0;
    
}

-(void)onDoubleTap:(UITapGestureRecognizer *)gr
{
    if (gb_Animation == 0)
        gb_Animation = 1;
    else
        gb_Animation = 0;
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

