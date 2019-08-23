package com.example.ogl_static_india;

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

public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener {
	private static Context context;
	private GestureDetector gestureDetector;

	private int shaderProgramObject;
	// Shader Objects
	private int gVertexShaderObject;
	private int gFragementShaderObject;

	private int[] vao = new int[1];

	private int[] vbo_position = new int[1];

	private int[] vbo_color = new int[1];

	private int mvpUniform;

	private float D_R_OrangeColor = 1.0f;
	private float D_G_OrangeColor = 0.6f;
	private float D_B_OrangeColor = 0.2f;
	private float D_R_GreenColor = 0.074f;
	private float D_G_GreenColor = 0.533f;
	private float D_B_GreenColor = 0.2f;

	private float[] perspectiveProjectionMatrix = new float[16];

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

		int stage = 5;
		float I_1_XPosition = -1.5f; // -2.25f;
		float A_XPosition = 0.75f;// 2.25f;
		float N_YPosition = 0.00f;// 2.25f;
		float I_2_YPosition = 0.00f;// -2.25f;

		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

		GLES32.glUseProgram(shaderProgramObject);

		// Declaration of Matrices
		float[] modelViewMatrix = new float[16];
		float[] modelViewProjectionMatrix = new float[16];
		float[] translationMatrix = new float[16];
		float[] rotationMatrix = new float[16];

		if (stage > 0) {

			// Initialize matrices
			Matrix.setIdentityM(modelViewMatrix, 0);
			Matrix.setIdentityM(modelViewProjectionMatrix, 0);
			Matrix.setIdentityM(translationMatrix, 0);
			Matrix.setIdentityM(rotationMatrix, 0);

			// Transformation

			Matrix.translateM(translationMatrix, 0, I_1_XPosition, 0.0f, -3.0f);

			// Matrix Multiplication

			Matrix.multiplyMM(modelViewMatrix, 0, translationMatrix, 0, rotationMatrix, 0);

			Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

			// Send necessary matrices to shader in resp. Uniforms

			GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);

			// Bind with vao

			GLES32.glBindVertexArray(vao[0]);

			// Bind with textures if any

			// Draw

			// GLES32.glDrawArrays(GLES32.GL_TRIANGLES,
			// 0,
			// 3);

			DrawLetter('I');

			// Unbind vao

			GLES32.glBindVertexArray(0);

		}
		if (stage > 2) {
			// Initialize matrices
			Matrix.setIdentityM(modelViewMatrix, 0);
			Matrix.setIdentityM(modelViewProjectionMatrix, 0);
			Matrix.setIdentityM(translationMatrix, 0);
			Matrix.setIdentityM(rotationMatrix, 0);

			// Transformation

			Matrix.translateM(translationMatrix, 0, -1.0f, N_YPosition, -3.0f);

			// Matrix Multiplication

			Matrix.multiplyMM(modelViewMatrix, 0, translationMatrix, 0, rotationMatrix, 0);

			Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

			// Send necessary matrices to shader in resp. Uniforms

			GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);

			// Bind with vao

			GLES32.glBindVertexArray(vao[0]);

			// Bind with textures if any

			// Draw

			DrawLetter('N');

			// Unbind vao

			GLES32.glBindVertexArray(0);

		}

		if (stage > 4) {
			// Initialize matrices
			Matrix.setIdentityM(modelViewMatrix, 0);
			Matrix.setIdentityM(modelViewProjectionMatrix, 0);
			Matrix.setIdentityM(translationMatrix, 0);
			Matrix.setIdentityM(rotationMatrix, 0);

			// Transformation

			Matrix.translateM(translationMatrix, 0, -0.25f, 0.0f, -3.0f);

			// Matrix Multiplication

			Matrix.multiplyMM(modelViewMatrix, 0, translationMatrix, 0, rotationMatrix, 0);

			Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

			// Send necessary matrices to shader in resp. Uniforms

			GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);

			// Bind with vao

			GLES32.glBindVertexArray(vao[0]);

			// Bind with textures if any

			// Draw

			DrawLetter('D');

			// Unbind vao

			GLES32.glBindVertexArray(0);

		}

		if (stage > 3) {

			// Initialize matrices
			Matrix.setIdentityM(modelViewMatrix, 0);
			Matrix.setIdentityM(modelViewProjectionMatrix, 0);
			Matrix.setIdentityM(translationMatrix, 0);
			Matrix.setIdentityM(rotationMatrix, 0);

			// Transformation

			Matrix.translateM(translationMatrix, 0, 0.25f, I_2_YPosition, -3.0f);

			// Matrix Multiplication

			Matrix.multiplyMM(modelViewMatrix, 0, translationMatrix, 0, rotationMatrix, 0);

			Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

			// Send necessary matrices to shader in resp. Uniforms

			GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);

			// Bind with vao

			GLES32.glBindVertexArray(vao[0]);

			// Bind with textures if any

			// Draw

			DrawLetter('I');

			// Unbind vao

			GLES32.glBindVertexArray(0);

		}
		if (stage > 1) {

			// Initialize matrices
			Matrix.setIdentityM(modelViewMatrix, 0);
			Matrix.setIdentityM(modelViewProjectionMatrix, 0);
			Matrix.setIdentityM(translationMatrix, 0);
			Matrix.setIdentityM(rotationMatrix, 0);

			// Transformation

			Matrix.translateM(translationMatrix, 0, A_XPosition, 0.0f, -3.0f);

			// Matrix Multiplication

			Matrix.multiplyMM(modelViewMatrix, 0, translationMatrix, 0, rotationMatrix, 0);

			Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

			// Send necessary matrices to shader in resp. Uniforms

			GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);

			// Bind with vao

			GLES32.glBindVertexArray(vao[0]);

			// Bind with textures if any

			// Draw

			DrawLetter('A');
			DrawLetter('T');

			// Unbind vao

			GLES32.glBindVertexArray(0);
		}

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

		// Vertex Shader Code
		final String vertextShaderSourceCode = String.format("#version 320 es " + "\n " + "in vec4 vPosition;"
				+ "in vec4 vColor;" + "uniform mat4 u_mvp_matrix;" + "out vec4 out_color;" + "void main(void)" + "{"
				+ "gl_Position=u_mvp_matrix * vPosition;" + "out_color=vColor;" + "} ");

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
		final String fragementShaderSourceCode = String
				.format("#version 320 es" + "\n " + "precision highp float;" + "in vec4 out_color;"
						+ "out vec4 fragColor;" + "void main(void)" + "{" + "fragColor=out_color;" + "} ");

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

		GLES32.glBindAttribLocation(shaderProgramObject, GLESMicros.AMC_ATTRIBUTE_COLOR, "vColor");

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
		mvpUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_mvp_matrix");

		// //Vertices
		// final float[] triangleVertices = new float[]
		// {
		// 0.0f, 1.0f, 0.0f,
		// -1.0f, -1.0f, 0.0f,
		// 1.0f, -1.0f, 0.0f
		// };

		// //Vertices
		// final float[] triangleColors = new float[]
		// {
		// 1.0f,0.0f,0.0f,
		// 0.0f,1.0f,0.0f,
		// 0.0f,0.0f,1.0f
		// };

		// Create vao
		// Save everying in single set
		GLES32.glGenVertexArrays(1, vao, 0);

		GLES32.glBindVertexArray(vao[0]);

		// Posistion
		// Generate Buffer
		GLES32.glGenBuffers(1, vbo_position, 0);
		// Bind Generated Buffer
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);

		// ByteBuffer byteBuffer=ByteBuffer.allocateDirect(triangleVertices.length*4);

		// byteBuffer.order(ByteOrder.nativeOrder());

		// FloatBuffer positionBuffer=byteBuffer.asFloatBuffer();

		// positionBuffer.put(triangleVertices);

		// positionBuffer.position(0);

		// Fill Buffer
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, null, GLES32.GL_DYNAMIC_DRAW);

		// GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
		// triangleColors.length*4,
		// colorBuffer,
		// GLES32.GL_STATIC_DRAW);

		// Set Vertex Attrib Pointer
		GLES32.glVertexAttribPointer(GLESMicros.AMC_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);

		// Enable Vertex Attrib Array
		GLES32.glEnableVertexAttribArray(GLESMicros.AMC_ATTRIBUTE_POSITION);
		// Unbind Buffer
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		// Unbind array
		// GLES32.glBindVertexArray(0);

		// COLOR
		// Generate Buffer
		GLES32.glGenBuffers(1, vbo_color, 0);
		// Bind Generated Buffer
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color[0]);

		// byteBuffer=ByteBuffer.allocateDirect(triangleColors.length*4);

		// byteBuffer.order(ByteOrder.nativeOrder());

		// FloatBuffer colorBuffer=byteBuffer.asFloatBuffer();

		// colorBuffer.put(triangleColors);

		// colorBuffer.position(0);

		// Fill Buffer
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, null, GLES32.GL_DYNAMIC_DRAW);

		// GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
		// triangleVertices.length*4,
		// positionBuffer,
		// GLES32.GL_STATIC_DRAW);

		// Set Vertex Attrib Pointer
		GLES32.glVertexAttribPointer(GLESMicros.AMC_ATTRIBUTE_COLOR, 3, GLES32.GL_FLOAT, false, 0, 0);

		// Enable Vertex Attrib Array
		GLES32.glEnableVertexAttribArray(GLESMicros.AMC_ATTRIBUTE_COLOR);
		// Unbind Buffer
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		// Unbind array
		GLES32.glBindVertexArray(0);

		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);
		GLES32.glClearDepthf(1.0f);

		GLES32.glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		// GLES32.glEnable(GLES32.GL_TEXTURE_2D);

		Matrix.setIdentityM(perspectiveProjectionMatrix, 0);

	}

	void UnInitialize() {
		if (vbo_position[0] != 0) {
			GLES32.glDeleteBuffers(1, vbo_position, 0);
			vbo_position[0] = 0;
		}
		if (vbo_color[0] != 0) {
			GLES32.glDeleteBuffers(1, vbo_color, 0);
			vbo_color[0] = 0;
		}
		if (vao[0] != 0) {
			GLES32.glDeleteVertexArrays(1, vao, 0);
			vao[0] = 0;
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

	private float rectangleVertices[] = { 0.275f, 1.0f, 0.0f, 0.225f, 1.0f, 0.0f, 0.225f, -1.0f, 0.0f, 0.275f, -1.0f,
			0.0f };

	private float rectangleColorCoords[] = { 1.0f, 0.6f, 0.2f, 1.0f, 0.6f, 0.2f, 0.074f, 0.533f, 0.2f, 0.074f, 0.533f,
			0.2f };

	private void DrawLetter(char letter) {
	
	ByteBuffer byteBuffer;
	FloatBuffer positionBuffer;
	FloatBuffer colorBuffer;

	switch (letter)
	{
	case 'I':
		rectangleVertices[0] = 0.275f;
		rectangleVertices[1] = 1.0f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = 0.225f;
		rectangleVertices[4] = 1.0f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = 0.225f;
		rectangleVertices[7] = -1.0f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = 0.275f;
		rectangleVertices[10] = -1.0f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = 1.0f;
		rectangleColorCoords[1] = 0.6f;
		rectangleColorCoords[2] = 0.2f;
		rectangleColorCoords[3] = 1.0f;
		rectangleColorCoords[4] = 0.6f;
		rectangleColorCoords[5] = 0.2f;
		rectangleColorCoords[6] = 0.074f;
		rectangleColorCoords[7] = 0.533f;
		rectangleColorCoords[8] = 0.2f;
		rectangleColorCoords[9] = 0.074f;
		rectangleColorCoords[10] = 0.533f;
		rectangleColorCoords[11] = 0.2f;

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);


		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleColorCoords.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		colorBuffer=byteBuffer.asFloatBuffer();
		colorBuffer.put(rectangleColorCoords);
		colorBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, colorBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN,
		0,
		4);

		break;

	case 'N':

		//Left side of N
		rectangleVertices[0] = 0.05f;
		rectangleVertices[1] = 1.0f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = 0.0f;
		rectangleVertices[4] = 1.0f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = 0.00f;
		rectangleVertices[7] = -1.0f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = 0.05f;
		rectangleVertices[10] = -1.0f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = 1.0f;
		rectangleColorCoords[1] = 0.6f;
		rectangleColorCoords[2] = 0.2f;
		rectangleColorCoords[3] = 1.0f;
		rectangleColorCoords[4] = 0.6f;
		rectangleColorCoords[5] = 0.2f;
		rectangleColorCoords[6] = 0.074f;
		rectangleColorCoords[7] = 0.533f;
		rectangleColorCoords[8] = 0.2f;
		rectangleColorCoords[9] = 0.074f;
		rectangleColorCoords[10] = 0.533f;
		rectangleColorCoords[11] = 0.2f;

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleColorCoords.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		colorBuffer=byteBuffer.asFloatBuffer();
		colorBuffer.put(rectangleColorCoords);
		colorBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, colorBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN,
		0,
		4);

		//Middle part of N
		rectangleVertices[0] = 0.05f;
		rectangleVertices[1] = 1.0f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = 0.00f;
		rectangleVertices[4] = 1.0f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = 0.45f;
		rectangleVertices[7] = -1.0f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = 0.50f;
		rectangleVertices[10] = -1.0f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = 1.0f;
		rectangleColorCoords[1] = 0.6f;
		rectangleColorCoords[2] = 0.2f;
		rectangleColorCoords[3] = 1.0f;
		rectangleColorCoords[4] = 0.6f;
		rectangleColorCoords[5] = 0.2f;
		rectangleColorCoords[6] = 0.074f;
		rectangleColorCoords[7] = 0.533f;
		rectangleColorCoords[8] = 0.2f;
		rectangleColorCoords[9] = 0.074f;
		rectangleColorCoords[10] = 0.533f;
		rectangleColorCoords[11] = 0.2f;

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleColorCoords.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		colorBuffer=byteBuffer.asFloatBuffer();
		colorBuffer.put(rectangleColorCoords);
		colorBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, colorBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN,
		0,
		4);


		//Right Side of N
		rectangleVertices[0] = 0.50f;
		rectangleVertices[1] = 1.0f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = 0.45f;
		rectangleVertices[4] = 1.0f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = 0.45f;
		rectangleVertices[7] = -1.0f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = 0.50f;
		rectangleVertices[10] = -1.0f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = 1.0f;
		rectangleColorCoords[1] = 0.6f;
		rectangleColorCoords[2] = 0.2f;
		rectangleColorCoords[3] = 1.0f;
		rectangleColorCoords[4] = 0.6f;
		rectangleColorCoords[5] = 0.2f;
		rectangleColorCoords[6] = 0.074f;
		rectangleColorCoords[7] = 0.533f;
		rectangleColorCoords[8] = 0.2f;
		rectangleColorCoords[9] = 0.074f;
		rectangleColorCoords[10] = 0.533f;
		rectangleColorCoords[11] = 0.2f;


		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleColorCoords.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		colorBuffer=byteBuffer.asFloatBuffer();
		colorBuffer.put(rectangleColorCoords);
		colorBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, colorBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN,
		0,
		4);

		break;

	case 'D':

		rectangleVertices[0] = 0.05f;
		rectangleVertices[1] = 1.0f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = 0.0f;
		rectangleVertices[4] = 1.0f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = 0.00f;
		rectangleVertices[7] = -1.0f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = 0.05f;
		rectangleVertices[10] = -1.0f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = D_R_OrangeColor;
		rectangleColorCoords[1] = D_G_OrangeColor;
		rectangleColorCoords[2] = D_B_OrangeColor;
		rectangleColorCoords[3] = D_R_OrangeColor;
		rectangleColorCoords[4] = D_G_OrangeColor;
		rectangleColorCoords[5] = D_B_OrangeColor;
		rectangleColorCoords[6] = D_R_GreenColor;
		rectangleColorCoords[7] = D_G_GreenColor;
		rectangleColorCoords[8] = D_B_GreenColor;
		rectangleColorCoords[9] = D_R_GreenColor;
		rectangleColorCoords[10] = D_G_GreenColor;
		rectangleColorCoords[11] = D_B_GreenColor;


		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleColorCoords.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		colorBuffer=byteBuffer.asFloatBuffer();
		colorBuffer.put(rectangleColorCoords);
		colorBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, colorBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN,
		0,
		4);

		rectangleVertices[0] = 0.5f;
		rectangleVertices[1] = 1.0f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = -0.01f;
		rectangleVertices[4] = 1.0f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = -0.01f;
		rectangleVertices[7] = 0.95f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = 0.5f;
		rectangleVertices[10] = 0.95f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = D_R_OrangeColor;
		rectangleColorCoords[1] = D_G_OrangeColor;
		rectangleColorCoords[2] = D_B_OrangeColor;
		rectangleColorCoords[3] = D_R_OrangeColor;
		rectangleColorCoords[4] = D_G_OrangeColor;
		rectangleColorCoords[5] = D_B_OrangeColor;
		rectangleColorCoords[6] = D_R_OrangeColor;
		rectangleColorCoords[7] = D_G_OrangeColor;
		rectangleColorCoords[8] = D_B_OrangeColor;
		rectangleColorCoords[9] = D_R_OrangeColor;
		rectangleColorCoords[10] = D_G_OrangeColor;
		rectangleColorCoords[11] = D_B_OrangeColor;
		
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleColorCoords.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		colorBuffer=byteBuffer.asFloatBuffer();
		colorBuffer.put(rectangleColorCoords);
		colorBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, colorBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN,
		0,
		4);

		rectangleVertices[0] = 0.50f;
		rectangleVertices[1] = 1.0f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = 0.45f;
		rectangleVertices[4] = 1.0f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = 0.45f;
		rectangleVertices[7] = -1.0f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = 0.50f;
		rectangleVertices[10] = -1.0f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = D_R_OrangeColor;
		rectangleColorCoords[1] = D_G_OrangeColor;
		rectangleColorCoords[2] = D_B_OrangeColor;
		rectangleColorCoords[3] = D_R_OrangeColor;
		rectangleColorCoords[4] = D_G_OrangeColor;
		rectangleColorCoords[5] = D_B_OrangeColor;
		rectangleColorCoords[6] = D_R_GreenColor;
		rectangleColorCoords[7] = D_G_GreenColor;
		rectangleColorCoords[8] = D_B_GreenColor;
		rectangleColorCoords[9] = D_R_GreenColor;
		rectangleColorCoords[10] = D_G_GreenColor;
		rectangleColorCoords[11] = D_B_GreenColor;

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleColorCoords.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		colorBuffer=byteBuffer.asFloatBuffer();
		colorBuffer.put(rectangleColorCoords);
		colorBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, colorBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN,
		0,
		4);

		rectangleVertices[0] = 0.50f;
		rectangleVertices[1] = -0.95f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = -0.01f;
		rectangleVertices[4] = -0.95f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = -0.01f;
		rectangleVertices[7] = -1.0f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = 0.50f;
		rectangleVertices[10] = -1.0f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = D_R_GreenColor;
		rectangleColorCoords[1] = D_G_GreenColor;
		rectangleColorCoords[2] = D_B_GreenColor;
		rectangleColorCoords[3] = D_R_GreenColor;
		rectangleColorCoords[4] = D_G_GreenColor;
		rectangleColorCoords[5] = D_B_GreenColor;
		rectangleColorCoords[6] = D_R_GreenColor;
		rectangleColorCoords[7] = D_G_GreenColor;
		rectangleColorCoords[8] = D_B_GreenColor;
		rectangleColorCoords[9] = D_R_GreenColor;
		rectangleColorCoords[10] = D_G_GreenColor;
		rectangleColorCoords[11] = D_B_GreenColor;

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleColorCoords.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		colorBuffer=byteBuffer.asFloatBuffer();
		colorBuffer.put(rectangleColorCoords);
		colorBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, colorBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN,
		0,
		4);

		break;
	case 'A':
		
		rectangleVertices[0] = 0.275f;
		rectangleVertices[1] = 1.0f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = 0.225f;
		rectangleVertices[4] = 1.0f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = 0.00f;
		rectangleVertices[7] = -1.0f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = 0.05f;
		rectangleVertices[10] = -1.0f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = 1.0f;
		rectangleColorCoords[1] = 0.6f;
		rectangleColorCoords[2] = 0.2f;
		rectangleColorCoords[3] = 1.0f;
		rectangleColorCoords[4] = 0.6f;
		rectangleColorCoords[5] = 0.2f;
		rectangleColorCoords[6] = 0.074f;
		rectangleColorCoords[7] = 0.533f;
		rectangleColorCoords[8] = 0.2f;
		rectangleColorCoords[9] = 0.074f;
		rectangleColorCoords[10] = 0.533f;
		rectangleColorCoords[11] = 0.2f;

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleColorCoords.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		colorBuffer=byteBuffer.asFloatBuffer();
		colorBuffer.put(rectangleColorCoords);
		colorBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, colorBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN,
		0,
		4);

		rectangleVertices[0] = 0.275f;
		rectangleVertices[1] = 1.0f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = 0.225f;
		rectangleVertices[4] = 1.0f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = 0.45f;
		rectangleVertices[7] = -1.0f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = 0.50f;
		rectangleVertices[10] = -1.0f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = 1.0f;
		rectangleColorCoords[1] = 0.6f;
		rectangleColorCoords[2] = 0.2f;
		rectangleColorCoords[3] = 1.0f;
		rectangleColorCoords[4] = 0.6f;
		rectangleColorCoords[5] = 0.2f;
		rectangleColorCoords[6] = 0.074f;
		rectangleColorCoords[7] = 0.533f;
		rectangleColorCoords[8] = 0.2f;
		rectangleColorCoords[9] = 0.074f;
		rectangleColorCoords[10] = 0.533f;
		rectangleColorCoords[11] = 0.2f;

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleColorCoords.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		colorBuffer=byteBuffer.asFloatBuffer();
		colorBuffer.put(rectangleColorCoords);
		colorBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, colorBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN,
		0,
		4);

	case 'T':

		//Tricolor of A
		rectangleVertices[0] = 0.380f;
		rectangleVertices[1] = 0.06f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = 0.12f;
		rectangleVertices[4] = 0.06f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = 0.12f;
		rectangleVertices[7] = 0.02f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = 0.380f;
		rectangleVertices[10] = 0.02f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = 1.0f;
		rectangleColorCoords[1] = 0.6f;
		rectangleColorCoords[2] = 0.2f;
		rectangleColorCoords[3] = 1.0f;
		rectangleColorCoords[4] = 0.6f;
		rectangleColorCoords[5] = 0.2f;
		rectangleColorCoords[6] = 1.0f;
		rectangleColorCoords[7] = 0.6f;
		rectangleColorCoords[8] = 0.2f;
		rectangleColorCoords[9] = 1.0f;
		rectangleColorCoords[10] = 0.6f;
		rectangleColorCoords[11] = 0.2f;

			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleColorCoords.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		colorBuffer=byteBuffer.asFloatBuffer();
		colorBuffer.put(rectangleColorCoords);
		colorBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, colorBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN,
		0,
		4);

		rectangleVertices[0] = 0.380f;
		rectangleVertices[1] = 0.02f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = 0.12f;
		rectangleVertices[4] = 0.02f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = 0.12f;
		rectangleVertices[7] = -0.02f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = 0.380f;
		rectangleVertices[10] = -0.02f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = 1.0f;
		rectangleColorCoords[1] = 1.0f;
		rectangleColorCoords[2] = 1.0f;
		rectangleColorCoords[3] = 1.0f;
		rectangleColorCoords[4] = 1.0f;
		rectangleColorCoords[5] = 1.0f;
		rectangleColorCoords[6] = 1.0f;
		rectangleColorCoords[7] = 1.0f;
		rectangleColorCoords[8] = 1.0f;
		rectangleColorCoords[9] = 1.0f;
		rectangleColorCoords[10] = 1.0f;
		rectangleColorCoords[11] = 1.0f;

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleColorCoords.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		colorBuffer=byteBuffer.asFloatBuffer();
		colorBuffer.put(rectangleColorCoords);
		colorBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, colorBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN,
		0,
		4);

		rectangleVertices[0] = 0.380f;
		rectangleVertices[1] = -0.02f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = 0.12f;
		rectangleVertices[4] = -0.02f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = 0.12f;
		rectangleVertices[7] = -0.06f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = 0.380f;
		rectangleVertices[10] = -0.06f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = 0.074f;
		rectangleColorCoords[1] = 0.533f;
		rectangleColorCoords[2] = 0.2f;
		rectangleColorCoords[3] = 0.074f;
		rectangleColorCoords[4] = 0.533f;
		rectangleColorCoords[5] = 0.2f;
		rectangleColorCoords[6] = 0.074f;
		rectangleColorCoords[7] = 0.533f;
		rectangleColorCoords[8] = 0.2f;
		rectangleColorCoords[9] = 0.074f;
		rectangleColorCoords[10] = 0.533f;
		rectangleColorCoords[11] = 0.2f;

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleColorCoords.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		colorBuffer=byteBuffer.asFloatBuffer();
		colorBuffer.put(rectangleColorCoords);
		colorBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, colorBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN,
		0,
		4);

		break;
	}
	
	}

}