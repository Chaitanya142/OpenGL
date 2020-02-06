#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<windows.h>
#include<gl\glew.h>
#include<gl\gl.h>
#include"vmath.h"
#include"VectorofFloat.h"
#include"VectorofIntegres.h"

#pragma comment (lib, "Opengl32.lib")
#pragma comment (lib, "glew32.lib")

using namespace vmath;

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

enum {
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXCOORD
};


/*Global variables*/
FILE *gpFile = NULL;
HWND ghwnd;
bool gbActiveWindow = false;
bool gbFullScreen = false;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
HDC ghdc;
HGLRC ghrc = NULL;
GLenum Result;

GLuint glShaderProgramObject;
GLuint vao;
GLuint vbo;
GLuint mvpUniform;
mat4 PerspectiveProjectionMatrix; //from vmath

/*Global Variables for Model*/
struct vec_float *gp_vec_vertices, *gp_vec_normals, *gp_vec_textures;
struct vec_float *gp_vec_vertex_sorted, *gp_vec_normals_sorted, *gp_vec_textures_sorted;
struct vec_int *gp_vertex_indices, *gp_normals_indces, *gp_texture_indices;
GLuint gElementBuffer;
char buffer[1024];
FILE *gpMesh = NULL;

/*Call back function declration*/
LRESULT CALLBACK  WndProc(HWND, UINT, WPARAM, LPARAM);

/*Main function*/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdline, int iCmdShow)
{
	/*function declration*/
	int initialize(void);
	void display(void);

	/*Loacl variable declrations*/
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("Tree");
	int iRet = 0;
	bool bDone = false;

	if (fopen_s(&gpFile, "log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Failed to create file!!!"), TEXT("Error"), MB_OK);
		exit(0);
	}
	else
	{
		fprintf_s(gpFile, "log file successfully opened!\n");
	}

	/*Cearing the struct*/
	memset(&wndclass, 0, sizeof(WNDCLASSEX));

	/*Initialize wndclass*/
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.lpszClassName = szAppName;

	/*Register class to OS*/
	RegisterClassEx(&wndclass);

	/*Create Window*/
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szAppName,
		TEXT("Tree"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		100,
		100,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;

	iRet = initialize();
	if (iRet == -1)
	{
		fprintf_s(gpFile, "Choose pixelformat failed.\n");
		DestroyWindow(hwnd);
	}
	else if (iRet == -2)
	{
		fprintf_s(gpFile, "SetPixelFormat failed.\n");
		DestroyWindow(hwnd);
	}
	else if (iRet == -3)
	{
		fprintf_s(gpFile, "wglCreateContext failed.\n");
		DestroyWindow(hwnd);
	}
	else if (iRet == -4)
	{
		fprintf_s(gpFile, "wglMakeCurrent Failed.\n");
		DestroyWindow(hwnd);
	}
	else
	{
		fprintf_s(gpFile, "Initialize Successful.\n");
	}

	ShowWindow(hwnd, iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	/*Message loop*/
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

			}
			display();
		}
	}
	return((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	/*Function Declration*/
	void reshape(int, int);
	void ToggleFullScreen(void);
	void display(void);
	void uninitialize(void);

	/*code*/
	switch (iMsg)
	{
	case WM_SETFOCUS:
		gbActiveWindow = true;
		break;

	case WM_KILLFOCUS:
		gbActiveWindow = false;
		break;

	case WM_SIZE:
		reshape(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_CLOSE:
		DestroyWindow(ghwnd);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(ghwnd);
			break;

		case 'f':
		case 'F':
			ToggleFullScreen();
			break;
		}
		break;

	case WM_RBUTTONDOWN:
		DestroyWindow(ghwnd);
		break;

	case WM_DESTROY:
		uninitialize();
		PostQuitMessage(0);
		break;

	default:
		break;
	}

	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void ToggleFullScreen(void)
{
	MONITORINFO mi;

	if (gbFullScreen == false)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi = { sizeof(MONITORINFO) };
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd,
					HWND_TOP,
					mi.rcMonitor.left,
					mi.rcMonitor.top,
					mi.rcMonitor.right - mi.rcMonitor.left,
					mi.rcMonitor.bottom - mi.rcMonitor.top,
					SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
		gbFullScreen = true;
	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0,
			SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);

		ShowCursor(TRUE);
		gbFullScreen = false;
	}
}

