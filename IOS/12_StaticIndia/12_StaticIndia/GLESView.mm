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
    
    ///////////// Fix Block
    EAGLContext *eaglContext;
    GLuint defaultFramebuffer;
    GLuint colorRenderbuffer;
    GLuint depthRenderbuffer;
    
    id displayLink;
    NSInteger animationFrameInterval;
    BOOL isAnimating;
    ///////////////////Fix Block
    
    
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
        "in vec4 out_color;"\
        "out vec4 fragColor;"
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

