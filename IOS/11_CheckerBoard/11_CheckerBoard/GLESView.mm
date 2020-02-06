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

#define checkImageWidth 64
#define checkImageHeight 64

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
    
        GLuint txtImage;
    
        GLuint vao_rectangle;
    
        GLuint vbo_position_rectangle;
    
        GLuint vbo_texCoord_rectangle;
    
        GLuint mvpUniform;
        
        GLint gVertexShaderObject;
        GLint gFragementShaderObject;
        GLint gShaderProgramObject;
        
        vmath::mat4 perspectiveProjectionMatrix;
        
        GLuint samplerUniform;
            
        GLubyte checkImage[checkImageWidth][checkImageHeight][4];
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
        
      //RECTANGLE
         glGenVertexArrays(1, &vao_rectangle);
     
         glBindVertexArray(vao_rectangle);
     
         //Generate Buffer
         glGenBuffers(1, &vbo_position_rectangle);
         //Bind Generated Buffer
         glBindBuffer(GL_ARRAY_BUFFER,
             vbo_position_rectangle);
         //Fill Buffer
         //glBufferData(GL_ARRAY_BUFFER,
         //    sizeof(rectangleVertices),
         //    rectangleVertices,
         //    GL_STATIC_DRAW);
         glBufferData(GL_ARRAY_BUFFER,
             4*3*sizeof(float),
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
     
         ////Generate Buffer
         //glGenBuffers(1, &vbo_color_rectangle);
         ////Bind Generated Buffer
         //glBindBuffer(GL_ARRAY_BUFFER,
         //    vbo_color_rectangle);
         ////Fill Buffer
         //glBufferData(GL_ARRAY_BUFFER,
         //    sizeof(rectangleColors),
         //    rectangleColors,
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
     
         //For Single Color
         //glVertexAttrib3f(AMC_ATTRIBUTE_COLOR,
         //    0.0f, 0.0f, 1.0f);
     
         //Generate Buffer
         glGenBuffers(1, &vbo_texCoord_rectangle);
         //Bind Generated Buffer
         glBindBuffer(GL_ARRAY_BUFFER,
             vbo_texCoord_rectangle);
     
         //Fill Buffer
         //glBufferData(GL_ARRAY_BUFFER,
         //    sizeof(rectangleTexCoords),
         //    rectangleTexCoords,
         //    GL_STATIC_DRAW);
     
         glBufferData(GL_ARRAY_BUFFER,
             8 * sizeof(float),
             NULL,
             GL_DYNAMIC_DRAW);
     
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

        [self loadTexture];
        
        
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


-(void) loadTexture
{
  int i, j, c;
  for (i = 0; i < checkImageHeight; i++) {
      for (j = 0; j < checkImageWidth; j++) {
          c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;
          checkImage[i][j][0] = (GLubyte)c;
          checkImage[i][j][1] = (GLubyte)c;
          checkImage[i][j][2] = (GLubyte)c;
          checkImage[i][j][3] = 255;

      }
  }
    glGenTextures(1,&txtImage);

    glPixelStorei(GL_UNPACK_ALIGNMENT,1);

    glBindTexture(GL_TEXTURE_2D,txtImage);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

   glTexImage2D(GL_TEXTURE_2D,
    0,
    GL_RGBA,
    checkImageWidth,
    checkImageHeight,
    0,
    GL_RGBA,
    GL_UNSIGNED_BYTE,
    checkImage);
    
}




/*
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

 */
 
+(Class)layerClass{
    return ([CAEAGLLayer class]);
}


-(void)drawView:(id)sender{
    [EAGLContext setCurrentContext:eaglContext];
    
    glBindFramebuffer(GL_FRAMEBUFFER,defaultFramebuffer);
    
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    
    
    
    GLfloat rectangleVertices[] = {
        -2.0f,-1.0f, 0.0f ,
        -2.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, -1.0f, 0.0f
        };
    
        GLfloat rectangleTexCoords[] = {
        0.0f,0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f
        };
    
        glUseProgram(gShaderProgramObject);
        
        //Initialize matrices
    
        vmath::mat4 modelViewMatrix = vmath::mat4::identity();
        vmath::mat4 modelViewProjectionMatrix = vmath::mat4::identity();
        vmath::mat4 translationMatrix = vmath::mat4::identity();
        vmath::mat4 rotationMatrix = vmath::mat4::identity();
        vmath::mat4 scaleMatrix =vmath::mat4::identity();
        
        //Transformation
    
        translationMatrix = vmath::translate(0.0f, 0.0f, -3.6f);
        //scaleMatrix=vmath::scale(1.0f,-1.0f,1.0f);
        //rotationMatrix = rotate(0.0f, 90.0f, 0.0f);
    
        //Matrix Multiplication
        modelViewMatrix = translationMatrix * rotationMatrix * scaleMatrix;
    
        modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
    
        //Texture
    
        glActiveTexture(GL_TEXTURE0);
    
        glBindTexture(GL_TEXTURE_2D,txtImage);
    
        //Send necessary matrices to shader in resp. Uniforms
    
        glUniformMatrix4fv(mvpUniform,
            1,
            GL_FALSE,
            modelViewProjectionMatrix);
    
        glUniform1i(samplerUniform,0);
    
        glBindVertexArray(vao_rectangle);
    
        //Bind with textures if any
        
        glBindBuffer(GL_ARRAY_BUFFER,
            vbo_position_rectangle);
        glBufferData(GL_ARRAY_BUFFER,
            4 * 3 * sizeof(float),
            rectangleVertices,
            GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
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
    
        //2nd Rectangle
        rectangleVertices[0] = 1.0f;
        rectangleVertices[1] = -1.0f;
        rectangleVertices[2] = 0.0f;
        rectangleVertices[3] = 1.0f;
        rectangleVertices[4] = 1.0f;
        rectangleVertices[5] = 0.0f;
        rectangleVertices[6] = 2.41421f;
        rectangleVertices[7] = 1.0f;
        rectangleVertices[8] = -1.41421f;
        rectangleVertices[9] = 2.41421f;
        rectangleVertices[10] = -1.0f;
        rectangleVertices[11] = -1.41421f;
    
        glBindBuffer(GL_ARRAY_BUFFER,
            vbo_position_rectangle);
        glBufferData(GL_ARRAY_BUFFER,
            4 * 3 * sizeof(float),
            rectangleVertices,
            GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    
        glDrawArrays(GL_TRIANGLE_FAN,
            0,
            4);
    
        //Unbind Texture
        glBindTexture(GL_TEXTURE_2D, 0);
    
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
  
    if (vbo_texCoord_rectangle) {
          glDeleteBuffers(1, &vbo_texCoord_rectangle);
          vbo_texCoord_rectangle = 0;
      }
      if (vbo_position_rectangle) {
          glDeleteBuffers(1, &vbo_position_rectangle);
          vbo_position_rectangle = 0;
      }
      if (vao_rectangle) {
          glDeleteVertexArrays(1, &vao_rectangle);
          vao_rectangle = 0;
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