int initialize(void)
{
	/*Function declrations*/
	void reshape(int, int);
	void uninitialize(void);
	void LoadMesh(void);

	/*Variable declarations*/
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;
	/*Shader object variables*/
	GLuint glVertexShaderObject;
	GLuint glFragmentShaderObject;

	/*code*/
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	/*enabling depth bits*/
	pfd.cDepthBits = 32;

	ghdc = GetDC(ghwnd);
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);

	if (iPixelFormatIndex == 0)
	{
		return (-1);
	}

	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
	{
		return(-2);
	}

	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		return(-3);
	}

	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		return(-4);
	}

	Result = glewInit();
	if (Result != GLEW_OK)
	{
		fprintf_s(gpFile, "Failed to initialize glew\n");
		uninitialize();
		DestroyWindow(ghwnd);
	}

	/*Define Vertex Shader object*/
	glVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	/*Write Vertex Shader code*/
	const GLchar* vertexShaderSourceCode[] =
	{
		"#version 430 core"\
		"\n"\
		"in vec4 vPos;"\
		"uniform mat4 u_mvp_matrix;"\
		"void main(void)"\
		"{"\
		"gl_Position = u_mvp_matrix * vPos;"\
		"}"
	};


	//specifiy above source code to the Vertex Shader Object
	glShaderSource(glVertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);

	//compile the vertex shader program
	glCompileShader(glVertexShaderObject);

	GLint iShaderCompileStatus = 0;
	GLint iInfoLength = 0;
	GLchar* szInfoLog = NULL;

	glGetShaderiv(glVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(glVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLength);
		if (iInfoLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(glVertexShaderObject, iInfoLength, &written, szInfoLog);
				fprintf_s(gpFile, "Vertex Shader error Log: \n%s", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghwnd);
				exit(0);
			}
		}
	}

	/*Fragment Shader code*/
	glFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	//Write Fragment shader code
	const GLchar* fragmentShaderSourceCode[] =
	{
		"#version 430 core"\
		"\n"\
		"out vec4 FragColor;"\
		"void main(void)"\
		"{"\
		"FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);"
		"}"
	};

	//specify above source code to the fragement shader object
	glShaderSource(glFragmentShaderObject, 1, (const char**)fragmentShaderSourceCode, NULL);

	//compile fragment shader code
	glCompileShader(glFragmentShaderObject);

	iShaderCompileStatus = 0;
	iInfoLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(glFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(glVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLength);
		if (iInfoLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(glVertexShaderObject, iInfoLength, &written, szInfoLog);
				fprintf_s(gpFile, "Fragement Shader error Log: \n%s", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghwnd);
				exit(0);
			}
		}
	}

	//Create Shader program object
	glShaderProgramObject = glCreateProgram();
	//Attach Vertex shader to shader program object
	glAttachShader(glShaderProgramObject, glVertexShaderObject);
	glAttachShader(glShaderProgramObject, glFragmentShaderObject);

	/*Prelinking Binding to vaertex attributes*/
	glBindAttribLocation(glShaderProgramObject, AMC_ATTRIBUTE_POSITION, "vPos");

	//Link the shader program
	glLinkProgram(glShaderProgramObject);

	//Error checking
	GLint iProgramLinkStatus = 0;
	szInfoLog = NULL;
	iInfoLength = 0;

	glGetProgramiv(glShaderProgramObject, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(glShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLength);
		if (iInfoLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(glShaderProgramObject, iInfoLength, &written, szInfoLog);
				fprintf_s(gpFile, "Link error Log: \n%s", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghwnd);
				exit(0);
			}
		}
	}

	/*Post linking retriving uniform loactions*/
	mvpUniform = glGetUniformLocation(glShaderProgramObject, "u_mvp_matrix");
	LoadMesh();

	/*Create vao*/
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	
	glBufferData(GL_ARRAY_BUFFER, gp_vec_vertices->size*sizeof(float), gp_vec_vertices->p, GL_STATIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, gp_vec_vertices->size * sizeof(float), gp_vec_vertices->p, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &gElementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gElementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, gp_vertex_indices->size * sizeof(unsigned int), gp_vertex_indices->p, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	/*Unbinding the buffer*/
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glFrontFace(GL_CCW);
	
	/*Enable depth buffer*/
	glClearDepth(1.0f);
	/*Clear screen*/
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	/*Enable depth test*/
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	PerspectiveProjectionMatrix = mat4::identity();
	reshape(WIN_WIDTH, WIN_HEIGHT);
	return(0);
}

