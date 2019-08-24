package com.example.ogl_3d_animation;

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
	
	private int[] vao_pyramid=new int[1];
	private int[] vao_cube=new int[1];

	private int[] vbo_position_pyramid=new int[1];
	private int[] vbo_position_cube=new int[1];

	private int[] vbo_color_pyramid=new int[1];
	private int[] vbo_color_cube=new int[1];

	private int mvpUniform;

	private float[] perspectiveProjectionMatrix=new float[16];

	private float anglePyramid=0.0f;
	private float angleCube=0.0f;
	
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

		System.out.println(Version);
		System.out.println(shaderVersion);

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

		//Transformation

		Matrix.translateM(translationMatrix,0,
		-1.5f, 0.0f, -6.0f);
		Matrix.setRotateM(rotationMatrix,0,
		anglePyramid,0.0f, 1.0f, 0.0f);

		//Matrix Multiplication
		
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
	
		//Bind with vao
	
		GLES32.glBindVertexArray(vao_pyramid[0]);
	
		//Bind with textures if any
	
		//Draw
	
		GLES32.glDrawArrays(GLES32.GL_TRIANGLES,
			0,
			12);
	
		//Unbind vao
	
		GLES32.glBindVertexArray(0);
	

		//Initialize matrices
		Matrix.setIdentityM(modelViewMatrix,0);
		Matrix.setIdentityM(modelViewProjectionMatrix,0);
		Matrix.setIdentityM(translationMatrix,0);
		Matrix.setIdentityM(rotationMatrix,0);
		Matrix.setIdentityM(scaleMatrix,0);

		//Transformation

		Matrix.translateM(translationMatrix,0,
		1.5f, 0.0f, -6.0f);
	
		Matrix.setRotateM(rotationMatrix,0,
		angleCube,1.0f, 1.0f, 1.0f);

		Matrix.scaleM(scaleMatrix,0,
		0.75f,0.75f,0.75f);
		//Matrix Multiplication

		Matrix.multiplyMM(modelViewMatrix,0,
		translationMatrix,0,
		rotationMatrix,0);

		Matrix.multiplyMM(modelViewMatrix,0,
		modelViewMatrix,0,
		scaleMatrix,0);

		Matrix.multiplyMM(modelViewProjectionMatrix,0,
		perspectiveProjectionMatrix,0,
		modelViewMatrix,0);

		//Send necessary matrices to shader in resp. Uniforms
	
		GLES32.glUniformMatrix4fv(mvpUniform,
			1,
			false,
			modelViewProjectionMatrix,0);
	
		//Bind with vao
	
		GLES32.glBindVertexArray(vao_cube[0]);
	
		//Bind with textures if any
	
		//Draw
	
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN,
			0,
			4);
	
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN,
			4,
			4);
	
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN,
			8,
			4);
	
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN,
			12,
			4);
	
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN,
			16,
			4);
	
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN,
			20,
			4);
	
		//Unbind vao
	
		GLES32.glBindVertexArray(0);

		GLES32.glUseProgram(0);

		Update();

		requestRender();

	}
	private void Update()
	{
		anglePyramid = anglePyramid + 0.2f;
		if (anglePyramid > 360.0f)
			anglePyramid = 0.0f;
		angleCube = angleCube - 0.2f;
		if (angleCube < -360.0f)
			angleCube = 0.0f;
	}


	private void Initialize(){

	//Vertex Shader
	//Define Shader Object
	gVertexShaderObject = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);

	//Vertex Shader Code
	final String vertextShaderSourceCode=String.format(
		"#version 320 es " +
		"\n " +
		"in vec4 vPosition;" +
		"in vec4 vColor;" +
		"uniform mat4 u_mvp_matrix;" +
		"out vec4 out_color;" +
		"void main(void)" +
		"{" +
		"gl_Position=u_mvp_matrix * vPosition;" +
		"out_color=vColor;"+
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

		//Fragement Shader
		//Define Shader Object
		gFragementShaderObject = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);

		//Vertex Shader Code
		final String fragementShaderSourceCode=String.format(
			"#version 320 es" +
			"\n " +
			"precision highp float;"+
			"in vec4 out_color;"+
			"out vec4 fragColor;"+
			"void main(void)" +
			"{" +
			"fragColor=out_color;" +
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



	//Program Object
	shaderProgramObject = GLES32.glCreateProgram();
	//Attach Shaders
	GLES32.glAttachShader(shaderProgramObject, gVertexShaderObject);
	GLES32.glAttachShader(shaderProgramObject, gFragementShaderObject);

	//Prelinking
	//Binding to attribute
	GLES32.glBindAttribLocation(shaderProgramObject,
		GLESMicros.AMC_ATTRIBUTE_POSITION,
		"vPosition");

	GLES32.glBindAttribLocation(shaderProgramObject,
		GLESMicros.AMC_ATTRIBUTE_COLOR,
		"vColor");

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


	//Vertices
	final float[] pyramidVertices = new float[]
	{
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

	final float[] cubeVertices = new float[]
	{
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

		//Vertices
	final float[] pyramidColors = new float[]
	{
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f
	};	

	final float[] cubeColors = new float[]
	{
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f
	};	

											//TRIANGLE
	//Create vao
	//Save everying in single set
	GLES32.glGenVertexArrays(1,vao_pyramid,0);

	GLES32.glBindVertexArray(vao_pyramid[0]);

	//Posistion
	//Generate Buffer
	GLES32.glGenBuffers(1,vbo_position_pyramid,0);
	//Bind Generated Buffer
	GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,
		vbo_position_pyramid[0]);

	ByteBuffer byteBuffer=ByteBuffer.allocateDirect(pyramidVertices.length*4);

	byteBuffer.order(ByteOrder.nativeOrder());

	FloatBuffer positionBuffer=byteBuffer.asFloatBuffer();

	positionBuffer.put(pyramidVertices);

	positionBuffer.position(0);

	//Fill Buffer
	GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
		pyramidVertices.length*4,
		positionBuffer,
		GLES32.GL_STATIC_DRAW);

	//Set Vertex Attrib Pointer
	GLES32.glVertexAttribPointer(GLESMicros.AMC_ATTRIBUTE_POSITION,
		3,
		GLES32.GL_FLOAT,
		false,
		0,
		0);

	//Enable Vertex Attrib Array
	GLES32.glEnableVertexAttribArray(GLESMicros.AMC_ATTRIBUTE_POSITION);
	//Unbind Buffer
	GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
	//Unbind array
	//GLES32.glBindVertexArray(0);

	//COLOR
	//Generate Buffer
	GLES32.glGenBuffers(1,vbo_color_pyramid,0);
	//Bind Generated Buffer
	GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,
	vbo_color_pyramid[0]);

	byteBuffer=ByteBuffer.allocateDirect(pyramidColors.length*4);

	byteBuffer.order(ByteOrder.nativeOrder());

	FloatBuffer colorBuffer=byteBuffer.asFloatBuffer();

	colorBuffer.put(pyramidColors);

	colorBuffer.position(0);

	//Fill Buffer
	GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
		pyramidColors.length*4,
		colorBuffer,
		GLES32.GL_STATIC_DRAW);

	//Set Vertex Attrib Pointer
	GLES32.glVertexAttribPointer(GLESMicros.AMC_ATTRIBUTE_COLOR,
		3,
		GLES32.GL_FLOAT,
		false,
		0,
		0);

	//Enable Vertex Attrib Array
	GLES32.glEnableVertexAttribArray(GLESMicros.AMC_ATTRIBUTE_COLOR);
	//Unbind Buffer
	GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
	//Unbind array
	GLES32.glBindVertexArray(0);

	
											//RECTANGLE
	//Create vao
	//Save everying in single set
	GLES32.glGenVertexArrays(1,vao_cube,0);

	GLES32.glBindVertexArray(vao_cube[0]);

	//Generate Buffer
	GLES32.glGenBuffers(1,vbo_position_cube,0);
	//Bind Generated Buffer
	GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,
		vbo_position_cube[0]);

	byteBuffer=ByteBuffer.allocateDirect(cubeVertices.length*4);

	byteBuffer.order(ByteOrder.nativeOrder());

	positionBuffer=byteBuffer.asFloatBuffer();

	positionBuffer.put(cubeVertices);

	positionBuffer.position(0);

	
	//Fill Buffer
	GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
		cubeVertices.length*4,
		positionBuffer,
		GLES32.GL_STATIC_DRAW);

	//Set Vertex Attrib Pointer
	GLES32.glVertexAttribPointer(GLESMicros.AMC_ATTRIBUTE_POSITION,
		3,
		GLES32.GL_FLOAT,
		false,
		0,
		0);

	//Enable Vertex Attrib Array
	GLES32.glEnableVertexAttribArray(GLESMicros.AMC_ATTRIBUTE_POSITION);
	//Unbind Buffer
	GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
	//Unbind array
	//GLES32.glBindVertexArray(0);

	//COLOR
	//GLES32.glVertexAttrib3f(GLESMicros.AMC_ATTRIBUTE_COLOR,
	//	0.0f,0.0f,1.0f);

	//Unbind array
	//GLES32.glBindVertexArray(0);

	//COLOR
	//Generate Buffer
	GLES32.glGenBuffers(1,vbo_color_cube,0);
	//Bind Generated Buffer
	GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,
	vbo_color_cube[0]);

	byteBuffer=ByteBuffer.allocateDirect(cubeColors.length*4);

	byteBuffer.order(ByteOrder.nativeOrder());

	colorBuffer=byteBuffer.asFloatBuffer();

	colorBuffer.put(cubeColors);

	colorBuffer.position(0);

	//Fill Buffer
	GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
		cubeColors.length*4,
		colorBuffer,
		GLES32.GL_STATIC_DRAW);

	//Set Vertex Attrib Pointer
	GLES32.glVertexAttribPointer(GLESMicros.AMC_ATTRIBUTE_COLOR,
		3,
		GLES32.GL_FLOAT,
		false,
		0,
		0);

	//Enable Vertex Attrib Array
	GLES32.glEnableVertexAttribArray(GLESMicros.AMC_ATTRIBUTE_COLOR);
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
		if (vbo_position_cube[0]!=0) {
			GLES32.glDeleteBuffers(1,vbo_position_cube,0);
			vbo_position_cube[0] = 0;
		}
		if (vao_cube[0]!=0) {
			GLES32.glDeleteVertexArrays(1,vao_cube,0);
			vao_cube[0] = 0;
		}
		if (vbo_color_cube[0]!=0) {
			GLES32.glDeleteBuffers(1,vbo_color_cube,0);
			vbo_color_cube[0] = 0;
		}
		if (vbo_position_pyramid[0]!=0) {
			GLES32.glDeleteBuffers(1,vbo_position_pyramid,0);
			vbo_position_pyramid[0] = 0;
		}
		if (vbo_color_pyramid[0]!=0) {
			GLES32.glDeleteBuffers(1,vbo_color_pyramid,0);
			vbo_color_pyramid[0] = 0;
		}
		if (vao_pyramid[0]!=0) {
			GLES32.glDeleteVertexArrays(1,vao_pyramid,0);
			vao_pyramid[0] = 0;
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