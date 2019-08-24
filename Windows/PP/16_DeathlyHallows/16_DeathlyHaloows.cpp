#include<Windows.h>
#include<stdio.h>

#include<GL\glew.h>
#include<gl\GL.h>
//#include <time.h>

#include"vmath.h"

#pragma comment(lib,"OpenGL32.lib")
#pragma comment(lib,"glew32.lib")

//Constants
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

//Vmath namespace
using namespace vmath;

//Global Variables
HWND ghwnd = NULL;
DWORD dwStyle;
bool bFullScreen = false;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
HDC ghdc = NULL;
HGLRC ghrc = NULL;
bool gbActiveWindow = false;
FILE *gpFile = NULL;
GLfloat angleRotation = 0.0f;

//Shader Program Objects
GLint gShaderProgramObject;

enum {
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXCOORD0
};

GLuint vao_line;
GLuint vao_point;

GLuint vbo_position_lines;
GLuint vbo_color_lines;

GLuint vbo_position_point;
GLuint vbo_color_point;

GLuint mvpUniform;
mat4 perspectiveProjectionMatrix;

//Method Declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//void PrintTime();
//WinMain Function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevinstance, LPSTR lpszCmdLine, int iCmdShow) {
	//Method Declaration
	int Initialize(void);
	void Display(void);
	void Update(void);
	//Variable Declaration
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("OGL Window");
	int iRet = 0;
	bool bDone = false;
	//Code
	if (fopen_s(&gpFile, "Log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Log File Can't Be Created"), TEXT("ERROR"), MB_OK);
		exit(0);
	}
	else {
		//PrintTime();
		fprintf_s(gpFile, "Log File Created Successfully\n");
	}
	wndclass.cbClsExtra = 0;
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.cbWndExtra = 0;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hInstance = hInstance;
	wndclass.lpfnWndProc = WndProc;
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

	RegisterClassEx(&wndclass);

	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szAppName,
		TEXT("OGL WINDOW"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		100,
		100,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL
	);
	ghwnd = hwnd;
	iRet = Initialize();
	if (iRet == -1) {
		fprintf_s(gpFile, "Choose Pixel Format Failed\n");
		DestroyWindow(0);
	}
	else if (iRet == -2) {
		fprintf_s(gpFile, "Set Pixel Format Failed\n");
		DestroyWindow(0);
	}
	else if (iRet == -3) {
		fprintf_s(gpFile, "wgl Create Context Failed\n");
		DestroyWindow(0);
	}
	else if (iRet == -4) {
		fprintf_s(gpFile, "wgl Make Current Failed\n");
		DestroyWindow(0);
	}
	else {
		fprintf_s(gpFile, "Initialization Succeeded\n");
	}
	ShowWindow(hwnd, iCmdShow);
	//Do not call update window
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);
	//Game Loop
	//SendMessage(hwnd, WM_PAINT, 0, 0);
	while (bDone == false) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				bDone = true;
			}
			else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else {
			if (gbActiveWindow == true) {
				Update();
			}
			Display();
			//Here Call Display
		}
	}
	return (int)msg.wParam;
}
//WndProc Function
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	//Function Declaration
	void ToggleFullScreen(void);
	void Resize(int, int);
	void UnInitialize(void);
	void Update(void);
	//Code
	switch (iMsg)
	{
	case WM_SETFOCUS:
		gbActiveWindow = true;
		break;
	case WM_KILLFOCUS:
		gbActiveWindow = false;
		break;
	case WM_SIZE:
		Resize(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_ERASEBKGND:
		return(0);
		break;
	case WM_CLOSE:
		DestroyWindow(0);
		break;
	case WM_CHAR:
		switch (wParam)
		{
		case 'F':
		case 'f':
			ToggleFullScreen();
			break;
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			break;
		}
		break;
	case WM_DESTROY:
		UnInitialize();
		PostQuitMessage(0);
		break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}
//ToggleFullScreen Function
void ToggleFullScreen(void) {
	//Variable Declaration
	MONITORINFO mi;
	if (bFullScreen == false) {
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW) {
			mi = { sizeof(MONITORINFO) };
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi)) {
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd,
					HWND_TOP,
					mi.rcMonitor.left,
					mi.rcMonitor.top,
					mi.rcMonitor.right - mi.rcMonitor.left,
					mi.rcMonitor.bottom - mi.rcMonitor.top,
					SWP_NOZORDER | SWP_FRAMECHANGED
				);
				ShowCursor(FALSE);
				bFullScreen = true;
			}
		}
	}
	else {
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd,
			HWND_TOP,
			0,
			0,
			0,
			0,
			SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);
		ShowCursor(TRUE);
		bFullScreen = false;
	}
}
//Initialize OpenGl
int Initialize(void) {
	//Function Declaration
	void Resize(int, int);
	void UnInitialize(void);

	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;
	GLenum result;
	//Code
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
	ghdc = GetDC(ghwnd);
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0) {
		return -1;
	}
	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
	{
		return -2;
	}
	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL) {
		return -3;
	}
	if (wglMakeCurrent(ghdc, ghrc) == FALSE) {
		return -4;
	}

	result = glewInit();
	if (result != GLEW_OK) {
		fprintf_s(gpFile, "glewInit() failed\n");
		UnInitialize();
		DestroyWindow(0);
	}

	//Shader Objects
	GLint gVertexShaderObject;
	GLint gFragementShaderObject;

	//Vertex Shader
	//Define Shader Object
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	//Vertex Shader Code
	const GLchar* vertextShaderSourceCode =
		"#version 430 core " \
		"\n " \
		"in vec4 vPosition;" \
		"in vec4 vColor;" \
		"uniform mat4 u_mvp_matrix;" \
		"out vec4 out_color;" \
		"void main(void)" \
		"{" \
		"gl_Position=u_mvp_matrix * vPosition;" \
		"out_color=vColor;"\
		"} ";

	//Shader object and source code mapping
	glShaderSource(gVertexShaderObject, 1, &vertextShaderSourceCode, NULL);

	//Compile Shader
	glCompileShader(gVertexShaderObject);

	//Error Checking
	GLint iShaderCompileStatus = 0;
	GLint iInfoLogLength = 0;
	GLchar * szInfoLog = NULL;

	glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);

	if (iShaderCompileStatus == GL_FALSE) {
		glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0) {
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL) {
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject,
					iInfoLogLength,
					&written,
					szInfoLog);
				fprintf_s(gpFile, "Vertex Shader Compilation Log %s\n", szInfoLog);
				free(szInfoLog);
				UnInitialize();
				DestroyWindow(0);
				exit(0);
			}
		}
	}

	//Fragement Shader
	//Define Shader Object
	gFragementShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	//Fragement Shader Code
	const GLchar* fragementShaderSourceCode =
		"#version 430 core " \
		"\n " \
		"out vec4 fragColor;"
		"in vec4 out_color;"\
		"void main(void)" \
		"{" \
		"fragColor=out_color;" \
		"} ";

	//Shader object and source code mapping
	glShaderSource(gFragementShaderObject, 1, &fragementShaderSourceCode, NULL);

	//Compile Shader
	glCompileShader(gFragementShaderObject);

	//Error Checking
	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(gFragementShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);

	if (iShaderCompileStatus == GL_FALSE) {
		glGetShaderiv(gFragementShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0) {
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL) {
				GLsizei written;
				glGetShaderInfoLog(gFragementShaderObject,
					iInfoLogLength,
					&written,
					szInfoLog);
				fprintf_s(gpFile, "Fragement Shader Compilation Log %s\n", szInfoLog);
				free(szInfoLog);
				UnInitialize();
				DestroyWindow(0);
				exit(0);
			}
		}
	}

	//Program Object
	gShaderProgramObject = glCreateProgram();
	//Attach Shaders
	glAttachShader(gShaderProgramObject, gVertexShaderObject);
	glAttachShader(gShaderProgramObject, gFragementShaderObject);

	//Prelinking
	//Binding to attribute
	glBindAttribLocation(gShaderProgramObject,
		AMC_ATTRIBUTE_POSITION,
		"vPosition");

	glBindAttribLocation(gShaderProgramObject,
		AMC_ATTRIBUTE_COLOR,
		"vColor");

	//Link Shader Program
	glLinkProgram(gShaderProgramObject);

	//Error Checking
	GLint iShaderLinkStatus = 0;

	glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &iShaderLinkStatus);

	if (iShaderLinkStatus == GL_FALSE) {
		glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0) {
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL) {
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject,
					iInfoLogLength,
					&written,
					szInfoLog);
				fprintf_s(gpFile, "Program Link Log %s\n", szInfoLog);
				free(szInfoLog);
				UnInitialize();
				DestroyWindow(0);
				exit(0);
			}
		}
	}

	//Post Linking
	//Retriving Uniform Location
	mvpUniform = glGetUniformLocation(gShaderProgramObject,
		"u_mvp_matrix");


	//Create vao_line
	//Save everying in single set
	glGenVertexArrays(1, &vao_line);

	glBindVertexArray(vao_line);


	//Line
	//Generate Buffer
	glGenBuffers(1, &vbo_position_lines);
	//Bind Generated Buffer
	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_position_lines);
	//Fill Buffer
	glBufferData(GL_ARRAY_BUFFER,
		6 * sizeof(float),
		NULL,
		GL_DYNAMIC_DRAW);
	//Set Vertex Attrib Pointer
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		NULL);
	//Enable Vertex Attrib Array
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	//Unbind Buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_color_lines);
	//Bind Generated Buffer
	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_color_lines);
	//Fill Buffer
	glBufferData(GL_ARRAY_BUFFER,
		6 * sizeof(float),
		NULL,
		GL_DYNAMIC_DRAW);
	//Set Vertex Attrib Pointer
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		NULL);
	//Enable Vertex Attrib Array
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	//Unbind Buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Unbind array
	glBindVertexArray(0);

	//Create vao_line
	//Save everying in single set
	glGenVertexArrays(1, &vao_point);

	glBindVertexArray(vao_point);

	//POINT
	//Generate Buffer
	glGenBuffers(1, &vbo_position_point);
	//Bind Generated Buffer
	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_position_point);
	//Fill Buffer
	glBufferData(GL_ARRAY_BUFFER,
		3 * sizeof(float)*((2 * 3.1415) / 0.001),
		NULL,
		GL_DYNAMIC_DRAW);
	//Set Vertex Attrib Pointer
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		NULL);
	//Enable Vertex Attrib Array
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	//Unbind Buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_color_point);
	//Bind Generated Buffer
	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_color_point);
	//Fill Buffer
	glBufferData(GL_ARRAY_BUFFER,
		3 * sizeof(float)*((2 * 3.1415) / 0.001),
		NULL,
		GL_DYNAMIC_DRAW);
	//Set Vertex Attrib Pointer
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		NULL);
	//Enable Vertex Attrib Array
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	//Unbind Buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Unbind array
	glBindVertexArray(0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0f);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

	perspectiveProjectionMatrix = mat4::identity();

	Resize(WIN_WIDTH, WIN_HEIGHT);
	return 0;
}
//Function Resize
void Resize(int width, int height) {
	if (height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	perspectiveProjectionMatrix = perspective(45.0f,
		(GLfloat)width / (GLfloat)height,
		0.1f,
		100.0f);

}

//Function Display
void Display(void) {
	//Vertices

	void DrawTriangle(float);
	void DrawInCircle(float x1, float y1, float z1,
		float x2, float y2, float z2,
		float x3, float y3, float z3
	);


	void DrawLine(float x1, float y1, float z1,
		float x2, float y2, float z2);


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(gShaderProgramObject);

	//Declaration of Matrices
	mat4 modelViewMatrix;
	mat4 modelViewProjectionMatrix;
	mat4 translationMatrix;
	mat4 rotationMatrix;

	//Initialize matrices

	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	//Transformation

	translationMatrix = translate(0.0f, 0.0f, -5.0f);
	rotationMatrix = rotate(angleRotation, 0.0f, 1.0f, 0.0f);

	//Matrix Multiplication
	modelViewMatrix = translationMatrix * rotationMatrix;

	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	//Send necessary matrices to shader in resp. Uniforms

	glUniformMatrix4fv(mvpUniform,
		1,
		GL_FALSE,
		modelViewProjectionMatrix);

	//Bind with vao

	glBindVertexArray(vao_line);

	//Bind with textures if any

	glLineWidth(3.0f);
	glPointSize(3.0f);

	DrawLine(0.0f, 1.0f, 0.0f,
		0.0f, -1.0f, 0.0f);

	DrawTriangle(1.0f);

	//Unbind vao
	glBindVertexArray(0);

	glBindVertexArray(vao_point);

	DrawInCircle(0.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f);

	glBindVertexArray(0);

	glUseProgram(0);

	SwapBuffers(ghdc);

}
void Update(void)
{
	angleRotation = angleRotation + 0.2f;
	if (angleRotation > 360.0f)
		angleRotation = 0.0f;
}
void UnInitialize(void) {
	if (bFullScreen == true) {
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd,
			HWND_TOP,
			0,
			0,
			0,
			0,
			SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);
		//ShowCursor(TRUE);
	}

	if (vbo_position_lines) {
		glDeleteBuffers(1, &vbo_position_lines);
		vbo_position_lines = 0;
	}

	if (vbo_color_lines) {
		glDeleteBuffers(1, &vbo_color_lines);
		vbo_color_lines = 0;
	}

	if (vbo_position_point) {
		glDeleteBuffers(1, &vbo_position_point);
		vbo_position_point = 0;
	}

	if (vbo_color_point) {
		glDeleteBuffers(1, &vbo_color_point);
		vbo_color_point = 0;
	}

	if (vao_line) {
		glDeleteVertexArrays(1, &vao_line);
		vao_line = 0;
	}

	if (vao_point) {
		glDeleteVertexArrays(1, &vao_point);
		vao_point = 0;
	}

	if (gShaderProgramObject) {
		GLsizei shaderCount;
		GLsizei shaderNumber;
		glUseProgram(gShaderProgramObject);

		glGetProgramiv(gShaderProgramObject,
			GL_ATTACHED_SHADERS,
			&shaderCount);

		GLuint * pShaders = (GLuint *)malloc(shaderCount * sizeof(GLuint));

		if (pShaders) {
			glGetAttachedShaders(gShaderProgramObject,
				shaderCount,
				&shaderCount,
				pShaders);

			for (shaderNumber = 0; shaderNumber < shaderCount; shaderNumber++) {
				glDetachShader(gShaderProgramObject, pShaders[shaderNumber]);
				glDeleteShader(pShaders[shaderNumber]);
				pShaders[shaderNumber] = 0;
			}
			free(pShaders);
		}
		glDeleteProgram(gShaderProgramObject);
		gShaderProgramObject = 0;
		glUseProgram(0);
	}

	if (wglGetCurrentContext() == ghrc) {
		wglMakeCurrent(NULL, NULL);
	}
	if (ghrc) {
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}
	if (ghdc) {
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}
	if (gpFile) {
		fprintf_s(gpFile, "Log File Closed Successfully");
		fclose(gpFile);
		gpFile = NULL;
	}
}


