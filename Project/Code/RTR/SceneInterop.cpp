#include"SceneInterop.h"



int SceneInterop::InitScene()
{
	//char* loadOclProgramSource(const char *, const char *, size_t *);
	const GLchar* InteropvertexShaderSourceCode =
	{
		"#version 430 core"\
		"\n"\
		"in vec4 vPosition;"\
		"uniform mat4 model;"\
		"uniform mat4 view;"\
		"uniform mat4 projection;"\
		"void main(void)"\
		"{"\
				"gl_Position = projection * view * model * vPosition;"\
		"}"
	};

	const GLchar* InteropfragmentShaderSourceCode =
	{
		"#version 430 core"\
		"\n"\
		"uniform vec4 color;"\
		"out vec4 FragColor;"\
		"void main(void)"\
		"{"\
			"FragColor = color;"\
		"}"
	};


	Shader objShader_Interop(true, InteropvertexShaderSourceCode, InteropfragmentShaderSourceCode);

	shaders.push_back(objShader_Interop);

	cl_uint platformCount;
	cl_platform_id* platforms;

	clGetPlatformIDs(0, NULL, &platformCount);
	platforms = (cl_platform_id*)malloc(sizeof(cl_platform_id) * platformCount);
	clGetPlatformIDs(platformCount, platforms, NULL);


	/*#################### OPENCL CODE #############*/
	// get OpenCL supporting platform's ID
	ret_ocl = clGetPlatformIDs(1, &oclPlatformID, NULL);
	if (ret_ocl != CL_SUCCESS)
	{
		fprintf_s(gpFile, "OpenCL Error - clGetDeviceIDs() Failed : %d. Exitting Now ...\n", ret_ocl);
		UninitializeScene();
		//exit(EXIT_FAILURE);
	}

	// get OpenCL supporting GPU device's ID

	ret_ocl = clGetDeviceIDs(oclPlatformID, CL_DEVICE_TYPE_GPU, 1, &oclComputeDeviceID, NULL);

	//To Select High END GPU IN HYBRID
	//0 Intel
	//1 AMD/NVIDIA
	//ret_ocl = clGetDeviceIDs(platforms[platformCount - 1], CL_DEVICE_TYPE_GPU, 1, &oclComputeDeviceID, NULL);


	if (ret_ocl != CL_SUCCESS)
	{
		fprintf_s(gpFile, "OpenCL Error - clGetDeviceIDs() Failed : %d. Exitting Now ...\n", ret_ocl);
		UninitializeScene();
		//exit(EXIT_FAILURE);
	}

	char gpu_name[255];
	clGetDeviceInfo(oclComputeDeviceID, CL_DEVICE_NAME, sizeof(gpu_name), &gpu_name, NULL);
	fprintf_s(gpFile, "GPU - %s\n", gpu_name);

	cl_context_properties props[] = {
		CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
		CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
		CL_CONTEXT_PLATFORM, (cl_context_properties)oclPlatformID,
		//CL_CONTEXT_PLATFORM, (cl_context_properties)platforms[platformCount - 1],
		0 };

	// create OpenCL compute context
	oclContext = clCreateContext(props, 1, &oclComputeDeviceID, NULL, NULL, &ret_ocl);
	if (ret_ocl != CL_SUCCESS)
	{
		fprintf_s(gpFile, "OpenCL Error - clCreateContext() Failed : %d. Exitting Now ...\n", ret_ocl);
		UninitializeScene();
		return 0;
		//exit(EXIT_FAILURE);
	}

	// create command queue
	oclCommandQueue = clCreateCommandQueue(oclContext, oclComputeDeviceID, 0, &ret_ocl);
	if (ret_ocl != CL_SUCCESS)
	{
		fprintf_s(gpFile, "OpenCL Error - clCreateCommandQueue() Failed : %d. Exitting Now ...\n", ret_ocl);
		UninitializeScene();
		return 0;
		//exit(EXIT_FAILURE);
	}

	/*OpenCL Program*/
	// create OpenCL program from .cl
	oclSourceCode = loadOclProgramSource("kernel.cl", "", &sizeKernelCodeLength);

	cl_int status = 0;
	oclProgram = clCreateProgramWithSource(oclContext, 1, (const char **)&oclSourceCode, &sizeKernelCodeLength, &ret_ocl);
	if (ret_ocl != CL_SUCCESS)
	{
		fprintf_s(gpFile, "OpenCL Error - clCreateProgramWithSource() Failed : %d. Exitting Now ...\n", ret_ocl);
		UninitializeScene();
		//exit(0);
	}

	// build OpenCL program
	ret_ocl = clBuildProgram(oclProgram, 0, NULL, NULL, NULL, NULL);
	if (ret_ocl != CL_SUCCESS)
	{
		fprintf_s(gpFile, "OpenCL Error - clBuildProgram() Failed : %d. Exitting Now ...\n", ret_ocl);

		size_t len;
		char buffer[2048];
		clGetProgramBuildInfo(oclProgram, oclComputeDeviceID, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
		fprintf_s(gpFile, "OpenCL Program Build Log : %s\n", buffer);

		UninitializeScene();
		//exit(EXIT_FAILURE);
	}

	// create OpenCL kernel by passing kernel function name that we used in .cl file
	oclKernel = clCreateKernel(oclProgram, "simple_vbo_kernel", &ret_ocl);
	if (ret_ocl != CL_SUCCESS)
	{
		fprintf_s(gpFile, "OpenCL Error - clCreateKernel() Failed : %d. Exitting Now ...\n", ret_ocl);
		UninitializeScene();
		//exit(EXIT_FAILURE);
	}

	/*Create buffer*/
	glGenVertexArrays(1, &vao_interop);
	glBindVertexArray(vao_interop);
	glGenBuffers(1, &hVbo);
	glBindBuffer(GL_ARRAY_BUFFER, hVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * gMeshWidth * gMeshHeight * 4, NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	/*Unbinding vertex array*/
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	Position = (cl_float4*)malloc(sizeof(cl_float4) * gMeshWidth * gMeshHeight * 4);
	if (Position == NULL)
	{
		fprintf_s(gpFile, "failed to allocate memory to Position\n");
		UninitializeScene();
		//exit(EXIT_FAILURE);
	}

	//Position = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	for (int i = 0; i < int(gMeshWidth * gMeshHeight); i++)
	{
		Position[i].x = 0.0f;
		Position[i].y = 0.0f;
		Position[i].z = 0.0f;
		Position[i].w = 1.0f;
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);
	/*Unbinding the buffer*/
	glBindVertexArray(0);

	/*Create buffer on GPU*/
	dVbo = clCreateFromGLBuffer(oclContext, CL_MEM_READ_WRITE, hVbo, &ret_ocl);
	if (ret_ocl != CL_SUCCESS)
	{
		fprintf_s(gpFile, "OpenCL Error - Failed to create buffer : %d. Exitting Now ...\n", ret_ocl);
		UninitializeScene();
		//exit(EXIT_FAILURE);
	}

	// set OpenCL kernel arguments. Our OpenCL kernel has 4 arguments 0,1,2,3
	// set 0 based 0th argument i.e. float4 Pos - cl_mem
	ret_ocl = clSetKernelArg(oclKernel, 0, sizeof(cl_mem), (void *)&dVbo);
	if (ret_ocl != CL_SUCCESS)
	{
		fprintf_s(gpFile, "OpenCL Error - clSetKernelArg() Failed For 1st Argument : %d. Exitting Now ...\n", ret_ocl);
		UninitializeScene();
		//exit(EXIT_FAILURE);
	}
	// set 0 based 1st argument i.e. width
	ret_ocl = clSetKernelArg(oclKernel, 1, sizeof(cl_uint), (void *)&gMeshWidth); // 'deviceInput2' maps to 'in2' param of kernel function in .cl file
	if (ret_ocl != CL_SUCCESS)
	{
		fprintf_s(gpFile, "OpenCL Error - clSetKernelArg() Failed For 2nd Argument : %d. Exitting Now ...\n", ret_ocl);
		UninitializeScene();
		//exit(EXIT_FAILURE);
	}

	// set 0 based 2nd argument i.e. height
	ret_ocl = clSetKernelArg(oclKernel, 2, sizeof(cl_uint), (void *)&gMeshHeight); // 'deviceOutput' maps to 'out' param of kernel function in .cl file
	if (ret_ocl != CL_SUCCESS)
	{
		printf("OpenCL Error - clSetKernelArg() Failed For 3rd Argument : %d. Exitting Now ...\n", ret_ocl);
		UninitializeScene();
		//exit(EXIT_FAILURE);
	}

	// set 0 based 3rd argument i.e. time
	ret_ocl = clSetKernelArg(oclKernel, 3, sizeof(cl_float), (void *)&gAnim); // 'iNumberOfArrayElements' maps to 'len' param of kernel function in .cl file
	if (ret_ocl != CL_SUCCESS)
	{
		printf("OpenCL Error - clSetKernelArg() Failed For 4th Argument : %d. Exitting Now ...\n", ret_ocl);
		UninitializeScene();
		//exit(EXIT_FAILURE);
	}

	// finish OpenCL command queue
	clFinish(oclCommandQueue);
	Color[0] = 1.0f;
	Color[1] = 0.0f;
	Color[2] = 0.0f;
	//Color[3] = 1.0f;
	Alpha = 1.0f;
	return 1;
}

char * SceneInterop::loadOclProgramSource(const char * filename, const char * preamble, size_t * sizeFinalLength)
{
	// locals
	FILE *pFile = NULL;
	size_t sizeSourceLength;

	pFile = fopen(filename, "rb"); // binary read
	if (pFile == NULL)
		return(NULL);

	size_t sizePreambleLength = (size_t)strlen(preamble);

	// get the length of the source code
	fseek(pFile, 0, SEEK_END);
	sizeSourceLength = ftell(pFile);
	fseek(pFile, 0, SEEK_SET); // reset to beginning

	// allocate a buffer for the source code string and read it in
	char *sourceString = (char *)malloc(sizeSourceLength + sizePreambleLength + 1);
	memcpy(sourceString, preamble, sizePreambleLength);
	if (fread((sourceString)+sizePreambleLength, sizeSourceLength, 1, pFile) != 1)
	{
		fclose(pFile);
		free(sourceString);
		return(0);
	}

	// close the file and return the total length of the combined (preamble + source) string
	fclose(pFile);
	if (sizeFinalLength != 0)
	{
		*sizeFinalLength = sizeSourceLength + sizePreambleLength;
	}
	sourceString[sizeSourceLength + sizePreambleLength] = '\0';

	return(sourceString);
}

void SceneInterop::DisplayScene()
{
	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	shaders[Shader_Interop].use();

	projection = perspective(camera.Zoom, (float)gWidth / (float)gHeight, 0.1f, 100.0f);//radians
	view = camera.GetViewMatrix();
	shaders[Shader_Interop].setMat4("projection", projection);
	shaders[Shader_Interop].setMat4("view", view);

	MakeModelMatricesIdentity();

	translationMatrix = translate(0.0f, 0.0f, 1.0f);
	modelMatrix = translationMatrix * rotationMatrix;
	modelMatrix = modelMatrix * scaleMatrix;

	shaders[Shader_Interop].setMat4("model", modelMatrix);

	shaders[Shader_Interop].setVec4("color", 1.0f, 0.0f, 0.0f, Alpha);

	ret_ocl = clSetKernelArg(oclKernel, 3, sizeof(cl_float), (void *)&gAnim); // 'iNumberOfArrayElements' maps to 'len' param of kernel function in .cl file
	if (ret_ocl != CL_SUCCESS)
	{
		printf("OpenCL Error - clSetKernelArg() Failed For 4th Argument : %d. Exitting Now ...\n", ret_ocl);
		UninitializeScene();
		//exit(EXIT_FAILURE);
	}

	ret_ocl = clEnqueueAcquireGLObjects(oclCommandQueue, 1, &dVbo, 0, NULL, NULL);
	if (ret_ocl != CL_SUCCESS)
	{
		printf("OpenCL Error - clEnqueueAcquireGLObjects() Failed : %d. Exitting Now ...\n", ret_ocl);
		UninitializeScene();
		//exit(EXIT_FAILURE);
	}
	// run the kernel
	//globalWorkSize = roundGlobalSizeToNearestMultipleOfLocalSize(localWorkSize, 1024 * 1024);
	size_t global_item_size[2] = { gMeshWidth, gMeshHeight };
	size_t local_item_size[2] = { 8,8 };

	ret_ocl = clEnqueueNDRangeKernel(oclCommandQueue, oclKernel, 2, NULL, global_item_size, local_item_size, 0, NULL, NULL);
	if (ret_ocl != CL_SUCCESS)
	{
		printf("OpenCL Error - clEnqueueNDRangeKernel() Failed : %d. Exitting Now ...\n", ret_ocl);
		UninitializeScene();
		//exit(EXIT_FAILURE);
	}

	clFinish(oclCommandQueue);
	ret_ocl = clEnqueueReleaseGLObjects(oclCommandQueue, 1, &dVbo, 0, NULL, NULL);
	if (ret_ocl != CL_SUCCESS)
	{
		printf("OpenCL Error - clEnqueueReleaseGLObjects() Failed : %d. Exitting Now ...\n", ret_ocl);
		UninitializeScene();
		//exit(EXIT_FAILURE);
	}

	glPointSize(2.0f);
	glBindVertexArray(vao_interop);
	glBindBuffer(GL_ARRAY_BUFFER, hVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * gMeshWidth * gMeshHeight * 4, pPos, GL_DYNAMIC_DRAW);
	glDrawArrays(GL_POINTS, 0, gMeshWidth * gMeshHeight);
	/*Unbind vao of rectangle*/
	glBindVertexArray(0);

	glDisable(GL_BLEND);

}


void SceneInterop::UpdateScene()
{
	gAnim += 0.005f;

	Alpha = Alpha - 0.0005f;
	if (Alpha <= 0.0f)
		Alpha = 0.0f;

	if (first)
	{
		camera.ResetCamera();
		first = false;
	}
}

void SceneInterop::UninitializeScene()
{
	clReleaseKernel(oclKernel);
	clReleaseProgram(oclProgram);
	clReleaseCommandQueue(oclCommandQueue);
	clReleaseMemObject(dVbo);
}
