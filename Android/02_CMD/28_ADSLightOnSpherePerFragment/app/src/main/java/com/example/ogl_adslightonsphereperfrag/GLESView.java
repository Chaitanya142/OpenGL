package com.example.ogl_adslightonsphereperfrag;

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

	private int[] vao_sphere = new int[1];
	private int[] vbo_sphere_position = new int[1];
	private int[] vbo_sphere_normal = new int[1];
	private int[] vbo_sphere_element = new int[1];

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

	private float[] lightAmbient = new float[4];
	private float[] lightDiffuse = new float[4];
	private float[] lightSpecular = new float[4];
	private float[] lightPosition = new float[4];

	private float[] materialAmbient = new float[4];
	private float[] materialDiffuse = new float[4];
	private float[] materialSpecular = new float[4];
	private float materialShininess;

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

		Matrix.translateM(translationMatrix, 0, 0.0f, 0.0f, -2.0f);

		// Matrix.setRotateM(rotationMatrix,0,
		// angleCube,1.0f, 1.0f, 1.0f);

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

			GLES32.glUniform3fv(ldUniform, 1, lightDiffuse, 0);
			GLES32.glUniform3fv(laUniform, 1, lightAmbient, 0);
			GLES32.glUniform3fv(lsUniform, 1, lightSpecular, 0);

			GLES32.glUniform3fv(kaUniform, 1, materialAmbient, 0);
			GLES32.glUniform3fv(kdUniform, 1, materialDiffuse, 0);
			GLES32.glUniform3fv(ksUniform, 1, materialSpecular, 0);

			GLES32.glUniform1f(shininessUniform, materialShininess);

			GLES32.glUniform4fv(lightPositionUniform, 1, lightPosition, 0);

		} else {
			GLES32.glUniform1i(lKeyIsPressedUniform, gb_Lighting);
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

	private void Update() {

	}

	private void Initialize() {

		// Vertex Shader
		// Define Shader Object
		gVertexShaderObject = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
		// https://stackoverflow.com/questions/5366416/in-opengl-es-2-0-glsl-where-do-you-need-precision-specifiers
		// Vertex Shader Code
		final String vertextShaderSourceCode = String.format("#version 320 es " + "\n " +"precision mediump int;"+ "in vec4 vPosition;"
				+ "in vec3 vNormal;" + "uniform mat4 u_m_matrix;" + "uniform mat4 u_v_matrix;"
				+ "uniform mat4 u_p_matrix;" + "uniform int u_lkeyispressed;" + "uniform vec4 u_lightposition;"
				+ "out vec3 tnorm;" + "out vec3 lightDirection;" + "out vec3 reflectionVector;"
				+ "out vec3 viewerVector;" + "void main(void)" + "{"
				+ "gl_Position=u_p_matrix*u_v_matrix*u_m_matrix * vPosition;" + "if(u_lkeyispressed==1)" + "{"
				+ "vec4 eyecoordinates=u_v_matrix*u_m_matrix*vPosition;" + "tnorm=mat3(u_v_matrix*u_m_matrix)*vNormal;"
				+ "lightDirection=vec3(u_lightposition-eyecoordinates);"
				+ "reflectionVector=reflect(-lightDirection,tnorm);" + "viewerVector=vec3(-eyecoordinates);" + "}"
				+ "}");

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
		final String fragementShaderSourceCode = String.format("#version 320 es" + "\n " + "precision highp float;"
				+ "out vec4 fragColor;" + "uniform vec3 u_ld;" + "uniform vec3 u_kd;" + "uniform vec3 u_la;"
				+ "uniform vec3 u_ka;" + "uniform vec3 u_ls;" + "uniform vec3 u_ks;" + "uniform float u_shininess;"
				+ "uniform int u_lkeyispressed;" + "in vec3 tnorm;" + "in vec3 lightDirection;"
				+ "in vec3 reflectionVector;" + "in vec3 viewerVector;" + "void main(void)" + "{"
				+ "vec3 phongADSLight;" + "if(u_lkeyispressed==1)" + "{" + "vec3 tnorm_normalize=normalize(tnorm);"
				+ "vec3 lightDirection_normalize=normalize(lightDirection);"
				+ "vec3 reflectionVector_normalize=normalize(reflectionVector);"
				+ "vec3 viewerVector_normalize=normalize(viewerVector);"
				+ "float tn_dot_ld=max(dot(lightDirection_normalize,tnorm_normalize),0.0);" + "vec3 ambient=u_la*u_ka;"
				+ "vec3 diffuse=u_ld*u_kd*tn_dot_ld;"
				+ "vec3 specular=u_ls*u_ks*pow(max(dot(reflectionVector_normalize,viewerVector_normalize),0.0),u_shininess);"
				+ "phongADSLight=ambient+diffuse+specular;" + "}" + "else" + "{" + "phongADSLight=vec3(1.0f,1.0f,1.0f);"
				+ "}" + "fragColor=vec4(phongADSLight,1.0f);" + "} "

		);

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

		Sphere sphere = new Sphere();
		float sphere_vertices[] = new float[1146];
		float sphere_normals[] = new float[1146];
		float sphere_textures[] = new float[764];
		short sphere_elements[] = new short[2280];
		sphere.getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
		numVertices = sphere.getNumberOfSphereVertices();
		numElements = sphere.getNumberOfSphereElements();

		// vao
		GLES32.glGenVertexArrays(1, vao_sphere, 0);
		GLES32.glBindVertexArray(vao_sphere[0]);

		// position vbo
		GLES32.glGenBuffers(1, vbo_sphere_position, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_sphere_position[0]);

		ByteBuffer byteBuffer = ByteBuffer.allocateDirect(sphere_vertices.length * 4);
		byteBuffer.order(ByteOrder.nativeOrder());
		FloatBuffer verticesBuffer = byteBuffer.asFloatBuffer();
		verticesBuffer.put(sphere_vertices);
		verticesBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, sphere_vertices.length * 4, verticesBuffer, GLES32.GL_STATIC_DRAW);

		GLES32.glVertexAttribPointer(GLESMicros.AMC_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);

		GLES32.glEnableVertexAttribArray(GLESMicros.AMC_ATTRIBUTE_POSITION);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		// normal vbo
		GLES32.glGenBuffers(1, vbo_sphere_normal, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_sphere_normal[0]);

		byteBuffer = ByteBuffer.allocateDirect(sphere_normals.length * 4);
		byteBuffer.order(ByteOrder.nativeOrder());
		verticesBuffer = byteBuffer.asFloatBuffer();
		verticesBuffer.put(sphere_normals);
		verticesBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, sphere_normals.length * 4, verticesBuffer, GLES32.GL_STATIC_DRAW);

		GLES32.glVertexAttribPointer(GLESMicros.AMC_ATTRIBUTE_NORMAL, 3, GLES32.GL_FLOAT, false, 0, 0);

		GLES32.glEnableVertexAttribArray(GLESMicros.AMC_ATTRIBUTE_NORMAL);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		// element vbo
		GLES32.glGenBuffers(1, vbo_sphere_element, 0);
		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);

		byteBuffer = ByteBuffer.allocateDirect(sphere_elements.length * 2);
		byteBuffer.order(ByteOrder.nativeOrder());
		ShortBuffer elementsBuffer = byteBuffer.asShortBuffer();
		elementsBuffer.put(sphere_elements);
		elementsBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ELEMENT_ARRAY_BUFFER, sphere_elements.length * 2, elementsBuffer,
				GLES32.GL_STATIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, 0);

		GLES32.glBindVertexArray(0);

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

		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);
		GLES32.glClearDepthf(1.0f);

		GLES32.glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		// GLES32.glEnable(GLES32.GL_TEXTURE_2D);

		Matrix.setIdentityM(perspectiveProjectionMatrix, 0);

	}

	void UnInitialize() {
		// destroy vao
		if (vao_sphere[0] != 0) {
			GLES32.glDeleteVertexArrays(1, vao_sphere, 0);
			vao_sphere[0] = 0;
		}

		// destroy position vbo
		if (vbo_sphere_position[0] != 0) {
			GLES32.glDeleteBuffers(1, vbo_sphere_position, 0);
			vbo_sphere_position[0] = 0;
		}

		// destroy normal vbo
		if (vbo_sphere_normal[0] != 0) {
			GLES32.glDeleteBuffers(1, vbo_sphere_normal, 0);
			vbo_sphere_normal[0] = 0;
		}

		// destroy element vbo
		if (vbo_sphere_element[0] != 0) {
			GLES32.glDeleteBuffers(1, vbo_sphere_element, 0);
			vbo_sphere_element[0] = 0;
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