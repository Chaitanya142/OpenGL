package com.example.ogl_graph_with_shapes;

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

import java.lang.Math;

public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener {
	private static Context context;
	private GestureDetector gestureDetector;

	private int shaderProgramObject;
	// Shader Objects
	private int gVertexShaderObject;
	private int gFragementShaderObject;

	private int[] vao_line = new int[1];
	private int[] vao_point = new int[1];

	private int[] vbo_position_lines = new int[1];
	private int[] vbo_color_lines = new int[1];

	private int[] vbo_position_point = new int[1];
	private int[] vbo_color_point = new int[1];

	private int mvpUniform;

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

	private ByteBuffer byteBuffer;
	private FloatBuffer positionBuffer;
	private FloatBuffer colorBuffer;

	private void Display() {

		float count = -1.0f;

		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

		GLES32.glUseProgram(shaderProgramObject);

		// Declaration of Matrices
		float[] modelViewMatrix = new float[16];
		float[] modelViewProjectionMatrix = new float[16];
		float[] translationMatrix = new float[16];
		float[] rotationMatrix = new float[16];

		// Initialize matrices
		Matrix.setIdentityM(modelViewMatrix, 0);
		Matrix.setIdentityM(modelViewProjectionMatrix, 0);
		Matrix.setIdentityM(translationMatrix, 0);
		Matrix.setIdentityM(rotationMatrix, 0);

		// Transformation

		Matrix.translateM(translationMatrix, 0, 0.0f, 0.0f, -3.0f);

		// Matrix Multiplication

		Matrix.multiplyMM(modelViewMatrix, 0, translationMatrix, 0, rotationMatrix, 0);

		Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

		// Send necessary matrices to shader in resp. Uniforms

		GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);

		// Bind with vao_line

		GLES32.glBindVertexArray(vao_line[0]);

		// Bind with textures if any

		// Draw

		DrawGraph(0.01f);

		DrawTriangle(0.5f);

		DrawRectangle(1.0f, 1.0f);

		GLES32.glBindVertexArray(0);

		GLES32.glBindVertexArray(vao_point[0]);

		DrawInCircle(0.0f, 0.5f, 0.0f, -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f);

		DrawCircleAroundRectangle(1.0f, 1.0f);

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

		// Create vao_line
		// Save everying in single set
		GLES32.glGenVertexArrays(1, vao_line, 0);
		GLES32.glBindVertexArray(vao_line[0]);
		// Line
		// Position
		// Generate Buffer
		GLES32.glGenBuffers(1, vbo_position_lines, 0);
		// Bind Generated Buffer
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_lines[0]);

		// Fill Buffer
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 2 * 3 * 4, null, GLES32.GL_DYNAMIC_DRAW);

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
		GLES32.glGenBuffers(1, vbo_color_lines, 0);
		// Bind Generated Buffer
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_lines[0]);

		// Fill Buffer
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 2 * 3 * 4, null, GLES32.GL_DYNAMIC_DRAW);

		// Set Vertex Attrib Pointer
		GLES32.glVertexAttribPointer(GLESMicros.AMC_ATTRIBUTE_COLOR, 3, GLES32.GL_FLOAT, false, 0, 0);

		// Enable Vertex Attrib Array
		GLES32.glEnableVertexAttribArray(GLESMicros.AMC_ATTRIBUTE_COLOR);
		// Unbind Buffer
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		// Unbind array
		GLES32.glBindVertexArray(0);

		// Points
		// Position
		// Create vao_line
		// Save everying in single set
		GLES32.glGenVertexArrays(1, vao_point, 0);
		GLES32.glBindVertexArray(vao_point[0]);
		// Generate Buffer
		GLES32.glGenBuffers(1, vbo_position_point, 0);
		// Bind Generated Buffer
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_point[0]);

		// Fill Buffer
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, (int)(3 * 4 * ((2 * 3.1415) / 0.001)), null, GLES32.GL_DYNAMIC_DRAW);

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
		GLES32.glGenBuffers(1, vbo_color_point, 0);
		// Bind Generated Buffer
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_point[0]);

		// Fill Buffer
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, (int)(3 * 4 * ((2 * 3.1415) / 0.001)), null, GLES32.GL_DYNAMIC_DRAW);

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
		if (vbo_position_lines[0] != 0) {
			GLES32.glDeleteBuffers(1, vbo_position_lines, 0);
			vbo_position_lines[0] = 0;
		}
		if (vbo_color_lines[0] != 0) {
			GLES32.glDeleteBuffers(1, vbo_color_lines, 0);
			vbo_color_lines[0] = 0;
		}
		if (vbo_position_point[0] != 0) {
			GLES32.glDeleteBuffers(1, vbo_position_point, 0);
			vbo_position_point[0] = 0;
		}
		if (vbo_color_point[0] != 0) {
			GLES32.glDeleteBuffers(1, vbo_color_point, 0);
			vbo_color_point[0] = 0;
		}
		if (vao_line[0] != 0) {
			GLES32.glDeleteVertexArrays(1, vao_line, 0);
			vao_line[0] = 0;
		}
		if (vao_point[0] != 0) {
			GLES32.glDeleteVertexArrays(1, vao_point, 0);
			vao_point[0] = 0;
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

	private void DrawGraph(float offset) {
		float count = -1.0f;
		float[] lineVertices = new float[6];

		float lineColor[] = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f };

		for (count = -1.0f; count <= 1.01f; count = count + offset) {

			lineVertices[0] = count;
			lineVertices[1] = -1.0f;
			lineVertices[2] = 0.0f;
			lineVertices[3] = count;
			lineVertices[4] = 1.0f;
			lineVertices[5] = 0.0f;

			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_lines[0]);

			byteBuffer = ByteBuffer.allocateDirect(lineVertices.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
			positionBuffer = byteBuffer.asFloatBuffer();
			positionBuffer.put(lineVertices);
			positionBuffer.position(0);

			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 3 * 2 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_lines[0]);

			byteBuffer = ByteBuffer.allocateDirect(lineColor.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
			colorBuffer = byteBuffer.asFloatBuffer();
			colorBuffer.put(lineColor);
			colorBuffer.position(0);

			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 3 * 2 * 4, colorBuffer, GLES32.GL_DYNAMIC_DRAW);

			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

			GLES32.glDrawArrays(GLES32.GL_LINES, 0, 2);

			lineVertices[0] = -1.0f;
			lineVertices[1] = count;
			lineVertices[2] = 0.0f;
			lineVertices[3] = 1.0f;
			lineVertices[4] = count;
			lineVertices[5] = 0.0f;

			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_lines[0]);

			byteBuffer = ByteBuffer.allocateDirect(lineVertices.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
			positionBuffer = byteBuffer.asFloatBuffer();
			positionBuffer.put(lineVertices);
			positionBuffer.position(0);

			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 3 * 2 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_lines[0]);

			byteBuffer = ByteBuffer.allocateDirect(lineColor.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
			colorBuffer = byteBuffer.asFloatBuffer();
			colorBuffer.put(lineColor);
			colorBuffer.position(0);

			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 3 * 2 * 4, colorBuffer, GLES32.GL_DYNAMIC_DRAW);

			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

			GLES32.glDrawArrays(GLES32.GL_LINES, 0, 2);
		}
	}

	private void DrawTriangle(float offset) {
		float[] lineVertices = new float[6];

		float lineColor[] = { 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f };

		lineVertices[0] = 0.0f;
		lineVertices[1] = offset;
		lineVertices[2] = 0.0f;
		lineVertices[3] = -offset;
		lineVertices[4] = -offset;
		lineVertices[5] = 0.0f;

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_lines[0]);

		byteBuffer = ByteBuffer.allocateDirect(lineVertices.length * 4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer = byteBuffer.asFloatBuffer();
		positionBuffer.put(lineVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 3 * 2 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_lines[0]);

		byteBuffer = ByteBuffer.allocateDirect(lineColor.length * 4);
		byteBuffer.order(ByteOrder.nativeOrder());
		colorBuffer = byteBuffer.asFloatBuffer();
		colorBuffer.put(lineColor);
		colorBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 3 * 2 * 4, colorBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glDrawArrays(GLES32.GL_LINES, 0, 2);

		lineVertices[0] = -offset;
		lineVertices[1] = -offset;
		lineVertices[2] = 0.0f;
		lineVertices[3] = offset;
		lineVertices[4] = -offset;
		lineVertices[5] = 0.0f;

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_lines[0]);

		byteBuffer = ByteBuffer.allocateDirect(lineVertices.length * 4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer = byteBuffer.asFloatBuffer();
		positionBuffer.put(lineVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 3 * 2 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_lines[0]);

		byteBuffer = ByteBuffer.allocateDirect(lineColor.length * 4);
		byteBuffer.order(ByteOrder.nativeOrder());
		colorBuffer = byteBuffer.asFloatBuffer();
		colorBuffer.put(lineColor);
		colorBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 3 * 2 * 4, colorBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glDrawArrays(GLES32.GL_LINES, 0, 2);

		lineVertices[0] = offset;
		lineVertices[1] = -offset;
		lineVertices[2] = 0.0f;
		lineVertices[3] = 0.0f;
		lineVertices[4] = offset;
		lineVertices[5] = 0.0f;

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_lines[0]);

		byteBuffer = ByteBuffer.allocateDirect(lineVertices.length * 4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer = byteBuffer.asFloatBuffer();
		positionBuffer.put(lineVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 3 * 2 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_lines[0]);

		byteBuffer = ByteBuffer.allocateDirect(lineColor.length * 4);
		byteBuffer.order(ByteOrder.nativeOrder());
		colorBuffer = byteBuffer.asFloatBuffer();
		colorBuffer.put(lineColor);
		colorBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 3 * 2 * 4, colorBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glDrawArrays(GLES32.GL_LINES, 0, 2);

	}

	private int isArrInCircleInit = 0;

	private float[] ArrInCirclePos = new float[(int) (((2 * 3.1415) / 0.001) * 3)+3];
	private float[] ArrInCircleColor = new float[(int) (((2 * 3.1415) / 0.001) * 3)+3];
	
	private void DrawInCircle(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3,
			float z3) {


		if (isArrInCircleInit == 0) {

			float angle = 0;

			float distP1P2 = (float) Math.sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) + (z2 - z1) * (z2 - z1));
			float distP2P3 = (float) Math.sqrt((x3 - x2) * (x3 - x2) + (y3 - y2) * (y3 - y2) + (z3 - z2) * (z3 - z2));
			float distP3P1 = (float) Math.sqrt((x1 - x3) * (x1 - x3) + (y1 - y3) * (y1 - y3) + (z1 - z3) * (z1 - z3));

			float semiPerimeter = (distP1P2 + distP2P3 + distP3P1) / 2;

			float radius = (float) Math.sqrt((semiPerimeter - distP1P2) * (semiPerimeter - distP2P3) * (semiPerimeter - distP3P1)
					/ semiPerimeter);

			float Ox = (x3 * distP1P2 + x1 * distP2P3 + x2 * distP3P1) / (semiPerimeter * 2);
			float Oy = (y3 * distP1P2 + y1 * distP2P3 + y2 * distP3P1) / (semiPerimeter * 2);
			float Oz = (z3 * distP1P2 + z1 * distP2P3 + z2 * distP3P1) / (semiPerimeter * 2);

			int indexPos = 0;
			int indexColor = 0;

			for (angle = 0.0f; angle < 2 * 3.1415; angle = angle + 0.001f) {
				ArrInCirclePos[indexPos++] = (float) Math.cos(angle) * radius + Ox;
				ArrInCirclePos[indexPos++] = (float) Math.sin(angle) * radius + Oy;
				ArrInCirclePos[indexPos++] = 0.0f + Oz;

				ArrInCircleColor[indexColor++] = 1.0f;
				ArrInCircleColor[indexColor++] = 1.0f;
				ArrInCircleColor[indexColor++] = 0.0f;
			}
			isArrInCircleInit = 1;
		}

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_point[0]);

		byteBuffer = ByteBuffer.allocateDirect(ArrInCircleColor.length * 4);
		byteBuffer.order(ByteOrder.nativeOrder());
		colorBuffer = byteBuffer.asFloatBuffer();
		colorBuffer.put(ArrInCircleColor);
		colorBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, (int)(3 * 4 * ((2 * 3.1415) / 0.001)), colorBuffer,
				GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_point[0]);

		byteBuffer = ByteBuffer.allocateDirect(ArrInCirclePos.length * 4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer = byteBuffer.asFloatBuffer();
		positionBuffer.put(ArrInCirclePos);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, (int)(3 * 4 * ((2 * 3.1415) / 0.001)), positionBuffer,
				GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glDrawArrays(GLES32.GL_POINTS, 0, (int)((2 * 3.1415) / 0.001));

	}

	private int isArrCircleInit = 0;
	private float[] ArrCirclePos = new float[(int) (((2 * 3.1415) / 0.001) * 3)+3];
	private float[] ArrCircleColor = new float[(int) (((2 * 3.1415) / 0.001) * 3)+3];

	private void DrawCircle(float radius) {

		if (isArrCircleInit == 0) {
			float angle = 0;
			int indexPos = 0;
			int indexColor = 0;

			for (angle = 0.0f; angle < 2 * 3.1415; angle = angle + 0.001f) {
				ArrCirclePos[indexPos++] = (float) Math.cos(angle) * radius ;
				ArrCirclePos[indexPos++] = (float) Math.sin(angle) * radius ;
				ArrCirclePos[indexPos++] = 0.0f ;

				ArrCircleColor[indexColor++] = 1.0f;
				ArrCircleColor[indexColor++] = 1.0f;
				ArrCircleColor[indexColor++] = 0.0f;
			}
			isArrCircleInit = 1;
		}

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_point[0]);

		byteBuffer = ByteBuffer.allocateDirect(ArrCircleColor.length * 4);
		byteBuffer.order(ByteOrder.nativeOrder());
		colorBuffer = byteBuffer.asFloatBuffer();
		colorBuffer.put(ArrCircleColor);
		colorBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, (int)(3 * 4 * ((2 * 3.1415) / 0.001)), colorBuffer,
				GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_point[0]);

		byteBuffer = ByteBuffer.allocateDirect(ArrCirclePos.length * 4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer = byteBuffer.asFloatBuffer();
		positionBuffer.put(ArrCirclePos);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, (int)(3 * 4 * ((2 * 3.1415) / 0.001)), positionBuffer,
				GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glDrawArrays(GLES32.GL_POINTS, 0, (int)((2 * 3.1415) / 0.001));

	}

	private void DrawRectangle(float width, float height) {

		float[] lineVertices = new float[6];

		float lineColor[] = { 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f };

		lineVertices[0] = width / 2;
		lineVertices[1] = height / 2;
		lineVertices[2] = 0.0f;
		lineVertices[3] = -width / 2;
		lineVertices[4] = height / 2;
		lineVertices[5] = 0.0f;

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_lines[0]);

		byteBuffer = ByteBuffer.allocateDirect(lineVertices.length * 4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer = byteBuffer.asFloatBuffer();
		positionBuffer.put(lineVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 3 * 2 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_lines[0]);

		byteBuffer = ByteBuffer.allocateDirect(lineColor.length * 4);
		byteBuffer.order(ByteOrder.nativeOrder());
		colorBuffer = byteBuffer.asFloatBuffer();
		colorBuffer.put(lineColor);
		colorBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 3 * 2 * 4, colorBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glDrawArrays(GLES32.GL_LINES, 0, 2);

		lineVertices[0] = -width / 2;
		lineVertices[1] = height / 2;
		lineVertices[2] = 0.0f;
		lineVertices[3] = -width / 2;
		lineVertices[4] = -height / 2;
		lineVertices[5] = 0.0f;

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_lines[0]);

		byteBuffer = ByteBuffer.allocateDirect(lineVertices.length * 4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer = byteBuffer.asFloatBuffer();
		positionBuffer.put(lineVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 3 * 2 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_lines[0]);

		byteBuffer = ByteBuffer.allocateDirect(lineColor.length * 4);
		byteBuffer.order(ByteOrder.nativeOrder());
		colorBuffer = byteBuffer.asFloatBuffer();
		colorBuffer.put(lineColor);
		colorBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 3 * 2 * 4, colorBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glDrawArrays(GLES32.GL_LINES, 0, 2);

		lineVertices[0] = -width / 2;
		lineVertices[1] = -height / 2;
		lineVertices[2] = 0.0f;
		lineVertices[3] = width / 2;
		lineVertices[4] = -height / 2;
		lineVertices[5] = 0.0f;

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_lines[0]);

		byteBuffer = ByteBuffer.allocateDirect(lineVertices.length * 4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer = byteBuffer.asFloatBuffer();
		positionBuffer.put(lineVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 3 * 2 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_lines[0]);

		byteBuffer = ByteBuffer.allocateDirect(lineColor.length * 4);
		byteBuffer.order(ByteOrder.nativeOrder());
		colorBuffer = byteBuffer.asFloatBuffer();
		colorBuffer.put(lineColor);
		colorBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 3 * 2 * 4, colorBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glDrawArrays(GLES32.GL_LINES, 0, 2);

		lineVertices[0] = width / 2;
		lineVertices[1] = -height / 2;
		lineVertices[2] = 0.0f;
		lineVertices[3] = width / 2;
		lineVertices[4] = height / 2;
		lineVertices[5] = 0.0f;

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_lines[0]);

		byteBuffer = ByteBuffer.allocateDirect(lineVertices.length * 4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer = byteBuffer.asFloatBuffer();
		positionBuffer.put(lineVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 3 * 2 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_lines[0]);

		byteBuffer = ByteBuffer.allocateDirect(lineColor.length * 4);
		byteBuffer.order(ByteOrder.nativeOrder());
		colorBuffer = byteBuffer.asFloatBuffer();
		colorBuffer.put(lineColor);
		colorBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 3 * 2 * 4, colorBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glDrawArrays(GLES32.GL_LINES, 0, 2);

	}

	private void DrawCircleAroundRectangle(float width, float height) {

		float radius = (float) Math.sqrt(width / 2 * width / 2 + height / 2 * height / 2);
		DrawCircle(radius);

	}

}
