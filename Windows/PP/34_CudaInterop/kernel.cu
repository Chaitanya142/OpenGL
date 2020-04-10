#include<stdio.h>

__global__ void simple_vbo_kernel(float4 *pos, unsigned int width, unsigned int height, float time)
{

unsigned int x=blockIdx.x*blockDim.x+threadIdx.x;
unsigned int y=blockIdx.y*blockDim.y+threadIdx.y;

float u=x/(float)width;
float v=y/(float)height;

u=u*2.0f-1.0f;
v=v*2.0f-1.0f;

float frequency=4.0f;

float w=sinf(u*frequency+time)*cosf(v*frequency+time)*0.5f;

pos[y*width+x]=make_float4(u,w,v,1.0);
}

extern FILE* gpFile;
void LaunchCUDAKernel(float4 *pos, unsigned int meshWidth, unsigned int meshHeight, float time)
{
	fprintf(gpFile, "\nInside GPU %f", time);
	dim3 block(8,8,1);

	dim3 grid(meshWidth/block.x, meshHeight/block.y,1);
	simple_vbo_kernel<<<grid,block>>>(pos,meshWidth,meshHeight,time);

}

