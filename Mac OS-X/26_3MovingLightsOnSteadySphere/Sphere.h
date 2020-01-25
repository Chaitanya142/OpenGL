#ifndef GEOMETRY_SPHERE_H
#define GEOMETRY_SPHERE_H

#include <vector>

class Sphere
{
public:
    // ctor/dtor
    Sphere();
    ~Sphere() {}
	
	GLuint vao_sphere;
	GLuint vbo_sphere;
    
	// getters/setters
    float getRadius() const                 { return radius; }
    int getSectorCount() const              { return sectorCount; }
    int getStackCount() const               { return stackCount; }
    void set(float radius, int sectorCount, int stackCount, bool smooth=true);
    void setRadius(float radius);
    void setSectorCount(int sectorCount);
    void setStackCount(int stackCount);
    void setSmooth(bool smooth);

    // for vertex data
    unsigned int getVertexCount() const     { return (unsigned int)vertices.size() / 3; }
    unsigned int getNormalCount() const     { return (unsigned int)normals.size() / 3; }
    unsigned int getTexCoordCount() const   { return (unsigned int)texCoords.size() / 2; }
    unsigned int getIndexCount() const      { return (unsigned int)indices.size(); }
    unsigned int getLineIndexCount() const  { return (unsigned int)lineIndices.size(); }
    unsigned int getTriangleCount() const   { return getIndexCount() / 3; }
    unsigned int getVertexSize() const      { return (unsigned int)vertices.size() * sizeof(float); }
    unsigned int getNormalSize() const      { return (unsigned int)normals.size() * sizeof(float); }
    unsigned int getTexCoordSize() const    { return (unsigned int)texCoords.size() * sizeof(float); }
    unsigned int getIndexSize() const       { return (unsigned int)indices.size() * sizeof(unsigned int); }
    unsigned int getLineIndexSize() const   { return (unsigned int)lineIndices.size() * sizeof(unsigned int); }
    const float* getVertices() const        { return vertices.data(); }
    const float* getNormals() const         { return normals.data(); }
    const float* getTexCoords() const       { return texCoords.data(); }
    const unsigned int* getIndices() const  { return indices.data(); }
    const unsigned int* getLineIndices() const  { return lineIndices.data(); }

    // for interleaved vertices: V/N/T
    unsigned int getInterleavedVertexCount() const  { return getVertexCount(); }    // # of vertices
    unsigned int getInterleavedVertexSize() const   { return (unsigned int)interleavedVertices.size() * sizeof(float); }    // # of bytes
    int getInterleavedStride() const                { return interleavedStride; }   // should be 32 bytes
    const float* getInterleavedVertices() const     { return interleavedVertices.data(); }

    // draw in VertexArray mode
    void draw() const;                                  // draw surface
	void drawLines() const;     // draw lines only

	GLuint *txtImage;
	bool extTexture=false;
	void setTextureColor(float R,float G, float B);
	void setColor(float R, float G, float B);

	void setCenter(float cx, float cy);
	void drawWithColor() const;

    std::vector<float> interleavedVertices;

    std::vector<unsigned int> indices;

    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> texCoords;
    std::vector<float> colorCoords;
    
    
protected:

private:
    // member functions
    void updateRadius();
    void buildVerticesSmooth();
    void buildVerticesFlat();
    void buildInterleavedVertices();
    void clearArrays();
    void addVertex(float x, float y, float z);
    void addNormal(float x, float y, float z);
    void addTexCoord(float s, float t);
    void addIndices(unsigned int i1, unsigned int i2, unsigned int i3);
    void addColor(float r, float g, float b);
    std::vector<float> computeFaceNormal(float x1, float y1, float z1,
                                         float x2, float y2, float z2,
                                         float x3, float y3, float z3);

    // memeber vars
    float radius;
    int sectorCount=90;                        // longitude, # of slices
    int stackCount=90;                         // latitude, # of stacks
    bool smooth;

    

    std::vector<unsigned int> lineIndices;

    // interleaved

    int interleavedStride=44;                  // # of bytes to hop to the next vertex (should be 32 bytes)
	void createVAO();
	enum {
		AMC_ATTRIBUTE_POSITION = 0,
		AMC_ATTRIBUTE_COLOR,
		AMC_ATTRIBUTE_NORMAL,
		AMC_ATTRIBUTE_TEXCOORD0
	};

	void LoadTexture();
	void CreateTexture();
	//GLubyte ***sphereImage;//GLubyte checkImage[checkImageWidth][checkImageHeight][4];

	float R = 255;
	float G = 255;
	float B = 255;

	GLubyte sphereImage[1][1][4];

	float cx = 0;
	float cy = 0;
};

#endif
