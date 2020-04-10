#include<Windows.h>
#include<stdio.h>
#include<GL/glew.h>
#include<gl/GL.h>
#include"vmath.h"



#include"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v10.1\include\cuda_gl_interop.h"
#include"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v10.1\include\cuda_runtime.h"


#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"opengl32.lib")

#pragma comment(lib,"cudart.lib")


#define WIN_WIDTH 800
#define WIN_HEIGHT 600

using namespace vmath;

HDC gHdc = NULL;
HGLRC ghrc = NULL;
HWND gHwnd = NULL;

int devCount;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

bool gbActiveWindow = false;
bool bIsFullScreen = false;
bool bDone = false;
int iRet;

//Shaders variable

GLint gVertexShaderObject;
GLint gFragmentShaderObject;
GLint gShaderProgramObject;


//Global Variables

GLuint vao_sphere;
GLuint vbo_position_sphere;
GLuint vbo_normal_sphere;
GLuint vbo_element_sphere;
GLuint mUniform;
GLuint vUniform;
GLuint pUniform;
GLuint laUniform;
GLuint kaUniform;
GLuint lsUniform;
GLuint ldUniform;
GLuint kdUniform;
GLuint ksUniform;
GLuint lightPositionUniform;
GLuint isLKeypressedUniform;
mat4 perspectiveProjectionMatrix;
GLuint material_shininess_uniform;
GLuint gVao;
GLuint gVboCPU;
GLuint gVboGPU;

float light_ambient[4];
float light_diffuse[4];
float light_specular[4];
float light_position[4];

float material_ambient[4];
float material_diffuse[4];
float material_specular[4];
float material_shininess[1];

bool onGPU = false;

int gbLight = 0;
float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_texture[764];
unsigned short sphere_elements[2280];
int gNumVertices, gNumElements;


const int gMeshWidth = 1024;
const int gMeshHeight = 1024;

#define MYARRAYSIZE gMeshWidth*gMeshHeight*4


extern void LaunchCUDAKernel(float4 *,unsigned int, unsigned int, float );

float pos[gMeshWidth][gMeshHeight][4];

struct cudaGraphicsResource* graphicsResource = NULL;

GLfloat gAnime = 0.0f;

cudaError_t error;

float4 * pPos;

size_t byteCount;


enum {
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXCOORD0
};

FILE * gpFile = NULL;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{

	int initialize(void);

	void display();
	void Update();
	MSG msg;
	HWND hwnd;
	TCHAR szAppName[] = TEXT("3D Rotation");
	WNDCLASSEX wndclass;

	if (fopen_s(&gpFile, "Log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Log File cannot be created"), TEXT("Error"), MB_OK);
		exit(0);
	}

	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbWndExtra = 0;
	wndclass.cbClsExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;

	RegisterClassEx(&wndclass);


	hwnd = CreateWindowEx(WS_EX_APPWINDOW, szAppName, TEXT("Diffused Lighting On A Sphere"), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, 100, 100, WIN_WIDTH, WIN_HEIGHT, NULL, NULL, hInstance, NULL);

	gHwnd = hwnd;
	iRet = initialize();
	if (iRet == -1)
	{
		fprintf(gpFile, "Choose Pixel Format Failed\n");
		DestroyWindow(0);
	}
	else if (iRet == -2)
	{
		fprintf(gpFile, "SetPixel Format failed");
		DestroyWindow(0);
	}
	else if (iRet == -3)
	{
		fprintf(gpFile, "wglCreateContext failed");
		DestroyWindow(0);
	}
	else if (iRet == -4)
	{
		fprintf(gpFile, "wglMakeCurrent failed");
		DestroyWindow(0);
	}
	else
	{
		fprintf(gpFile, "Initialize Function succeded");
	}

	ShowWindow(hwnd, iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	while (bDone == false)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				bDone = true;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

		}
		else
		{
			if (gbActiveWindow == true)
			{
				Update();
			}

			display();
		}
	}
	return((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	void resize(int, int);
	void ToggleFullScreen();
	void uninitialize();

	switch (iMsg)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 0x46:
			if (bIsFullScreen == false)
			{
				ToggleFullScreen();
			}
			else
			{
				ToggleFullScreen();
			}
			break;
		case'L':
			if (gbLight == 0)
			{
				gbLight = 1;
			}
			else
			{
				gbLight = 0;
			}
			break;

			case 'G':
				if (onGPU==false)
				{
					onGPU = true;
				}
				else
				{
					onGPU = false;
				}
				break;
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			break;
		}
		break;
	case WM_SETFOCUS:
		gbActiveWindow = true;
		break;
	case WM_KILLFOCUS:
		gbActiveWindow = false;
		break;
	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_ERASEBKGND:
		return 0;
	case WM_DESTROY:
		uninitialize();
		PostQuitMessage(0);
		break;
	}

	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}


