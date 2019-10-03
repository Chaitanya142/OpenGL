package com.example.ogl_robotic_arm;

import java.util.*;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.IntBuffer;

import android.opengl.Matrix;

import android.opengl.GLES32;

import android.graphics.BitmapFactory;
import android.graphics.Bitmap;
import android.opengl.GLUtils;
import java.lang.Math.*;


public class Sphere{

    public static final Integer MIN_SECTOR_COUNT = 3;
    public static final Integer MIN_STACK_COUNT = 2;
	private int[] vao_sphere=new int[1];
	private int[] vbo_sphere=new int[1];
	private int[] txtImage = new int[1];
        // memeber vars
        Float radius;
        Integer sectorCount;                        // longitude, # of slices
        Integer stackCount;                         // latitude, # of stacks
        Boolean smooth;
        List<Float> vertices;
        List<Float> normals;
        List<Float> texCoords;
        List<Float> indices;
        List<Float> lineIndices;
    
        // interleaved
        List<Float> interleavedVertices;
        Integer interleavedStride;                  // # of bytes to hop to the next vertex (should be 32 bytes)
        //GLubyte ***sphereImage;//GLubyte checkImage[checkImageWidth][checkImageHeight][4];
    
        float R = 255;
        float G = 255;
		float B = 255;
		Boolean extTexture=false;

		public GLESView glesview;

        //GLubyte sphereImage[1][1][4];

    public Sphere() 
    {

	//set(radius, sectors, stacks, smooth);
    }

    public void set(Float radius, Integer sectors, Integer stacks)
    {
        vertices=new ArrayList();
        normals=new ArrayList();
        texCoords=new ArrayList();
        indices=new ArrayList();
        lineIndices=new ArrayList();
    	interleavedVertices=new ArrayList();

        this.radius = radius;
        this.sectorCount = sectors;
        if (sectors < MIN_SECTOR_COUNT)
            this.sectorCount = MIN_SECTOR_COUNT;
        this.stackCount = stacks;
        if (sectors < MIN_STACK_COUNT)
            this.sectorCount = MIN_STACK_COUNT;

        buildVerticesSmooth();
        
    }

    // public void setRadius(Float radius)
    // {
	//     this.radius = radius;
	//     updateRadius();
    // }

    public void setSectorCount(Integer sectors)
    {
	    set(radius, sectors, stackCount);
    }

    public void setStackCount(Integer stacks)
    {
	    set(radius, sectorCount, stacks);
    }



    public void draw()
    {

	GLES32.glActiveTexture(GLES32.GL_TEXTURE0);

	GLES32.glBindVertexArray(vao_sphere[0]);

	GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, txtImage[0]);

	//GLES32.glDrawElements(GLES32.GL_TRIANGLES, indices.size(), GLES32.GL_UNSIGNED_BYTE, indicesbuffer);
//changed

	GLES32.glDrawElements(GLES32.GL_TRIANGLES, indicesbuffer.capacity(),  GLES32.GL_UNSIGNED_INT, indicesbuffer);
	
	GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, 0);

	GLES32.glBindVertexArray(0);


	// glActiveTexture(GL_TEXTURE0);

	// glBindVertexArray(vao_sphere);

	// glBindTexture(GL_TEXTURE_2D, *txtImage);

	// glDrawElements(GL_TRIANGLES, (unsigned Integer)indices.size(), GL_UNSIGNED_INT, indices.data());

	// glBindTexture(GL_TEXTURE_2D, 0);

	// glBindVertexArray(0);

    }

    public void drawLines()
    {

		GLES32.glActiveTexture(GLES32.GL_TEXTURE0);

		GLES32.glBindVertexArray(vao_sphere[0]);
	
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, txtImage[0]);
	
		GLES32.glDrawElements(GLES32.GL_LINES, indicesbuffer.capacity(),  GLES32.GL_UNSIGNED_INT, indicesbuffer);
		
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, 0);
	
		GLES32.glBindVertexArray(0);
	
	// glActiveTexture(GL_TEXTURE0);

	// glBindVertexArray(vao_sphere);

	// //Bind texture
	// glBindTexture(GL_TEXTURE_2D, *txtImage);

	// glDrawElements(GL_LINES, (unsigned Integer)lineIndices.size(), GL_UNSIGNED_INT, lineIndices.data());

	// //Unbind vao

	// glBindTexture(GL_TEXTURE_2D, 0);

	// glBindVertexArray(0);

    }


    void clearArrays()
    {
        vertices.clear();
        normals.clear();
        texCoords.clear();
        indices.clear();
        lineIndices.clear();
    }
    
    void buildVerticesSmooth()
{
	Float PI = 3.1415926f;

	// clear memory of prev arrays
	clearArrays();

	Float x, y, z, xy;                              // vertex position
	Float nx, ny, nz, lengthInv = 1.0f / radius;    // normal
	Float s, t;                                     // texCoord

	Float sectorStep = 2 * PI / sectorCount;
	Float stackStep = PI / stackCount;
	Float sectorAngle, stackAngle;

	for (Integer i = 0; i <= stackCount; ++i)
	{
		stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
		xy = radius * (float)java.lang.Math.cos((double)stackAngle);             // r * (float)java.lang.Math.cos((double)u)
		z = radius * (float)java.lang.Math.sin((double)stackAngle);              // r * (float)java.lang.Math.sin((double)u)

		// add (sectorCount+1) vertices per stack
		// the first and last vertices have same position and normal, but different tex coords
		for (Integer j = 0; j <= sectorCount; ++j)
		{
			sectorAngle = j * sectorStep;           // starting from 0 to 2pi

			// vertex position
			x = xy * (float)java.lang.Math.cos((double)sectorAngle);             // r * (float)java.lang.Math.cos((double)u) * (float)java.lang.Math.cos((double)v)
			y = xy * (float)java.lang.Math.sin((double)sectorAngle);             // r * (float)java.lang.Math.cos((double)u) * (float)java.lang.Math.sin((double)v)
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
    Integer k1, k2;
	for (Integer i = 0; i < stackCount; ++i)
	{
		k1 = i * (sectorCount + 1);     // beginning of current stack
		k2 = k1 + sectorCount + 1;      // beginning of next stack

		for (Integer j = 0; j < sectorCount; ++j, ++k1, ++k2)
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

			// vertical lines for all stacks
			lineIndices.add((float)k1);
			lineIndices.add((float)k2);
			if (i != 0)  // horizontal lines except 1st stack
			{
				lineIndices.add((float)k1);
				lineIndices.add((float)k1 + 1);
			}
		}
	}

	// generate interleaved vertex array as well
	buildInterleavedVertices();
}


void buildInterleavedVertices()
{

	interleavedVertices.clear();

	Integer i,j;
	Integer count = vertices.size();
	for (i = 0, j = 0; i < count; i += 3, j += 2)
	{
		interleavedVertices.add(vertices.get(i));
		interleavedVertices.add(vertices.get(i + 1));
		interleavedVertices.add(vertices.get(i + 2));

		interleavedVertices.add(normals.get(i));
		interleavedVertices.add(normals.get(i + 1));
		interleavedVertices.add(normals.get(i + 2));

		interleavedVertices.add(texCoords.get(j));
		interleavedVertices.add(texCoords.get(j + 1));
	}
	createVAO();
}

//FloatBuffer indicesbuffer;
IntBuffer indicesbuffer;


void createVAO() {


	GLES32.glGenVertexArrays(1,vao_sphere,0);

	GLES32.glBindVertexArray(vao_sphere[0]);

	//Posistion
	//Generate Buffer
	GLES32.glGenBuffers(1,vbo_sphere,0);
	//Bind Generated Buffer
	GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,
	vbo_sphere[0]);

	ByteBuffer byteBuffer=ByteBuffer.allocateDirect(interleavedVertices.size()*4);

	byteBuffer.order(ByteOrder.nativeOrder());

	FloatBuffer buffer=byteBuffer.asFloatBuffer();

	float[] floatArray = new float[interleavedVertices.size()];
	int i = 0;
	
	for (Float f : interleavedVertices) {
		floatArray[i++] = (f != null ? f : Float.NaN); // Or whatever default you want.
	}
	//interleavedVertices.toArray(elements);

	buffer.put(floatArray);

	buffer.position(0);

	//Fill Buffer
	GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
		floatArray.length*4,
		buffer,
		GLES32.GL_STATIC_DRAW);

	//Set Vertex Attrib Pointer
	GLES32.glVertexAttribPointer(GLESMicros.AMC_ATTRIBUTE_POSITION,
		3,
		GLES32.GL_FLOAT,
		false,
		8 * 4,
		(0 * 4));

	//Enable Vertex Attrib Array
	GLES32.glEnableVertexAttribArray(GLESMicros.AMC_ATTRIBUTE_POSITION);

	//Set Vertex Attrib Pointer
	GLES32.glVertexAttribPointer(GLESMicros.AMC_ATTRIBUTE_NORMAL,
		3,
		GLES32.GL_FLOAT,
		false,
		8 * 4,
		(3 * 4));
	//Enable Vertex Attrib Array
	GLES32.glEnableVertexAttribArray(GLESMicros.AMC_ATTRIBUTE_NORMAL);

	//Set Vertex Attrib Pointer
	GLES32.glVertexAttribPointer(GLESMicros.AMC_ATTRIBUTE_TEXCOORD0,
		2,
		GLES32.GL_FLOAT,
		false,
		8 * 4,
		(6 * 4));
	//Enable Vertex Attrib Array
	GLES32.glEnableVertexAttribArray(GLESMicros.AMC_ATTRIBUTE_TEXCOORD0);

	//Unbind Buffer
	GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
	//Unbind array
	GLES32.glBindVertexArray(0);


