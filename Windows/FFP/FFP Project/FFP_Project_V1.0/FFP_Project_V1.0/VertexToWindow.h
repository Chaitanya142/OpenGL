///////////////////////////////////////////////////////////////////////////////
// Sphere.h
// ========
// Sphere for OpenGL with (radius, sectors, stacks)
// The min number of sectors is 3 and The min number of stacks are 2.
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2017-11-01
// UPDATED: 2018-12-13
///////////////////////////////////////////////////////////////////////////////
#ifndef VERTEXT_WINDOWS_H
#define VERTEXT_WINDOWS_H
#include <vector>
class VertexToWindow
{
public:
	// ctor/dtor
	VertexToWindow(); //float radius = 1.0f, int sectorCount = 36, int stackCount = 18, bool smooth = true
	~VertexToWindow() {}
	// getters/setters
	void setTransactionalMatrix(float tx = 0,float ty = 0,float tz = 0);
	void setRotationMatrix(float angleX = 0, float angleY = 0, float angleZ = 0);
	void setScaleMatrix(float sx = 0, float sy = 0, float sz = 0);
	void setViewMatrix(float eyex,float eyey,float eyez,
		float targetx,float targety,float targetz,
		float upDirx=0,float uDiry=1,float upDirz=0);
	void setPerspective(float left,
		float right,
		float bottom,
		float top,
		float nearVal,
		float farVal);//glFrustrum
	void setPerspective(float fovy,
		float aspect,
		float zNear,
		float zFar);
	void setViewPortMatrix(float x = 0, float y = 0, float width = 0,float height=1);
	void setRCy(float Rcy = 0);
	void calculateMatrix();
	void getWindowCoordinates(float vx, float vy, float vz, float *wx, float *wy, float *wz);


protected:
private:
	//Member Functions
	void makeIdentity4by4(float matrix[][4]);
	void copyMatrix4by4(float(*source)[4][4], float(*dest)[4][4]);
	void multiplyMatrix4by4(float(*first)[4][4], float(*second)[4][4], float(*multiply)[4][4], int r1, int c1, int r2, int c2);
	void zeroMatrix(float matrix[][1]);
	void multiplyMatrix4by1(float first[][4], float second[][1], float multiply[][1], int r1, int c1, int r2, int c2);
	void copyMatrix4by1(float source[][1], float dest[][1]);
	void perspectiveGL(float fovY, float aspect, float zNear, float zFar, float *fW, float *fH);
	void normalize(float matrix[]);
	void crossProduct(float vect_A[], float vect_B[], float cross_P[]);
	float getRadFromDegree(float);


	//Member Variables
	float scaleMatrix[4][4];
	float translationMatrix[4][4];
	float rotationX[4][4];
	float rotationY[4][4];
	float rotationZ[4][4];

	float vertex[4][1];
	float windowCoordinate[3];

	float multiply4by4[4][4];
	float temp4by4[4][4];
	float multiply4by1[4][1];
	float temp4by1[4][1];

	float viewMatrix[4][4];

	float perspectiveMatrix[4][4];
	float orthoMatrix[4][4];

	float viewPortV[4];
	float constMatrix[4][4];

	float RCy=0;
};
#endif