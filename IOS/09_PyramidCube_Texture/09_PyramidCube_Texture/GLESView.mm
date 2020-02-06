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
    
   GLuint vao_pyramid;
        GLuint vao_cube;
    
        GLuint vbo_position_pyramid;
        GLuint vbo_position_cube;
    
        GLuint vbo_texCoord_Pyramid;
        GLuint vbo_texCoord_Cube;
    
        GLuint mvpUniform;
        
        GLint gVertexShaderObject;
        GLint gFragementShaderObject;
        GLint gShaderProgramObject;
        
        vmath::mat4 perspectiveProjectionMatrix;
        
        GLuint texture_kundali;
        GLuint texture_stone;
        GLfloat anglePyramid;
        GLfloat angleCube;
    
        GLuint samplerUniform;
    
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
            "in vec2 vTexCoord;" \
            "uniform mat4 u_mvp_matrix;" \
            "out vec2 out_TexCoord;" \
            "void main(void)" \
            "{" \
            "gl_Position=u_mvp_matrix * vPosition;" \
            "out_TexCoord=vTexCoord;"\
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
       "in vec2 out_TexCoord;"
            "uniform sampler2D u_sampler;"
            "out vec4 fragColor;"
            "void main(void)" \
            "{" \
            "fragColor=texture(u_sampler,out_TexCoord);" \
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
                AMC_ATTRIBUTE_TEXCOORD0,
                "vTexCoord");
        
        /*glBindAttribLocation(gShaderProgramObject,
            AMC_ATTRIBUTE_COLOR,
            "vColor");*/
        
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
        
        //Retriving Uniform Location
          mvpUniform = glGetUniformLocation(gShaderProgramObject,
              "u_mvp_matrix");
      
          samplerUniform = glGetUniformLocation(gShaderProgramObject,
              "u_sampler");
      

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
        
            const GLfloat pyramidTexCoord[] =
            { 0.5f, 1.0f,
                0.0f, 0.0f,
                1.0f, 0.0f,
                0.5f, 1.0f,
                1.0f, 0.0f,
                0.0f, 0.0f,
                0.5f, 1.0f,
                1.0f, 0.0f,
                0.0f, 0.0f,
                0.5f, 1.0f,
                0.0f, 0.0f,
                1.0f, 0.0f };
        
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
        
            glGenBuffers(1, &vbo_texCoord_Pyramid);
            //Bind Generated Buffer
            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_texCoord_Pyramid);
            //Fill Buffer
            glBufferData(GL_ARRAY_BUFFER,
                sizeof(pyramidTexCoord),
                pyramidTexCoord,
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
        
        
        
          glEnable(GL_TEXTURE_2D);
        

        //texture_smiley=[self loadTextureFromBMPFile:"Smiley.bmp"];

        texture_kundali=[self loadTextureFromBMPFile:@"Vijay_Kundali" :@"bmp"];
        texture_stone=[self loadTextureFromBMPFile:@"Stone" :@"bmp"];
        
        
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


-(GLuint)loadTextureFromBMPFile : (NSString *)texFileName:(NSString*) extension
{
    
    NSString * textureFileNameWithPath=[[NSBundle mainBundle]pathForResource:texFileName ofType:extension];
    
    UIImage * bmpImage=[[UIImage alloc]initWithContentsOfFile:textureFileNameWithPath];
    
    if(!bmpImage){
        NSLog(@"can't find %@",textureFileNameWithPath);
        return(0);
    }
    
    CGImageRef cgImage=bmpImage.CGImage;
    int w=(int)CGImageGetWidth(cgImage);
    int h=(int)CGImageGetHeight(cgImage);
    
    
    CFDataRef imagedata=CGDataProviderCopyData(CGImageGetDataProvider(cgImage));
    void *pixels=(void*)CFDataGetBytePtr(imagedata);
    
    GLuint bmpTexture;
    glGenTextures(1, &bmpTexture);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    glBindTexture(GL_TEXTURE_2D, bmpTexture);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 w,
                 h,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 pixels
                 );
    glGenerateMipmap(GL_TEXTURE_2D);
    
    CFRelease(imagedata);
    
    return(bmpTexture);
    
    
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
   
           translationMatrix = vmath::translate(-1.5f, 0.0f, -6.0f);
           rotationMatrix = vmath::rotate(anglePyramid, 0.0f, 1.0f, 0.0f);
   
           //Matrix Multiplication
           modelViewMatrix = translationMatrix * rotationMatrix;
   
           modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
   
           glActiveTexture(GL_TEXTURE0);
   
           glBindTexture(GL_TEXTURE_2D, texture_stone);
   
           //Send necessary matrices to shader in resp. Uniforms
   
           glUniformMatrix4fv(mvpUniform,
               1,
               GL_FALSE,
               modelViewProjectionMatrix);
   
           glUniform1i(samplerUniform, 0);
   
           //Bind with vao
   
           glBindVertexArray(vao_pyramid);
   
           //Bind with textures if any
   
           //Draw
   
           glDrawArrays(GL_TRIANGLES,
               0,
               12);
   
           //Unbind vao
   
           glBindVertexArray(0);
   
           glBindTexture(GL_TEXTURE_2D, 0);
   
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
   
           glActiveTexture(GL_TEXTURE0);
   
           glBindTexture(GL_TEXTURE_2D, texture_kundali);
   
           //Send necessary matrices to shader in resp. Uniforms
   
           glUniformMatrix4fv(mvpUniform,
               1,
               GL_FALSE,
               modelViewProjectionMatrix);
   
           glUniform1i(samplerUniform, 0);
   
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
    
    
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
    
    [eaglContext presentRenderbuffer:GL_RENDERBUFFER];
    
    
    anglePyramid = anglePyramid + 0.2f;
          if (anglePyramid > 360.0f)
              anglePyramid = 0.0f;
          angleCube = angleCube + 0.2f;
          if (angleCube > 360.0f)
              angleCube = 0.0f;

    
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
    if (vbo_position_pyramid) {
           glDeleteBuffers(1, &vbo_position_pyramid);
           vbo_position_pyramid = 0;
       }
   
       if (vbo_texCoord_Pyramid) {
           glDeleteBuffers(1, &vbo_texCoord_Pyramid);
           vbo_texCoord_Pyramid = 0;
       }
   
       if (vao_pyramid) {
           glDeleteVertexArrays(1, &vao_pyramid);
           vao_pyramid = 0;
       }
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

