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
#import <stack>




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
    
    GLuint txtImage1;
         GLuint txtImage2;
         GLuint txtImage3;
     
     Sphere sphereObj1;
     
     GLuint vaosphere1;
     GLuint vbospherepos1;
     GLuint vbospheretex1;
     GLuint vbo_sphere_element1;
     
     
     Sphere sphereObj2;
      
      GLuint vaosphere2;
      GLuint vbospherepos2;
      GLuint vbospheretex2;
      GLuint vbo_sphere_element2;
      
     
     
     Sphere sphereObj3;
      
      GLuint vaosphere3;
      GLuint vbospherepos3;
      GLuint vbospheretex3;
      GLuint vbo_sphere_element3;
      
     
     
     GLuint mvpUniform;
     
     GLint gVertexShaderObject;
     GLint gFragementShaderObject;
     GLint gShaderProgramObject;
     
     vmath::mat4 perspectiveProjectionMatrix;
     
     GLuint samplerUniform;
         
     GLubyte sphereTexColor[1][1][4];
     
     int year;
     int day;
     int moon;
     
     
     std::stack <vmath::mat4> ms;
    
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
        
        
        year=0;
        moon=0;
        day=0;

        
        
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
        
        //Post Linking
             //Retriving Uniform Location
             mvpUniform = glGetUniformLocation(gShaderProgramObject,
                 "u_mvp_matrix");
         
             samplerUniform = glGetUniformLocation(gShaderProgramObject,
                 "u_sampler");
         
             
             sphereObj1.setCenter(0.0f,0.0f);
              // sphereObj.setTextureColor(0.729f , 0.886f , 0.933f);//(1.0f*255.0f, 1.0f*255.0f, 1.0f * 255);
              sphereObj1.set(0.75f, 30, 30);
                  
               glGenVertexArrays(1, &vaosphere1);
         
               glBindVertexArray(vaosphere1);
         
              float arrVer1[sphereObj1.vertices.size()];
              for(int i=0;i<sphereObj1.vertices.size();i++){
                  arrVer1[i]=sphereObj1.vertices.at(i);
              }
              
              //fprintf(gpFile,"Colors \n ");
              
              float arrTexCoord1[sphereObj1.texCoords.size()];
              for(int i=0;i<sphereObj1.texCoords.size();i++){
                  arrTexCoord1[i]=sphereObj1.texCoords.at(i);
              }
              
               unsigned int arrIndices1[sphereObj1.indices.size()];
              
                 for(unsigned int i=0;i<sphereObj1.indices.size();i++){
                     arrIndices1[i]=sphereObj1.indices.at(i);
                 }
             
                 
              //Generate Buffer
               glGenBuffers(1, &vbospherepos1);
               //Bind Generated Buffer
               glBindBuffer(GL_ARRAY_BUFFER,
                   vbospherepos1);
             
               glBufferData(GL_ARRAY_BUFFER,
                   sphereObj1.vertices.size() * sizeof(float),
                   arrVer1,
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
         
              
               glGenBuffers(1, &vbospheretex1);
               //Bind Generated Buffer
               glBindBuffer(GL_ARRAY_BUFFER,
                   vbospheretex1);
              
               glBufferData(GL_ARRAY_BUFFER,
                   sphereObj1.texCoords.size() * sizeof(float),
                   arrTexCoord1,
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
              
              glGenBuffers(1, &vbo_sphere_element1);
              glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element1);
              glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereObj1.indices.size()*sizeof(unsigned int), arrIndices1, GL_STATIC_DRAW);
              glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
             
               //Unbind Buffer
               glBindBuffer(GL_ARRAY_BUFFER, 0);
         
             /////////////////////
             
             
             
             sphereObj2.setCenter(0.0f,0.0f);
               // sphereObj.setTextureColor(0.729f , 0.886f , 0.933f);//(1.0f*255.0f, 1.0f*255.0f, 1.0f * 255);
               sphereObj2.set(0.2f, 20, 20);
                   
                glGenVertexArrays(1, &vaosphere2);
         
                glBindVertexArray(vaosphere2);
         
               float arrVer2[sphereObj2.vertices.size()];
               for(int i=0;i<sphereObj2.vertices.size();i++){
                   arrVer2[i]=sphereObj2.vertices.at(i);
               }
               
               float arrTexCoord2[sphereObj2.texCoords.size()];
               for(int i=0;i<sphereObj2.texCoords.size();i++){
                   arrTexCoord2[i]=sphereObj2.texCoords.at(i);
               }
               
             /*
                unsigned int arrIndices[sphereObj.indices.size()];
               
                  for(unsigned int i=0;i<sphereObj.indices.size();i++){
                      arrIndices[i]=sphereObj.indices.at(i);
                  }
              */
              
              unsigned int arrayLineIndices2[sphereObj2.lineIndices.size()];
              
              for(unsigned int i=0;i<sphereObj2.lineIndices.size();i++){
                      arrayLineIndices2[i]=sphereObj2.lineIndices.at(i);
                  }
              
              
                  
               //Generate Buffer
                glGenBuffers(1, &vbospherepos2);
                //Bind Generated Buffer
                glBindBuffer(GL_ARRAY_BUFFER,
                    vbospherepos2);
              
                glBufferData(GL_ARRAY_BUFFER,
                    sphereObj2.vertices.size() * sizeof(float),
                    arrVer2,
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
         
               
                glGenBuffers(1, &vbospheretex2);
                //Bind Generated Buffer
                glBindBuffer(GL_ARRAY_BUFFER,
                    vbospheretex2);
               
                glBufferData(GL_ARRAY_BUFFER,
                    sphereObj2.texCoords.size() * sizeof(float),
                    arrTexCoord2,
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
               
               /*glGenBuffers(1, &vbo_sphere_element);
               glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
               glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereObj.indices.size()*sizeof(unsigned int), arrIndices, GL_STATIC_DRAW);
               glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
              */
              
              glGenBuffers(1, &vbo_sphere_element2);
                 glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element2);
                 glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereObj2.lineIndices.size()*sizeof(unsigned int), arrayLineIndices2, GL_STATIC_DRAW);
                 glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
               
                //Unbind Buffer
                glBindBuffer(GL_ARRAY_BUFFER, 0);
         
             
             ///////////////
             
             
             
             sphereObj3.setCenter(0.0f,0.0f);
               // sphereObj.setTextureColor(0.729f , 0.886f , 0.933f);//(1.0f*255.0f, 1.0f*255.0f, 1.0f * 255);
               sphereObj3.set(0.1f, 15, 15);
                   
                glGenVertexArrays(1, &vaosphere3);
         
                glBindVertexArray(vaosphere3);
         
               float arrVer3[sphereObj3.vertices.size()];
               for(int i=0;i<sphereObj3.vertices.size();i++){
                   arrVer3[i]=sphereObj3.vertices.at(i);
               }
               
               float arrTexCoord3[sphereObj3.texCoords.size()];
               for(int i=0;i<sphereObj3.texCoords.size();i++){
                   arrTexCoord3[i]=sphereObj3.texCoords.at(i);
               }
               
                unsigned int arrIndices3[sphereObj3.indices.size()];
               
                  for(unsigned int i=0;i<sphereObj3.indices.size();i++){
                      arrIndices3[i]=sphereObj3.indices.at(i);
                  }
              
              /*
              unsigned int arrayLineIndices[sphereObj.lineIndices.size()];
              
              for(unsigned int i=0;i<sphereObj.lineIndices.size();i++){
                      arrayLineIndices[i]=sphereObj.lineIndices.at(i);
                  }
              */
              
                  
               //Generate Buffer
                glGenBuffers(1, &vbospherepos3);
                //Bind Generated Buffer
                glBindBuffer(GL_ARRAY_BUFFER,
                    vbospherepos3);
              
                glBufferData(GL_ARRAY_BUFFER,
                    sphereObj3.vertices.size() * sizeof(float),
                    arrVer3,
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
         
               
                glGenBuffers(1, &vbospheretex3);
                //Bind Generated Buffer
                glBindBuffer(GL_ARRAY_BUFFER,
                    vbospheretex3);
               
                glBufferData(GL_ARRAY_BUFFER,
                    sphereObj3.texCoords.size() * sizeof(float),
                    arrTexCoord3,
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
               
               glGenBuffers(1, &vbo_sphere_element3);
               glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element3);
               glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereObj3.indices.size()*sizeof(unsigned int), arrIndices3, GL_STATIC_DRAW);
               glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
              
              /*
              glGenBuffers(1, &vbo_sphere_element);
                 glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
                 glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereObj.lineIndices.size()*sizeof(unsigned int), arrayLineIndices, GL_STATIC_DRAW);
                 glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
               */
                //Unbind Buffer
                glBindBuffer(GL_ARRAY_BUFFER, 0);
         
             
             
             //////////////
             
    
             glEnable(GL_TEXTURE_2D);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        
          //texture_smiley=[self loadTextureFromBMPFile:"Smiley.bmp"];
          sphereTexColor[0][0][0] = (GLubyte)255;
          sphereTexColor[0][0][1] = (GLubyte)255;
          sphereTexColor[0][0][2] = (GLubyte)255;
          sphereTexColor[0][0][3] = 255;
          
          [self loadTexture:&txtImage1];
          
          sphereTexColor[0][0][0] = (GLubyte)102;
             sphereTexColor[0][0][1] = (GLubyte)229;
             sphereTexColor[0][0][2] = (GLubyte)255;
             sphereTexColor[0][0][3] = 255;
             
             [self loadTexture:&txtImage2];
             
          
          sphereTexColor[0][0][0] = (GLubyte)127;
        sphereTexColor[0][0][1] = (GLubyte)127;
            sphereTexColor[0][0][2] = (GLubyte)127;
            sphereTexColor[0][0][3] = 255;
             
            [self loadTexture:&txtImage3];
             
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
        vmath::mat4 scaleMatrix =vmath::mat4::identity();
                                
        
        
        /*
        translationMatrix = vmath::translate(0.0f, 0.0f, -12.0f);
    
        //Matrix Multiplication
        modelViewMatrix = translationMatrix * rotationMatrix;
    
        //modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
    
        ms.push(modelViewMatrix);
    
        rotationMatrix = vmath::rotate((float)shoulder, 0.0f, 0.0f, 1.0f);
    
        modelViewMatrix = modelViewMatrix * rotationMatrix;
    
        translationMatrix = vmath::mat4::identity();
        translationMatrix = vmath::translate(1.0f, 0.0f, 0.0f);
    
        modelViewMatrix = modelViewMatrix * translationMatrix;
    
        ms.push(modelViewMatrix);
    
        scaleMatrix = vmath::scale(2.0f, 0.5f, 1.0f);
    
        modelViewMatrix = modelViewMatrix * scaleMatrix;
    
        modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
    
        glActiveTexture(GL_TEXTURE0);
    
          glBindTexture(GL_TEXTURE_2D,txtImage);
    
        glUniformMatrix4fv(mvpUniform,
            1,
            GL_FALSE,
            modelViewProjectionMatrix);
    
        glUniform1i(samplerUniform, 0);
    
        glBindVertexArray(vaosphere);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
        glDrawElements(GL_LINES, sphereObj.indices.size(), GL_UNSIGNED_INT, 0);
        
        glBindVertexArray(0);
        
        //earth.drawLines(color);
    
        modelViewMatrix = ms.top();
    
        ms.pop();
        
        translationMatrix = vmath::mat4::identity();
        translationMatrix = vmath::translate(1.0f, 0.0f, 0.0f);
    
        modelViewMatrix = modelViewMatrix * translationMatrix;
    
        rotationMatrix = vmath::mat4::identity();
        rotationMatrix = vmath::rotate((float)elbow, 0.0f, 0.0f, 1.0f);
    
        modelViewMatrix = modelViewMatrix * rotationMatrix;
    
        translationMatrix = vmath::mat4::identity();
        translationMatrix = vmath::translate(1.0f, 0.0f, 0.0f);
    
        modelViewMatrix = modelViewMatrix * translationMatrix;
    
        ms.push(modelViewMatrix);
    
        scaleMatrix= vmath::mat4::identity();
        scaleMatrix = vmath::scale(2.0f, 0.5f, 1.0f);
    
        modelViewMatrix = modelViewMatrix * scaleMatrix;
    
        modelViewProjectionMatrix = vmath::mat4::identity();
        modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
    
        glUniformMatrix4fv(mvpUniform,
            1,
            GL_FALSE,
            modelViewProjectionMatrix);
    
        glUniform1i(samplerUniform, 0);
    
       glBindVertexArray(vaosphere);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
        glDrawElements(GL_LINES, sphereObj.indices.size(), GL_UNSIGNED_INT, 0);
        
        glBindVertexArray(0);
        
        ms.pop();
        ms.pop();
    
        */
        ////////////////////////////////
        
        
        
        vmath::vec3 eye(0.0f, 0.0f, 5.0f);
        vmath::vec3 center(0.0f, 0.0f, 0.0f);
        vmath::vec3 up(0.0f, 1.0f, 0.0f);
        translationMatrix=vmath::lookat(eye,center,up);
    
        modelViewMatrix = translationMatrix * rotationMatrix;
    
        ms.push(modelViewMatrix);
    
        rotationMatrix = vmath::rotate(90.0f, 1.0f, 0.0f, 0.0f);
    
        modelViewMatrix = translationMatrix * rotationMatrix;
    
        modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
    
        glUniformMatrix4fv(mvpUniform,
            1,
            GL_FALSE,
            modelViewProjectionMatrix);
    
        glUniform1i(samplerUniform, 0);
    
        //Sun.draw();
    
        glActiveTexture(GL_TEXTURE0);
    
          glBindTexture(GL_TEXTURE_2D,txtImage1);
        
        
        glBindVertexArray(vaosphere1);
          
          glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element1);
          glDrawElements(GL_TRIANGLES, sphereObj1.indices.size(), GL_UNSIGNED_INT, 0);
          
          glBindVertexArray(0);
        
        
        modelViewMatrix = ms.top();
    
        ms.pop();
        
        ms.push(modelViewMatrix);
    
        rotationMatrix = vmath::mat4::identity();
        rotationMatrix = vmath::rotate((GLfloat)year, 0.0f, 1.0f, 0.0f);
    
        modelViewMatrix = modelViewMatrix * rotationMatrix;
    
        translationMatrix = vmath::mat4::identity();
        translationMatrix = vmath::translate(2.0f, 0.0f, 0.0f);
    
        modelViewMatrix = modelViewMatrix * translationMatrix;
    
        rotationMatrix = vmath::mat4::identity();
        rotationMatrix = vmath::rotate(90.0f, 1.0f, 0.0f, 0.0f);
    
        modelViewMatrix = modelViewMatrix * rotationMatrix;
    
        rotationMatrix = vmath::mat4::identity();
        rotationMatrix = vmath::rotate((GLfloat)day, 0.0f, 0.0f, 1.0f);
    
        modelViewMatrix = modelViewMatrix * rotationMatrix;
    
        modelViewProjectionMatrix = vmath::mat4::identity();
        modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
    
        glUniformMatrix4fv(mvpUniform,
            1,
            GL_FALSE,
            modelViewProjectionMatrix);
    
        glUniform1i(samplerUniform, 0);
    
        //Earth.drawLines();
        
          glBindTexture(GL_TEXTURE_2D,txtImage2);
        
        glBindVertexArray(vaosphere2);
          
          glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element2);
          glDrawElements(GL_LINES, sphereObj2.lineIndices.size(), GL_UNSIGNED_INT, 0);
          
          glBindVertexArray(0);
        
        ms.push(modelViewMatrix);
    
        rotationMatrix = vmath::mat4::identity();
        rotationMatrix = vmath::rotate((float)moon, 0.0f, 0.0f, 1.0f);
    
        modelViewMatrix = modelViewMatrix * rotationMatrix;
    
        translationMatrix = vmath::mat4::identity();
        translationMatrix = vmath::translate(0.5f, 0.0f, 0.0f);
        
        modelViewMatrix = modelViewMatrix * translationMatrix;
    
        modelViewProjectionMatrix = vmath::mat4::identity();
        modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
    
        glUniformMatrix4fv(mvpUniform,
            1,
            GL_FALSE,
            modelViewProjectionMatrix);
    
        glUniform1i(samplerUniform, 0);
    
        //Moon.drawLines();
        
          glBindTexture(GL_TEXTURE_2D,txtImage3);
        
        
        glBindVertexArray(vaosphere3);
          
          glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element3);
          glDrawElements(GL_TRIANGLES, sphereObj3.indices.size(), GL_UNSIGNED_INT, 0);
          
          glBindVertexArray(0);
        
    
        ms.pop();
        ms.pop();
        
        
        glBindTexture(GL_TEXTURE_2D, 0);
    
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
          year = (year + 10) % 360;
}