void DrawTriangle(float offset)
{
	GLfloat lineVertices[6];

	GLfloat lineColor[] = {
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f
	};

	lineVertices[0] = 0.0f;
	lineVertices[1] = offset;
	lineVertices[2] = 0.0f;
	lineVertices[3] = -offset;
	lineVertices[4] = -offset;
	lineVertices[5] = 0.0f;

	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_position_lines);
	glBufferData(GL_ARRAY_BUFFER,
		3 * 2 * sizeof(float),
		lineVertices,
		GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_color_lines);
	glBufferData(GL_ARRAY_BUFFER,
		3 * 2 * sizeof(float),
		lineColor,
		GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Draw

	glDrawArrays(GL_LINES,
		0,
		2);

	lineVertices[0] = -offset;
	lineVertices[1] = -offset;
	lineVertices[2] = 0.0f;
	lineVertices[3] = offset;
	lineVertices[4] = -offset;
	lineVertices[5] = 0.0f;

	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_position_lines);
	glBufferData(GL_ARRAY_BUFFER,
		3 * 2 * sizeof(float),
		lineVertices,
		GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_color_lines);
	glBufferData(GL_ARRAY_BUFFER,
		3 * 2 * sizeof(float),
		lineColor,
		GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Draw

	glDrawArrays(GL_LINES,
		0,
		2);

	lineVertices[0] = offset;
	lineVertices[1] = -offset;
	lineVertices[2] = 0.0f;
	lineVertices[3] = 0.0f;
	lineVertices[4] = offset;
	lineVertices[5] = 0.0f;

	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_position_lines);
	glBufferData(GL_ARRAY_BUFFER,
		3 * 2 * sizeof(float),
		lineVertices,
		GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_color_lines);
	glBufferData(GL_ARRAY_BUFFER,
		3 * 2 * sizeof(float),
		lineColor,
		GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Draw

	glDrawArrays(GL_LINES,
		0,
		2);

}

