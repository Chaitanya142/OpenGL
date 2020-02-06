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
    
        GLuint vao_line;
         GLuint vao_point;
     
         GLuint vbo_position_lines;
         GLuint vbo_color_lines;
     
         GLuint vbo_position_point;
         GLuint vbo_color_point;
         
         GLuint mvpUniform;
         
         GLint gVertexShaderObject;
         GLint gFragementShaderObject;
         GLint gShaderProgramObject;
         
         vmath::mat4 perspectiveProjectionMatrix;
         
         GLfloat angleRotation;
         GLfloat lineVertices[6];
     
         GLfloat lineColor[6] ;
    
    GLfloat XCircle;
       GLfloat YCircle;
       
       GLfloat XTriangle;
       GLfloat YTriangle;
       
       GLfloat YLine;
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
        
         XCircle=-3.0f;
         YCircle=-3.0f;
        
         XTriangle=3.0f;
         YTriangle=-3.0f;
        
         YLine=3.0f;

        
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
        
   //Create vao_line
       //Save everying in single set
       glGenVertexArrays(1, &vao_line);
 
       glBindVertexArray(vao_line);
 
 
       //Line
       //Generate Buffer
       glGenBuffers(1, &vbo_position_lines);
       //Bind Generated Buffer
       glBindBuffer(GL_ARRAY_BUFFER,
           vbo_position_lines);
       //Fill Buffer
       glBufferData(GL_ARRAY_BUFFER,
           6 * sizeof(float),
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
 
       glGenBuffers(1, &vbo_color_lines);
       //Bind Generated Buffer
       glBindBuffer(GL_ARRAY_BUFFER,
           vbo_color_lines);
       //Fill Buffer
       glBufferData(GL_ARRAY_BUFFER,
           6 * sizeof(float),
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
 
       //Create vao_point
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
    
    
    
     
     vmath::mat4 modelViewMatrix ;
             vmath::mat4 translationMatrix;
             vmath::mat4 rotationMatrix;
             vmath::mat4 modelViewProjectionMatrix;
         
             glUseProgram(gShaderProgramObject);
         
             //Initialize matrices
         
             modelViewMatrix = vmath::mat4::identity();
             translationMatrix = vmath::mat4::identity();
             rotationMatrix = vmath::mat4::identity();
             modelViewProjectionMatrix = vmath::mat4::identity();
         
             //Transformation
         
             translationMatrix = vmath::translate(0.0f, YLine, -6.0f);
             rotationMatrix = vmath::rotate(angleRotation,0.0f,1.0f,0.0f);
            //Matrix Multiplication
             modelViewMatrix = translationMatrix * rotationMatrix;
         
             modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
         
             //Send necessary matrices to shader in resp. Uniforms
         
             glUniformMatrix4fv(mvpUniform,
                 1,
                 GL_FALSE,
                 modelViewProjectionMatrix);
         
               lineColor[0] = 1.0f;
               lineColor[1] = 1.0f;
               lineColor[2] = 1.0f;
               lineColor[3] = 1.0;
               lineColor[4] =1.0f;
               lineColor[5] = 1.0f;
            
              
             glBindVertexArray(vao_line);
             
         
             lineVertices[0] = 0.0f;
             lineVertices[1] = 1.0f;
             lineVertices[2] = 0.0f;
             lineVertices[3] = 0.0f;
             lineVertices[4] = -1.0f;
             lineVertices[5] = 0.0f;
         
             glBindBuffer(GL_ARRAY_BUFFER,
                 vbo_position_lines);
             glBufferData(GL_ARRAY_BUFFER,
                 3 * 2 * sizeof(float),
                 lineVertices,
                 GL_DYNAMIC_DRAW);
             glBindBuffer(GL_ARRAY_BUFFER, 0);
         
             glBindBuffer(GL_ARRAY_BUFFER,
                 vbo_color_lines);
             glBufferData(GL_ARRAY_BUFFER,
                 3 * 2 * sizeof(float),
                 lineColor,
                 GL_DYNAMIC_DRAW);
             glBindBuffer(GL_ARRAY_BUFFER, 0);
           
             glDrawArrays(GL_LINES,
                   0,
                   2);
             
             
             glBindVertexArray(0);
                
                //Initialize matrices
         
                   modelViewMatrix = vmath::mat4::identity();
                   translationMatrix = vmath::mat4::identity();
                   rotationMatrix = vmath::mat4::identity();
                   modelViewProjectionMatrix = vmath::mat4::identity();
         
                   //Transformation
         
                   translationMatrix = vmath::translate(XTriangle, YTriangle, -6.0f);
                   rotationMatrix = vmath::rotate(angleRotation,0.0f,1.0f,0.0f);
                  //Matrix Multiplication
                   modelViewMatrix = translationMatrix * rotationMatrix;
         
                   modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
         
                   //Send necessary matrices to shader in resp. Uniforms
         
                   glUniformMatrix4fv(mvpUniform,
                       1,
                       GL_FALSE,
                       modelViewProjectionMatrix);
         
                
                
                glBindVertexArray(vao_line);
             
         
             //Draw
         
             
                float offset=    1.0f;
             lineVertices[0] = 0.0f;
             lineVertices[1] = offset;
             lineVertices[2] = 0.0f;
             lineVertices[3] = -offset;
             lineVertices[4] = -offset;
             lineVertices[5] = 0.0f;
         
             glBindBuffer(GL_ARRAY_BUFFER,
                 vbo_position_lines);
             glBufferData(GL_ARRAY_BUFFER,
                 3 * 2 * sizeof(float),
                 lineVertices,
                 GL_DYNAMIC_DRAW);
             glBindBuffer(GL_ARRAY_BUFFER, 0);
         
             glBindBuffer(GL_ARRAY_BUFFER,
                 vbo_color_lines);
             glBufferData(GL_ARRAY_BUFFER,
                 3 * 2 * sizeof(float),
                 lineColor,
                 GL_DYNAMIC_DRAW);
             glBindBuffer(GL_ARRAY_BUFFER, 0);
         
             //Draw
         
             glDrawArrays(GL_LINES,
                 0,
                 2);
          
             lineVertices[0] = -offset;
             lineVertices[1] = -offset;
             lineVertices[2] = 0.0f;
             lineVertices[3] = offset;
             lineVertices[4] = -offset;
             lineVertices[5] = 0.0f;
         
             glBindBuffer(GL_ARRAY_BUFFER,
                 vbo_position_lines);
             glBufferData(GL_ARRAY_BUFFER,
                 3 * 2 * sizeof(float),
                 lineVertices,
                 GL_DYNAMIC_DRAW);
             glBindBuffer(GL_ARRAY_BUFFER, 0);
         
             glBindBuffer(GL_ARRAY_BUFFER,
                 vbo_color_lines);
             glBufferData(GL_ARRAY_BUFFER,
                 3 * 2 * sizeof(float),
                 lineColor,
                 GL_DYNAMIC_DRAW);
             glBindBuffer(GL_ARRAY_BUFFER, 0);
         
             //Draw
         
             glDrawArrays(GL_LINES,
                 0,
                 2);
         
             lineVertices[0] = offset;
             lineVertices[1] = -offset;
             lineVertices[2] = 0.0f;
             lineVertices[3] = 0.0f;
             lineVertices[4] = offset;
             lineVertices[5] = 0.0f;
         
             glBindBuffer(GL_ARRAY_BUFFER,
                 vbo_position_lines);
             glBufferData(GL_ARRAY_BUFFER,
                 3 * 2 * sizeof(float),
                 lineVertices,
                 GL_DYNAMIC_DRAW);
             glBindBuffer(GL_ARRAY_BUFFER, 0);
         
             glBindBuffer(GL_ARRAY_BUFFER,
                 vbo_color_lines);
             glBufferData(GL_ARRAY_BUFFER,
                 3 * 2 * sizeof(float),
                 lineColor,
                 GL_DYNAMIC_DRAW);
             glBindBuffer(GL_ARRAY_BUFFER, 0);
         
             //Draw
         
             glDrawArrays(GL_LINES,
                 0,
                 2);
         
             glBindVertexArray(0);
         
             
             
             //Initialize matrices
         
                modelViewMatrix = vmath::mat4::identity();
                translationMatrix = vmath::mat4::identity();
                rotationMatrix = vmath::mat4::identity();
                modelViewProjectionMatrix = vmath::mat4::identity();
         
                //Transformation
         
                translationMatrix = vmath::translate(XCircle, YCircle, -6.0f);
                rotationMatrix = vmath::rotate(angleRotation,0.0f,1.0f,0.0f);
               //Matrix Multiplication
                modelViewMatrix = translationMatrix * rotationMatrix;
         
                modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
         
                //Send necessary matrices to shader in resp. Uniforms
         
                glUniformMatrix4fv(mvpUniform,
                    1,
                    GL_FALSE,
                    modelViewProjectionMatrix);
         
             
             
             glBindVertexArray(vao_point);
             
         
             
             float x1=0.0f;
             float y1=1.0f;
             float z1=0.0f;
             float x2=-1.0f;
             float y2=-1.0f;
             float z2=0.0f;
             float x3=1.0f;
             float y3=-1.0f;
             float z3=0.0f;
             
              static float ArrInCirclePos[(unsigned int)((2 * 3.1415) / 0.001) * 3];
                static float ArrInCircleColor[(unsigned int)((2 * 3.1415) / 0.001) * 3];
         
                static int isArrInCircleInit = 0;
         
                if (isArrInCircleInit == 0) {
                    
                    float count = -1;
                    int noCount = 2000;
                    GLfloat angle = 0;
         
                    float distP1P2 = sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1) + (z2 - z1)*(z2 - z1));
                    float distP2P3 = sqrt((x3 - x2)*(x3 - x2) + (y3 - y2)*(y3 - y2) + (z3 - z2)*(z3 - z2));
                    float distP3P1 = sqrt((x1 - x3)*(x1 - x3) + (y1 - y3)*(y1 - y3) + (z1 - z3)*(z1 - z3));
         
                    float semiPerimeter = (distP1P2 + distP2P3 + distP3P1) / 2;
         
                    float radius = sqrt((semiPerimeter - distP1P2)*(semiPerimeter - distP2P3)*(semiPerimeter - distP3P1) / semiPerimeter);
         
                    float Ox = (x3 * distP1P2 + x1 * distP2P3 + x2 * distP3P1) / (semiPerimeter * 2);
                    float Oy = (y3 * distP1P2 + y1 * distP2P3 + y2 * distP3P1) / (semiPerimeter * 2);
                    float Oz = (z3 * distP1P2 + z1 * distP2P3 + z2 * distP3P1) / (semiPerimeter * 2);
         
                    int indexPos = 0;
                    int indexColor = 0;
         
                    for (angle = 0.0f; angle < 2 * 3.14159265; angle = angle + 0.001f) {
                        ArrInCirclePos[indexPos++] = cos(angle)*radius + Ox;
                        ArrInCirclePos[indexPos++] = sin(angle)*radius + Oy;
                        ArrInCirclePos[indexPos++] = 0.0f + Oz;
         
                        ArrInCircleColor[indexColor++] = 1.0f;
                        ArrInCircleColor[indexColor++] = 1.0f;
                        ArrInCircleColor[indexColor++] = 1.0f;
                    }
                    isArrInCircleInit = 1;
                }
         
         
                glBindBuffer(GL_ARRAY_BUFFER,
                    vbo_color_point);
                glBufferData(GL_ARRAY_BUFFER,
                    3 * sizeof(float)*((2 * 3.1415) / 0.001),
                    ArrInCircleColor,
                    GL_DYNAMIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
         
                glBindBuffer(GL_ARRAY_BUFFER,
                    vbo_position_point);
                glBufferData(GL_ARRAY_BUFFER,
                    3 * sizeof(float)*((2 * 3.1415) / 0.001) ,
                    ArrInCirclePos,
                    GL_DYNAMIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
         
                //Draw
         
                glDrawArrays(GL_POINTS,
                    0,
                    ((2 * 3.1415) / 0.001));
         
             glBindVertexArray(0);
         
             glUseProgram(0);
         
    
    
    
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
    
    [eaglContext presentRenderbuffer:GL_RENDERBUFFER];
    
    
    angleRotation = angleRotation + 0.5f;
       if (angleRotation > 360.0f)
           angleRotation = 0.0f;
    
     if(YLine>0){
            YLine-=0.005f;
        }
        if(XTriangle>0){
            XTriangle-=0.005;
        }
        if(YTriangle<0){
              YTriangle+=0.005;
          }
    
        if(XCircle<0){
            XCircle+=0.005;
        }
        if(YCircle<0){
              YCircle+=0.005;
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
  if (vbo_position_lines) {
           glDeleteBuffers(1, &vbo_position_lines);
           vbo_position_lines = 0;
       }
    
       if (vbo_color_lines) {
           glDeleteBuffers(1, &vbo_color_lines);
           vbo_color_lines = 0;
       }
    
       if (vbo_position_point) {
           glDeleteBuffers(1, &vbo_position_point);
           vbo_position_point = 0;
       }
    
       if (vbo_color_point) {
           glDeleteBuffers(1, &vbo_color_point);
           vbo_color_point = 0;
       }
    
       if (vao_line) {
           glDeleteVertexArrays(1, &vao_line);
           vao_line = 0;
       }
    
       if (vao_point) {
           glDeleteVertexArrays(1, &vao_point);
           vao_point = 0;
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

