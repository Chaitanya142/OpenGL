///////////////////////////////////////////////////////////////////////////////
// VertexToWindow.cpp
// ==========
///////////////////////////////////////////////////////////////////////////////
#ifdef _WIN32
#include <windows.h>    // include windows.h to avoid thousands of compile errors even though this class is not depending on Windows
#endif
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#include <iostream>
#include <iomanip>
#include <cmath>
#include "VertexToWindow.h"
#define _USE_MATH_DEFINES 1
#include<math.h>
// constants //////////////////////////////////////////////////////////////////
//const int MIN_SECTOR_COUNT = 3;
///////////////////////////////////////////////////////////////////////////////
// ctor
///////////////////////////////////////////////////////////////////////////////
VertexToWindow::VertexToWindow()
{
	makeIdentity4by4(scaleMatrix);
	makeIdentity4by4(translationMatrix);
	makeIdentity4by4(rotationX);
	makeIdentity4by4(rotationY);
	makeIdentity4by4(rotationZ);
	makeIdentity4by4(multiply4by4);
	makeIdentity4by4(temp4by4);
	makeIdentity4by4(viewMatrix);
	makeIdentity4by4(orthoMatrix);
	makeIdentity4by4(perspectiveMatrix);
	makeIdentity4by4(constMatrix);

	zeroMatrix(multiply4by1);
	zeroMatrix(temp4by1);
	zeroMatrix(vertex);
	vertex[3][0] = 1;
}
///////////////////////////////////////////////////////////////////////////////
// setters
///////////////////////////////////////////////////////////////////////////////


float VertexToWindow :: getRadFromDegree(float Degree) {
	return(M_PI*Degree / 180);
}


//To  Multiply 4 by 4 Matrix
void VertexToWindow :: multiplyMatrix4by4(float(*first)[4][4], float(*second)[4][4], float(*multiply)[4][4], int r1, int c1, int r2, int c2) {//(float **first,float **second,float **multiply,int r1,int c1,int r2,int c2) {
	float sum = 0;
	for (int c = 0; c < r1; c++) {
		for (int d = 0; d < c2; d++) {
			for (int k = 0; k < r2; k++) {//c1==r2
				sum = sum + (*first)[c][k] * (*second)[k][d];
			}
			(*multiply)[c][d] = sum;
			sum = 0;
		}
	}
}
//To Create Identity matrix 4 by 4
void VertexToWindow :: makeIdentity4by4(float matrix[][4]) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (i == j) {
				matrix[i][j] = 1;
			}
			else {
				matrix[i][j] = 0;
			}
		}
	}
}
//Copy Matrix 4 by 4
void VertexToWindow :: copyMatrix4by4(float(*source)[4][4], float(*dest)[4][4]) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			(*dest)[i][j] = (*source)[i][j];
		}
	}
}
//To  Multiply 4 by 1 Matrix
void VertexToWindow :: multiplyMatrix4by1(float first[][4], float second[][1], float multiply[][1], int r1, int c1, int r2, int c2) {//(float **first,float **second,float **multiply,int r1,int c1,int r2,int c2) {
	float sum = 0;
	for (int c = 0; c < r1; c++) {
		for (int d = 0; d < c2; d++) {
			for (int k = 0; k < r2; k++) {//c1==r2
				sum = sum + first[c][k] * second[k][d];
			}
			multiply[c][d] = sum;
			sum = 0;
		}
	}
}
//Copy Matrix 4 by 1
void VertexToWindow :: copyMatrix4by1(float source[][1], float dest[][1]) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			dest[i][j] = source[i][j];
		}
	}
}
//Zero Out matrix 4 by 1
void VertexToWindow :: zeroMatrix(float matrix[][1]) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 0; j++) {
			matrix[i][j] = 0;
		}
	}
}
//Cross Product of array
void VertexToWindow :: crossProduct(float vect_A[], float vect_B[], float cross_P[])
{
	cross_P[0] = vect_A[1] * vect_B[2] - vect_A[2] * vect_B[1];
	cross_P[1] = vect_A[2] * vect_B[0] - vect_A[0] * vect_B[2];
	cross_P[2] = vect_A[0] * vect_B[1] - vect_A[1] * vect_B[0];
}
//Normalise Vector values
void VertexToWindow :: normalize(float matrix[])
{
	float w = sqrt(matrix[0] * matrix[0] + matrix[1] * matrix[1] + matrix[2] * matrix[2]);
	matrix[0] /= w;
	matrix[1] /= w;
	matrix[2] /= w;
}
//gluPerspective To GlFrustrum
void VertexToWindow :: perspectiveGL(float fovY, float aspect, float zNear, float zFar, float *fW, float *fH)
{
	*fH = tan(fovY / 360 * M_PI) * zNear;
	*fW = *fH * aspect;
}


