///////////////////////////////////////////////////////////////////////////////
// Sphere.cpp
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
#include "SphereOGL.h"
// constants //////////////////////////////////////////////////////////////////
const int MIN_SECTOR_COUNT = 3;
const int MIN_STACK_COUNT = 2;
///////////////////////////////////////////////////////////////////////////////
// ctor
///////////////////////////////////////////////////////////////////////////////
Sphere::Sphere(float radius, int sectors, int stacks)
{
	set(radius, sectors, stacks);
}
///////////////////////////////////////////////////////////////////////////////
// setters
///////////////////////////////////////////////////////////////////////////////
void Sphere::set(float radius, int sectors, int stacks)
{
	this->radius = radius;
	this->sectorCount = sectors;
	if (sectors < MIN_SECTOR_COUNT)
		this->sectorCount = MIN_SECTOR_COUNT;
	this->stackCount = stacks;
	if (sectors < MIN_STACK_COUNT)
		this->sectorCount = MIN_STACK_COUNT;
	buildVerticesSmooth();
}
void Sphere::setRadius(float radius)
{
	this->radius = radius;
	updateRadius();
}
void Sphere::setSectorCount(int sectors)
{
	set(radius, sectors, stackCount);
}
void Sphere::setStackCount(int stacks)
{
	set(radius, sectorCount, stacks);
}
///////////////////////////////////////////////////////////////////////////////
// draw a sphere in VertexArray mode
///////////////////////////////////////////////////////////////////////////////
void Sphere::draw() const
{
	GLint currentIndex = 0;
	glBegin(GL_TRIANGLE_STRIP);
	for (GLint i = 0; i < getIndexCount(); i++) {
		currentIndex = indices[i];
		glTexCoord2f(texCoords[2 * currentIndex], texCoords[(2 * currentIndex) + 1]);
		glNormal3f(normals[(3 * currentIndex)], normals[(3 * currentIndex) + 1], normals[(3 * currentIndex) + 2]);
		glVertex3f(vertices[(3 * currentIndex)], vertices[(3 * currentIndex) + 1], vertices[(3 * currentIndex) + 2]);
	}
	glEnd();
}
void Sphere::updateRadius()
{
	float scale = sqrtf(radius * radius / (vertices[0] * vertices[0] + vertices[1] * vertices[1] + vertices[2] * vertices[2]));
	std::size_t i, j;
	std::size_t count = vertices.size();
	for (i = 0, j = 0; i < count; i += 3, j += 8)
	{
		vertices[i] *= scale;
		vertices[i + 1] *= scale;
		vertices[i + 2] *= scale;
	}
}
///////////////////////////////////////////////////////////////////////////////
// dealloc vectors
///////////////////////////////////////////////////////////////////////////////
void Sphere::clearArrays()
{
	std::vector<float>().swap(vertices);
	std::vector<float>().swap(normals);
	std::vector<float>().swap(texCoords);
	std::vector<unsigned int>().swap(indices);
	//std::vector<unsigned int>().swap(lineIndices);
}
void Sphere::buildVerticesSmooth()
{
	const float PI = 3.1415926f;
	// clear memory of prev arrays
	clearArrays();
	float x, y, z, xy;                              // vertex position
	float nx, ny, nz, lengthInv = 1.0f / radius;    // normal
	float s, t;                                     // texCoord
	float sectorStep = 2 * PI / sectorCount;
	float stackStep = PI / stackCount;
	float sectorAngle, stackAngle;
	for (int i = 0; i <= stackCount; ++i)
	{
		stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
		xy = radius * cosf(stackAngle);             // r * cos(u)
		z = radius * sinf(stackAngle);              // r * sin(u)
	// add (sectorCount+1) vertices per stack
	// the first and last vertices have same position and normal, but different tex coords
		for (int j = 0; j <= sectorCount; ++j)
		{
			sectorAngle = j * sectorStep;           // starting from 0 to 2pi
													// vertex position
			x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
			y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
			addVertex(x, y, z);
			// normalized vertex normal
			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;
			addNormal(nx, ny, nz);
			// vertex tex coord between [0, 1]
			s = (float)j / sectorCount;
			t = (float)i / stackCount;
			addTexCoord(s, t);
		}
	}
	// indices
	//  k1--k1+1
	//  |  / |
	//  | /  |
	//  k2--k2+1
	unsigned int k1, k2;
	for (int i = 0; i < stackCount; ++i)
	{
		k1 = i * (sectorCount + 1);     // beginning of current stack
		k2 = k1 + sectorCount + 1;      // beginning of next stack
		for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
		{
			// 2 triangles per sector excluding 1st and last stacks
			if (i != 0)
			{
				addIndices(k1, k2, k1 + 1);   // k1---k2---k1+1
			}
			if (i != (stackCount - 1))
			{
				addIndices(k1 + 1, k2, k2 + 1); // k1+1---k2---k2+1
			}
		}
	}
}
///////////////////////////////////////////////////////////////////////////////
// add single vertex to array
///////////////////////////////////////////////////////////////////////////////
void Sphere::addVertex(float x, float y, float z)
{
	vertices.push_back(x);
	vertices.push_back(y);
	vertices.push_back(z);
}
///////////////////////////////////////////////////////////////////////////////
// add single normal to array
///////////////////////////////////////////////////////////////////////////////
void Sphere::addNormal(float nx, float ny, float nz)
{
	normals.push_back(nx);
	normals.push_back(ny);
	normals.push_back(nz);
}
///////////////////////////////////////////////////////////////////////////////
// add single texture coord to array
///////////////////////////////////////////////////////////////////////////////
void Sphere::addTexCoord(float s, float t)
{
	texCoords.push_back(s);
	texCoords.push_back(t);
}
///////////////////////////////////////////////////////////////////////////////
// add 3 indices to array
///////////////////////////////////////////////////////////////////////////////
void Sphere::addIndices(unsigned int i1, unsigned int i2, unsigned int i3)
{
	indices.push_back(i1);
	indices.push_back(i2);
	indices.push_back(i3);
}
//
//void Sphere::DrawAirport(float latitude, float longitude)
//{
//
//            latitude = 90 + latitude;
//            longitude = 180 + longitude;
//            glDisable(GL_TEXTURE_2D);
//            const float PI = 3.1415926f;
//            float x, y, z, xy;                              // vertex position
//            float nx, ny, nz, lengthInv = 1.0f / (radius + 0.005);    // normal
//            float s, t;                                     // texCoord
//            float sectorStep = 2 * PI / sectorCount;
//            float stackStep = PI / stackCount;
//            float sectorAngle, stackAngle;
//            glPointSize(1.0f);
//            glBegin(GL_POINTS);
//            //glBegin(GL_LINES);
//            glColor3f(1.0f, 1.0f, 0.0f);
//            //glVertex3f(0.0f, 0.0f, 0.0f);
//            float i = latitude;//stack count
//            stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
//            xy = (radius + 0.005) * cosf(stackAngle);             // r * cos(u)
//            z = (radius + 0.015) * sinf(stackAngle);              // r * sin(u)
//                                                                                                                                                                                                                              // add (sectorCount+1) vertices per stack
//                                                                                                                                                                                                                              // the first and last vertices have same position and normal, but different tex coords
//            float j = longitude;
//            sectorAngle = j * sectorStep;           // starting from 0 to 2pi
//                                                                                                                                                                            // vertex position
//            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
//            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
//            glVertex3f(x, y, z);
//            // normalized vertex normal
//            nx = x * lengthInv;
//            ny = y * lengthInv;
//            nz = z * lengthInv;
//            glNormal3f(nx, ny, nz);
//            // vertex tex coord between [0, 1]
//            s = (float)j / sectorCount;
//            t = (float)i / stackCount;
//            glTexCoord2f(s, t);
//            glEnd();
//            glEnable(GL_TEXTURE_2D);
//}
//
//void Sphere::DrawRoute(float latitude1, float longitude1, float latitude2, float longitude2)
//{
//            latitude1 = 90 + latitude1;//Consider from Center + -
//            longitude1 = 180 + longitude1;//Cosider from center + -
//            latitude2 = 90 + latitude2;
//            longitude2 = 180 + longitude2;
//            glDisable(GL_TEXTURE_2D);
//            const float PI = 3.1415926f;
//            float x, y, z, xy;                              // vertex position
//            float nx, ny, nz, lengthInv = 1.0f / (radius + 0.005);    // normal
//            float s, t;                                     // texCoord
//            float sectorStep = 2 * PI / sectorCount;
//            float stackStep = PI / stackCount;
//            float sectorAngle, stackAngle;
//            //glBegin(GL_POINTS);
//            glBegin(GL_LINES);
//            glColor3f(1.0f, 0.0f, 0.0f);
//            //glVertex3f(0.0f, 0.0f, 0.0f);
//            float i = latitude1;//stack count
//            stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
//            xy = (radius + 0.005) * cosf(stackAngle);             // r * cos(u)
//            z = (radius + 0.015) * sinf(stackAngle);              // r * sin(u)
//                                                                                                                                                                                                                              // add (sectorCount+1) vertices per stack
//                                                                                                                                                                                                                              // the first and last vertices have same position and normal, but different tex coords
//            float j = longitude1;
//            sectorAngle = j * sectorStep;           // starting from 0 to 2pi
//                                                                                                                                                                            // vertex position
//            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
//            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
//            glVertex3f(x, y, z);
//            // normalized vertex normal
//            nx = x * lengthInv;
//            ny = y * lengthInv;
//            nz = z * lengthInv;
//            //glNormal3f(nx, ny, nz);
//            // vertex tex coord between [0, 1]
//            s = (float)j / sectorCount;
//            t = (float)i / stackCount;
//            //glTexCoord2f(s, t);
//            i = latitude2;//stack count
//            stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
//            xy = (radius + 0.005) * cosf(stackAngle);             // r * cos(u)
//            z = (radius + 0.015) * sinf(stackAngle);              // r * sin(u)
//                                                                                                                                                                                                                              // add (sectorCount+1) vertices per stack
//                                                                                                                                                                                                                              // the first and last vertices have same position and normal, but different tex coords
//            j = longitude2;
//            sectorAngle = j * sectorStep;           // starting from 0 to 2pi
//                                                                                                                                                                            // vertex position
//            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
//            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
//            glVertex3f(x, y, z);
//            // normalized vertex normal
//            nx = x * lengthInv;
//            ny = y * lengthInv;
//            nz = z * lengthInv;
//            //glNormal3f(nx, ny, nz);
//            // vertex tex coord between [0, 1]
//            s = (float)j / sectorCount;
//            t = (float)i / stackCount;
//            //glTexCoord2f(s, t);
//            glEnd();
//            glEnable(GL_TEXTURE_2D);
//}
//void Sphere::GetAirportDetails(float latitude, float longitude, float *Ax, float *Ay, float *Az)
//{
//            //0.015 0.005 Offset
//            latitude = 90 + latitude;
//            longitude = 180 + longitude;
//
//            const float PI = 3.1415926f;
//            float x, y, z, xy;                              
//            float nx, ny, nz, lengthInv = 1.0f / (radius + 0.005);  
//            float s, t;                                   
//            float sectorStep = 2 * PI / sectorCount;
//            float stackStep = PI / stackCount;
//            float sectorAngle, stackAngle;
//
//            float i = latitude;
//            stackAngle = PI / 2 - i * stackStep;      
//            xy = (radius + 0.005) * cosf(stackAngle);           
//            z = (radius + 0.015) * sinf(stackAngle);             
//                                                                                                                                                                                                                             
//            float j = longitude;
//            sectorAngle = j * sectorStep;          
//                                                                                                                                                                           
//            x = xy * cosf(sectorAngle);         
//            y = xy * sinf(sectorAngle);          
//                                                                                                                                                                           
//                                                                                                                                                                           
//            nx = x * lengthInv;
//            ny = y * lengthInv;
//            nz = z * lengthInv;
//
//            s = (float)j / sectorCount;
//            t = (float)i / stackCount;
//
//            *Ax = x;
//            *Ay = y;
//            *Az = z;
//}
