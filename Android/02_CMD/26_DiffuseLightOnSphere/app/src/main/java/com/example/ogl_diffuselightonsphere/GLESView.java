package com.example.ogl_diffuselightonsphere;

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

import java.nio.ShortBuffer;

import android.opengl.Matrix;

public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener
{
	private static Context context;
	private GestureDetector gestureDetector;
	
	private int shaderProgramObject;
	//Shader Objects
	private int gVertexShaderObject;
	private int gFragementShaderObject;
	
	private int[] vao_sphere = new int[1];
    private int[] vbo_sphere_position = new int[1];
    private int[] vbo_sphere_normal = new int[1];
    private int[] vbo_sphere_element = new int[1];
	
	private int mvUniform;
	private int pUniform;
	private int lKeyIsPressedUniform;
	private int ldUniform;
	private int kdUniform;
	private int lightPositionUniform;

	private int gb_Lighting = 0;

	private float[] perspectiveProjectionMatrix=new float[16];

	private float angleCube=0.0f;

	int numVertices;
	int numElements;
	
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
		if (gb_Lighting == 0)
			gb_Lighting = 1;
		else
			gb_Lighting = 0;
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
		float[] translationMatrix=new float[16];
		float[] rotationMatrix=new float[16];
		float[] scaleMatrix=new float[16];

		//Initialize matrices
		Matrix.setIdentityM(modelViewMatrix,0);
		Matrix.setIdentityM(translationMatrix,0);
		Matrix.setIdentityM(rotationMatrix,0);
		Matrix.setIdentityM(scaleMatrix,0);

		//Transformation

		Matrix.translateM(translationMatrix,0,
		0.0f, 0.0f, -2.0f);
	
		// Matrix.setRotateM(rotationMatrix,0,
		// angleCube,1.0f, 1.0f, 1.0f);

		// Matrix.scaleM(scaleMatrix,0,
		// 0.75f,0.75f,0.75f);
		//Matrix Multiplication

		Matrix.multiplyMM(modelViewMatrix,0,
		translationMatrix,0,
		scaleMatrix,0);

		Matrix.multiplyMM(modelViewMatrix,0,
		modelViewMatrix,0,
		rotationMatrix,0);

		// Matrix.multiplyMM(modelViewProjectionMatrix,0,
		// perspectiveProjectionMatrix,0,
		// modelViewMatrix,0);

		//Send necessary matrices to shader in resp. Uniforms
	
		GLES32.glUniformMatrix4fv(mvUniform,
			1,
			false,
			modelViewMatrix,0);
		
		GLES32.glUniformMatrix4fv(pUniform,
			1,
			false,
			perspectiveProjectionMatrix,0);
	
	if (gb_Lighting == 1) {
		GLES32.glUniform1i(lKeyIsPressedUniform, 
					gb_Lighting);
		GLES32.glUniform3f(ldUniform, 1.0f, 1.0f, 1.0f);
		GLES32.glUniform3f(kdUniform, 0.5f, 0.5f, 0.5f);
		GLES32.glUniform4f(lightPositionUniform, 0.0f, 0.0f, 2.0f, 1.0f);
	}
	else {
		GLES32.glUniform1i(lKeyIsPressedUniform, 
			gb_Lighting);
	}	

		        // bind vao
				GLES32.glBindVertexArray(vao_sphere[0]);
        
				// *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
				GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
				GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
				
				// unbind vao
				GLES32.glBindVertexArray(0);
		
		GLES32.glUseProgram(0);

		Update();

		requestRender();

	}
	private void Update()
	{
		
	}


	private void Initialize(){

	//Vertex Shader
	//Define Shader Object
	gVertexShaderObject = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
	//https://stackoverflow.com/questions/5366416/in-opengl-es-2-0-glsl-where-do-you-need-precision-specifiers
	//Vertex Shader Code
	final String vertextShaderSourceCode=String.format(
		"#version 320 es " +
		"\n " +
		"precision mediump int;"+
		"in vec4 vPosition;" +
		"in vec3 vNormal;" +
		"uniform mat4 u_mv_matrix;" +
		"uniform int u_lkeyispressed;" +
		"uniform mat4 u_p_matrix;" +
		"uniform vec3 u_ld;" +
		"uniform vec3 u_kd;" +
		"uniform vec4 u_lightposition;" +
		"out vec3 diffusecolor;" +
		"void main(void)" +
		"{" +
		"gl_Position=u_p_matrix*u_mv_matrix * vPosition;" +
		"if(u_lkeyispressed==1)"+
		"{"+
		"vec4 eyecoordinates=u_mv_matrix*vPosition;"+
		"mat3 normalmatrix=mat3(transpose(inverse(u_mv_matrix)));"+
		"vec3 tnorm=normalize(normalmatrix*vNormal);"+
		"vec3 s=normalize(vec3(u_lightposition)-eyecoordinates.xyz);"+
		"diffusecolor=u_ld*u_kd*max(dot(s,tnorm),0.0);"+
		"};"+
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
			"out vec4 fragColor;"+
			"in vec3 diffusecolor;" +
			"uniform int u_lkeyispressed;" +
			"void main(void)" +
			"{" +
			"if(u_lkeyispressed==1)"+
			"{"+
			"fragColor=vec4(diffusecolor,1.0f);"+
			"}"+
			"else"+
			"{"+
			"fragColor=vec4(1.0f,1.0f,1.0f,1.0f);"+
			"}"+
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
		GLESMicros.AMC_ATTRIBUTE_NORMAL,
		"vNormal");

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
	mvUniform = GLES32.glGetUniformLocation(shaderProgramObject,
		"u_mv_matrix");
		pUniform = GLES32.glGetUniformLocation(shaderProgramObject,
		"u_p_matrix");
		lKeyIsPressedUniform = GLES32.glGetUniformLocation(shaderProgramObject,
		"u_lkeyispressed");
		ldUniform = GLES32.glGetUniformLocation(shaderProgramObject,
		"u_ld");
		kdUniform = GLES32.glGetUniformLocation(shaderProgramObject,
		"u_kd");
		lightPositionUniform = GLES32.glGetUniformLocation(shaderProgramObject,
		"u_lightposition");

		
        Sphere sphere=new Sphere();
        float sphere_vertices[]=new float[1146];
        float sphere_normals[]=new float[1146];
        float sphere_textures[]=new float[764];
        short sphere_elements[]=new short[2280];
        sphere.getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
        numVertices = sphere.getNumberOfSphereVertices();
        numElements = sphere.getNumberOfSphereElements();

        // vao
        GLES32.glGenVertexArrays(1,vao_sphere,0);
        GLES32.glBindVertexArray(vao_sphere[0]);
        
        // position vbo
        GLES32.glGenBuffers(1,vbo_sphere_position,0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,vbo_sphere_position[0]);
        
        ByteBuffer byteBuffer=ByteBuffer.allocateDirect(sphere_vertices.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer verticesBuffer=byteBuffer.asFloatBuffer();
        verticesBuffer.put(sphere_vertices);
        verticesBuffer.position(0);
        
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
                            sphere_vertices.length * 4,
                            verticesBuffer,
                            GLES32.GL_STATIC_DRAW);
        
        GLES32.glVertexAttribPointer(GLESMicros.AMC_ATTRIBUTE_POSITION,
                                     3,
                                     GLES32.GL_FLOAT,
                                     false,0,0);
        
        GLES32.glEnableVertexAttribArray(GLESMicros.AMC_ATTRIBUTE_POSITION);
        
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
        
        // normal vbo
        GLES32.glGenBuffers(1,vbo_sphere_normal,0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,vbo_sphere_normal[0]);
        
        byteBuffer=ByteBuffer.allocateDirect(sphere_normals.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        verticesBuffer=byteBuffer.asFloatBuffer();
        verticesBuffer.put(sphere_normals);
        verticesBuffer.position(0);
        
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
                            sphere_normals.length * 4,
                            verticesBuffer,
                            GLES32.GL_STATIC_DRAW);
        
        GLES32.glVertexAttribPointer(GLESMicros.AMC_ATTRIBUTE_NORMAL,
                                     3,
                                     GLES32.GL_FLOAT,
                                     false,0,0);
        
        GLES32.glEnableVertexAttribArray(GLESMicros.AMC_ATTRIBUTE_NORMAL);
        
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
        
        // element vbo
        GLES32.glGenBuffers(1,vbo_sphere_element,0);
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER,vbo_sphere_element[0]);
        
        byteBuffer=ByteBuffer.allocateDirect(sphere_elements.length * 2);
        byteBuffer.order(ByteOrder.nativeOrder());
        ShortBuffer elementsBuffer=byteBuffer.asShortBuffer();
        elementsBuffer.put(sphere_elements);
        elementsBuffer.position(0);
        
        GLES32.glBufferData(GLES32.GL_ELEMENT_ARRAY_BUFFER,
                            sphere_elements.length * 2,
                            elementsBuffer,
                            GLES32.GL_STATIC_DRAW);
        
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER,0);

        GLES32.glBindVertexArray(0);


	GLES32.glEnable(GLES32.GL_DEPTH_TEST);
	GLES32.glDepthFunc(GLES32.GL_LEQUAL);
	GLES32.glClearDepthf(1.0f);

	GLES32.glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	//GLES32.glEnable(GLES32.GL_TEXTURE_2D);

	Matrix.setIdentityM(perspectiveProjectionMatrix,0);

	}


	void UnInitialize() {
		        // destroy vao
				if(vao_sphere[0] != 0)
				{
					GLES32.glDeleteVertexArrays(1, vao_sphere, 0);
					vao_sphere[0]=0;
				}
				
				// destroy position vbo
				if(vbo_sphere_position[0] != 0)
				{
					GLES32.glDeleteBuffers(1, vbo_sphere_position, 0);
					vbo_sphere_position[0]=0;
				}
				
				// destroy normal vbo
				if(vbo_sphere_normal[0] != 0)
				{
					GLES32.glDeleteBuffers(1, vbo_sphere_normal, 0);
					vbo_sphere_normal[0]=0;
				}
				
				// destroy element vbo
				if(vbo_sphere_element[0] != 0)
				{
					GLES32.glDeleteBuffers(1, vbo_sphere_element, 0);
					vbo_sphere_element[0]=0;
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