void VertexToWindow::setTransactionalMatrix(float tx, float ty, float tz) {
	if (tx != 0) {
		translationMatrix[0][3] = tx;
	}
	if (ty != 0) {
		translationMatrix[1][3] = ty;
	}
	if (tz != 0) {
		translationMatrix[2][3] = tz;
	}
}
void VertexToWindow::setRotationMatrix(float angleX, float angleY, float angleZ) {
	if (angleX != 0)
	{
		rotationX[1][1] = cos(getRadFromDegree(angleX));
		rotationX[1][2] = -sin(getRadFromDegree(angleX));
		rotationX[2][1] = sin(getRadFromDegree(angleX));
		rotationX[2][2] = cos(getRadFromDegree(angleX));
	}
	if (angleY != 0)
	{
		rotationY[0][0] = cos(getRadFromDegree(angleY));
		rotationY[0][3] = sin(getRadFromDegree(angleY));
		rotationY[2][0] = -sin(getRadFromDegree(angleY));
		rotationY[2][2] = cos(getRadFromDegree(angleY));
	}
	if (angleZ != 0)
	{
		rotationZ[0][0] = cos(getRadFromDegree(angleZ));
		rotationZ[0][1] = -sin(getRadFromDegree(angleZ));
		rotationZ[1][0] = sin(getRadFromDegree(angleZ));
		rotationZ[1][1] = cos(getRadFromDegree(angleZ));
	}
}
void VertexToWindow::setScaleMatrix(float sx, float sy, float sz) {
	if (sx != 0) {
		scaleMatrix[0][0] = sx;
	}
	if (sy != 0) {
		scaleMatrix[1][1] = sy;
	}
	if (sz != 0) {
		scaleMatrix[2][2] = sz;
	}
}

//http://www.songho.ca/opengl/gl_camera.html
//https://www.cs.sfu.ca/~haoz/teaching/htmlman/lookat.html
void VertexToWindow::setViewMatrix(float eyex, float eyey, float eyez,
	float targetx, float targety, float targetz,
	float upDirx, float uDiry, float upDirz) 
{
	float eyeCoordV[3];
	float targetCoordV[3];
	float upCoordV[3];
	float forwardV[3];
	float leftV[3];
	float upV[3];
	eyeCoordV[0] = eyex;
	eyeCoordV[1] = eyey;
	eyeCoordV[2] = eyez;
	targetCoordV[0] = targetx;
	targetCoordV[1] = targety;
	targetCoordV[2] = targetz;
	upCoordV[0] = upDirx;
	upCoordV[1] = uDiry;
	upCoordV[2] = upDirz;

	forwardV[0] = eyeCoordV[0] - targetCoordV[0];
	forwardV[1] = eyeCoordV[1] - targetCoordV[1];
	forwardV[2] = eyeCoordV[2] - targetCoordV[2];
	normalize(forwardV);
	crossProduct(upCoordV, forwardV, leftV);
	normalize(leftV);
	crossProduct(forwardV, leftV, upV);

	viewMatrix[0][0] = leftV[0];
	viewMatrix[0][1] = leftV[1];
	viewMatrix[0][2] = leftV[2];
	viewMatrix[0][3] = -leftV[0] * eyeCoordV[0] - leftV[1] * eyeCoordV[1] - leftV[2] * eyeCoordV[2];
	viewMatrix[1][0] = upV[0];
	viewMatrix[1][1] = upV[1];
	viewMatrix[1][2] = upV[2];
	viewMatrix[1][3] = -upV[0] * eyeCoordV[0] - upV[1] * eyeCoordV[1] - upV[2] * eyeCoordV[2];
	viewMatrix[2][0] = forwardV[0];
	viewMatrix[2][1] = forwardV[1];
	viewMatrix[2][2] = forwardV[2];
	viewMatrix[2][3] = -forwardV[0] * eyeCoordV[0] - forwardV[1] * eyeCoordV[1] - forwardV[2] * eyeCoordV[2];

}