void LoadMesh()
{
	void uninitialize(void);
	char *first_token = NULL;
	char *token = NULL;
	const char *space = " ";
	const char *slash = "/";
	char* f_entries[] = { NULL, NULL, NULL };
	char* next_token = NULL;

	gp_vec_vertices = create_vec_float();
	if (gp_vec_vertices == NULL)
	{
		fprintf(gpFile, "Failed to allocate memory to vertices vector\n");
		uninitialize();
		exit(0);
	}
	gp_vec_normals = create_vec_float();
	if (gp_vec_normals == NULL)
	{
		fprintf(gpFile, "Failed to allocate memory to vertices vector\n");
		uninitialize();
		exit(0);
	}
	gp_vec_textures = create_vec_float();
	if (gp_vec_textures == NULL)
	{
		fprintf(gpFile, "Failed to allocate memory to vertices vector\n");
		uninitialize();
		exit(0);
	}
	gp_vec_vertex_sorted = create_vec_float();
	if (gp_vec_vertex_sorted == NULL)
	{
		fprintf(gpFile, "Failed to allocate memory to vertices vector\n");
		uninitialize();
		exit(0);
	}
	gp_vec_normals_sorted = create_vec_float();
	if (gp_vec_normals_sorted == NULL)
	{
		fprintf(gpFile, "Failed to allocate memory to vertices vector\n");
		uninitialize();
		exit(0);
	}
	gp_vec_textures_sorted = create_vec_float();
	if (gp_vec_textures_sorted == NULL)
	{
		fprintf(gpFile, "Failed to allocate memory to vertices vector\n");
		uninitialize();
		exit(0);
	}
	gp_vertex_indices = create_vec_int();
	if (gp_vertex_indices == NULL)
	{
		fprintf(gpFile, "Failed to allocate memory to vertices vector\n");
		uninitialize();
		exit(0);
	}
	gp_normals_indces = create_vec_int();
	if (gp_normals_indces == NULL)
	{
		fprintf(gpFile, "Failed to allocate memory to vertices vector\n");
		uninitialize();
		exit(0);
	}
	gp_texture_indices = create_vec_int();
	if (gp_texture_indices == NULL)
	{
		fprintf(gpFile, "Failed to allocate memory to vertices vector\n");
		uninitialize();
		exit(0);
	}

	fopen_s(&gpMesh, "MonkeyHead1.obj", "r");
	if (gpMesh == NULL)
	{
		fprintf(gpFile, "Failed to load model file\n");
		uninitialize();
		exit(0);
	}
	
	//while (fgets(buffer, 1024, gpMesh) != NULL)
	//{

	//int a = strcmp("vt", "v");
	while ((fgets(buffer, 1024, gpMesh) != NULL) && !feof(gpMesh)) {
		first_token = strtok_s(buffer, space, &next_token);
		if (strcmp(first_token, "v") == 0)
		{
			token = strtok_s(NULL, space, &next_token);
			float x = atof(token);
			fprintf_s(gpFile, "\n%f\t", x);
			push_back_vec_float(gp_vec_vertices, x);
			token = strtok_s(NULL, space, &next_token);
			float y = atof(token);
			fprintf_s(gpFile, "%f\t", y);
			push_back_vec_float(gp_vec_vertices, y);
			token = strtok_s(NULL, space, &next_token);
			float z = atof(token);
			fprintf_s(gpFile, "%f\t", z);
			push_back_vec_float(gp_vec_vertices, z);
		}
		else if (strcmp(first_token, "vt") == 0)
		{
			token = strtok_s(NULL, space, &next_token);
			float s = atof(token);
			fprintf_s(gpFile, "\n%f\t", s);
			push_back_vec_float(gp_vec_textures, s);
			token = strtok_s(NULL, space, &next_token);
			float t = atof(token);
			fprintf_s(gpFile, "%f\t", t);
			push_back_vec_float(gp_vec_textures, t);
		}
		else if (strcmp(first_token, "vn") == 0)
		{
			token = strtok_s(NULL, space, &next_token);
			float x = atof(token);
			fprintf_s(gpFile, "\n%f\t", x);
			push_back_vec_float(gp_vec_normals, x);
			token = strtok_s(NULL, space, &next_token);
			float y = atof(token);
			fprintf_s(gpFile, "%f\t", y);
			push_back_vec_float(gp_vec_normals, y);
			token = strtok_s(NULL, space, &next_token);
			float z = atof(token);
			fprintf_s(gpFile, "%f\t", z);
			push_back_vec_float(gp_vec_normals, z);
		}
		else if (strcmp(first_token, "f") == 0)
		{
			for (int i = 0; i < 3; i++)
			{
				f_entries[i] = strtok_s(NULL, space, &next_token);
			}
			for (int j = 0; j < 3; j++)
			{
				token = strtok_s(f_entries[j], slash, &next_token);
				push_back_vec_int(gp_vertex_indices, atoi(token)-1);
				fprintf_s(gpFile, "\n%d\t", atoi(token)-1);
				token = strtok_s(NULL, slash, &next_token);
				push_back_vec_int(gp_texture_indices, atoi(token)-1);
				token = strtok_s(NULL, slash, &next_token);
				push_back_vec_int(gp_normals_indces, atoi(token)-1);
			}
		}
	}

	for (int i = 0; i < gp_vertex_indices->size; i++)
	{
		//int temp = gp_vertex_indices->p[i];
		int temp = i;
		//push_back_vec_float(gp_vec_vertex_sorted, gp_vec_vertices->p[temp*3+0]);
		//push_back_vec_float(gp_vec_vertex_sorted, gp_vec_vertices->p[temp*3+1]);
		//push_back_vec_float(gp_vec_vertex_sorted, gp_vec_vertices->p[temp*3+2]);
		push_back_vec_float(gp_vec_vertex_sorted, gp_vec_vertices->p[temp]);
		//struct vec_float *vec_vertices = create_vec_float();

	}

	for (int i = 0; i < gp_normals_indces->size; i++)
	{
		int temp = gp_normals_indces->p[i];
		push_back_vec_float(gp_vec_normals_sorted, gp_vec_normals->p[temp]);
	}

	for (int i = 0; i < gp_texture_indices->size; i++)
	{
		int temp = gp_texture_indices->p[i];
		push_back_vec_float(gp_vec_textures_sorted, gp_vec_textures->p[temp]);
	}

	fclose(gpMesh);
}

