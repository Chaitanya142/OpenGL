package com.example.ogl_dynamic_india;

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

	private int[] vao_quad = new int[1];
	private int[] vbo_position_quad = new int[1];
	private int[] vbo_color_quad = new int[1];
	
	private int[] vao_triangle = new int[1];
	private int[] vbo_position_triangle = new int[1];
	private int[] vbo_color_triangle = new int[1];

	private int mvpUniform;

	private float D_R_OrangeColor = 1.0f;
	private float D_G_OrangeColor = 0.6f;
	private float D_B_OrangeColor = 0.2f;
	private float D_R_GreenColor = 0.074f;
	private float D_G_GreenColor = 0.533f;
	private float D_B_GreenColor = 0.2f;

	private float Line_Orange_R = 1.0f;
	private float Line_Orange_G = 0.6f;
	private float Line_Orange_B = 0.2f;
	private float Line_White_R = 1.00f;
	private float Line_White_G = 1.00f;
	private float Line_White_B = 1.00f;
	private float Line_Green_R = 0.074f;
	private float Line_Green_G = 0.533f;
	private float Line_Green_B = 0.2f;

	private int stage = 1;
	private float I_1_XPosition = -2.25f;
	private float A_XPosition = 2.25f;
	private float N_YPosition = 2.25f;
	private float I_2_YPosition = -2.25f;
	private float Plane1_Y_Position = 0.00f;
	private float Plane1_Z_Rotation_Angle = -90.0f;

	private float Plane1_X_Position = -2.5f;
	private float angle = 0.0f;
	private float Line_X_Position = -1.28f;

	Sphere planeFrontSphere=new Sphere();

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

			// Bind with vao_quad


			// Bind with textures if any

			// Draw

			// GLES32.glDrawArrays(GLES32.GL_TRIANGLES,
			// 0,
			// 3);

			DrawLetter('I');

			// Unbind vao_quad

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

			// Bind with vao_quad

			// Bind with textures if any

			// Draw

			DrawLetter('N');

			// Unbind vao_quad

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

			// Bind with vao_quad

			// Bind with textures if any

			// Draw

			DrawLetter('D');

			// Unbind vao_quad

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

			// Bind with vao_quad

			// Bind with textures if any

			// Draw

			DrawLetter('I');

			// Unbind vao_quad

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
			// Bind with vao_quad
			// Bind with textures if any
			// Draw
			DrawLetter('A');
			//DrawLetter('T');
			// Unbind vao_quad
		}

		if (stage == 8) {
			Plane1_Y_Position = (float)((Plane1_X_Position - 1.3)*(Plane1_X_Position - 1.3) / (float)(4 * 0.15f)) - 0.0f;
			
			// Initialize matrices
			Matrix.setIdentityM(modelViewMatrix, 0);
			Matrix.setIdentityM(modelViewProjectionMatrix, 0);
			Matrix.setIdentityM(translationMatrix, 0);
			Matrix.setIdentityM(rotationMatrix, 0);
			// Transformation
			Matrix.translateM(translationMatrix, 0, Plane1_X_Position, Plane1_Y_Position, -3.0f);
			Matrix.setRotateM(rotationMatrix,0,
			Plane1_Z_Rotation_Angle,0.0f, 0.0f, 1.0f);
			// Matrix Multiplication
			Matrix.multiplyMM(modelViewMatrix, 0, translationMatrix, 0, rotationMatrix, 0);
			Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);
			// Send necessary matrices to shader in resp. Uniforms
			GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);
			// Bind with vao_quad
			// Bind with textures if any
			// Draw
			DrawLetter('P');
			// Unbind vao_quad


			// Initialize matrices
			Matrix.setIdentityM(modelViewMatrix, 0);
			Matrix.setIdentityM(modelViewProjectionMatrix, 0);
			Matrix.setIdentityM(translationMatrix, 0);
			Matrix.setIdentityM(rotationMatrix, 0);
			// Transformation
			Matrix.translateM(translationMatrix, 0, Plane1_X_Position, 0.0f, -3.0f);
			// Matrix Multiplication
			Matrix.multiplyMM(modelViewMatrix, 0, translationMatrix, 0, rotationMatrix, 0);
			Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);
			// Send necessary matrices to shader in resp. Uniforms
			GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);
			// Bind with vao_quad
			// Bind with textures if any
			// Draw
			DrawLetter('P');
			// Unbind vao_quad

			// Initialize matrices
			Matrix.setIdentityM(modelViewMatrix, 0);
			Matrix.setIdentityM(modelViewProjectionMatrix, 0);
			Matrix.setIdentityM(translationMatrix, 0);
			Matrix.setIdentityM(rotationMatrix, 0);
			// Transformation
			Matrix.translateM(translationMatrix, 0, Plane1_X_Position, -Plane1_Y_Position, -3.0f);
			Matrix.setRotateM(rotationMatrix,0,
			-Plane1_Z_Rotation_Angle, 0.0f, 0.0f, 1.0f);
			// Matrix Multiplication
			Matrix.multiplyMM(modelViewMatrix, 0, translationMatrix, 0, rotationMatrix, 0);
			Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);
			// Send necessary matrices to shader in resp. Uniforms
			GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);
			// Bind with vao_quad
			// Bind with textures if any
			// Draw
			DrawLetter('P');
			// Unbind vao_quad
			
		}
	
		if (stage == 7) {
			// Initialize matrices
			Matrix.setIdentityM(modelViewMatrix, 0);
			Matrix.setIdentityM(modelViewProjectionMatrix, 0);
			Matrix.setIdentityM(translationMatrix, 0);
			Matrix.setIdentityM(rotationMatrix, 0);
			// Transformation
			Matrix.translateM(translationMatrix, 0, Plane1_X_Position, 0.0f, -3.0f);
			Matrix.setRotateM(rotationMatrix,0,
			-Plane1_Z_Rotation_Angle, 0.0f, 0.0f, 1.0f);
			// Matrix Multiplication
			Matrix.multiplyMM(modelViewMatrix, 0, translationMatrix, 0, rotationMatrix, 0);
			Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);
			// Send necessary matrices to shader in resp. Uniforms
			GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);
			// Bind with vao_quad
			// Bind with textures if any
			// Draw
			DrawLetter('P');
			// Unbind vao_quad
	
		}
	
		if ((stage == 7 || stage == 8) && Plane1_X_Position > -1.02f && Plane1_X_Position < 1.40f) {
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
			// Bind with vao_quad
			// Bind with textures if any
			// Draw
			DrawLetter('L');
			// Unbind vao_quad
		}
	
		if (stage > 7 && Plane1_X_Position > 1.40f) {
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
			// Bind with vao_quad
			// Bind with textures if any
			// Draw
			DrawLetter('T');
			// Unbind vao_quad
		}
	
		if (stage > 7 && Plane1_X_Position > 1.40f && Line_X_Position <= 1.10f) {
			Line_X_Position = Line_X_Position + 0.006f;
	
			if (Line_White_R > 0.0f)
				Line_White_R = Line_White_R - Line_White_R / 256;
			if (Line_White_G > 0.0f)
				Line_White_G = Line_White_G - Line_White_G / 256;
			if (Line_White_B > 0.0f)
				Line_White_B = Line_White_B - Line_White_B / 256;
	
			if (Line_Orange_R > 0.0f)
				Line_Orange_R = Line_Orange_R - Line_Orange_R / 256;
			if (Line_Orange_G > 0.0f)
				Line_Orange_G = Line_Orange_G - Line_Orange_G / 256;
			if (Line_Orange_B > 0.0f)
				Line_Orange_B = Line_Orange_B - Line_Orange_B / 256;
	
			if (Line_Green_R > 0.0f)
				Line_Green_R = Line_Green_R - Line_Green_R / 256;
			if (Line_Green_G > 0.0f)
				Line_Green_G = Line_Green_G - Line_Green_G / 256;
			if (Line_Green_B > 0.0f)
				Line_Green_B = Line_Green_B - Line_Green_B / 256;
	
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
			// Bind with vao_quad
			// Bind with textures if any
			// Draw
			DrawLetter('L');
			// Unbind vao_quad
		}
	
		if (stage > 7 && Plane1_X_Position > 1.40f) {
			
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
			// Bind with vao_quad
			// Bind with textures if any
			// Draw
			DrawLetter('T');
			// Unbind vao_quad
	
		}
	
	
		if (stage == 6)
		{
			Plane1_Y_Position = ((Plane1_X_Position + 1.3f)*(Plane1_X_Position + 1.3f) / (float)(4 * 0.15)) - 0.0f;
	
			// Initialize matrices
			Matrix.setIdentityM(modelViewMatrix, 0);
			Matrix.setIdentityM(modelViewProjectionMatrix, 0);
			Matrix.setIdentityM(translationMatrix, 0);
			Matrix.setIdentityM(rotationMatrix, 0);
			// Transformation
			Matrix.translateM(translationMatrix, 0, Plane1_X_Position, Plane1_Y_Position, -3.0f);
			Matrix.setRotateM(rotationMatrix,0,
			Plane1_Z_Rotation_Angle, 0.0f, 0.0f, 1.0f);
			// Matrix Multiplication
			Matrix.multiplyMM(modelViewMatrix, 0, translationMatrix, 0, rotationMatrix, 0);
			Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);
			// Send necessary matrices to shader in resp. Uniforms
			GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);
			// Bind with vao_quad
			// Bind with textures if any
			// Draw
			DrawLetter('P');
			// Unbind vao_quad


			// Initialize matrices
			Matrix.setIdentityM(modelViewMatrix, 0);
			Matrix.setIdentityM(modelViewProjectionMatrix, 0);
			Matrix.setIdentityM(translationMatrix, 0);
			Matrix.setIdentityM(rotationMatrix, 0);
			// Transformation
			Matrix.translateM(translationMatrix, 0, Plane1_X_Position, 0.0f, -3.0f);
			// Matrix Multiplication
			Matrix.multiplyMM(modelViewMatrix, 0, translationMatrix, 0, rotationMatrix, 0);
			Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);
			// Send necessary matrices to shader in resp. Uniforms
			GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);
			// Bind with vao_quad
			// Bind with textures if any
			// Draw
			DrawLetter('P');
			// Unbind vao_quad

			// Initialize matrices
			Matrix.setIdentityM(modelViewMatrix, 0);
			Matrix.setIdentityM(modelViewProjectionMatrix, 0);
			Matrix.setIdentityM(translationMatrix, 0);
			Matrix.setIdentityM(rotationMatrix, 0);
			// Transformation
			Matrix.translateM(translationMatrix, 0, Plane1_X_Position, -1 * (Plane1_Y_Position), -3.0f);
			Matrix.setRotateM(rotationMatrix,0,
			-Plane1_Z_Rotation_Angle, 0.0f, 0.0f, 1.0f);
			// Matrix Multiplication
			Matrix.multiplyMM(modelViewMatrix, 0, translationMatrix, 0, rotationMatrix, 0);
			Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);
			// Send necessary matrices to shader in resp. Uniforms
			GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);
			// Bind with vao_quad
			// Bind with textures if any
			// Draw
			DrawLetter('P');
			// Unbind vao_quad
		

		}

		GLES32.glUseProgram(0);

		Update();

		requestRender();

	}

	private void Update() {
		switch (stage) {
			case 1:
				//Print I
				if (I_1_XPosition < -1.5f)
					//I_1_XPosition = I_1_XPosition + 0.00022f;
					I_1_XPosition = I_1_XPosition + 0.005f;
				else {
					I_1_XPosition = -1.5f;
					stage++;
				}
				break;
			case 2:
				//Print A
				if (A_XPosition > 0.75f)
					//A_XPosition = A_XPosition - 0.00022f;
					A_XPosition = A_XPosition - 0.005f;
				else {
					A_XPosition = 0.75f;
					stage++;
				}
				break;
			case 3:
				//Print N
				if (N_YPosition > 0.00f)
					//N_YPosition = N_YPosition - 0.00022f;
					N_YPosition = N_YPosition - 0.005f;
				else {
					N_YPosition = 0.00f;
					stage++;
				}
				break;
			case 4:
				//Print I
				if (I_2_YPosition < 0.00f)
					//I_2_YPosition = I_2_YPosition + 0.00022f;
					I_2_YPosition = I_2_YPosition + 0.005f;
				else {
					I_2_YPosition = 0.00f;
					stage++;
				}
				break;
			case 5:
				//Print D
				if (D_R_OrangeColor < 1.0f)
					//D_R_OrangeColor = D_R_OrangeColor + 1.0f / 5050;
					D_R_OrangeColor = D_R_OrangeColor + 1.0f / 1000;
				if (D_G_OrangeColor < 0.6f)
					D_G_OrangeColor = D_G_OrangeColor + 0.6f / 1000;
				if (D_B_OrangeColor < 0.2f)
					D_B_OrangeColor = D_B_OrangeColor + 0.2f / 1000;
		
				if (D_R_GreenColor < 0.074f)
					D_R_GreenColor = D_R_GreenColor + 0.074f / 1000;
				if (D_G_GreenColor < 0.533f)
					D_G_GreenColor = D_G_GreenColor + 0.533f / 1000;
				if (D_B_GreenColor < 0.2f)
					D_B_GreenColor = D_B_GreenColor + 0.2f / 1000;
		
				if (D_R_OrangeColor >= 1.0f && D_G_OrangeColor >= 0.6f && D_B_OrangeColor >= 0.2f && D_R_GreenColor >= 0.074f && D_G_GreenColor >= 0.533f && D_B_GreenColor >= 0.2f)
					stage++;
				break;
			case 6:
				if (Plane1_X_Position < -1.3f) {
					//Plane1_X_Position = Plane1_X_Position + 0.0008f;
					Plane1_X_Position = Plane1_X_Position + 0.001f;
					//Plane1_Z_Rotation_Angle = Plane1_Z_Rotation_Angle + 90.0f / (1.2 / 0.0008);//0.015f
					Plane1_Z_Rotation_Angle = Plane1_Z_Rotation_Angle + 90.0f / (float)(1.2 / 0.001);//0.015f
				}
				else
					stage++;
				break;
			case 7:
				if (Plane1_X_Position < 1.3f)
					//Plane1_X_Position = Plane1_X_Position + 0.0008f;
					Plane1_X_Position = Plane1_X_Position + 0.005f;//+ 0.001f;
				else
					stage++;
				break;
			case 8:
				if (Plane1_X_Position < 2.5f)
				{
					//Plane1_X_Position = Plane1_X_Position + 0.0008f;
					//Plane1_Z_Rotation_Angle = Plane1_Z_Rotation_Angle + 90.0f / (1.3 / 0.0008);
					Plane1_X_Position = Plane1_X_Position + 0.001f;
					Plane1_Z_Rotation_Angle = Plane1_Z_Rotation_Angle + 90.0f / (float)(1.3 / 0.001);
				}
				else
					stage++;
				break;
			}
		
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

		// Create vao_quad
		// Save everying in single set
		GLES32.glGenVertexArrays(1, vao_quad, 0);

		GLES32.glBindVertexArray(vao_quad[0]);

		// Posistion
		// Generate Buffer
		GLES32.glGenBuffers(1, vbo_position_quad, 0);
		// Bind Generated Buffer
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_quad[0]);

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
		GLES32.glGenBuffers(1, vbo_color_quad, 0);
		// Bind Generated Buffer
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_quad[0]);

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









		// Create vao_quad
		// Save everying in single set
		GLES32.glGenVertexArrays(1, vao_triangle, 0);

		GLES32.glBindVertexArray(vao_triangle[0]);

		// Posistion
		// Generate Buffer
		GLES32.glGenBuffers(1, vbo_position_triangle, 0);
		// Bind Generated Buffer
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_triangle[0]);

		// Fill Buffer
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 3 * 3 * 4, null, GLES32.GL_DYNAMIC_DRAW);

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
		GLES32.glGenBuffers(1, vbo_color_triangle, 0);
		// Bind Generated Buffer
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_triangle[0]);

		// Fill Buffer
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 3 * 3 * 4, null, GLES32.GL_DYNAMIC_DRAW);

		// Set Vertex Attrib Pointer
		GLES32.glVertexAttribPointer(GLESMicros.AMC_ATTRIBUTE_COLOR, 3, GLES32.GL_FLOAT, false, 0, 0);

		// Enable Vertex Attrib Array
		GLES32.glEnableVertexAttribArray(GLESMicros.AMC_ATTRIBUTE_COLOR);
		// Unbind Buffer
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		// Unbind array
		GLES32.glBindVertexArray(0);

	planeFrontSphere.setCenter(0.21f,0.0f);
	planeFrontSphere.setTextureColor(0.729f , 0.886f , 0.933f);//(1.0f*255.0f, 1.0f*255.0f, 1.0f * 255); 
	planeFrontSphere.set(0.07f, 30, 30);


	GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);
		GLES32.glClearDepthf(1.0f);

		GLES32.glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		// GLES32.glEnable(GLES32.GL_TEXTURE_2D);

		Matrix.setIdentityM(perspectiveProjectionMatrix, 0);

	}

	void UnInitialize() {
		if (vbo_position_quad[0] != 0) {
			GLES32.glDeleteBuffers(1, vbo_position_quad, 0);
			vbo_position_quad[0] = 0;
		}
		if (vbo_color_quad[0] != 0) {
			GLES32.glDeleteBuffers(1, vbo_color_quad, 0);
			vbo_color_quad[0] = 0;
		}
		if (vao_quad[0] != 0) {
			GLES32.glDeleteVertexArrays(1, vao_quad, 0);
			vao_quad[0] = 0;
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

	private float triangleVertices[] = {
				0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f
				};
		
	private float triangleColorCoords[] = {
					0.729f,
					0.886f,
					0.933f,
					0.729f,
					0.886f,
					0.933f,
					0.729f,
					0.886f,
					0.933f
				};
		

	private void DrawLetter(char letter) {
	
	ByteBuffer byteBuffer;
	FloatBuffer positionBuffer;
	FloatBuffer colorBuffer;

	switch (letter)
	{
	case 'I':
		GLES32.glBindVertexArray(vao_quad[0]);

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

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_quad[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);


		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_quad[0]);

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

		GLES32.glBindVertexArray(0);

		break;

	case 'N':
		GLES32.glBindVertexArray(vao_quad[0]);

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

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_quad[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_quad[0]);

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

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_quad[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_quad[0]);

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


		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_quad[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_quad[0]);

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

		GLES32.glBindVertexArray(0);

		break;

	case 'D':
		GLES32.glBindVertexArray(vao_quad[0]);

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


		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_quad[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_quad[0]);

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
		
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_quad[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_quad[0]);

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

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_quad[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_quad[0]);

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

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_quad[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_quad[0]);

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
		GLES32.glBindVertexArray(0);

		break;
	case 'A':
		GLES32.glBindVertexArray(vao_quad[0]);
		
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

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_quad[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_quad[0]);

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

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_quad[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_quad[0]);

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
		GLES32.glBindVertexArray(0);

		break;
	case 'T':

		GLES32.glBindVertexArray(vao_quad[0]);

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

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_quad[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_quad[0]);

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

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_quad[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_quad[0]);

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

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_quad[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_quad[0]);

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

		GLES32.glBindVertexArray(0);

		break;




	case 'L':
		GLES32.glBindVertexArray(vao_quad[0]);

		if (Plane1_X_Position < 1.40) {
			//Line Red
			rectangleVertices[0] = Plane1_X_Position - 0.27f;
			rectangleVertices[1] = 0.06f;
			rectangleVertices[2] = 0.0f;
			rectangleVertices[3] = Line_X_Position;
			rectangleVertices[4] = 0.06f;
			rectangleVertices[5] = 0.0f;
			rectangleVertices[6] = Line_X_Position;
			rectangleVertices[7] = 0.02f;
			rectangleVertices[8] = 0.0f;
			rectangleVertices[9] = Plane1_X_Position - 0.27f;
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

			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_quad[0]);

			byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
			byteBuffer.order(ByteOrder.nativeOrder());
			positionBuffer=byteBuffer.asFloatBuffer();
			positionBuffer.put(rectangleVertices);
			positionBuffer.position(0);
	
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);
	
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
	
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_quad[0]);
	
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
			// W
			rectangleVertices[0] = Plane1_X_Position - 0.27f;
			rectangleVertices[1] = 0.02f;
			rectangleVertices[2] = 0.0f;
			rectangleVertices[3] = Line_X_Position;
			rectangleVertices[4] = 0.02f;
			rectangleVertices[5] = 0.0f;
			rectangleVertices[6] = Line_X_Position;
			rectangleVertices[7] = -0.02f;
			rectangleVertices[8] = 0.0f;
			rectangleVertices[9] = Plane1_X_Position - 0.27f;
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

			
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_quad[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_quad[0]);

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

			// G		
			rectangleVertices[0] = Plane1_X_Position - 0.27f;
			rectangleVertices[1] = -0.02f;
			rectangleVertices[2] = 0.0f;
			rectangleVertices[3] = Line_X_Position;
			rectangleVertices[4] = -0.02f;
			rectangleVertices[5] = 0.0f;
			rectangleVertices[6] = Line_X_Position;
			rectangleVertices[7] = -0.06f;
			rectangleVertices[8] = 0.0f;
			rectangleVertices[9] = Plane1_X_Position - 0.27f;
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

			
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_quad[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_quad[0]);

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
		}
		else {

			//R
			rectangleVertices[0] = 1.1f;
			rectangleVertices[1] = 0.06f;
			rectangleVertices[2] = 0.0f;
			rectangleVertices[3] = Line_X_Position;
			rectangleVertices[4] = 0.06f;
			rectangleVertices[5] = 0.0f;
			rectangleVertices[6] = Line_X_Position;
			rectangleVertices[7] = 0.02f;
			rectangleVertices[8] = 0.0f;
			rectangleVertices[9] = 1.1f;
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

			
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_quad[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_quad[0]);

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
			// W
			rectangleVertices[0] = 1.1f;
			rectangleVertices[1] = 0.02f;
			rectangleVertices[2] = 0.0f;
			rectangleVertices[3] = Line_X_Position;
			rectangleVertices[4] = 0.02f;
			rectangleVertices[5] = 0.0f;
			rectangleVertices[6] = Line_X_Position;
			rectangleVertices[7] = -0.02f;
			rectangleVertices[8] = 0.0f;
			rectangleVertices[9] = 1.1f;
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

			
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_quad[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_quad[0]);

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
			// G		
			rectangleVertices[0] = 1.1f;
			rectangleVertices[1] = -0.02f;
			rectangleVertices[2] = 0.0f;
			rectangleVertices[3] = Line_X_Position;
			rectangleVertices[4] = -0.02f;
			rectangleVertices[5] = 0.0f;
			rectangleVertices[6] = Line_X_Position;
			rectangleVertices[7] = -0.06f;
			rectangleVertices[8] = 0.0f;
			rectangleVertices[9] = 1.1f;
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

			
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_quad[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_quad[0]);

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
		}

		GLES32.glBindVertexArray(0);

		break;
	
		//Plane
	case 'P':


	GLES32.glBindVertexArray(vao_quad[0]);

		//Smoke R
		rectangleVertices[0] = -0.24f;
		rectangleVertices[1] = 0.06f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = -0.40f;
		rectangleVertices[4] = 0.06f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = -0.40f;
		rectangleVertices[7] = -0.02f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = -0.24f;
		rectangleVertices[10] = -0.02f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = 1.0f;
		rectangleColorCoords[1] = 0.6f;
		rectangleColorCoords[2] = 0.2f;
		rectangleColorCoords[3] = 0.0f;
		rectangleColorCoords[4] = 0.0f;
		rectangleColorCoords[5] = 0.0f;
		rectangleColorCoords[6] = 0.0f;
		rectangleColorCoords[7] = 0.0f;
		rectangleColorCoords[8] = 0.0f;
		rectangleColorCoords[9] = 1.0f;
		rectangleColorCoords[10] = 0.6f;
		rectangleColorCoords[11] = 0.2f;
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_quad[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_quad[0]);

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

		//Smoke W

		rectangleVertices[0] = -0.24f;
		rectangleVertices[1] = 0.02f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = -0.40f;
		rectangleVertices[4] = 0.02f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = -0.40f;
		rectangleVertices[7] = -0.02f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = -0.24f;
		rectangleVertices[10] = -0.02f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = 1.0f;
		rectangleColorCoords[1] = 1.0f;
		rectangleColorCoords[2] = 1.0f;
		rectangleColorCoords[3] = 0.0f;
		rectangleColorCoords[4] = 0.0f;
		rectangleColorCoords[5] = 0.0f;
		rectangleColorCoords[6] = 0.0f;
		rectangleColorCoords[7] = 0.0f;
		rectangleColorCoords[8] = 0.0f;
		rectangleColorCoords[9] = 1.0f;
		rectangleColorCoords[10] = 1.0f;
		rectangleColorCoords[11] = 1.0f;

				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_quad[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_quad[0]);

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

		//Smoke G

		rectangleVertices[0] = -0.24f;
		rectangleVertices[1] = -0.02f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = -0.40f;
		rectangleVertices[4] = -0.02f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = -0.40f;
		rectangleVertices[7] = -0.06f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = -0.24f;
		rectangleVertices[10] = -0.06f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = 0.074f;
		rectangleColorCoords[1] = 0.533f;
		rectangleColorCoords[2] = 0.2f;
		rectangleColorCoords[3] = 0.0f;
		rectangleColorCoords[4] = 0.0f;
		rectangleColorCoords[5] = 0.0f;
		rectangleColorCoords[6] = 0.0f;
		rectangleColorCoords[7] = 0.0f;
		rectangleColorCoords[8] = 0.0f;
		rectangleColorCoords[9] = 0.074f;
		rectangleColorCoords[10] = 0.533f;
		rectangleColorCoords[11] = 0.2f;

				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_quad[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_quad[0]);

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


		rectangleVertices[0] = 0.20f;
		rectangleVertices[1] = 0.07f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = -0.20f;
		rectangleVertices[4] = 0.07f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = -0.20f;
		rectangleVertices[7] = -0.07f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = 0.20f;
		rectangleVertices[10] = -0.07f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = 0.729f;
		rectangleColorCoords[1] = 0.886f;
		rectangleColorCoords[2] = 0.933f;
		rectangleColorCoords[3] = 0.729f;
		rectangleColorCoords[4] = 0.886f;
		rectangleColorCoords[5] = 0.933f;
		rectangleColorCoords[6] = 0.729f;
		rectangleColorCoords[7] = 0.886f;
		rectangleColorCoords[8] = 0.933f;
		rectangleColorCoords[9] = 0.729f;
		rectangleColorCoords[10] = 0.886f;
		rectangleColorCoords[11] = 0.933f;

				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_quad[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_quad[0]);

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
		//Back Wings

		rectangleVertices[0] = -0.10f;
		rectangleVertices[1] = 0.0f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = -0.27f;
		rectangleVertices[4] = 0.12f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = -0.31f;
		rectangleVertices[7] = 0.12f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = -0.24f;
		rectangleVertices[10] = 0.0f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = 0.729f;
		rectangleColorCoords[1] = 0.886f;
		rectangleColorCoords[2] = 0.933f;
		rectangleColorCoords[3] = 0.729f;
		rectangleColorCoords[4] = 0.886f;
		rectangleColorCoords[5] = 0.933f;
		rectangleColorCoords[6] = 0.729f;
		rectangleColorCoords[7] = 0.886f;
		rectangleColorCoords[8] = 0.933f;
		rectangleColorCoords[9] = 0.729f;
		rectangleColorCoords[10] = 0.886f;
		rectangleColorCoords[11] = 0.933f;

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_quad[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_quad[0]);

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
		rectangleVertices[0] = -0.10f;
		rectangleVertices[1] = 0.0f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = -0.27f;
		rectangleVertices[4] = -0.12f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = -0.31f;
		rectangleVertices[7] = -0.12f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = -0.24f;
		rectangleVertices[10] = 0.0f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = 0.729f;
		rectangleColorCoords[1] = 0.886f;
		rectangleColorCoords[2] = 0.933f;
		rectangleColorCoords[3] = 0.729f;
		rectangleColorCoords[4] = 0.886f;
		rectangleColorCoords[5] = 0.933f;
		rectangleColorCoords[6] = 0.729f;
		rectangleColorCoords[7] = 0.886f;
		rectangleColorCoords[8] = 0.933f;
		rectangleColorCoords[9] = 0.729f;
		rectangleColorCoords[10] = 0.886f;
		rectangleColorCoords[11] = 0.933f;
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_quad[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_quad[0]);

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

		//Letter I
		rectangleVertices[0] = -0.05f;
		rectangleVertices[1] = 0.05f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = -0.07f;
		rectangleVertices[4] = 0.05f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = -0.07f;
		rectangleVertices[7] = -0.05f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = -0.05f;
		rectangleVertices[10] = -0.05f;
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

				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_quad[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_quad[0]);

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
		//Letter A			
		rectangleVertices[0] = 0.00f;
		rectangleVertices[1] = 0.05f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = -0.02f;
		rectangleVertices[4] = 0.05f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = -0.04f;
		rectangleVertices[7] = -0.05f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = -0.02f;
		rectangleVertices[10] = -0.05f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = 0.0f;
		rectangleColorCoords[1] = 0.0f;
		rectangleColorCoords[2] = 0.530f;
		rectangleColorCoords[3] = 0.0f;
		rectangleColorCoords[4] = 0.0f;
		rectangleColorCoords[5] = 0.530f;
		rectangleColorCoords[6] = 0.0f;
		rectangleColorCoords[7] = 0.0f;
		rectangleColorCoords[8] = 0.530f;
		rectangleColorCoords[9] = 0.0f;
		rectangleColorCoords[10] = 0.0f;
		rectangleColorCoords[11] = 0.530f;

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_quad[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_quad[0]);

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

		rectangleVertices[0] = 0.00f;
		rectangleVertices[1] = 0.05f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = 0.02f;
		rectangleVertices[4] = 0.05f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = 0.04f;
		rectangleVertices[7] = -0.05f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = 0.02f;
		rectangleVertices[10] = -0.05f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = 0.0f;
		rectangleColorCoords[1] = 0.0f;
		rectangleColorCoords[2] = 0.530f;
		rectangleColorCoords[3] = 0.0f;
		rectangleColorCoords[4] = 0.0f;
		rectangleColorCoords[5] = 0.530f;
		rectangleColorCoords[6] = 0.0f;
		rectangleColorCoords[7] = 0.0f;
		rectangleColorCoords[8] = 0.530f;
		rectangleColorCoords[9] = 0.0f;
		rectangleColorCoords[10] = 0.0f;
		rectangleColorCoords[11] = 0.530f;

				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_quad[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_quad[0]);

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

		rectangleVertices[0] = 0.02f;
		rectangleVertices[1] = 0.01f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = -0.02f;
		rectangleVertices[4] = 0.01f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = -0.02f;
		rectangleVertices[7] = -0.01f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = 0.02f;
		rectangleVertices[10] = -0.01f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = 0.0f;
		rectangleColorCoords[1] = 0.0f;
		rectangleColorCoords[2] = 0.530f;
		rectangleColorCoords[3] = 0.0f;
		rectangleColorCoords[4] = 0.0f;
		rectangleColorCoords[5] = 0.530f;
		rectangleColorCoords[6] = 0.0f;
		rectangleColorCoords[7] = 0.0f;
		rectangleColorCoords[8] = 0.530f;
		rectangleColorCoords[9] = 0.0f;
		rectangleColorCoords[10] = 0.0f;
		rectangleColorCoords[11] = 0.530f;

				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_quad[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_quad[0]);

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
		//Letter F

		rectangleVertices[0] = 0.06f;
		rectangleVertices[1] = 0.05f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = 0.04f;
		rectangleVertices[4] = 0.05f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = 0.04f;
		rectangleVertices[7] = -0.05f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = 0.06f;
		rectangleVertices[10] = -0.05f;
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

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_quad[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_quad[0]);

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

		rectangleVertices[0] = 0.09f;
		rectangleVertices[1] = 0.05f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = 0.06f;
		rectangleVertices[4] = 0.05f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = 0.06f;
		rectangleVertices[7] = 0.035f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = 0.09f;
		rectangleVertices[10] = 0.035f;
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

				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_quad[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_quad[0]);

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
		rectangleVertices[0] = 0.09f;
		rectangleVertices[1] = 0.02f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = 0.06f;
		rectangleVertices[4] = 0.02f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = 0.06f;
		rectangleVertices[7] = 0.005f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = 0.09f;
		rectangleVertices[10] = 0.005f;
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

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_quad[0]);

		byteBuffer=ByteBuffer.allocateDirect(rectangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(rectangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_quad[0]);

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
		
		GLES32.glBindVertexArray(0);



		GLES32.glBindVertexArray(vao_triangle[0]);

		//Upper Wing
		triangleVertices[0] = -0.05f;
		triangleVertices[1] = 0.07f;
		triangleVertices[2] = 0.0f;
		triangleVertices[3] = 0.05f;
		triangleVertices[4] = 0.07f;
		triangleVertices[5] = 0.0f;
		triangleVertices[6] = -0.15f;
		triangleVertices[7] = 0.22f;
		triangleVertices[8] = 0.0f;

		/*triangleColorCoords[0] = 0.729f;
		triangleColorCoords[1] = 0.886f;
		triangleColorCoords[2] = 0.933f;
		triangleColorCoords[3] = 1.0f;
		triangleColorCoords[4] = 0.6f;
		triangleColorCoords[5] = 0.2f;
		triangleColorCoords[6] = 0.074f;
		triangleColorCoords[7] = 0.533f;
		triangleColorCoords[8] = 0.2f;*/


			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_triangle[0]);

			byteBuffer=ByteBuffer.allocateDirect(triangleVertices.length*4);
			byteBuffer.order(ByteOrder.nativeOrder());
			positionBuffer=byteBuffer.asFloatBuffer();
			positionBuffer.put(triangleVertices);
			positionBuffer.position(0);
	
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 3 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);
	
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
	
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_triangle[0]);
	
			byteBuffer=ByteBuffer.allocateDirect(triangleColorCoords.length*4);
			byteBuffer.order(ByteOrder.nativeOrder());
			colorBuffer=byteBuffer.asFloatBuffer();
			colorBuffer.put(triangleColorCoords);
			colorBuffer.position(0);
	
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 3 * 3 * 4, colorBuffer, GLES32.GL_DYNAMIC_DRAW);
	
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
	
			GLES32.glDrawArrays(GLES32.GL_TRIANGLES,
			0,
			3);

		GLES32.glBindVertexArray(0);


		GLES32.glBindVertexArray(vao_triangle[0]);

		triangleVertices[0] = -0.05f;
		triangleVertices[1] = -0.07f;
		triangleVertices[2] = 0.0f;
		triangleVertices[3] = 0.05f;
		triangleVertices[4] = -0.07f;
		triangleVertices[5] = 0.0f;
		triangleVertices[6] = -0.15f;
		triangleVertices[7] = -0.22f;
		triangleVertices[8] = 0.0f;

		/*triangleColorCoords[0] = 0.729f;
		triangleColorCoords[1] = 0.886f;
		triangleColorCoords[2] = 0.933f;
		triangleColorCoords[3] = 1.0f;
		triangleColorCoords[4] = 0.6f;
		triangleColorCoords[5] = 0.2f;
		triangleColorCoords[6] = 0.074f;
		triangleColorCoords[7] = 0.533f;
		triangleColorCoords[8] = 0.2f;*/


		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_triangle[0]);

		byteBuffer=ByteBuffer.allocateDirect(triangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(triangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 3 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_triangle[0]);

		byteBuffer=ByteBuffer.allocateDirect(triangleColorCoords.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		colorBuffer=byteBuffer.asFloatBuffer();
		colorBuffer.put(triangleColorCoords);
		colorBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 3 * 3 * 4, colorBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glDrawArrays(GLES32.GL_TRIANGLES,
		0,
		3);

	GLES32.glBindVertexArray(0);

		//Lower Wing

		GLES32.glBindVertexArray(vao_triangle[0]);

		triangleVertices[0] = 0.05f;
		triangleVertices[1] = 0.07f;
		triangleVertices[2] = 0.0f;
		triangleVertices[3] = -0.10f;
		triangleVertices[4] = 0.22f;
		triangleVertices[5] = 0.0f;
		triangleVertices[6] = -0.15f;
		triangleVertices[7] = 0.22f;
		triangleVertices[8] = 0.0f;

		/*triangleColorCoords[0] = 0.729f;
		triangleColorCoords[1] = 0.886f;
		triangleColorCoords[2] = 0.933f;
		triangleColorCoords[3] = 1.0f;
		triangleColorCoords[4] = 0.6f;
		triangleColorCoords[5] = 0.2f;
		triangleColorCoords[6] = 0.074f;
		triangleColorCoords[7] = 0.533f;
		triangleColorCoords[8] = 0.2f;*/


		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_triangle[0]);

		byteBuffer=ByteBuffer.allocateDirect(triangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(triangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 3 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_triangle[0]);

		byteBuffer=ByteBuffer.allocateDirect(triangleColorCoords.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		colorBuffer=byteBuffer.asFloatBuffer();
		colorBuffer.put(triangleColorCoords);
		colorBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 3 * 3 * 4, colorBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glDrawArrays(GLES32.GL_TRIANGLES,
		0,
		3);

	GLES32.glBindVertexArray(0);


	GLES32.glBindVertexArray(vao_triangle[0]);

		triangleVertices[0] = 0.05f;
		triangleVertices[1] = -0.07f;
		triangleVertices[2] = 0.0f;
		triangleVertices[3] = -0.10f;
		triangleVertices[4] = -0.22f;
		triangleVertices[5] = 0.0f;
		triangleVertices[6] = -0.15f;
		triangleVertices[7] = -0.22f;
		triangleVertices[8] = 0.0f;

		/*triangleColorCoords[0] = 0.729f;
		triangleColorCoords[1] = 0.886f;
		triangleColorCoords[2] = 0.933f;
		triangleColorCoords[3] = 1.0f;
		triangleColorCoords[4] = 0.6f;
		triangleColorCoords[5] = 0.2f;
		triangleColorCoords[6] = 0.074f;
		triangleColorCoords[7] = 0.533f;
		triangleColorCoords[8] = 0.2f;*/

	
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_triangle[0]);

		byteBuffer=ByteBuffer.allocateDirect(triangleVertices.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		positionBuffer=byteBuffer.asFloatBuffer();
		positionBuffer.put(triangleVertices);
		positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 3 * 3 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_triangle[0]);

		byteBuffer=ByteBuffer.allocateDirect(triangleColorCoords.length*4);
		byteBuffer.order(ByteOrder.nativeOrder());
		colorBuffer=byteBuffer.asFloatBuffer();
		colorBuffer.put(triangleColorCoords);
		colorBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 3 * 3 * 4, colorBuffer, GLES32.GL_DYNAMIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		GLES32.glDrawArrays(GLES32.GL_TRIANGLES,
		0,
		3);

	GLES32.glBindVertexArray(0);

		planeFrontSphere.drawWithColor();

		break;

	}
	
	}

}