//http://www.songho.ca/opengl/gl_projectionmatrix.html
void VertexToWindow::setPerspective(float left,
	float right,
	float bottom,
	float top,
	float nearVal,
	float farVal)//glFrustrum
{
	perspectiveMatrix[0][0] = 2 * nearVal / (right - left);
	perspectiveMatrix[0][2] = (right + left) / (right - left);
	perspectiveMatrix[1][1] = 2 * nearVal / (top - bottom);
	perspectiveMatrix[1][2] = (top + bottom) / (top - bottom);
	perspectiveMatrix[2][2] = -(farVal + nearVal) / (farVal - nearVal);//-1
	perspectiveMatrix[2][3] = -2 * nearVal*farVal / (farVal - nearVal);//-1
	perspectiveMatrix[3][2] = -1;
	perspectiveMatrix[3][3] = 0;
}

//https://stackoverflow.com/questions/12943164/replacement-for-gluperspective-with-glfrustrum
void VertexToWindow::setPerspective(float fovy,
	float aspect,//width/height
	float zNear,
	float zFar) 
{
	float fH = 0, fW = 0;

	perspectiveGL(fovy, aspect, zNear, zFar, &fW, &fH);

	float left = -fW;
	float right = fW;
	float bottom = -fH;
	float top = fH;
	float nearVal = zNear;
	float farVal = zFar;

	perspectiveMatrix[0][0] = 2 * nearVal / (right - left);
	perspectiveMatrix[0][2] = (right + left) / (right - left);
	perspectiveMatrix[1][1] = 2 * nearVal / (top - bottom);
	perspectiveMatrix[1][2] = (top + bottom) / (top - bottom);
	perspectiveMatrix[2][2] = -(farVal + nearVal) / (farVal - nearVal);//-1
	perspectiveMatrix[2][3] = -2 * nearVal*farVal / (farVal - nearVal);//-1
	perspectiveMatrix[3][2] = -1;
	perspectiveMatrix[3][3] = 0;
}


void VertexToWindow::setViewPortMatrix(float x, float y, float width, float height) {
	viewPortV[0] = x;
	viewPortV[1] = y;
	viewPortV[2] = width;
	viewPortV[3] = height;
}

void VertexToWindow::setRCy(float Rcy) {
	RCy = Rcy;
}