///////////////////////////////////////////////////////////////////////////////

	// ByteBuffer byteBufferInd=ByteBuffer.allocateDirect(indices.size()*4);

	// byteBufferInd.order(ByteOrder.nativeOrder());

	// indicesbuffer=byteBufferInd.asFloatBuffer();

	// float[] indicesArray = new float[indices.size()];
	// i = 0;
	
	// for (Float f : indices) {
	// 	indicesArray[i++] = (f != null ? f : Float.NaN); // Or whatever default you want.
	// }

	// indicesbuffer.put(indicesArray);

	// indicesbuffer.position(0);

////////////////////////////////////////////////////////////////////////////////////////

	ByteBuffer byteBufferInd=ByteBuffer.allocateDirect(indices.size()*4);

	byteBufferInd.order(ByteOrder.nativeOrder());

	indicesbuffer=byteBufferInd.asIntBuffer();

	int[] indicesArray = new int[indices.size()];
	i = 0;
	
	for (float f : indices) {
		indicesArray[i++] = (int)f; // Or whatever default you want.
	}

	indicesbuffer.put(indicesArray);

	indicesbuffer.position(0);


	if (extTexture == false) {
		txtImage[0] = LoadTexture();
		System.out.println("RTR : "+txtImage[0]);
	}else{
		txtImage=glesview.texture;
	}
}

///////////////////////////////////////////////////////////////////////////////
// add single vertex to array
///////////////////////////////////////////////////////////////////////////////
void addVertex(Float x, Float y, Float z)
{
	vertices.add(x);
	vertices.add(y);
	vertices.add(z);
}


///////////////////////////////////////////////////////////////////////////////
// add single normal to array
///////////////////////////////////////////////////////////////////////////////
void addNormal(Float nx, Float ny, Float nz)
{
	normals.add(nx);
	normals.add(ny);
	normals.add(nz);
}

///////////////////////////////////////////////////////////////////////////////
// add single texture coord to array
///////////////////////////////////////////////////////////////////////////////
void addTexCoord(Float s, Float t)
{
	texCoords.add(s);
	texCoords.add(t);
}

///////////////////////////////////////////////////////////////////////////////
// add 3 indices to array
///////////////////////////////////////////////////////////////////////////////
void addIndices(Integer i1,Integer i2,Integer i3)
{
	indices.add((float)i1);
	indices.add((float)i2);
	indices.add((float)i3);
}