void ToggleFullScreen(void)
{
	MONITORINFO mi;
	if (bIsFullScreen == false)
	{
		dwStyle = GetWindowLong(gHwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi = { sizeof(MONITORINFO) };

			if (GetWindowPlacement(gHwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(gHwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(gHwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(gHwnd, HWND_TOP,
					mi.rcMonitor.left,
					mi.rcMonitor.top,
					mi.rcMonitor.right - mi.rcMonitor.left,
					mi.rcMonitor.bottom - mi.rcMonitor.top,
					SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(false);
		bIsFullScreen = true;
	}

	else
	{
		SetWindowLong(gHwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(gHwnd, &wpPrev);
		SetWindowPos(gHwnd,
			HWND_TOP,
			0,
			0,
			0,
			0,
			SWP_NOZORDER |
			SWP_FRAMECHANGED |
			SWP_NOMOVE |
			SWP_NOSIZE |
			SWP_NOOWNERZORDER);

		ShowCursor(true);
		bIsFullScreen = false;
	}
}

void LaunchCPUKernel(unsigned int mesh_width, unsigned int mesh_height, float time)
{
	fprintf(gpFile, "\nInside CPU %f", time);
	for (int i = 0; i < mesh_width; i++)
	{
		for (int j = 0; j < mesh_height; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				float u = i / (float)mesh_width;
				float v = j / (float)mesh_height;
				
				u = u * 2.0f - 1.0f;
				v = v * 2.0f - 1.0f;

				float frequency = 4.0f;
				float w = sinf(u*frequency + time)*cosf(v*frequency + time)*0.5f;

				if (k == 0)
				{
					pos[i][j][k] = u;
				}
				if (k == 1)
				{
					pos[i][j][k] = w;
				}
				if (k == 2)
				{
					pos[i][j][k] = v;
				}
				if (k == 3)
				{
					pos[i][j][k] = 1.0f;
				}

			}
		}
	}
}

int initialize()
{
	void resize(int, int);
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatDescriptor;
	GLenum result;
	void uninitialize();
	GLint iShaderCompileStatus = 0;
	GLint iProgramLinkStatus = 0;
	GLint iInfoLogLength = 0;
	GLchar* szInfolog = NULL;


	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER;

	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;

	gHdc = GetDC(gHwnd);
	iPixelFormatDescriptor = ChoosePixelFormat(gHdc, &pfd);

	if (iPixelFormatDescriptor == 0)
	{
		return -1;
	}
	if (SetPixelFormat(gHdc, iPixelFormatDescriptor, &pfd) == FALSE)
	{
		return -2;
	}
	ghrc = wglCreateContext(gHdc);

	if (ghrc == NULL)
	{
		return -3;
	}

	if (wglMakeCurrent(gHdc, ghrc) == FALSE)
	{
		return -4;
	}

	result = glewInit();
	if (result != GLEW_OK)
	{
		uninitialize();
		fprintf(gpFile, "Inside GLEW\n");
		DestroyWindow(gHwnd);
	}


	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	//write vertex shader code.
	const GLchar* vertexShaderSourceCode =
		"#version 440" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec3 vnormal;" \
		"uniform mat4 u_m_matrix;" \
		"uniform mat4 u_v_matrix;" \
		"uniform mat4 u_p_matrix;" \
		"uniform int u_lkeyispressed;" \
		"uniform vec4 u_light_position;" \
		"out vec4 eye_coordinates;" \
		"out vec3 light_direction;" \
		"out vec3 tnorm;" \
		"out vec3 viewer_vector;" \
		"void main(void)" \
		"{" \
		"gl_Position=u_p_matrix*u_v_matrix*u_m_matrix*vPosition;" \
		"if(u_lkeyispressed == 1)" \
		"{" \
		"vec4 eye_coordinates=u_v_matrix*u_m_matrix*vPosition;" \
		"tnorm=mat3(u_v_matrix*u_m_matrix)*vnormal;" \
		"light_direction=vec3(u_light_position)-eye_coordinates.xyz;" \
		"viewer_vector=vec3(-eye_coordinates);" \
		"}" \
		"else" \
		"{" \
		"}" \

		"}";



	glShaderSource(gVertexShaderObject, 1, (const GLchar **)&vertexShaderSourceCode, NULL);
	glCompileShader(gVertexShaderObject);

	glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);

	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);

		if (iInfoLogLength > 0)
		{
			szInfolog = (GLchar *)malloc(iInfoLogLength);
			if (szInfolog != NULL)
			{
				GLsizei written;

				glGetShaderInfoLog(gVertexShaderObject, iInfoLogLength, &written, szInfolog);
				fprintf(gpFile, "Inside Vertex Shader %s", szInfolog);
				free(szInfolog);

				uninitialize();
				DestroyWindow(0);
				exit(0);
			}
		}
	}


	//For Fragment Shader.

	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar* fragmentShaderSourceCode =
		"#version 440" \
		"\n" \
		"out vec4 FragColor;" \
		"uniform int u_lkeyispressed;" \
		"uniform vec3 u_ld;" \
		"uniform vec3 u_la;" \
		"uniform vec3 u_ls;" \
		"uniform vec3 u_ka;" \
		"uniform vec3 u_ks;" \
		"uniform vec3 u_kd;" \
		"uniform float m_shininess;"
		"in vec3 light_direction;" \
		"in vec3 tnorm;"
		"in vec3 viewer_vector;" \
		"void main(void)" \
		"{" \
		"if(u_lkeyispressed == 1)" \
		"{" \
		"vec3 tdnorm=normalize(tnorm);" \
		"vec3 light_direction1=normalize(light_direction);" \
		"float tn_dot_ld=max(dot(light_direction1,tdnorm),0.0);" \
		"vec3 reflection_vector=reflect(-light_direction1,tdnorm);" \
		"vec3 viewer_vector1=normalize(viewer_vector);" \
		"vec3 ambient = u_la * u_ka;" \
		"vec3 diffuse = u_ld * u_kd * tn_dot_ld;" \
		"vec3 specular=u_ls * u_ks * pow(max(dot(reflection_vector,viewer_vector1),0.0),m_shininess);" \
		"vec3 phong_ads_light=ambient+diffuse+specular;" \
		"FragColor=vec4(phong_ads_light,1.0);" \
		"}" \
		"else"
		"{"
		"FragColor=vec4(1.0,1.0,0.0,1.0);" \
		"}"
		"}";



	glShaderSource(gFragmentShaderObject, 1, (const GLchar **)&fragmentShaderSourceCode, NULL);
	glCompileShader(gFragmentShaderObject);

	glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);

	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);

		if (iInfoLogLength > 0)
		{
			szInfolog = (GLchar *)malloc(iInfoLogLength);
			if (szInfolog != NULL)
			{
				GLsizei written;

				glGetShaderInfoLog(gFragmentShaderObject, iInfoLogLength, &written, szInfolog);
				fprintf(gpFile, "Inside Fragment Shader %s", szInfolog);
				free(szInfolog);

				uninitialize();
				DestroyWindow(0);
				exit(0);
			}
		}
	}


	//Create shader program object

	gShaderProgramObject = glCreateProgram();

	glAttachShader(gShaderProgramObject, gVertexShaderObject);

	glAttachShader(gShaderProgramObject, gFragmentShaderObject);

	glBindAttribLocation(gShaderProgramObject, AMC_ATTRIBUTE_POSITION, "vPosition");

	glBindAttribLocation(gShaderProgramObject, AMC_ATTIBUTE_NORMAL, "vnormal");

	glLinkProgram(gShaderProgramObject);

	glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &iProgramLinkStatus);

	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);

		if (iInfoLogLength > 0)
		{
			szInfolog = (GLchar *)malloc(iInfoLogLength);
			if (szInfolog != NULL)
			{
				GLsizei written;

				glGetProgramInfoLog(gShaderProgramObject, iInfoLogLength, &written, szInfolog);
				fprintf(gpFile, "Inside Linking of program %s", szInfolog);
				free(szInfolog);

				uninitialize();
				DestroyWindow(0);
				exit(0);
			}
		}
	}

	//
	mUniform = glGetUniformLocation(gShaderProgramObject, "u_m_matrix");
	vUniform = glGetUniformLocation(gShaderProgramObject, "u_v_matrix");
	pUniform = glGetUniformLocation(gShaderProgramObject, "u_p_matrix");
	laUniform = glGetUniformLocation(gShaderProgramObject, "u_la");
	lsUniform = glGetUniformLocation(gShaderProgramObject, "u_ls");
	ldUniform = glGetUniformLocation(gShaderProgramObject, "u_ld");
	kdUniform = glGetUniformLocation(gShaderProgramObject, "u_kd");
	kaUniform = glGetUniformLocation(gShaderProgramObject, "u_ka");
	ksUniform = glGetUniformLocation(gShaderProgramObject, "u_ks");
	material_shininess_uniform = glGetUniformLocation(gShaderProgramObject, "m_shininess");
	lightPositionUniform = glGetUniformLocation(gShaderProgramObject, "u_light_position");
	isLKeypressedUniform = glGetUniformLocation(gShaderProgramObject, "u_lkeyispressed");

	error = cudaGetDeviceCount(&devCount);

	if (error != cudaSuccess)
	{
		fprintf(gpFile, "NVDIA DEVICE NOT FOUND");
		uninitialize();
		exit(0);

	}
	else if (devCount == 0)
	{
		fprintf(gpFile, "NVDIA DEVICE FOUND");
	}

	else
	{
		cudaSetDevice(0);
	}

	/*getSphereVertexData(sphere_vertices, sphere_normals, sphere_texture, sphere_elements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();*/

	for (int i = 0; i < gMeshWidth; i++) {
		for (int j = 0; j < gMeshHeight; j++) {
			for (int k = 0; k < 4; k++) {
				pos[i][j][k] = 0;
			}
		}
	}


	//Sphere
	glGenVertexArrays(1, &gVao);
	glBindVertexArray(gVao);
	glGenBuffers(1, &gVboCPU);
	glBindBuffer(GL_ARRAY_BUFFER, gVboCPU);
	glBufferData(GL_ARRAY_BUFFER, MYARRAYSIZE*sizeof(float), NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, &gVboGPU);
	glBindBuffer(GL_ARRAY_BUFFER, gVboGPU);
	glBufferData(GL_ARRAY_BUFFER, MYARRAYSIZE * sizeof(float), NULL, GL_DYNAMIC_DRAW);

	error = cudaGraphicsGLRegisterBuffer(&graphicsResource, gVboGPU, cudaGraphicsMapFlagsWriteDiscard);

	if (error != cudaSuccess)
	{
		fprintf(gpFile, "CUDA NOT SUCCESS");
			uninitialize();
		exit(0);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/*glGenBuffers(1, &vbo_element_sphere);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element_sphere);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);*/

	glBindVertexArray(0);

	light_ambient[0] = 0.0f;
	light_ambient[1] = 0.0f;
	light_ambient[2] = 0.0f;
	light_ambient[3] = 0.0f;

	light_diffuse[0] = 1.0f;
	light_diffuse[1] = 1.0f;
	light_diffuse[2] = 1.0f;
	light_diffuse[3] = 1.0f;

	light_specular[0] = 1.0f;
	light_specular[1] = 1.0f;
	light_specular[2] = 1.0f;
	light_specular[3] = 1.0f;

	light_position[0] = 100.0f;
	light_position[1] = 100.0f;
	light_position[2] = 100.0f;
	light_position[3] = 1.0f;


	material_ambient[0] = 0.0f;
	material_ambient[1] = 0.0f;
	material_ambient[2] = 0.0f;
	material_ambient[3] = 0.0f;

	material_diffuse[0] = 1.0f;
	material_diffuse[1] = 1.0f;
	material_diffuse[2] = 1.0f;
	material_diffuse[3] = 1.0f;

	material_specular[0] = 1.0f;
	material_specular[1] = 1.0f;
	material_specular[2] = 1.0f;
	material_specular[3] = 1.0f;


	material_shininess[0] = 50.0f;


	glClearDepth(1.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	perspectiveProjectionMatrix = mat4::identity();
	//glEnable(GL_CULL_FACE);
	//glDisable(GL_CULL_FACE);
	//glEnable(GL_FRONT_FACE);
	//glEnable(GL_BACK);
	resize(WIN_WIDTH, WIN_HEIGHT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	return 0;
}


void resize(int width, int height)
{
	if (height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	perspectiveProjectionMatrix = perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(gShaderProgramObject);
	mat4 modelMatrix;
	mat4 viewMatrix;
	mat4 projectionMatrix;
	mat4 rotationMatrix;


	modelMatrix = mat4::identity();
	projectionMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	modelMatrix = translate(0.0f, 0.0f, -2.0f);
	//rotationMatrix = rotate(angle_cube, 0.0f, 1.0f, 0.0f);
	projectionMatrix = projectionMatrix * perspectiveProjectionMatrix;
	//modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix*rotationMatrix;
	glUniformMatrix4fv(mUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(vUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(pUniform, 1, GL_FALSE, projectionMatrix);
	
		glUniform1i(isLKeypressedUniform, 1);
		glUniform3fv(laUniform, 1, light_ambient);
		glUniform3fv(ldUniform, 1, light_diffuse);
		glUniform3fv(lsUniform, 1, light_specular);
		glUniform4fv(lightPositionUniform, 1, light_position);

		glUniform3fv(kaUniform, 1, material_ambient);
		glUniform3fv(kdUniform, 1, material_diffuse);
		glUniform3fv(ksUniform, 1, material_specular);
		glUniform1fv(material_shininess_uniform, 1, material_shininess);


	


	glBindVertexArray(gVao);
	if (onGPU == true)
	{
		glBindBuffer(GL_ARRAY_BUFFER, gVboGPU);

	
	error = cudaGraphicsMapResources(1, &graphicsResource, 0);
	if (error != cudaSuccess)
	{
		fprintf(gpFile, "Not bind in display");
	}

	error = cudaGraphicsResourceGetMappedPointer((void**)&pPos, &byteCount, graphicsResource);

	if (error != cudaSuccess)
	{
		fprintf(gpFile, "Not bind in display for second error checking");
	}
	LaunchCUDAKernel(pPos, gMeshWidth, gMeshHeight, gAnime);
	cudaGraphicsUnmapResources(1, &graphicsResource, 0);

	}

	else 
	{
		glBindBuffer(GL_ARRAY_BUFFER, gVboCPU);

		LaunchCPUKernel(gMeshWidth, gMeshHeight, gAnime);

		glBufferData(GL_ARRAY_BUFFER, MYARRAYSIZE * sizeof(float), pos,GL_DYNAMIC_DRAW);
		

	}


	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glDrawArrays(GL_POINTS,0,gMeshWidth*gMeshHeight);

	glBindBuffer(GL_ARRAY_BUFFER ,0);
	glBindVertexArray(0);
	

	glUseProgram(0);
	SwapBuffers(gHdc);

	gAnime = gAnime + 0.05;

}

void Update()
{

}

void uninitialize()
{
	if (bIsFullScreen == true) {
		SetWindowLong(gHwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(gHwnd, &wpPrev);
		SetWindowPos(gHwnd,
			HWND_TOP,
			0,
			0,
			0,
			0,
			SWP_NOZORDER |
			SWP_FRAMECHANGED |
			SWP_NOMOVE |
			SWP_NOSIZE |
			SWP_NOOWNERZORDER);
		ShowCursor(TRUE);
	}

	if (wglGetCurrentContext() == ghrc)
	{
		wglMakeCurrent(NULL, NULL);
		ghrc = NULL;
	}
	if (ghrc)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}
	if (ghrc)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}
	if (gpFile)
	{
		fprintf(gpFile, "Log File closed successfully");
		fclose(gpFile);
		gpFile = NULL;
	}


	glUseProgram(gShaderProgramObject);
	glDetachShader(gShaderProgramObject, gFragmentShaderObject);
	glDetachShader(gShaderProgramObject, gVertexShaderObject);
	glDeleteShader(gFragmentShaderObject);
	gFragmentShaderObject = 0;
	glDeleteShader(gVertexShaderObject);
	gVertexShaderObject = 0;
	glDeleteProgram(gShaderProgramObject);
	gShaderProgramObject = 0;
	glUseProgram(0);
}


