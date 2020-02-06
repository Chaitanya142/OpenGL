package com.example.ogl_tesselation_sheader;

import android.content.Context;
import android.view.Gravity;
import android.graphics.Color;

import android.view.MotionEvent;
import android.view.GestureDetector;
import android.view.GestureDetector.OnGestureListener;
import android.view.GestureDetector.OnDoubleTapListener;

import android.opengl.GLES32;
import android.opengl.GLSurfaceView;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import android.opengl.Matrix;

public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener
{
	private static Context context;
	private GestureDetector gestureDetector;
	
	private int shaderProgramObject;
	//Shader Objects
	private int gVertexShaderObject;
	private int gFragementShaderObject;
	private int gTessellationControlShaderObject;
	private int gTessellationEvaluationShaderObject;

	private int[] vao=new int[1];
	private int[] vbo=new int[1];

	private int mvpUniform;
	private int gNumberOfSegmentsUniform;
	private int gNumberOfStripsUniform;
	private int gLineColorUniform;

	private int gNumberOfLineSegments;

	private float[] perspectiveProjectionMatrix=new float[16];
	
	public GLESView(Context drawingContext){
		super(drawingContext);
		
		context=drawingContext;
		setEGLContextClientVersion(3);
		setRenderer(this);
		setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
		
		gestureDetector=new GestureDetector(drawingContext,this,null,false);
		gestureDetector.setOnDoubleTapListener(this);
	}
	
	@Override
	public boolean onTouchEvent(MotionEvent event)
	{
		int eventaction=event.getAction();
		if(!gestureDetector.onTouchEvent(event))
		{
			super.onTouchEvent(event);
		}
		return(true);
	}
	
	@Override
	public boolean onDoubleTap(MotionEvent e)
	{
		System.out.println("RTR :"+gNumberOfLineSegments);
		gNumberOfLineSegments--;
		if (gNumberOfLineSegments <= 0)
			gNumberOfLineSegments = 1;
		return(true);
	}
	
	@Override
	public boolean onDoubleTapEvent(MotionEvent e)
	{
		return(true);
	}
	
	@Override
	public boolean onSingleTapConfirmed(MotionEvent e)
	{
		System.out.println("RTR :"+gNumberOfLineSegments);
		gNumberOfLineSegments++;
		if (gNumberOfLineSegments >= 50)
			gNumberOfLineSegments = 50;
		return(true);
	}
	
	@Override
	public boolean onDown(MotionEvent e)
	{
		return(true);	
	}
	
	@Override
	public boolean onFling(MotionEvent e1,MotionEvent e2,float velocityX,float velocityY)
	{
		return(true);
	}
	
	@Override
	public void onLongPress(MotionEvent e)
	{
		
	}
	
	@Override
	public boolean onScroll(MotionEvent e1, MotionEvent e2,float distanceX,float distanceY)
	{
		UnInitialize();
		System.exit(0);
		return(true);
	}
	
	@Override
	public void onShowPress(MotionEvent e)
	{
		
	}
	
	@Override
	public boolean onSingleTapUp(MotionEvent e)
	{
		return(true);
	}
	
	
	@Override
	public void onSurfaceCreated(GL10 gl,EGLConfig config)
	{
		String Version=gl.glGetString(GL10.GL_VERSION);
		String shaderVersion=gl.glGetString(GLES32.GL_SHADING_LANGUAGE_VERSION);

		System.out.println("RTR :"+Version);
		System.out.println("RTR :"+shaderVersion);

		Initialize();
	}
	
	@Override
	public void onSurfaceChanged(GL10 unused,int width,int height)
	{
		Resize(width,height);
	}
	
	@Override
	public void onDrawFrame(GL10 unused)
	{
		Display();
	}
	
	private void Resize(int width, int height) {
		if (height == 0)
			height = 1;
	
		GLES32.glViewport(0, 0, width,height);
	
		Matrix.perspectiveM(perspectiveProjectionMatrix,0,
		45.0f,
		(float)width / (float)height,
		0.1f,
		100.0f);

	}
	
	private void Display() {
		
		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

		GLES32.glUseProgram(shaderProgramObject);
	
		//Declaration of Matrices
		float[] modelViewMatrix=new float[16];
		float[] modelViewProjectionMatrix=new float[16];
		float[] translationMatrix=new float[16];
		float[] rotationMatrix=new float[16];
		float[] scaleMatrix=new float[16];

		//Initialize matrices
		Matrix.setIdentityM(modelViewMatrix,0);
		Matrix.setIdentityM(modelViewProjectionMatrix,0);
		Matrix.setIdentityM(translationMatrix,0);
		Matrix.setIdentityM(rotationMatrix,0);
		Matrix.setIdentityM(scaleMatrix,0);

		final float[] colorVec = new float[]
		{
			1.0f,1.0f,1.0f,1.0f
		};	

		//Transformation

		Matrix.translateM(translationMatrix,0,
		0.5f, 0.5f, -5.0f);
	
		
		Matrix.multiplyMM(modelViewMatrix,0,
		translationMatrix,0,
		rotationMatrix,0);
		
		Matrix.multiplyMM(modelViewProjectionMatrix,0,
		perspectiveProjectionMatrix,0,
		modelViewMatrix,0);
		
		//Send necessary matrices to shader in resp. Uniforms
	
		GLES32.glUniformMatrix4fv(mvpUniform,
			1,
			false,
			modelViewProjectionMatrix,0);

		GLES32.glUniform1i(gNumberOfSegmentsUniform,gNumberOfLineSegments);
		
		GLES32.glUniform1i(gNumberOfStripsUniform,1);

		GLES32.glUniform4fv(gLineColorUniform,
			1,
			colorVec,0);

		GLES32.glPatchParameteri(GLES32.GL_PATCH_VERTICES,4);

		GLES32.glBindVertexArray(vao[0]);
	
		//Bind with textures if any
	
		//Draw
	
		GLES32.glDrawArrays(GLES32.GL_PATCHES,
			0,
			4);
	
		//Unbind vao
	
		GLES32.glBindVertexArray(0);

		Update();

		requestRender();

	}
	private void Update()
	{
	}


	private void Initialize(){

	gNumberOfLineSegments=1;
	
	//Vertex Shader
	//Define Shader Object
	gVertexShaderObject = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);

	//Vertex Shader Code
	final String vertextShaderSourceCode=String.format(
		"#version 320 es " +
		"\n " +
		"in vec2 vPosition;" +
		"void main(void)" +
		"{" +
		"gl_Position=vec4(vPosition,0.0f,1.0f);"+
		"} "
	);

	//Shader object and source code mapping
	GLES32.glShaderSource(gVertexShaderObject,
						vertextShaderSourceCode);

	//Compile Shader
	GLES32.glCompileShader(gVertexShaderObject);

	//Error Checking
	int[] iShaderCompileStatus = new int[1];
	int[] iInfoLogLength = new int[1];
	String szInfoLog = null;

	GLES32.glGetShaderiv(gVertexShaderObject, GLES32.GL_COMPILE_STATUS, 
						iShaderCompileStatus,0);

	if (iShaderCompileStatus[0] == GLES32.GL_FALSE) {
		
		GLES32.glGetShaderiv(gVertexShaderObject, GLES32.GL_INFO_LOG_LENGTH, 
							iInfoLogLength,0);
		
		if (iInfoLogLength[0] > 0) {

				szInfoLog=GLES32.glGetShaderInfoLog(gVertexShaderObject);

				System.out.println("RTR : gVertexShaderObject "+szInfoLog);
				UnInitialize();
				System.exit(0);			
		}
	}



	gTessellationControlShaderObject = GLES32.glCreateShader(GLES32.GL_TESS_CONTROL_SHADER);

	//Tesselation Shader Code
	final String tessellationControlShaderSourceCode=String.format(
		"#version 320 es " +
		"\n " +
		"layout(vertices=4)out;"+
		"uniform int numberOfSegments;"+
		"uniform int numberOfStrips;"+
		"void main(void)" +
		"{" +
		"gl_out[gl_InvocationID].gl_Position=gl_in[gl_InvocationID].gl_Position;"+
		"gl_TessLevelOuter[0]=float(numberOfStrips);"+
		"gl_TessLevelOuter[1]=float(numberOfSegments);"+
		"}"
	);

	//Shader object and source code mapping
	GLES32.glShaderSource(gTessellationControlShaderObject,
	tessellationControlShaderSourceCode);

	//Compile Shader
	GLES32.glCompileShader(gTessellationControlShaderObject);

	//Error Checking	
	szInfoLog = null;

	GLES32.glGetShaderiv(gTessellationControlShaderObject, GLES32.GL_COMPILE_STATUS, 
						iShaderCompileStatus,0);

	if (iShaderCompileStatus[0] == GLES32.GL_FALSE) {
		
		GLES32.glGetShaderiv(gTessellationControlShaderObject, GLES32.GL_INFO_LOG_LENGTH, 
							iInfoLogLength,0);
		
		if (iInfoLogLength[0] > 0) {

				szInfoLog=GLES32.glGetShaderInfoLog(gTessellationControlShaderObject);

				System.out.println("RTR : Tesselation Control Shader "+szInfoLog);
				UnInitialize();
				System.exit(0);			
		}
	}


	gTessellationEvaluationShaderObject = GLES32.glCreateShader(GLES32.GL_TESS_EVALUATION_SHADER);
	
	//Vertex Shader Code
	final String tessellationEvaluationShaderSourceCode=String.format(
		"#version 320 es " +
		"\n " +
		"layout(isolines)in;"+
		"uniform mat4 u_mvp_matrix;" +
		"void main(void)" +
		"{" +
		"float u=gl_TessCoord.x;"+
		"vec3 p0=gl_in[0].gl_Position.xyz;"+
		"vec3 p1=gl_in[1].gl_Position.xyz;"+
		"vec3 p2=gl_in[2].gl_Position.xyz;"+
		"vec3 p3=gl_in[3].gl_Position.xyz;"+
		"float u1=	(1.0f	-	u);"+
		"float u2=	u	*	u;"+
		"float b3=	u2	*	u;"+
		"float b2=	3.0f	*	u2	*	u1;"+
		"float b1=	3.0f	*	u	*	u1	*	u1;"+
		"float b0=	u1	*	u1	*	u1;"+
		"vec3 p=	p0*b0	+	p1*b1	+	p2*b2	+	p3*b3;"+
		"gl_Position=u_mvp_matrix * vec4(	p,	1.0f);" +
		"} "
	);

	//Shader object and source code mapping
	GLES32.glShaderSource(gTessellationEvaluationShaderObject,
	tessellationEvaluationShaderSourceCode);

	//Compile Shader
	GLES32.glCompileShader(gTessellationEvaluationShaderObject);

	//Error Checking
	szInfoLog = null;

	GLES32.glGetShaderiv(gTessellationEvaluationShaderObject, GLES32.GL_COMPILE_STATUS, 
						iShaderCompileStatus,0);

	if (iShaderCompileStatus[0] == GLES32.GL_FALSE) {
		
		GLES32.glGetShaderiv(gTessellationEvaluationShaderObject, GLES32.GL_INFO_LOG_LENGTH, 
							iInfoLogLength,0);
		
		if (iInfoLogLength[0] > 0) {

				szInfoLog=GLES32.glGetShaderInfoLog(gTessellationEvaluationShaderObject);

				System.out.println("RTR : Tesselation Evaluation Shader "+szInfoLog);
				UnInitialize();
				System.exit(0);			
		}
	}


		//Fragement Shader
		//Define Shader Object
		gFragementShaderObject = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);

		//Vertex Shader Code
		final String fragementShaderSourceCode=String.format(
			"#version 320 es" +
			"\n " +
			"precision highp float;"+
			"uniform vec4 lineColor;"+
			"out vec4 fragColor;"+
			"void main(void)" +
			"{" +
			"fragColor=lineColor;" +
			"} "
		);
	
		//Shader object and source code mapping
		GLES32.glShaderSource(gFragementShaderObject,
							fragementShaderSourceCode);
	
		//Compile Shader
		GLES32.glCompileShader(gFragementShaderObject);

		//Error Checking
		iShaderCompileStatus[0] = 0;
		iInfoLogLength[0] = 0;
		szInfoLog = null;
	
		GLES32.glGetShaderiv(gFragementShaderObject, GLES32.GL_COMPILE_STATUS, 
							iShaderCompileStatus,0);
	
		if (iShaderCompileStatus[0] == GLES32.GL_FALSE) {
			
			GLES32.glGetShaderiv(gFragementShaderObject, GLES32.GL_INFO_LOG_LENGTH, 
								iInfoLogLength,0);
			
			if (iInfoLogLength[0] > 0) {
	
					szInfoLog=GLES32.glGetShaderInfoLog(gFragementShaderObject);
	
					System.out.println("RTR : gFragementShaderObject"+szInfoLog);
					UnInitialize();
					System.exit(0);			
			}
		}


	//	System.out.println("RTR : above tes control ");

	//Program Object
	shaderProgramObject = GLES32.glCreateProgram();
	//Attach Shaders
	GLES32.glAttachShader(shaderProgramObject, gVertexShaderObject);
	GLES32.glAttachShader(shaderProgramObject, gTessellationControlShaderObject);
	GLES32.glAttachShader(shaderProgramObject, gTessellationEvaluationShaderObject);
	GLES32.glAttachShader(shaderProgramObject, gFragementShaderObject);

	//Prelinking
	//Binding to attribute
	GLES32.glBindAttribLocation(shaderProgramObject,
		GLESMicros.AMC_ATTRIBUTE_POSITION,
		"vPosition");

	// GLES32.glBindAttribLocation(shaderProgramObject,
	// 	GLESMicros.AMC_ATTRIBUTE_COLOR,
	// 	"vColor");

	//Link Shader Program
	GLES32.glLinkProgram(shaderProgramObject);

	//Error Checking
	int[] iShaderLinkStatus = new int[1];
	iInfoLogLength[0] = 0;
	szInfoLog = null;

	GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_LINK_STATUS,
						 iShaderLinkStatus,0);

	if (iShaderLinkStatus[0] == GLES32.GL_FALSE) {
		GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_INFO_LOG_LENGTH,
							 iInfoLogLength,0);

		if (iInfoLogLength[0] > 0) {
			szInfoLog=GLES32.glGetProgramInfoLog(shaderProgramObject);

			System.out.println("RTR : shaderProgramObject"+szInfoLog);
			UnInitialize();
			System.exit(0);	
			
		}
	}

	//Post Linking
	//Retriving Uniform Location
	mvpUniform = GLES32.glGetUniformLocation(shaderProgramObject,
		"u_mvp_matrix");


	gNumberOfSegmentsUniform = GLES32.glGetUniformLocation(shaderProgramObject,
		"numberOfSegments");

	gNumberOfStripsUniform = GLES32.glGetUniformLocation(shaderProgramObject,
		"numberOfStrips");

	gLineColorUniform = GLES32.glGetUniformLocation(shaderProgramObject,
		"lineColor");



		final float[] vertices = new float[]
	{
		-1.0f,-1.0f,
		-0.5f,1.0f,
		0.5f,-1.0f,
		1.0f,1.0f
	};	


	//Create vao
	//Save everying in single set
	GLES32.glGenVertexArrays(1,vao,0);

	GLES32.glBindVertexArray(vao[0]);

	//Posistion
	//Generate Buffer
	GLES32.glGenBuffers(1,vbo,0);
	//Bind Generated Buffer
	GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,
		vbo[0]);

	ByteBuffer byteBuffer=ByteBuffer.allocateDirect(vertices.length*4);

	byteBuffer.order(ByteOrder.nativeOrder());

	FloatBuffer positionBuffer=byteBuffer.asFloatBuffer();

	positionBuffer.put(vertices);

	positionBuffer.position(0);

	//Fill Buffer
	GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
		vertices.length*4,
		positionBuffer,
		GLES32.GL_STATIC_DRAW);

	//Set Vertex Attrib Pointer
	GLES32.glVertexAttribPointer(GLESMicros.AMC_ATTRIBUTE_POSITION,
		2,
		GLES32.GL_FLOAT,
		false,
		0,
		0);

	//Enable Vertex Attrib Array
	GLES32.glEnableVertexAttribArray(GLESMicros.AMC_ATTRIBUTE_POSITION);
	//Unbind Buffer
	GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
	//Unbind array
	GLES32.glBindVertexArray(0);


	GLES32.glEnable(GLES32.GL_DEPTH_TEST);
	GLES32.glDepthFunc(GLES32.GL_LEQUAL);
	GLES32.glClearDepthf(1.0f);

	GLES32.glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	//GLES32.glEnable(GLES32.GL_TEXTURE_2D);

	Matrix.setIdentityM(perspectiveProjectionMatrix,0);

	}


	void UnInitialize() {
		if (vbo[0]!=0) {
			GLES32.glDeleteBuffers(1,vbo,0);
			vbo[0] = 0;
		}
		if (vao[0]!=0) {
			GLES32.glDeleteVertexArrays(1,vao,0);
			vao[0] = 0;
		}
	
		
		if (shaderProgramObject != 0) {
			int[] shaderCount=new int[1];

			GLES32.glUseProgram(shaderProgramObject);
			
			GLES32.glGetProgramiv(shaderProgramObject,
								GLES32.GL_ATTACHED_SHADERS,
								shaderCount,0);	
	
			if (shaderCount[0]!=0) {
				
				int shaderNumber;
				int[] shaders = new int[shaderCount[0]];

				GLES32.glGetAttachedShaders(shaderProgramObject,
											shaderCount[0],
											shaderCount,0,
											shaders,0);
	
				for (shaderNumber = 0; shaderNumber < shaderCount[0]; shaderNumber++) {
					GLES32.glDetachShader(shaderProgramObject, shaders[shaderNumber]);
					GLES32.glDeleteShader(shaders[shaderNumber]);
					shaders[shaderNumber] = 0;
				}
			}
			GLES32.glDeleteProgram(shaderProgramObject);
			shaderProgramObject = 0;
			GLES32.glUseProgram(0);
		}	
	}
	
}