///////////////////////////////////////////////////////////////////////////////
// return face normal of a triangle v1-v2-v3
// if a triangle has no surface (normal length = 0), then return a zero vector
///////////////////////////////////////////////////////////////////////////////
List<Float> computeFaceNormal(Float x1, Float y1, Float z1,  // v1
	Float x2, Float y2, Float z2,  // v2
	Float x3, Float y3, Float z3)  // v3
{
	Float EPSILON = 0.000001f;

    Float[] nos = new Float[3];
    Arrays.fill(nos, 0.0);
    List<Float> normal = Arrays.asList(nos);
	//List<Float> normal=new List<Float>(3, 0.0f);     // default return value (0,0,0)
	Float nx, ny, nz;

	// find 2 edge vectors: v1-v2, v1-v3
	Float ex1 = x2 - x1;
	Float ey1 = y2 - y1;
	Float ez1 = z2 - z1;
	Float ex2 = x3 - x1;
	Float ey2 = y3 - y1;
	Float ez2 = z3 - z1;

	// cross product: e1 x e2
	nx = ey1 * ez2 - ez1 * ey2;
	ny = ez1 * ex2 - ex1 * ez2;
	nz = ex1 * ey2 - ey1 * ex2;

	// normalize only if the length is > 0
	Float length = (float)java.lang.Math.sqrt((double)(nx * nx + ny * ny + nz * nz));
	if (length > EPSILON)
	{
		// normalize
		Float lengthInv = 1.0f / length;
        normal.set(0, nx * lengthInv);
		normal.set(1, ny * lengthInv);
		normal.set(2, nz * lengthInv);        
	}

	return normal;
}

	private int LoadTexture() 
	{	
		int[] texture = new int[1];
		
		CreateTexture();
		int Width=1;
		int Height=1;
		ByteBuffer bytebuffer = ByteBuffer.allocateDirect(Width * Height * 4);
		bytebuffer.order(ByteOrder.nativeOrder());
		bytebuffer.put(sphereImage);
		bytebuffer.position(0);
		
		Bitmap bitmap = Bitmap.createBitmap(Width, Height, Bitmap.Config.ARGB_8888);
		bitmap.copyPixelsFromBuffer(bytebuffer);
		GLES32.glGenTextures(1, texture, 0);
	
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, texture[0]);
		
		GLES32.glPixelStorei(GLES32.GL_UNPACK_ALIGNMENT, 1);
		
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_WRAP_S, GLES32.GL_REPEAT);
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_WRAP_T, GLES32.GL_REPEAT);
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MAG_FILTER, GLES32.GL_NEAREST);
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MIN_FILTER, GLES32.GL_NEAREST);
		
		GLUtils.texImage2D(GLES32.GL_TEXTURE_2D, 0, bitmap, 0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, 0);
		return texture[0];
	}

	byte sphereImage[]=new byte[4];
	private void CreateTexture()
	{
		sphereImage[0]=(byte)this.R;
		sphereImage[1]=(byte)this.G;	
		sphereImage[2]=(byte)this.B;	
		sphereImage[3]=(byte)255;	
		System.out.println("RTR : s0"+sphereImage[0]);
		System.out.println("RTR : s1"+sphereImage[1]);
		System.out.println("RTR : s2"+sphereImage[2]);
		System.out.println("RTR : s3"+sphereImage[3]);
	}


// void LoadTexture() {
// 	CreateTexture();
// 	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
// 	glGenTextures(1, txtImage);
// 	glBindTexture(GL_TEXTURE_2D, *txtImage);

// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

// 	glTexImage2D(GL_TEXTURE_2D,
// 		0,
// 		GL_RGBA,
// 		1,//stack count
// 		1,//sector count
// 		0,
// 		GL_RGBA,
// 		GL_UNSIGNED_BYTE,
// 		sphereImage);

// 	glBindTexture(GL_TEXTURE_2D, 0);

// }

// void CreateTexture() {
// 	sphereImage[0][0][0] = (GLubyte)this.R;
// 	sphereImage[0][0][1] = (GLubyte)this.G;
// 	sphereImage[0][0][2] = (GLubyte)this.B;
// 	sphereImage[0][0][3] = (GLubyte)255;

// 	//Integer r, c, h;
// 	//Integer height = 1;
// 	//Integer rows = 1;
// 	//Integer cols = 4;
// 	//sphereImage = (GLubyte ***)malloc(sizeof(GLubyte ***)*height);
// 	//for (h = 0; h < height; h++) {
// 	//	sphereImage[h] = (GLubyte **)malloc(sizeof(GLubyte*)*rows);
// 	//	for (r = 0; r < rows; r++) {
// 	//		sphereImage[h][r] = (GLubyte *)malloc(sizeof(GLubyte)*cols);
// 	//	}
// 	//}
// 	//for (h = 0; h < height; h++) {
// 	//	for (r = 0; r < rows; r++) {
// 	//		/*for (c = 0; c < cols; c++) {
// 	//			printf("Enter Array Element [%d][%d][%d] : ", h, r, c);
// 	//			scanf("%d", &array[h][r][c]);
// 	//		}*/
// 	//			sphereImage[h][r][0] = (GLubyte)this->R;
// 	//			sphereImage[h][r][1] = (GLubyte)this->B;
// 	//			sphereImage[h][r][2] = (GLubyte)this->G;
// 	//			sphereImage[h][r][3] = 255;
// 	//	}
// 	//}
	
// }

public void setTextureColor(float R, float G, float B) {
	this.R = R;
	this.G = G;
	this.B = B;
}


}
