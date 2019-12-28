package com.example.ogl_twolightsonpyramid;

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

public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener {
	private static Context context;
	private GestureDetector gestureDetector;

	private int shaderProgramObject;
	// Shader Objects
	private int gVertexShaderObject;
	private int gFragementShaderObject;

	
	private int mUniform;
	private int vUniform;
	private int pUniform;
	private int lKeyIsPressedUniform;
	private int ldUniform;
	private int kdUniform;
	private int laUniform;
	private int kaUniform;
	private int lsUniform;
	private int ksUniform;
	private int lightPositionUniform;
	private int shininessUniform;

	private int gb_Lighting = 0;

	private float[] perspectiveProjectionMatrix = new float[16];

	int numVertices;
	int numElements;

	private float anglePyramid=0.0f;

	// private float[] lightAmbient = new float[4];
	// private float[] lightDiffuse = new float[4];
	// private float[] lightSpecular = new float[4];
	// private float[] lightPosition = new float[4];

	private float[] lightAmbient = new float[3*2];
	private float[] lightDiffuse = new float[3*2];
	private float[] lightSpecular = new float[3*2];
	private float[] lightPosition = new float[4*2];

	private float[] materialAmbient = new float[4];
	private float[] materialDiffuse = new float[4];
	private float[] materialSpecular = new float[4];
	private float materialShininess;

	private int[] vao_pyramid=new int[1];
	private int[] vbo_position_pyramid=new int[1];
	//private int[] vbo_texCoord_pyramid=new int[1];
	private int[] vbo_normal_pyramid=new int[1];

	public GLESView(Context drawingContext) {
		super(drawingContext);

		context = drawingContext;
		setEGLContextClientVersion(3);
		setRenderer(this);
		setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);

		gestureDetector = new GestureDetector(drawingContext, this, null, false);
		gestureDetector.setOnDoubleTapListener(this);
	}

	@Override
	public boolean onTouchEvent(MotionEvent event) {
		int eventaction = event.getAction();
		if (!gestureDetector.onTouchEvent(event)) {
			super.onTouchEvent(event);
		}
		return (true);
	}

	@Override
	public boolean onDoubleTap(MotionEvent e) {
		return (true);
	}

	@Override
	public boolean onDoubleTapEvent(MotionEvent e) {
		return (true);
	}

	@Override
	public boolean onSingleTapConfirmed(MotionEvent e) {
		return (true);
	}

	@Override
	public boolean onDown(MotionEvent e) {
		return (true);
	}

	@Override
	public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY) {
		return (true);
	}

	@Override
	public void onLongPress(MotionEvent e) {

	}

	@Override
	public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY) {
		UnInitialize();
		System.exit(0);
		return (true);
	}

	@Override
	public void onShowPress(MotionEvent e) {

	}

	@Override
	public boolean onSingleTapUp(MotionEvent e) {
		if (gb_Lighting == 0)
			gb_Lighting = 1;
		else
			gb_Lighting = 0;
		return (true);
	}

	@Override
	public void onSurfaceCreated(GL10 gl, EGLConfig config) {
		String Version = gl.glGetString(GL10.GL_VERSION);
		String shaderVersion = gl.glGetString(GLES32.GL_SHADING_LANGUAGE_VERSION);

		System.out.println(Version);
		System.out.println(shaderVersion);

		Initialize();
	}

	@Override
	public void onSurfaceChanged(GL10 unused, int width, int height) {
		Resize(width, height);
	}

	@Override
	public void onDrawFrame(GL10 unused) {
		Display();
	}

	private void Resize(int width, int height) {
		if (height == 0)
			height = 1;

		GLES32.glViewport(0, 0, width, height);

		Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f, (float) width / (float) height, 0.1f, 100.0f);

	}

	private void Display() {

		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

		GLES32.glUseProgram(shaderProgramObject);

		// Declaration of Matrices
		float[] modelMatrix = new float[16];
		float[] viewMatrix = new float[16];
		float[] translationMatrix = new float[16];
		float[] rotationMatrix = new float[16];
		float[] scaleMatrix = new float[16];

		// Initialize matrices
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.setIdentityM(viewMatrix, 0);
		Matrix.setIdentityM(translationMatrix, 0);
		Matrix.setIdentityM(rotationMatrix, 0);
		Matrix.setIdentityM(scaleMatrix, 0);

		// Transformation

		Matrix.translateM(translationMatrix, 0, 0.0f, 0.0f, -4.0f);

		Matrix.setRotateM(rotationMatrix,0,
		anglePyramid,0.0f, 1.0f, 0.0f);

		// Matrix.scaleM(scaleMatrix,0,
		// 0.75f,0.75f,0.75f);
		// Matrix Multiplication

		Matrix.multiplyMM(modelMatrix, 0, translationMatrix, 0, scaleMatrix, 0);

		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, rotationMatrix, 0);

		// Matrix.multiplyMM(modelViewProjectionMatrix,0,
		// perspectiveProjectionMatrix,0,
		// modelViewMatrix,0);

		// Send necessary matrices to shader in resp. Uniforms

		GLES32.glUniformMatrix4fv(mUniform, 1, false, modelMatrix, 0);
		GLES32.glUniformMatrix4fv(vUniform, 1, false, viewMatrix, 0);
		GLES32.glUniformMatrix4fv(pUniform, 1, false, perspectiveProjectionMatrix, 0);

		if (gb_Lighting == 1) {
			GLES32.glUniform1i(lKeyIsPressedUniform, gb_Lighting);

			GLES32.glUniform3fv(ldUniform, 2, lightDiffuse, 0);
			GLES32.glUniform3fv(laUniform, 2, lightAmbient, 0);
			GLES32.glUniform3fv(lsUniform, 2, lightSpecular, 0);

			GLES32.glUniform3fv(kaUniform, 1, materialAmbient, 0);
			GLES32.glUniform3fv(kdUniform, 1, materialDiffuse, 0);
			GLES32.glUniform3fv(ksUniform, 1, materialSpecular, 0);

			GLES32.glUniform1f(shininessUniform, materialShininess);

			GLES32.glUniform4fv(lightPositionUniform, 2, lightPosition, 0);

		} else {
			GLES32.glUniform1i(lKeyIsPressedUniform, gb_Lighting);
		}

	
		GLES32.glBindVertexArray(vao_pyramid[0]);
	
		GLES32.glDrawArrays(GLES32.GL_TRIANGLES,
			0,
			12);
	
		GLES32.glBindVertexArray(0);
	
		GLES32.glUseProgram(0);

		Update();

		requestRender();

	}

	private void Update() {
		anglePyramid = anglePyramid + 1.0f;
		if (anglePyramid > 360.0f)
			anglePyramid = 0.0f;
	}

	private void Initialize() {

		// Vertex Shader
		// Define Shader Object
		gVertexShaderObject = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
		// https://stackoverflow.com/questions/5366416/in-opengl-es-2-0-glsl-where-do-you-need-precision-specifiers
		// Vertex Shader Code
		final String vertextShaderSourceCode = String
				.format("#version 320 es " +
				"\n " + 
				"precision mediump int;"+
				"in vec4 vPosition;" +
				"in vec3 vNormal;" +
				"uniform mat4 u_m_matrix;" +
				"uniform mat4 u_v_matrix;" +
				"uniform mat4 u_p_matrix;" +
				"uniform int u_lkeyispressed;" +
		
				"uniform vec3 u_ld[2];" +
				"uniform vec3 u_kd;" +
				"uniform vec3 u_la[2];" +
				"uniform vec3 u_ka;" +
				"uniform vec3 u_ls[2];" +
				"uniform vec3 u_ks;" +
				"uniform float u_shininess;" +
				"uniform vec4 u_lightposition[2];" +
				"out vec3 phongADSLight;" +
				"void main(void)" +
				"{" +
				"gl_Position=u_p_matrix*u_v_matrix*u_m_matrix * vPosition;" +
				"if(u_lkeyispressed==1)"+
				"{"+
				"vec4 eyecoordinates=u_v_matrix*u_m_matrix*vPosition;"+
				"vec3 tnorm=normalize(mat3(u_v_matrix*u_m_matrix)*vNormal);"+
				"vec3 viewerVector=normalize(vec3(-eyecoordinates));"+
				"phongADSLight = vec3(0.0f, 0.0f, 0.0f);"+
				"for(int i=0;i<2;i++)"+
				"{"+	
				"vec3 lightDirection=normalize(vec3(u_lightposition[i]-eyecoordinates));"+
				"float tn_dot_ld=max(dot(lightDirection,tnorm),0.0);"+
				"vec3 reflectionVector=reflect(-lightDirection,tnorm);"+
				"vec3 ambient=u_la[i]*u_ka;"+
				"vec3 diffuse=u_ld[i]*u_kd*tn_dot_ld;"+
				"vec3 specular=u_ls[i]*u_ks*pow(max(dot(reflectionVector,viewerVector),0.0),u_shininess);"+
				"phongADSLight=phongADSLight+ambient+diffuse+specular;"+
				"}"+
				"}"+
				"else"+
				"{"+
				"phongADSLight=vec3(1.0f,1.0f,1.0f);"+
				"}"+
				"}"
				);

		// Shader object and source code mapping
		GLES32.glShaderSource(gVertexShaderObject, vertextShaderSourceCode);

		// Compile Shader
		GLES32.glCompileShader(gVertexShaderObject);

		// Error Checking
		int[] iShaderCompileStatus = new int[1];
		int[] iInfoLogLength = new int[1];
		String szInfoLog = null;

		GLES32.glGetShaderiv(gVertexShaderObject, GLES32.GL_COMPILE_STATUS, iShaderCompileStatus, 0);

		if (iShaderCompileStatus[0] == GLES32.GL_FALSE) {

			GLES32.glGetShaderiv(gVertexShaderObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);

			if (iInfoLogLength[0] > 0) {

				szInfoLog = GLES32.glGetShaderInfoLog(gVertexShaderObject);

				System.out.println("RTR : gVertexShaderObject " + szInfoLog);
				UnInitialize();
				System.exit(0);
			}
		}

		// Fragement Shader
		// Define Shader Object
		gFragementShaderObject = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);

		// Vertex Shader Code
		final String fragementShaderSourceCode = String.format(
				"#version 320 es" + "\n " + "precision highp float;" 
				+ "out vec4 fragColor;" + "in vec3 phongADSLight;"
						+ "void main(void)" + "{" + "fragColor=vec4(phongADSLight,1.0f);" + "} ");

		// Shader object and source code mapping
		GLES32.glShaderSource(gFragementShaderObject, fragementShaderSourceCode);

		// Compile Shader
		GLES32.glCompileShader(gFragementShaderObject);

		// Error Checking
		iShaderCompileStatus[0] = 0;
		iInfoLogLength[0] = 0;
		szInfoLog = null;

		GLES32.glGetShaderiv(gFragementShaderObject, GLES32.GL_COMPILE_STATUS, iShaderCompileStatus, 0);

		if (iShaderCompileStatus[0] == GLES32.GL_FALSE) {

			GLES32.glGetShaderiv(gFragementShaderObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);

			if (iInfoLogLength[0] > 0) {

				szInfoLog = GLES32.glGetShaderInfoLog(gFragementShaderObject);

				System.out.println("RTR : gFragementShaderObject" + szInfoLog);
				UnInitialize();
				System.exit(0);
			}
		}

		// Program Object
		shaderProgramObject = GLES32.glCreateProgram();
		// Attach Shaders
		GLES32.glAttachShader(shaderProgramObject, gVertexShaderObject);
		GLES32.glAttachShader(shaderProgramObject, gFragementShaderObject);

		// Prelinking
		// Binding to attribute
		GLES32.glBindAttribLocation(shaderProgramObject, GLESMicros.AMC_ATTRIBUTE_POSITION, "vPosition");

		GLES32.glBindAttribLocation(shaderProgramObject, GLESMicros.AMC_ATTRIBUTE_NORMAL, "vNormal");

		// Link Shader Program
		GLES32.glLinkProgram(shaderProgramObject);

		// Error Checking
		int[] iShaderLinkStatus = new int[1];
		iInfoLogLength[0] = 0;
		szInfoLog = null;

		GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_LINK_STATUS, iShaderLinkStatus, 0);

		if (iShaderLinkStatus[0] == GLES32.GL_FALSE) {
			GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);

			if (iInfoLogLength[0] > 0) {
				szInfoLog = GLES32.glGetProgramInfoLog(shaderProgramObject);

				System.out.println("RTR : shaderProgramObject" + szInfoLog);
				UnInitialize();
				System.exit(0);

			}
		}

		// Post Linking
		// Retriving Uniform Location
		mUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_m_matrix");
		vUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_v_matrix");
		pUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_p_matrix");
		lKeyIsPressedUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_lkeyispressed");

		ldUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_ld");
		kdUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_kd");
		laUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_la");
		kaUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_ka");
		lsUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_ls");
		ksUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_ks");

		shininessUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_shininess");

		lightPositionUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_lightposition");

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

		final float[] pyramidTexCoord = new float[]
	{
		0.5f, 1.0f,
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
		1.0f, 0.0f
	};	


	final float[] pyramidNormals = new float[]{

	0.0f, 0.447214f, 0.894427f,
	0.0f, 0.447214f, 0.894427f,
	0.0f, 0.447214f, 0.894427f,

	0.894424f, 0.447214f, 0.0f,
	0.894424f, 0.447214f, 0.0f,
	0.894424f, 0.447214f, 0.0f,

	0.0f, 0.447214f, -0.894427f,
	0.0f, 0.447214f, -0.894427f,
	0.0f, 0.447214f, -0.894427f,

	-0.894427f, 0.447214f, 0.0f,
	-0.894427f, 0.447214f, 0.0f,
	-0.894427f, 0.447214f, 0.0f
	};


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

	// //Texcoord
	// //Generate Buffer
	// GLES32.glGenBuffers(1,vbo_texCoord_pyramid,0);
	// //Bind Generated Buffer
	// GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,
	// vbo_texCoord_pyramid[0]);

	// byteBuffer=ByteBuffer.allocateDirect(pyramidTexCoord.length*4);

	// byteBuffer.order(ByteOrder.nativeOrder());

	// FloatBuffer texCoordBuffer=byteBuffer.asFloatBuffer();

	// texCoordBuffer.put(pyramidTexCoord);

	// texCoordBuffer.position(0);

	// //Fill Buffer
	// GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
	// 	pyramidTexCoord.length*4,
	// 	texCoordBuffer,
	// 	GLES32.GL_STATIC_DRAW);

	// //Set Vertex Attrib Pointer
	// GLES32.glVertexAttribPointer(GLESMicros.AMC_ATTRIBUTE_TEXCOORD0,
	// 	2,
	// 	GLES32.GL_FLOAT,
	// 	false,
	// 	0,
	// 	0);

	// //Enable Vertex Attrib Array
	// GLES32.glEnableVertexAttribArray(GLESMicros.AMC_ATTRIBUTE_TEXCOORD0);
	// //Unbind Buffer
	// GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
	// //Unbind array
	// GLES32.glBindVertexArray(0);



	//Normals
	//Generate Buffer
	GLES32.glGenBuffers(1,vbo_normal_pyramid,0);
	//Bind Generated Buffer
	GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,
	vbo_normal_pyramid[0]);

	byteBuffer=ByteBuffer.allocateDirect(pyramidNormals.length*4);

	byteBuffer.order(ByteOrder.nativeOrder());

	FloatBuffer normalBuffer=byteBuffer.asFloatBuffer();

	normalBuffer.put(pyramidNormals);

	normalBuffer.position(0);

	//Fill Buffer
	GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
		pyramidNormals.length*4,
		normalBuffer,
		GLES32.GL_STATIC_DRAW);

	//Set Vertex Attrib Pointer
	GLES32.glVertexAttribPointer(GLESMicros.AMC_ATTRIBUTE_NORMAL,
		3,
		GLES32.GL_FLOAT,
		false,
		0,
		0);

	//Enable Vertex Attrib Array
	GLES32.glEnableVertexAttribArray(GLESMicros.AMC_ATTRIBUTE_NORMAL);
	//Unbind Buffer
	GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
	//Unbind array
	GLES32.glBindVertexArray(0);

		lightAmbient[0] = 0.0f;
		lightAmbient[1] = 0.0f;
		lightAmbient[2] = 0.0f;
		
		lightDiffuse[0] = 1.0f;
		lightDiffuse[1] = 0.0f;
		lightDiffuse[2] = 0.0f;
		
		lightSpecular[0] = 1.0f;
		lightSpecular[1] = 0.0f;
		lightSpecular[2] = 0.0f;
		
		lightPosition[0] = -2.0f;
		lightPosition[1] = 0.0f;
		lightPosition[2] = 0.0f;
		lightPosition[3] = 1.0f;
		
		lightAmbient[3] = 0.0f;
		lightAmbient[4] = 0.0f;
		lightAmbient[5] = 0.0f;
		
		lightDiffuse[3] = 0.0f;
		lightDiffuse[4] = 0.0f;
		lightDiffuse[5] = 1.0f;
		
		lightSpecular[3] = 0.0f;
		lightSpecular[4] = 0.0f;
		lightSpecular[5] = 1.0f;
		
		lightPosition[4] = 2.0f;
		lightPosition[5] = 0.0f;
		lightPosition[6] = 0.0f;
		lightPosition[7] = 1.0f;
		
		materialAmbient[0] = 0.5f;
		materialAmbient[1] = 0.5f;
		materialAmbient[2] = 0.5f;
		materialAmbient[3] = 1.0f;
		materialDiffuse[0] = 1.0f;
		materialDiffuse[1] = 1.0f;
		materialDiffuse[2] = 1.0f;
		materialDiffuse[3] = 1.0f;
		materialSpecular[0] = 1.0f;
		materialSpecular[1] = 1.0f;
		materialSpecular[2] = 1.0f;
		materialSpecular[3] = 1.0f;
		materialShininess = 128.0f;

		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);
		GLES32.glClearDepthf(1.0f);

		GLES32.glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		// GLES32.glEnable(GLES32.GL_TEXTURE_2D);

		Matrix.setIdentityM(perspectiveProjectionMatrix, 0);

	}

	void UnInitialize() {

		if (vbo_position_pyramid[0]!=0) {
			GLES32.glDeleteBuffers(1,vbo_position_pyramid,0);
			vbo_position_pyramid[0] = 0;
		}
		// if (vbo_texCoord_pyramid[0]!=0) {
		// 	GLES32.glDeleteBuffers(1,vbo_texCoord_pyramid,0);
		// 	vbo_texCoord_pyramid[0] = 0;
		// }
		if (vao_pyramid[0]!=0) {
			GLES32.glDeleteVertexArrays(1,vao_pyramid,0);
			vao_pyramid[0] = 0;
		}

		if (shaderProgramObject != 0) {
			int[] shaderCount = new int[1];

			GLES32.glUseProgram(shaderProgramObject);

			GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_ATTACHED_SHADERS, shaderCount, 0);

			if (shaderCount[0] != 0) {

				int shaderNumber;
				int[] shaders = new int[shaderCount[0]];

				GLES32.glGetAttachedShaders(shaderProgramObject, shaderCount[0], shaderCount, 0, shaders, 0);

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