void DrawInCircle(float x1, float y1, float z1,
	float x2, float y2, float z2,
	float x3, float y3, float z3
)
{
	static float ArrInCirclePos[(unsigned int)((2 * 3.1415) / 0.001) * 3];
	static float ArrInCircleColor[(unsigned int)((2 * 3.1415) / 0.001) * 3];

	int isArrInCircleInit = 0;

	if (isArrInCircleInit == 0) {

		float count = -1;
		int noCount = 2000;
		GLfloat angle = 0;

		float distP1P2 = sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1) + (z2 - z1)*(z2 - z1));
		float distP2P3 = sqrt((x3 - x2)*(x3 - x2) + (y3 - y2)*(y3 - y2) + (z3 - z2)*(z3 - z2));
		float distP3P1 = sqrt((x1 - x3)*(x1 - x3) + (y1 - y3)*(y1 - y3) + (z1 - z3)*(z1 - z3));

		float semiPerimeter = (distP1P2 + distP2P3 + distP3P1) / 2;

		float radius = sqrt((semiPerimeter - distP1P2)*(semiPerimeter - distP2P3)*(semiPerimeter - distP3P1) / semiPerimeter);

		float Ox = (x3 * distP1P2 + x1 * distP2P3 + x2 * distP3P1) / (semiPerimeter * 2);
		float Oy = (y3 * distP1P2 + y1 * distP2P3 + y2 * distP3P1) / (semiPerimeter * 2);
		float Oz = (z3 * distP1P2 + z1 * distP2P3 + z2 * distP3P1) / (semiPerimeter * 2);

		int indexPos = 0;
		int indexColor = 0;

		for (angle = 0.0f; angle < 2 * 3.14159265; angle = angle + 0.001f) {
			ArrInCirclePos[indexPos++] = cos(angle)*radius + Ox;
			ArrInCirclePos[indexPos++] = sin(angle)*radius + Oy;
			ArrInCirclePos[indexPos++] = 0.0f + Oz;

			ArrInCircleColor[indexColor++] = 1.0f;
			ArrInCircleColor[indexColor++] = 1.0f;
			ArrInCircleColor[indexColor++] = 1.0f;
		}
		isArrInCircleInit = 1;
	}


	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_color_point);
	glBufferData(GL_ARRAY_BUFFER,
		3 * sizeof(float)*((2 * 3.1415) / 0.001),
		ArrInCircleColor,
		GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_position_point);
	glBufferData(GL_ARRAY_BUFFER,
		3 * sizeof(float)*((2 * 3.1415) / 0.001),
		ArrInCirclePos,
		GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Draw

	glDrawArrays(GL_POINTS,
		0,
		((2 * 3.1415) / 0.001));


	//float count = -1;
	//int noCount = 2000;
	//GLfloat angle = 0;

	//float distP1P2 = sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1) + (z2 - z1)*(z2 - z1));
	//float distP2P3 = sqrt((x3 - x2)*(x3 - x2) + (y3 - y2)*(y3 - y2) + (z3 - z2)*(z3 - z2));
	//float distP3P1 = sqrt((x1 - x3)*(x1 - x3) + (y1 - y3)*(y1 - y3) + (z1 - z3)*(z1 - z3));

	//float semiPerimeter = (distP1P2 + distP2P3 + distP3P1) / 2;

	//float radius = sqrt((semiPerimeter - distP1P2)*(semiPerimeter - distP2P3)*(semiPerimeter - distP3P1) / semiPerimeter);

	//float Ox = (x3 * distP1P2 + x1 * distP2P3 + x2 * distP3P1) / (semiPerimeter * 2);
	//float Oy = (y3 * distP1P2 + y1 * distP2P3 + y2 * distP3P1) / (semiPerimeter * 2);
	//float Oz = (z3 * distP1P2 + z1 * distP2P3 + z2 * distP3P1) / (semiPerimeter * 2);

	//float pointVertices[3];
	//GLfloat pointColors[] = {
	//	1.0f,1.0f,0.0f
	//};

	//glBindBuffer(GL_ARRAY_BUFFER,
	//	vbo_color_point);
	//glBufferData(GL_ARRAY_BUFFER,
	//	3 * sizeof(float),
	//	pointColors,
	//	GL_DYNAMIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	//for (angle = 0.0f; angle < 2 * 3.14159265; angle = angle + 0.001f) {

	//	pointVertices[0] = cos(angle)*radius+Ox;
	//	pointVertices[1] = sin(angle)*radius+Oy;
	//	pointVertices[2] = 0.0f+Oz;

	//	glBindBuffer(GL_ARRAY_BUFFER,
	//		vbo_position_point);
	//	glBufferData(GL_ARRAY_BUFFER,
	//		3 * sizeof(float),
	//		pointVertices,
	//		GL_DYNAMIC_DRAW);
	//	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//	//Draw

	//	glDrawArrays(GL_POINTS,
	//		0,
	//		1);
	//}
}

