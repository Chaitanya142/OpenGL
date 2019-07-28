///////////////////////////////////////////////////////////////////////////////
// SphereOGL.h
// ========
// Sphere for OpenGL with (radius, sectors, stacks)
// The min number of sectors is 3 and The min number of stacks are 2.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef GEOMETRY_SPHERE_H
#define GEOMETRY_SPHERE_H
#include <vector>
class Sphere
{
public:
	// ctor/dtor
	Sphere(float radius = 1.0f, int sectorCount = 36, int stackCount = 18);
	~Sphere() {}
	// getters/setters
	float getRadius() const { return radius; }
	int getSectorCount() const { return sectorCount; }
	int getStackCount() const { return stackCount; }
	void set(float radius, int sectorCount, int stackCount);
	void setRadius(float radius);
	void setSectorCount(int sectorCount);
	void setStackCount(int stackCount);
	// for vertex data
	unsigned int getVertexCount() const { return (unsigned int)vertices.size() / 3; }
	unsigned int getNormalCount() const { return (unsigned int)normals.size() / 3; }
	unsigned int getTexCoordCount() const { return (unsigned int)texCoords.size() / 2; }
	unsigned int getIndexCount() const { return (unsigned int)indices.size(); }
	unsigned int getTriangleCount() const { return getIndexCount() / 3; }
	unsigned int getVertexSize() const { return (unsigned int)vertices.size() * sizeof(float); }
	unsigned int getNormalSize() const { return (unsigned int)normals.size() * sizeof(float); }
	unsigned int getTexCoordSize() const { return (unsigned int)texCoords.size() * sizeof(float); }
	unsigned int getIndexSize() const { return (unsigned int)indices.size() * sizeof(unsigned int); }
	const float* getVertices() const { return vertices.data(); }
	const float* getNormals() const { return normals.data(); }
	const float* getTexCoords() const { return texCoords.data(); }
	const unsigned int* getIndices() const { return indices.data(); }
	void draw() const;
	//void DrawAirport(float latitudes, float longitudes);
	//void DrawRoute(float latitude1, float longitude1, float latitude2, float longitude2);
	//void GetAirportDetails(float latitude, float longitude, float *ax, float *ay, float *az);
protected:
	// member functions
	float radius;
	int sectorCount;                        // longitude, # of slices
	int stackCount;                         // latitude, # of stacks
private:
	std::vector<float> vertices;
	std::vector<float> normals;
	std::vector<float> texCoords;
	std::vector<unsigned int> indices;
	void updateRadius();
	void buildVerticesSmooth();
	void clearArrays();
	void addVertex(float x, float y, float z);
	void addNormal(float x, float y, float z);
	void addTexCoord(float s, float t);
	void addIndices(unsigned int i1, unsigned int i2, unsigned int i3);
};
#endif