void VertexToWindow::calculateMatrix() {
	//Tranlation*RotationX
	multiplyMatrix4by4(&translationMatrix, &rotationX, &multiply4by4, 4, 4, 4, 4);
	copyMatrix4by4(&multiply4by4, &temp4by4);
	// Above * Rotation Y
	multiplyMatrix4by4(&temp4by4, &rotationY, &multiply4by4, 4, 4, 4, 4);
	copyMatrix4by4(&multiply4by4, &temp4by4);
	// Above * Rotation Z
	multiplyMatrix4by4(&temp4by4, &rotationZ, &multiply4by4, 4, 4, 4, 4);
	copyMatrix4by4(&multiply4by4, &temp4by4);
	// Above * Scale
	multiplyMatrix4by4(&temp4by4, &scaleMatrix, &multiply4by4, 4, 4, 4, 4);
	copyMatrix4by4(&multiply4by4, &temp4by4);
	//View*Above
	multiplyMatrix4by4(&viewMatrix, &temp4by4, &multiply4by4, 4, 4, 4, 4);
	copyMatrix4by4(&multiply4by4, &temp4by4);
	//Perspective Matrix*Above ie Perspective*V*M
	multiplyMatrix4by4(&perspectiveMatrix, &temp4by4, &multiply4by4, 4, 4, 4, 4);
	copyMatrix4by4(&multiply4by4, &temp4by4);
	//OrthoMatrix*Above ie Ortho*V*M
	multiplyMatrix4by4(&orthoMatrix, &temp4by4, &multiply4by4, 4, 4, 4, 4);
	copyMatrix4by4(&multiply4by4, &constMatrix);
}

void VertexToWindow::getWindowCoordinates(float vx, float vy, float vz, float *wx, float *wy, float *wz) {

	vertex[0][0] = vx;
	vertex[1][0] = vy;
	vertex[2][0] = vz;
	vertex[3][0] = 1;

	zeroMatrix(temp4by1);
	zeroMatrix(multiply4by1);
	multiplyMatrix4by1(constMatrix, vertex, multiply4by1, 4, 4, 4, 1);
	copyMatrix4by1(multiply4by1, temp4by1);

	//Calculate Normalized Coordinates
	float xn = temp4by1[0][0] / temp4by1[3][0];//divide by w
	float yn = temp4by1[1][0] / temp4by1[3][0];//divide by w
	float zn = temp4by1[2][0] / temp4by1[3][0];//divide by w

	windowCoordinate[0] = (xn * 0.5 + 0.5) * viewPortV[2] + viewPortV[0];
	windowCoordinate[1] = (yn * 0.5 + 0.5) * viewPortV[3] + viewPortV[1];
	windowCoordinate[2] = (1.0 + zn) * 0.5;	// Convert to 0.0 to 1.0 range. Anything outside that range gets clipped.
	windowCoordinate[1] = viewPortV[3] - windowCoordinate[1] - (viewPortV[3] - RCy);//-1;// -GetSystemMetrics(SM_CYCAPTION);//Because of origin difference in Open GL And Windows // int height1 = (GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION) +GetSystemMetrics(SM_CXPADDEDBORDER));

	*wx = windowCoordinate[0];
	*wy = windowCoordinate[1];
	*wz = windowCoordinate[2];

	//float viewPortMatrix[4][4];
	//float normalizedMatrix[4][1];
	//normalizedMatrix[0][0] = xn;
	//normalizedMatrix[1][0] = yn;
	//normalizedMatrix[2][0] = zn;
	//normalizedMatrix[3][0] = 1;
	//makeIdentity4by4(viewPortMatrix);
	//viewPortMatrix[0][0] = viewPortV[2] / 2;
	//viewPortMatrix[0][3] = (viewPortV[2]-1) / 2;
	//viewPortMatrix[1][1] = viewPortV[3] / 2;
	//viewPortMatrix[1][3] = (viewPortV[3]-1) / 2;
	//viewPortMatrix[2][2] = 0.5;
	//viewPortMatrix[2][3] = 0.5;
	//float windowMatix[4][1];
	//multiplyMatrix4by1(viewPortMatrix,normalizedMatrix,windowMatix,4,4,4,1);
	//windowCoordinate[0] = windowMatix[0][0];
	//windowCoordinate[1] = windowMatix[1][0];
	//// Convert to 0.0 to 1.0 range. Anything outside that range gets clipped.
	//windowCoordinate[2] = windowMatix[2][0];
	//windowCoordinate[1] = ViewPortV[3] - windowCoordinate[1];// -31;//opengl and window origin
}