void DrawLine(float x1,float y1,float z1,
	float x2,float y2,float z2) {

	GLfloat lineVertices[6];

	GLfloat lineColor[] = {
		1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f
	};

	lineVertices[0] = x1;
	lineVertices[1] = y1;
	lineVertices[2] = z1;
	lineVertices[3] = x2;
	lineVertices[4] = y2;
	lineVertices[5] = z2;

	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_position_lines);
	glBufferData(GL_ARRAY_BUFFER,
		3 * 2 * sizeof(float),
		lineVertices,
		GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_color_lines);
	glBufferData(GL_ARRAY_BUFFER,
		3 * 2 * sizeof(float),
		lineColor,
		GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Draw

	glDrawArrays(GL_LINES,
		0,
		2);

}

//void PrintTime() {
//	struct tm newtime;
//	__time64_t long_time;
//	errno_t err;
//	_time64(&long_time);// Get time as 64-bit integer.
//	err = _localtime64_s(&newtime, &long_time);// Convert to local time.
//	if (!err && gpFile) {
//		fprintf_s(gpFile, "%i:%i:%i %i-%i-%i ",newtime.tm_mday,newtime.tm_mon+1,newtime.tm_year+1900,newtime.tm_hour,newtime.tm_min,newtime.tm_sec);
//	}
//}
