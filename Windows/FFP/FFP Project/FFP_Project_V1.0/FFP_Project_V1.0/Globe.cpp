///////////////////////////////////////////////////////////////////////////////
// SphereOGL.cpp
// ==========
// Sphere for OpenGL with (radius, sectors, stacks)
// The min number of sectors is 3 and the min number of stacks are 2.
///////////////////////////////////////////////////////////////////////////////
#ifdef _WIN32
#include <windows.h>   
#endif
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#include <iostream>
#include <iomanip>
#include <cmath>
#include "Globe.h"
// constants //////////////////////////////////////////////////////////////////
const int MIN_SECTOR_COUNT = 3;
const int MIN_STACK_COUNT = 2;
///////////////////////////////////////////////////////////////////////////////
// ctor
///////////////////////////////////////////////////////////////////////////////
Globe::Globe(float radius, int sectors, int stacks)
{
	set(radius, sectors, stacks);
	longFactor = 360 / sectors;
	latFactor = 180 / stacks;
}
///////////////////////////////////////////////////////////////////////////////
// setters
///////////////////////////////////////////////////////////////////////////////
void Globe::Set(float radius, int sectors, int stacks)
{
	set(radius, sectors, stacks);
	longFactor = 360 / sectors;
	latFactor = 180 / stacks;
}
///////////////////////////////////////////////////////////////////////////////
// draw a sphere in VertexArray mode
///////////////////////////////////////////////////////////////////////////////
void Globe::DrawGlobe() const
{
	draw();
}
void Globe::DrawAirport(float latitude, float longitude)
{
	latitude = (90 + latitude)/latFactor;
	longitude = (180 + longitude)/longFactor;
	glDisable(GL_TEXTURE_2D);
	const float PI = 3.1415926f;
	float x, y, z, xy;                              // vertex position
	float nx, ny, nz, lengthInv = 1.0f / (radius + 0.005);    // normal
	float s, t;                                     // texCoord
	float sectorStep = 2 * PI / sectorCount;
	float stackStep = PI / stackCount;
	float sectorAngle, stackAngle;
	glPointSize(1.0f);
	glBegin(GL_POINTS);
	//glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 0.0f);
	//glVertex3f(0.0f, 0.0f, 0.0f);
	float i = latitude;//stack count
	stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
	xy = (radius + 0.005) * cosf(stackAngle);             // r * cos(u)
	z = (radius + 0.005) * sinf(stackAngle);              // r * sin(u)
																							   // add (sectorCount+1) vertices per stack
																							   // the first and last vertices have same position and normal, but different tex coords
	float j = longitude;
	sectorAngle = j * sectorStep;           // starting from 0 to 2pi
																		// vertex position
	x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
	y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
	glVertex3f(x, y, z);
	// normalized vertex normal
	nx = x * lengthInv;
	ny = y * lengthInv;
	nz = z * lengthInv;
	glNormal3f(nx, ny, nz);
	// vertex tex coord between [0, 1]
	s = (float)j / sectorCount;
	t = (float)i / stackCount;
	glTexCoord2f(s, t);
	glEnd();
	glEnable(GL_TEXTURE_2D);
}