void reshape(int width, int height)
{
	if (height == 0)
	{
		height = 1;
	}
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	PerspectiveProjectionMatrix = perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void display(void)
{
	/*code*/
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(glShaderProgramObject);

	mat4 modelViewMatrix;
	mat4 modelViewProjectionMatrix;

	/*Initialize matrices to identity*/
	modelViewMatrix = mat4::identity();
	modelViewMatrix = translate(0.0f, 0.0f, -6.0f);
	
	modelViewMatrix = modelViewMatrix * rotate(90.0f,0.0f,1.0f,0.0f);

	modelViewProjectionMatrix = mat4::identity();

	//Do necessary transformations

	//Do necessary matrix multiplication
	modelViewProjectionMatrix = PerspectiveProjectionMatrix * modelViewMatrix;

	//Send this necessary matrices to shaders in respective uniforms
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	/*Bind with vao (this will avoid any repetative binding with vbo)*/
	glBindVertexArray(vao);
	//Smilarly bind with texture if any
	//Draw necessary scene
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gElementBuffer);
	//glDrawElements(GL_TRIANGLES, gp_vertex_indices->size, GL_UNSIGNED_BYTE, NULL);
	glDrawElements(GL_TRIANGLES, gp_vertex_indices->size, GL_UNSIGNED_INT, NULL);

	/*Unbind vao*/
	glBindVertexArray(0);

	/*Unuse program*/
	glUseProgram(0);
	SwapBuffers(ghdc);
}

void uninitialize(void)
{
	if (gbFullScreen == true)
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);
		ShowCursor(true);
	}

	if (wglGetCurrentContext() == ghrc)
	{
		wglMakeCurrent(NULL, NULL);
	}

	if (ghrc)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}

	if (ghdc)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (vbo)
	{
		glDeleteBuffers(1, &vbo);
		vbo = 0;
	}

	if (vao)
	{
		glDeleteBuffers(1, &vao);
		vao = 0;
	}

	if (gElementBuffer)
	{
		glDeleteBuffers(1, &gElementBuffer);
		gElementBuffer = 0;
	}

	destroy_vec_float(gp_vec_vertices);
	destroy_vec_float(gp_vec_normals);
	destroy_vec_float(gp_vec_textures);
	destroy_vec_float(gp_vec_normals_sorted);
	destroy_vec_float(gp_vec_textures_sorted);
	destroy_vec_float(gp_vec_vertex_sorted);
	destroy_vec_int(gp_normals_indces);
	destroy_vec_int(gp_texture_indices);
	destroy_vec_int(gp_vertex_indices);

	/*variable declration for safe release of shader objects*/
	GLsizei shaderCount;
	GLsizei shaderNumber;

	if (glShaderProgramObject)
	{
		glUseProgram(glShaderProgramObject);
		//Ask program how many shaders attached to you
		glGetProgramiv(glShaderProgramObject, GL_ATTACHED_SHADERS, &shaderCount);

		GLuint *pShaders;

		pShaders = (GLuint*)malloc(sizeof(GLuint) * shaderCount);
		if (pShaders)
		{
			glGetAttachedShaders(glShaderProgramObject, shaderCount, &shaderCount, pShaders);
			for (shaderNumber = 0; shaderNumber < shaderCount; shaderNumber++)
			{
				glDetachShader(glShaderProgramObject, pShaders[shaderNumber]);
				//Delete the detached shader
				glDeleteShader(pShaders[shaderNumber]);
				pShaders[shaderNumber] = 0;
			}
			free(pShaders);

		}
		glDeleteProgram(glShaderProgramObject);
		glShaderProgramObject = 0;
		glUseProgram(0);
	}

	if (gpFile)
	{
		fprintf_s(gpFile, "log file is closed successfully.");
		fclose(gpFile);
		gpFile = NULL;
	}
}

