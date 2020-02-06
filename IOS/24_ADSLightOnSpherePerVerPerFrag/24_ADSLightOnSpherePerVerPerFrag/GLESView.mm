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
    
    
    Sphere sphereObj;
    
    GLuint mUniform;
    GLuint vUniform;
    GLuint pUniform;
    GLuint lKeyIsPressedUniform;
    GLuint ldUniform;
    GLuint kdUniform;
    GLuint laUniform;
    GLuint kaUniform;
    GLuint lsUniform;
    GLuint ksUniform;
    GLuint lightPositionUniform;
    GLuint shininessUniform;
    
    vmath::mat4 perspectiveProjectionMatrix;
    
    float lightAmbient[4];
    float lightDiffuse[4];
    float lightSpecular[4];
    float lightPosition[4];
    
    float materialAmbient[4];
    float materialDiffuse[4];
    float materialSpecular[4];
    float materialShininess;
    
    GLuint gb_Lighting;
    
    GLint gShaderProgramObject_PF;
    GLint gShaderProgramObject_PV;
    
    GLint gVertexShaderObject_PF;
    GLint gFragementShaderObject_PF;
    
    GLint gVertexShaderObject_PV;
    GLint gFragementShaderObject_PV;
    
    GLuint vaosphere;
    GLuint vbospherepos;
    GLuint vbospherenor;
    GLuint vbo_sphere_element;
    
    GLuint resetUniform;
    
    GLint perVertexOrFragement;
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
        
        perVertexOrFragement=0;
        
        //Vertex Shader
        //Define Shader Object
        gVertexShaderObject_PF = glCreateShader(GL_VERTEX_SHADER);
        
        //Vertex Shader Code
        const GLchar* vertextShaderSourceCode_PF =
        "#version 300 es" \
        "\n " \
        "precision highp float;"\
        "precision highp int;" \
        "in vec4 vPosition;" \
        "in vec3 vNormal;" \
        "uniform mat4 u_m_matrix;" \
        "uniform mat4 u_v_matrix;" \
        "uniform mat4 u_p_matrix;" \
        "uniform int u_lkeyispressed;" \
        "uniform vec4 u_lightposition;" \
        "out vec3 tnorm;"\
        "out vec3 lightDirection;"\
        "out vec3 reflectionVector;"\
        "out vec3 viewerVector;"\
        "void main(void)" \
        "{" \
        "gl_Position=u_p_matrix*u_v_matrix*u_m_matrix * vPosition;" \
        "if(u_lkeyispressed==1)"\
        "{"\
        "vec4 eyecoordinates=u_v_matrix*u_m_matrix*vPosition;"\
        "tnorm=mat3(u_v_matrix*u_m_matrix)*vNormal;"\
        "lightDirection=vec3(u_lightposition-eyecoordinates);"\
        "reflectionVector=reflect(-lightDirection,tnorm);"\
        "viewerVector=vec3(-eyecoordinates);"\
        "}"\
        "}";
        
        //Shader object and source code mapping
        glShaderSource(gVertexShaderObject_PF, 1, &vertextShaderSourceCode_PF, NULL);
        
        //Compile Shader
        glCompileShader(gVertexShaderObject_PF);
        
        //Error Checking
        GLint iShaderCompileStatus = 0;
        GLint iInfoLogLength = 0;
        GLchar * szInfoLog = NULL;
        
        glGetShaderiv(gVertexShaderObject_PF, GL_COMPILE_STATUS, &iShaderCompileStatus);
        
        if (iShaderCompileStatus == GL_FALSE) {
            glGetShaderiv(gVertexShaderObject_PF, GL_INFO_LOG_LENGTH, &iInfoLogLength);
            if (iInfoLogLength > 0) {
                szInfoLog = (GLchar *)malloc(iInfoLogLength);
                if (szInfoLog != NULL) {
                    GLsizei written;
                    glGetShaderInfoLog(gVertexShaderObject_PF,
                                       iInfoLogLength,
                                       &written,
                                       szInfoLog);
                    printf("Vertex Shader PF Compilation Log %s\n", szInfoLog);
                    free(szInfoLog);
                    [self dealloc];
                    
                    exit(0);
                }
            }
        }
        
        //Fragement Shader
        //Define Shader Object
        gFragementShaderObject_PF = glCreateShader(GL_FRAGMENT_SHADER);
        
        //Fragement Shader Code
        const GLchar* fragementShaderSourceCode_PF =
        "#version 300 es" \
        "\n " \
        "precision highp float;"\
        "precision highp int;" \
        "out vec4 fragColor;"
        "uniform vec3 u_ld;" \
        "uniform vec3 u_kd;" \
        "uniform vec3 u_la;" \
        "uniform vec3 u_ka;" \
        "uniform vec3 u_ls;" \
        "uniform vec3 u_ks;" \
        "uniform float u_shininess;" \
        "uniform int u_lkeyispressed;" \
        "in vec3 tnorm;"\
        "in vec3 lightDirection;"\
        "in vec3 reflectionVector;"\
        "in vec3 viewerVector;"\
        "void main(void)" \
        "{" \
        "vec3 phongADSLight;"\
        "if(u_lkeyispressed==1)"\
        "{"\
        "vec3 tnorm_normalize=normalize(tnorm);"\
        "vec3 lightDirection_normalize=normalize(lightDirection);"\
        "vec3 reflectionVector_normalize=normalize(reflectionVector);"\
        "vec3 viewerVector_normalize=normalize(viewerVector);"\
        "float tn_dot_ld=max(dot(lightDirection_normalize,tnorm_normalize),0.0);"\
        "vec3 ambient=u_la*u_ka;"\
        "vec3 diffuse=u_ld*u_kd*tn_dot_ld;"
        "vec3 specular=u_ls*u_ks*pow(max(dot(reflectionVector_normalize,viewerVector_normalize),0.0),u_shininess);"\
        "phongADSLight=ambient+diffuse+specular;"\
        "}"\
        "else"\
        "{"\
        "phongADSLight=vec3(1.0f,1.0f,1.0f);"\
        "}"\
        "fragColor=vec4(phongADSLight,1.0f);"\
        "} ";
        
        //Shader object and source code mapping
        glShaderSource(gFragementShaderObject_PF, 1, &fragementShaderSourceCode_PF, NULL);
        
        //Compile Shader
        glCompileShader(gFragementShaderObject_PF);
        
        //Error Checking
        iShaderCompileStatus = 0;
        iInfoLogLength = 0;
        szInfoLog = NULL;
        
        glGetShaderiv(gFragementShaderObject_PF, GL_COMPILE_STATUS, &iShaderCompileStatus);
        
        if (iShaderCompileStatus == GL_FALSE) {
            glGetShaderiv(gFragementShaderObject_PF, GL_INFO_LOG_LENGTH, &iInfoLogLength);
            if (iInfoLogLength > 0) {
                szInfoLog = (GLchar *)malloc(iInfoLogLength);
                if (szInfoLog != NULL) {
                    GLsizei written;
                    glGetShaderInfoLog(gFragementShaderObject_PF,
                                       iInfoLogLength,
                                       &written,
                                       szInfoLog);
                    printf( "Fragement Shader Compilation Log %s\n", szInfoLog);
                    free(szInfoLog);
                    [self dealloc];
                    
                    exit(0);
                }
            }
        }
        
        //Program Object
        gShaderProgramObject_PF = glCreateProgram();
        //Attach Shaders
        glAttachShader(gShaderProgramObject_PF, gVertexShaderObject_PF);
        glAttachShader(gShaderProgramObject_PF, gFragementShaderObject_PF);
        
        //Prelinking
        //Binding to attribute
        glBindAttribLocation(gShaderProgramObject_PF,
                             AMC_ATTRIBUTE_POSITION,
                             "vPosition");
        
        glBindAttribLocation(gShaderProgramObject_PF,
                             AMC_ATTRIBUTE_NORMAL,
                             "vNormal");
        
        //Link Shader Program
        glLinkProgram(gShaderProgramObject_PF);
        
        //Error Checking
        GLint iShaderLinkStatus = 0;
        
        glGetProgramiv(gShaderProgramObject_PF, GL_LINK_STATUS, &iShaderLinkStatus);
        
        if (iShaderLinkStatus == GL_FALSE) {
            glGetProgramiv(gShaderProgramObject_PF, GL_INFO_LOG_LENGTH, &iInfoLogLength);
            if (iInfoLogLength > 0) {
                szInfoLog = (GLchar *)malloc(iInfoLogLength);
                if (szInfoLog != NULL) {
                    GLsizei written;
                    glGetProgramInfoLog(gShaderProgramObject_PF,
                                        iInfoLogLength,
                                        &written,
                                        szInfoLog);
                    printf("Program Link Log %s\n", szInfoLog);
                    free(szInfoLog);
                    [self dealloc];
                    
                    exit(0);
                }
            }
        }
        
        if (perVertexOrFragement == 0) {
            //[self getUniformLocation:gShaderProgramObject_PF];
            //getUniformLocation(gShaderProgramObject_PF);
            mUniform = glGetUniformLocation(gShaderProgramObject_PF,
                                            "u_m_matrix");
            vUniform = glGetUniformLocation(gShaderProgramObject_PF,
                                            "u_v_matrix");
            pUniform = glGetUniformLocation(gShaderProgramObject_PF,
                                            "u_p_matrix");
            lKeyIsPressedUniform = glGetUniformLocation(gShaderProgramObject_PF,
                                                        "u_lkeyispressed");
            ldUniform = glGetUniformLocation(gShaderProgramObject_PF,
                                             "u_ld");
            kdUniform = glGetUniformLocation(gShaderProgramObject_PF,
                                             "u_kd");
            laUniform = glGetUniformLocation(gShaderProgramObject_PF,
                                             "u_la");
            kaUniform = glGetUniformLocation(gShaderProgramObject_PF,
                                             "u_ka");
            lsUniform = glGetUniformLocation(gShaderProgramObject_PF,
                                             "u_ls");
            ksUniform = glGetUniformLocation(gShaderProgramObject_PF,
                                             "u_ks");
            lightPositionUniform = glGetUniformLocation(gShaderProgramObject_PF,
                                                        "u_lightposition");
            shininessUniform = glGetUniformLocation(gShaderProgramObject_PF,
                                                    "u_shininess");
            
        }
        
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        //Vertex Shader
        //Define Shader Object
        gVertexShaderObject_PV = glCreateShader(GL_VERTEX_SHADER);
        
        //Vertex Shader Code
        const GLchar* vertextShaderSourceCode_PV =
        "#version 300 es" \
        "\n " \
        "precision highp float;"\
        "precision highp int;" \
        "in vec4 vPosition;" \
        "in vec3 vNormal;" \
        "uniform mat4 u_m_matrix;" \
        "uniform mat4 u_v_matrix;" \
        "uniform mat4 u_p_matrix;" \
        "uniform int u_lkeyispressed;" \
        "uniform vec3 u_ld;" \
        "uniform vec3 u_kd;" \
        "uniform vec3 u_la;" \
        "uniform vec3 u_ka;" \
        "uniform vec3 u_ls;" \
        "uniform vec3 u_ks;" \
        "uniform float u_shininess;" \
        "uniform vec4 u_lightposition;" \
        "out vec3 phongADSLight;" \
        "void main(void)" \
        "{" \
        "gl_Position=u_p_matrix*u_v_matrix*u_m_matrix * vPosition;" \
        "if(u_lkeyispressed==1)"\
        "{"\
        "vec4 eyecoordinates=u_v_matrix*u_m_matrix*vPosition;"\
        "vec3 tnorm=normalize(mat3(u_v_matrix*u_m_matrix)*vNormal);"\
        "vec3 lightDirection=normalize(vec3(u_lightposition-eyecoordinates));"\
        "float tn_dot_ld=max(dot(lightDirection,tnorm),0.0);"\
        "vec3 reflectionVector=reflect(-lightDirection,tnorm);"\
        "vec3 viewerVector=normalize(vec3(-eyecoordinates));"\
        "vec3 ambient=u_la*u_ka;"\
        "vec3 diffuse=u_ld*u_kd*tn_dot_ld;"
        "vec3 specular=u_ls*u_ks*pow(max(dot(reflectionVector,viewerVector),0.0),u_shininess);"\
        "phongADSLight=ambient+diffuse+specular;"\
        "}"\
        "else"\
        "{"\
        "phongADSLight=vec3(1.0f,1.0f,1.0f);"\
        "}"\
        "}";
        
        //Shader object and source code mapping
        glShaderSource(gVertexShaderObject_PV, 1, &vertextShaderSourceCode_PV, NULL);
        
        //Compile Shader
        glCompileShader(gVertexShaderObject_PV);
        
        //Error Checking
        iShaderCompileStatus = 0;
        iInfoLogLength = 0;
        szInfoLog = NULL;
        
        glGetShaderiv(gVertexShaderObject_PV, GL_COMPILE_STATUS, &iShaderCompileStatus);
        
        if (iShaderCompileStatus == GL_FALSE) {
            glGetShaderiv(gVertexShaderObject_PV, GL_INFO_LOG_LENGTH, &iInfoLogLength);
            if (iInfoLogLength > 0) {
                szInfoLog = (GLchar *)malloc(iInfoLogLength);
                if (szInfoLog != NULL) {
                    GLsizei written;
                    glGetShaderInfoLog(gVertexShaderObject_PV,
                                       iInfoLogLength,
                                       &written,
                                       szInfoLog);
                    printf("Vertex Shader PF Compilation Log %s\n", szInfoLog);
                    free(szInfoLog);
                    [self dealloc];
                    
                    exit(0);
                }
            }
        }
        
        //Fragement Shader
        //Define Shader Object
        gFragementShaderObject_PV = glCreateShader(GL_FRAGMENT_SHADER);
        
        //Fragement Shader Code
        const GLchar* fragementShaderSourceCode_PV =
        "#version 300 es" \
        "\n " \
        "precision highp float;"\
        "precision highp int;" \
        "out vec4 fragColor;"
        "in vec3 phongADSLight;" \
        "void main(void)" \
        "{" \
        "fragColor=vec4(phongADSLight,1.0f);"\
        "} ";
        
        //Shader object and source code mapping
        glShaderSource(gFragementShaderObject_PV, 1, &fragementShaderSourceCode_PV, NULL);
        
        //Compile Shader
        glCompileShader(gFragementShaderObject_PV);
        
        //Error Checking
        iShaderCompileStatus = 0;
        iInfoLogLength = 0;
        szInfoLog = NULL;
        
        glGetShaderiv(gFragementShaderObject_PV, GL_COMPILE_STATUS, &iShaderCompileStatus);
        
        if (iShaderCompileStatus == GL_FALSE) {
            glGetShaderiv(gFragementShaderObject_PV, GL_INFO_LOG_LENGTH, &iInfoLogLength);
            if (iInfoLogLength > 0) {
                szInfoLog = (GLchar *)malloc(iInfoLogLength);
                if (szInfoLog != NULL) {
                    GLsizei written;
                    glGetShaderInfoLog(gFragementShaderObject_PV,
                                       iInfoLogLength,
                                       &written,
                                       szInfoLog);
                    printf( "Fragement Shader Compilation Log %s\n", szInfoLog);
                    free(szInfoLog);
                    [self dealloc];
                    
                    exit(0);
                }
            }
        }
        
        //Program Object
        gShaderProgramObject_PV = glCreateProgram();
        //Attach Shaders
        glAttachShader(gShaderProgramObject_PV, gVertexShaderObject_PV);
        glAttachShader(gShaderProgramObject_PV, gFragementShaderObject_PV);
        
        //Prelinking
        //Binding to attribute
        glBindAttribLocation(gShaderProgramObject_PV,
                             AMC_ATTRIBUTE_POSITION,
                             "vPosition");
        
        glBindAttribLocation(gShaderProgramObject_PV,
                             AMC_ATTRIBUTE_NORMAL,
                             "vNormal");
        
        //Link Shader Program
        glLinkProgram(gShaderProgramObject_PV);
        
        //Error Checking
        iShaderLinkStatus = 0;
        
        glGetProgramiv(gShaderProgramObject_PV, GL_LINK_STATUS, &iShaderLinkStatus);
        
        if (iShaderLinkStatus == GL_FALSE) {
            glGetProgramiv(gShaderProgramObject_PV, GL_INFO_LOG_LENGTH, &iInfoLogLength);
            if (iInfoLogLength > 0) {
                szInfoLog = (GLchar *)malloc(iInfoLogLength);
                if (szInfoLog != NULL) {
                    GLsizei written;
                    glGetProgramInfoLog(gShaderProgramObject_PV,
                                        iInfoLogLength,
                                        &written,
                                        szInfoLog);
                    printf( "Program Link Log %s\n", szInfoLog);
                    free(szInfoLog);
                    [self dealloc];
                    
                    exit(0);
                }
            }
        }
        
        if (perVertexOrFragement == 1) {
            //getUniformLocation(gShaderProgramObject_PV);
            //[self getUniformLocation:gShaderProgramObject_PV];
            mUniform = glGetUniformLocation(gShaderProgramObject_PV,
                                            "u_m_matrix");
            vUniform = glGetUniformLocation(gShaderProgramObject_PV,
                                            "u_v_matrix");
            pUniform = glGetUniformLocation(gShaderProgramObject_PV,
                                            "u_p_matrix");
            lKeyIsPressedUniform = glGetUniformLocation(gShaderProgramObject_PV,
                                                        "u_lkeyispressed");
            ldUniform = glGetUniformLocation(gShaderProgramObject_PV,
                                             "u_ld");
            kdUniform = glGetUniformLocation(gShaderProgramObject_PV,
                                             "u_kd");
            laUniform = glGetUniformLocation(gShaderProgramObject_PV,
                                             "u_la");
            kaUniform = glGetUniformLocation(gShaderProgramObject_PV,
                                             "u_ka");
            lsUniform = glGetUniformLocation(gShaderProgramObject_PV,
                                             "u_ls");
            ksUniform = glGetUniformLocation(gShaderProgramObject_PV,
                                             "u_ks");
            lightPositionUniform = glGetUniformLocation(gShaderProgramObject_PV,
                                                        "u_lightposition");
            shininessUniform = glGetUniformLocation(gShaderProgramObject_PV,
                                                    "u_shininess");
            
        }
        
        
        sphereObj.setCenter(0.0f,0.0f);
        // sphereObj.setTextureColor(0.729f , 0.886f , 0.933f);//(1.0f*255.0f, 1.0f*255.0f, 1.0f * 255);
        sphereObj.set(1.0f, 90, 90);
        
        glGenVertexArrays(1, &vaosphere);
        
        glBindVertexArray(vaosphere);
        
        float arrVer[sphereObj.vertices.size()];
        for(int i=0;i<sphereObj.vertices.size();i++){
            arrVer[i]=sphereObj.vertices.at(i);
        }
        
        //fprintf(gpFile,"Colors \n ");
        
        float arrNor[sphereObj.normals.size()];
        for(int i=0;i<sphereObj.normals.size();i++){
            arrNor[i]=sphereObj.normals.at(i);
        }
        
        unsigned int arrIndices[sphereObj.indices.size()];
        
        for(unsigned int i=0;i<sphereObj.indices.size();i++){
            arrIndices[i]=sphereObj.indices.at(i);
        }
        
        //Generate Buffer
        glGenBuffers(1, &vbospherepos);
        //Bind Generated Buffer
        glBindBuffer(GL_ARRAY_BUFFER,
                     vbospherepos);
        
        glBufferData(GL_ARRAY_BUFFER,
                     sphereObj.vertices.size() * sizeof(float),
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
        
        glBufferData(GL_ARRAY_BUFFER,
                     sphereObj.normals.size() * sizeof(float),
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
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereObj.indices.size()*sizeof(unsigned int), arrIndices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
        //Unbind Buffer
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        
        lightAmbient[0] = 0.0f;
        lightAmbient[1] = 0.0f;
        lightAmbient[2] = 0.0f;
        lightAmbient[3] = 0.0f;
        lightDiffuse[0] = 1.0f;
        lightDiffuse[1] = 1.0f;
        lightDiffuse[2] = 1.0f;
        lightDiffuse[3] = 1.0f;
        lightSpecular[0] = 1.0f;
        lightSpecular[1] = 1.0f;
        lightSpecular[2] = 1.0f;
        lightSpecular[3] = 1.0f;
        lightPosition[0] = 100.0f;
        lightPosition[1] = 100.0f;
        lightPosition[2] = 100.0f;
        lightPosition[3] = 100.0f;
        materialAmbient[0] = 0.0f;
        materialAmbient[1] = 0.0f;
        materialAmbient[2] = 0.0f;
        materialAmbient[3] = 0.0f;
        materialDiffuse[0] = 1.0f;
        materialDiffuse[1] = 1.0f;
        materialDiffuse[2] = 1.0f;
        materialDiffuse[3] = 1.0f;
        materialSpecular[0] = 1.0f;
        materialSpecular[1] = 1.0f;
        materialSpecular[2] = 1.0f;
        materialSpecular[3] = 1.0f;
        materialShininess = 128.0f;
        
        
        
        
        
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
    
    if (perVertexOrFragement == 1){
        glUseProgram(gShaderProgramObject_PV);
        if(resetUniform==1){
            mUniform = glGetUniformLocation(gShaderProgramObject_PV,
                                            "u_m_matrix");
            vUniform = glGetUniformLocation(gShaderProgramObject_PV,
                                            "u_v_matrix");
            pUniform = glGetUniformLocation(gShaderProgramObject_PV,
                                            "u_p_matrix");
            lKeyIsPressedUniform = glGetUniformLocation(gShaderProgramObject_PV,
                                                        "u_lkeyispressed");
            ldUniform = glGetUniformLocation(gShaderProgramObject_PV,
                                             "u_ld");
            kdUniform = glGetUniformLocation(gShaderProgramObject_PV,
                                             "u_kd");
            laUniform = glGetUniformLocation(gShaderProgramObject_PV,
                                             "u_la");
            kaUniform = glGetUniformLocation(gShaderProgramObject_PV,
                                             "u_ka");
            lsUniform = glGetUniformLocation(gShaderProgramObject_PV,
                                             "u_ls");
            ksUniform = glGetUniformLocation(gShaderProgramObject_PV,
                                             "u_ks");
            lightPositionUniform = glGetUniformLocation(gShaderProgramObject_PV,
                                                        "u_lightposition");
            shininessUniform = glGetUniformLocation(gShaderProgramObject_PV,
                                                    "u_shininess");
            resetUniform=0;
            
        }
    }
    else{
        glUseProgram(gShaderProgramObject_PF);
        if(resetUniform==1){
            mUniform = glGetUniformLocation(gShaderProgramObject_PF,
                                            "u_m_matrix");
            vUniform = glGetUniformLocation(gShaderProgramObject_PF,
                                            "u_v_matrix");
            pUniform = glGetUniformLocation(gShaderProgramObject_PF,
                                            "u_p_matrix");
            lKeyIsPressedUniform = glGetUniformLocation(gShaderProgramObject_PF,
                                                        "u_lkeyispressed");
            ldUniform = glGetUniformLocation(gShaderProgramObject_PF,
                                             "u_ld");
            kdUniform = glGetUniformLocation(gShaderProgramObject_PF,
                                             "u_kd");
            laUniform = glGetUniformLocation(gShaderProgramObject_PF,
                                             "u_la");
            kaUniform = glGetUniformLocation(gShaderProgramObject_PF,
                                             "u_ka");
            lsUniform = glGetUniformLocation(gShaderProgramObject_PF,
                                             "u_ls");
            ksUniform = glGetUniformLocation(gShaderProgramObject_PF,
                                             "u_ks");
            lightPositionUniform = glGetUniformLocation(gShaderProgramObject_PF,
                                                        "u_lightposition");
            shininessUniform = glGetUniformLocation(gShaderProgramObject_PF,
                                                    "u_shininess");
            resetUniform=0;
        }
    }
    
    vmath::mat4 modelMatrix = vmath::mat4::identity();
    vmath::mat4 viewMatrix = vmath::mat4::identity();
    vmath::mat4 translationMatrix = vmath::mat4::identity();
    vmath::mat4 rotationMatrix = vmath::mat4::identity();
    vmath::mat4 scaleMatrix = vmath::mat4::identity();
    
    //Transformation
    
    translationMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
    //rotationMatrix = vmath::rotate(angleCube,angleCube,angleCube);
    //scaleMatrix=vmath::scale(0.75f,0.75f,0.75f);
    //Matrix Multiplication
    modelMatrix =   translationMatrix * scaleMatrix;
    modelMatrix =    modelMatrix    *   rotationMatrix;
    
    //modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
    //Send necessary matrices to shader in resp. Uniforms
    
    glUniformMatrix4fv(mUniform,
                       1,
                       GL_FALSE,
                       modelMatrix);
    glUniformMatrix4fv(vUniform,
                       1,
                       GL_FALSE,
                       viewMatrix);
    glUniformMatrix4fv(pUniform,
                       1,
                       GL_FALSE,
                       perspectiveProjectionMatrix);
    
    if (gb_Lighting) {
        glUniform1i(lKeyIsPressedUniform, gb_Lighting);
        
        glUniform3fv(ldUniform,1,lightDiffuse);
        glUniform3fv(laUniform, 1, lightAmbient);
        glUniform3fv(lsUniform, 1, lightSpecular);
        
        glUniform3fv(kaUniform, 1, materialAmbient);
        glUniform3fv(kdUniform, 1, materialDiffuse);
        glUniform3fv(ksUniform, 1, materialSpecular);
        
        glUniform1f(shininessUniform, materialShininess);
        
        glUniform4fv(lightPositionUniform, 1, lightPosition);
    }
    else {
        glUniform1i(lKeyIsPressedUniform, gb_Lighting);
    }
    
    glBindVertexArray(vaosphere);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
    glDrawElements(GL_TRIANGLES, sphereObj.indices.size(), GL_UNSIGNED_INT, 0);
    
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
    resetUniform=1;
    //[self getUniformLocation:gShaderProgramObject_PV];
    
    
    if(perVertexOrFragement==0)
        perVertexOrFragement = 1;
    else
        perVertexOrFragement=0;
    
}

-(void)onDoubleTap:(UITapGestureRecognizer *)gr
{
    //resetUniform=1;
    //[self getUniformLocation:gShaderProgramObject_PF];
    //perVertexOrFragement = 0;
    if (gb_Lighting == 0)
        gb_Lighting = 1;
    else
        gb_Lighting = 0;
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
    
    if (gShaderProgramObject_PV) {
        GLsizei shaderCount;
        GLsizei shaderNumber;
        glUseProgram(gShaderProgramObject_PV);
        
        glGetProgramiv(gShaderProgramObject_PV,
                       GL_ATTACHED_SHADERS,
                       &shaderCount);
        
        GLuint * pShaders = (GLuint *)malloc(shaderCount * sizeof(GLuint));
        
        if (pShaders) {
            glGetAttachedShaders(gShaderProgramObject_PV,
                                 shaderCount,
                                 &shaderCount,
                                 pShaders);
            
            for (shaderNumber = 0; shaderNumber < shaderCount; shaderNumber++) {
                glDetachShader(gShaderProgramObject_PV, pShaders[shaderNumber]);
                glDeleteShader(pShaders[shaderNumber]);
                pShaders[shaderNumber] = 0;
            }
            free(pShaders);
        }
        glDeleteProgram(gShaderProgramObject_PV);
        gShaderProgramObject_PV = 0;
        glUseProgram(0);
    }
    
    if (gShaderProgramObject_PF) {
        GLsizei shaderCount;
        GLsizei shaderNumber;
        glUseProgram(gShaderProgramObject_PF);
        
        glGetProgramiv(gShaderProgramObject_PF,
                       GL_ATTACHED_SHADERS,
                       &shaderCount);
        
        GLuint * pShaders = (GLuint *)malloc(shaderCount * sizeof(GLuint));
        
        if (pShaders) {
            glGetAttachedShaders(gShaderProgramObject_PF,
                                 shaderCount,
                                 &shaderCount,
                                 pShaders);
            
            for (shaderNumber = 0; shaderNumber < shaderCount; shaderNumber++) {
                glDetachShader(gShaderProgramObject_PF, pShaders[shaderNumber]);
                glDeleteShader(pShaders[shaderNumber]);
                pShaders[shaderNumber] = 0;
            }
            free(pShaders);
        }
        glDeleteProgram(gShaderProgramObject_PF);
        gShaderProgramObject_PF = 0;
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