void Globe::DrawAirport(float latitude, float longitude,float *vx,float *vy,float *vz)
{
	latitude = (90 + latitude) / latFactor;
	longitude = (180 + longitude) / longFactor;
	glDisable(GL_TEXTURE_2D);
	const float PI = 3.1415926f;
	float x, y, z, xy;                              // vertex position
	float nx, ny, nz, lengthInv = 1.0f / (radius + 0.005);    // normal
	float s, t;                                     // texCoord
	float sectorStep = 2 * PI / sectorCount;
	float stackStep = PI / stackCount;
	float sectorAngle, stackAngle;
	glPointSize(2.0f);
	glBegin(GL_POINTS);
	//glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 0.0f);
	//glVertex3f(0.0f, 0.0f, 0.0f);
	float i = latitude;//stack count
	stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
	xy = (radius + 0.005) * cosf(stackAngle);             // r * cos(u)
	z = (radius + 0.005) * sinf(stackAngle);              // r * sin(u)
																							   // add (sectorCount+1) vertices per stack
																							   // the first and last vertices have same position and normal, but different tex coords
	float j = longitude;
	sectorAngle = j * sectorStep;           // starting from 0 to 2pi
																		// vertex position
	x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
	y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
	glVertex3f(x, y, z);
	// normalized vertex normal
	nx = x * lengthInv;
	ny = y * lengthInv;
	nz = z * lengthInv;
	glNormal3f(nx, ny, nz);
	// vertex tex coord between [0, 1]
	s = (float)j / sectorCount;
	t = (float)i / stackCount;
	glTexCoord2f(s, t);
	glEnd();
	glEnable(GL_TEXTURE_2D);

	*vx = x;
	*vy = y;
	*vz = z;
}
void Globe::DrawRoute(float latitude1, float longitude1, float latitude2, float longitude2)
{
	latitude1 = (90 + latitude1)/latFactor;//Consider from Center + -
	longitude1 = (180 + longitude1)/longFactor;//Cosider from center + -
	latitude2 = (90 + latitude2)/latFactor;
	longitude2 = (180 + longitude2)/longFactor;
	glDisable(GL_TEXTURE_2D);
	const float PI = 3.1415926f;
	float x, y, z, xy;                              // vertex position
	float nx, ny, nz, lengthInv = 1.0f / (radius + 0.005);    // normal
	float s, t;                                     // texCoord
	float sectorStep = 2 * PI / sectorCount;
	float stackStep = PI / stackCount;
	float sectorAngle, stackAngle;
	////glBegin(GL_POINTS);
	//glBegin(GL_LINES);
	//glColor3f(1.0f, 0.0f, 0.0f);
	////glVertex3f(0.0f, 0.0f, 0.0f);
	//float i = latitude1;//stack count
	//stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
	//xy = (radius + 0.005) * cosf(stackAngle);             // r * cos(u)
	//z = (radius + 0.005) * sinf(stackAngle);              // r * sin(u)
	//                                                                                         // add (sectorCount+1) vertices per stack
	//                                                                                         // the first and last vertices have same position and normal, but different tex coords
	//float j = longitude1;
	//sectorAngle = j * sectorStep;           // starting from 0 to 2pi
	//                                                                   // vertex position
	//x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
	//y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
	//glVertex3f(x, y, z);
	//// normalized vertex normal
	//nx = x * lengthInv;
	//ny = y * lengthInv;
	//nz = z * lengthInv;
	////glNormal3f(nx, ny, nz);
	//// vertex tex coord between [0, 1]
	//s = (float)j / sectorCount;
	//t = (float)i / stackCount;
	////glTexCoord2f(s, t);
	//i = latitude2;//stack count
	//stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
	//xy = (radius + 0.005) * cosf(stackAngle);             // r * cos(u)
	//z = (radius + 0.005) * sinf(stackAngle);              // r * sin(u)
	//                                                                                         // add (sectorCount+1) vertices per stack
	//                                                                                         // the first and last vertices have same position and normal, but different tex coords
	//j = longitude2;
	//sectorAngle = j * sectorStep;           // starting from 0 to 2pi
	//                                                                   // vertex position
	//x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
	//y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
	//glVertex3f(x, y, z);
	//// normalized vertex normal
	//nx = x * lengthInv;
	//ny = y * lengthInv;
	//nz = z * lengthInv;
	////glNormal3f(nx, ny, nz);
	//// vertex tex coord between [0, 1]
	//s = (float)j / sectorCount;
	//t = (float)i / stackCount;
	////glTexCoord2f(s, t);
	//glEnd();
	float deltaLat = (latitude2 - latitude1) / 1000;
	float deltaLong = (longitude2 - longitude1) / 1000;
	int count = 0;
	while (count < 1000) {
		glBegin(GL_LINES);
		glColor3f(1.0f, 0.0f, 0.0f);
		//glVertex3f(0.0f, 0.0f, 0.0f);
		float i = latitude1 + deltaLat * count;//stack count
		stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
		xy = (radius + 0.005) * cosf(stackAngle);             // r * cos(u)
		z = (radius + 0.005) * sinf(stackAngle);              // r * sin(u)
																								   // add (sectorCount+1) vertices per stack																							   // the first and last vertices have same position and normal, but different tex coords
		float j = longitude1 + deltaLong * count;
		sectorAngle = j * sectorStep;           // starting from 0 to 2pi
																			// vertex position
		x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
		y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
		glVertex3f(x, y, z);
		// normalized vertex normal
		nx = x * lengthInv;
		ny = y * lengthInv;
		nz = z * lengthInv;
		//glNormal3f(nx, ny, nz);
		// vertex tex coord between [0, 1]
		s = (float)j / sectorCount;
		t = (float)i / stackCount;
		//glTexCoord2f(s, t);
		
		i = latitude1 + deltaLat * (count + 1);//stack count
		stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
		xy = (radius + 0.005) * cosf(stackAngle);             // r * cos(u)
		z = (radius + 0.005) * sinf(stackAngle);              // r * sin(u)
		// add (sectorCount+1) vertices per stack
		// the first and last vertices have same position and normal, but different tex coords
		j = longitude1 + deltaLong * (count + 1);
		sectorAngle = j * sectorStep;           // starting from 0 to 2pi
		// vertex position
		x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
		y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
		glVertex3f(x, y, z);
		// normalized vertex normal
		nx = x * lengthInv;
		ny = y * lengthInv;
		nz = z * lengthInv;
		//glNormal3f(nx, ny, nz);
		// vertex tex coord between [0, 1]
		s = (float)j / sectorCount;
		t = (float)i / stackCount;
		//glTexCoord2f(s, t);
		glEnd();
		count = count + 1;
	}
	glEnable(GL_TEXTURE_2D);
}
void Globe::GetAirportDetails(float latitude, float longitude, float *Ax, float *Ay, float *Az)
{
	latitude = (90 + latitude) / latFactor;
	longitude = (180 + longitude) / longFactor;
	const float PI = 3.1415926f;
	float x, y, z, xy;
	float nx, ny, nz, lengthInv = 1.0f / (radius + 0.005);
	float s, t;
	float sectorStep = 2 * PI / sectorCount;
	float stackStep = PI / stackCount;
	float sectorAngle, stackAngle;
	float i = latitude;
	stackAngle = PI / 2 - i * stackStep;
	xy = (radius + 0.005) * cosf(stackAngle);
	z = (radius + 0.005) * sinf(stackAngle);
	float j = longitude;
	sectorAngle = j * sectorStep;
	x = xy * cosf(sectorAngle);
	y = xy * sinf(sectorAngle);
	nx = x * lengthInv;
	ny = y * lengthInv;
	nz = z * lengthInv;
	s = (float)j / sectorCount;
	t = (float)i / stackCount;
	*Ax = x;
	*Ay = y;
	*Az = z;
}
