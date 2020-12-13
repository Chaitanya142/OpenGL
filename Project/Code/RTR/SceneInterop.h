#include"Common.h"
/*OpenCL Includes*/

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#include<CL/cl.hpp>
#include<CL/opencl.h>
#include<CL/cl_gl.h>
#include<CL/cl_gl_ext.h>

#pragma comment (lib, "OpenCL.lib")

class SceneInterop : public IScene
{
private:

	unsigned int gMeshWidth = 128;
	unsigned int gMeshHeight = 128;
	cl_float gAnim = 0.0f;
	size_t localWorkSize = 256;
	size_t globalWorkSize;

	/*OpenCL variables*/
	// global OpenCL variables
	cl_int ret_ocl;
	cl_platform_id oclPlatformID;
	cl_device_id oclComputeDeviceID; // compute device id
	cl_context oclContext; // compute context
	cl_command_queue oclCommandQueue; // compute command queue
	cl_program oclProgram; // compute program
	cl_kernel oclKernel; // compute kernel

	char *oclSourceCode = NULL;
	size_t sizeKernelCodeLength;
	bool first = true;
	/*GPU buffer*/
	cl_mem dVbo;
	cl_float4 *pPos;
	cl_float4 *Position;
	GLuint vao_interop;
	GLuint hVbo;
	GLfloat Color[3];
	GLfloat Alpha;
	char* loadOclProgramSource(const char *filename, const char *preamble, size_t *sizeFinalLength);

public:
	// Inherited via IScene
	virtual int InitScene() override;

	virtual void DisplayScene() override;

	virtual void UpdateScene() override;

	virtual void UninitializeScene() override;

};