-(void)onDoubleTap:(UITapGestureRecognizer *)gr
{
     day = (day + 10) % 360;
}

-(void)onSwipe:(UISwipeGestureRecognizer *)gr
{
    [self release];
    exit(0);
}

-(void)onLongPress:(UILongPressGestureRecognizer *)gr
{
     moon = (moon + 10) % 360;
}

-(void)dealloc
{
    
    
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

/*
-(GLuint) loadTextureFromBMPFile:(const char *)texFileName
{
    NSBundle *mainBundle=[NSBundle mainBundle];
    NSString *appDirName=[mainBundle bundlePath];
    NSString *parentDirPath=[appDirName stringByDeletingLastPathComponent];
    NSString *texFileNameWithPath=[NSString stringWithFormat:@"%@/%s",parentDirPath,texFileName];

    NSImage *bmpImage=[[NSImage alloc]initWithContentsOfFile:texFileNameWithPath];

    if(!bmpImage){
        NSLog(@"can't find %@",texFileNameWithPath);
        return(0);
    }

    CGImageRef cgImage=[bmpImage CGImageForProposedRect:nil context:nil hints:nil];

    int w=(int)CGImageGetWidth(cgImage);
    int h=(int)CGImageGetHeight(cgImage);


    CFDataRef imageData=CGDataProviderCopyData(CGImageGetDataProvider(cgImage));

    void* pixels=(void *) CFDataGetBytePtr(imageData);

    GLuint bmpTexture;

    glGenTextures(1,&bmpTexture);

    glPixelStorei(GL_UNPACK_ALIGNMENT,1);

    glBindTexture(GL_TEXTURE_2D,bmpTexture);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);

    glTexImage2D(GL_TEXTURE_2D,
                    0,
                    GL_RGBA,
                    w,
                    h,
                    0,
                    GL_RGBA,
                    GL_UNSIGNED_BYTE,
                    pixels);

    glGenerateMipmap(GL_TEXTURE_2D);

    CFRelease(imageData);

    return(bmpTexture);
}
*/


-(void) loadTexture:(GLuint *) txtimg
{


    glGenTextures(1,txtimg);

    glPixelStorei(GL_UNPACK_ALIGNMENT,1);

    glBindTexture(GL_TEXTURE_2D,*txtimg);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

   glTexImage2D(GL_TEXTURE_2D,
    0,
    GL_RGBA,
    1,
    1,
    0,
    GL_RGBA,
    GL_UNSIGNED_BYTE,
    sphereTexColor);
    
}




@end

