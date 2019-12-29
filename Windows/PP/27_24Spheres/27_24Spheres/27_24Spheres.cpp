#include<Windows.h>
#include<stdio.h>

#include<GL\glew.h>
#include<gl\GL.h>
//#include <time.h>

#include"vmath.h"
#include"Sphere.h"

#pragma comment(lib,"OpenGL32.lib")
#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"Sphere.lib")

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

GLfloat lightAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat lightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightSpecular[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat lightPosition[] = { 0.0f,0.0f,0.0f,1.0f };
GLint keyPress = 0;

//Shader Program Objects
GLint gShaderProgramObject;

enum {
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXCOORD0
};

GLuint vao_sphere;

GLuint vbo_sphere_element;
GLuint vbo_sphere_position;
GLuint vbo_sphere_normal;

//GLuint vbo_color_cube;

GLuint mUniform;
GLuint vUniform;
GLuint pUniform;
GLuint lKeyIsPressedUniform;
GLuint ldUniform;
GLuint kdUniform;
GLuint laUniform;
GLuint kaUniform;
GLuint lsUniform;
GLuint ksUniform;
GLuint lightPositionUniform;
GLuint shininessUniform;

mat4 perspectiveProjectionMatrix;

//GLuint gb_Animation = 0;
GLuint gb_Lighting = 0;

float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];
int gNumVertices;
int gNumElements;
float angleRotation = 0.0f;

GLsizei gWidth;
GLsizei gHeight;

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
		case 'l':
		case 'L':
			if (gb_Lighting == 0)
				gb_Lighting = 1;
			else
				gb_Lighting = 0;
			break;
		case 'x':
		case 'X':
			keyPress = 1;
			lightPosition[0] = 0.0f;
			lightPosition[1] = 0.0f;
			lightPosition[2] = 0.0f;

			angleRotation = 0.0f;
			break;
		case 'y':
		case 'Y':
			keyPress = 2;
			lightPosition[0] = 0.0f;
			lightPosition[1] = 0.0f;
			lightPosition[2] = 0.0f;

			angleRotation = 0.0f;
			break;
		case 'z':
		case 'Z':
			keyPress = 3;
			lightPosition[0] = 0.0f;
			lightPosition[1] = 0.0f;
			lightPosition[2] = 0.0f;

			angleRotation = 0.0f;
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

	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumElements = getNumberOfSphereElements();
	gNumVertices = getNumberOfSphereVertices();

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
		"in vec3 vNormal;" \
		"uniform mat4 u_m_matrix;" \
		"uniform mat4 u_v_matrix;" \
		"uniform mat4 u_p_matrix;" \
		"uniform int u_lkeyispressed;" \
		"uniform vec4 u_lightposition;" \
		"out vec3 tnorm;"\
		"out vec3 lightDirection;"\
		"out vec3 reflectionVector;"\
		"out vec3 viewerVector;"\
		"void main(void)" \
		"{" \
		"gl_Position=u_p_matrix*u_v_matrix*u_m_matrix * vPosition;" \
		"if(u_lkeyispressed==1)"\
		"{"\
		"vec4 eyecoordinates=u_v_matrix*u_m_matrix*vPosition;"\
		"tnorm=mat3(u_v_matrix*u_m_matrix)*vNormal;"\
		"lightDirection=vec3(u_lightposition-eyecoordinates);"\
		"reflectionVector=reflect(-lightDirection,tnorm);"\
		"viewerVector=vec3(-eyecoordinates);"\
		"}"\
		"}";

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
		"uniform vec3 u_ld;" \
		"uniform vec3 u_kd;" \
		"uniform vec3 u_la;" \
		"uniform vec3 u_ka;" \
		"uniform vec3 u_ls;" \
		"uniform vec3 u_ks;" \
		"uniform float u_shininess;" \
		"uniform int u_lkeyispressed;" \
		"in vec3 tnorm;"\
		"in vec3 lightDirection;"\
		"in vec3 reflectionVector;"\
		"in vec3 viewerVector;"\
		"void main(void)" \
		"{" \
		"vec3 phongADSLight;"\
		"if(u_lkeyispressed==1)"\
		"{"\
		"vec3 tnorm_normalize=normalize(tnorm);"\
		"vec3 lightDirection_normalize=normalize(lightDirection);"\
		"vec3 reflectionVector_normalize=normalize(reflectionVector);"\
		"vec3 viewerVector_normalize=normalize(viewerVector);"\
		"float tn_dot_ld=max(dot(lightDirection_normalize,tnorm_normalize),0.0);"\
		"vec3 ambient=u_la*u_ka;"\
		"vec3 diffuse=u_ld*u_kd*tn_dot_ld;"
		"vec3 specular=u_ls*u_ks*pow(max(dot(reflectionVector_normalize,viewerVector_normalize),0.0),u_shininess);"\
		"phongADSLight=ambient+diffuse+specular;"\
		"}"\
		"else"\
		"{"\
		"phongADSLight=vec3(1.0f,1.0f,1.0f);"\
		"}"\
		"fragColor=vec4(phongADSLight,1.0f);"\
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
		AMC_ATTRIBUTE_NORMAL,
		"vNormal");

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
	mUniform = glGetUniformLocation(gShaderProgramObject,
		"u_m_matrix");
	vUniform = glGetUniformLocation(gShaderProgramObject,
		"u_v_matrix");
	pUniform = glGetUniformLocation(gShaderProgramObject,
		"u_p_matrix");
	lKeyIsPressedUniform = glGetUniformLocation(gShaderProgramObject,
		"u_lkeyispressed");
	ldUniform = glGetUniformLocation(gShaderProgramObject,
		"u_ld");
	kdUniform = glGetUniformLocation(gShaderProgramObject,
		"u_kd");
	laUniform = glGetUniformLocation(gShaderProgramObject,
		"u_la");
	kaUniform = glGetUniformLocation(gShaderProgramObject,
		"u_ka");
	lsUniform = glGetUniformLocation(gShaderProgramObject,
		"u_ls");
	ksUniform = glGetUniformLocation(gShaderProgramObject,
		"u_ks");
	lightPositionUniform = glGetUniformLocation(gShaderProgramObject,
		"u_lightposition");
	shininessUniform = glGetUniformLocation(gShaderProgramObject,
		"u_shininess");

	// vao
	glGenVertexArrays(1, &vao_sphere);
	glBindVertexArray(vao_sphere);

	// position vbo
	glGenBuffers(1, &vbo_sphere_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_sphere_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// normal vbo
	glGenBuffers(1, &vbo_sphere_normal);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_sphere_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// element vbo
	glGenBuffers(1, &vbo_sphere_element);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// unbind vao
	glBindVertexArray(0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0f);

	//glEnable(GL_CULL_FACE);
	//glDisable(GL_CULL_FACE);

	//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);

	glEnable(GL_TEXTURE_2D);

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

	gWidth = width;
	gHeight = height;

	/*if (width <= height) {
		perspectiveProjectionMatrix = ortho(0.0f,
			15.0f,
			0.0f,
			(15.0f*(GLfloat)height / (GLfloat)width),
			-10.0f,
			10.0f
		);
	}
	else
	{
		perspectiveProjectionMatrix = ortho(
			0.0f,
			(15.0f*(GLfloat)width / (GLfloat)height),
			0.0f,
			15.5f,
			-10.0f,
			10.0f);
	}*/


}
void draw24Sphere();

void drawSphere(vmath::mat4 &modelMatrix, vmath::mat4 &translationMatrix, vmath::mat4 &scaleMatrix, vmath::mat4 &rotationMatrix, vmath::mat4 &viewMatrix, GLfloat  materialAmbient[4], GLfloat  materialDiffuse[4], GLfloat  materialSpecular[4], const GLfloat &materialShininess);

//Function Display
void Display(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(gShaderProgramObject);

	draw24Sphere();
	////Declaration of Matrices
	//mat4 modelMatrix;
	//mat4 viewMatrix;
	//mat4 translationMatrix;
	//mat4 rotationMatrix;
	//mat4 scaleMatrix;

	////Initialize matrices

	//modelMatrix = mat4::identity();
	//viewMatrix = mat4::identity();
	//translationMatrix = mat4::identity();
	//rotationMatrix = mat4::identity();
	//scaleMatrix = mat4::identity();

	////Transformation

	//translationMatrix = translate(0.0f, 0.0f, -3.0f);
	//modelMatrix = translationMatrix * scaleMatrix * rotationMatrix;

	//glUniformMatrix4fv(mUniform,
	//	1,
	//	GL_FALSE,
	//	modelMatrix);
	//glUniformMatrix4fv(vUniform,
	//	1,
	//	GL_FALSE,
	//	viewMatrix);
	//glUniformMatrix4fv(pUniform,
	//	1,
	//	GL_FALSE,
	//	perspectiveProjectionMatrix);

	//if (gb_Lighting) {
	//	glUniform1i(lKeyIsPressedUniform, gb_Lighting);

	//	glUniform3fv(ldUniform,1,lightDiffuse);
	//	glUniform3fv(laUniform, 1, lightAmbient);
	//	glUniform3fv(lsUniform, 1, lightSpecular);

	//	glUniform3fv(kaUniform, 1, materialAmbient);
	//	glUniform3fv(kdUniform, 1, materialDiffuse);
	//	glUniform3fv(ksUniform, 1, materialSpecular);

	//	glUniform1f(shininessUniform, materialShininess);

	//	glUniform4fv(lightPositionUniform, 1, lightPosition);
	//}
	//else {
	//	glUniform1i(lKeyIsPressedUniform, gb_Lighting);
	//}

	//// *** bind vao ***
	//glBindVertexArray(vao_sphere);

	//// *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
	//glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	//// *** unbind vao ***
	//glBindVertexArray(0);

	glUseProgram(0);

	SwapBuffers(ghdc);

}


void Update(void)
{
	angleRotation = angleRotation + 0.01f;
	if (angleRotation > 2 * 3.14159265)
		angleRotation = 0.0f;

	if (keyPress == 1) {
		lightPosition[1] = 100 * sin(angleRotation);
		lightPosition[2] = 100 * cos(angleRotation);
	}
	if (keyPress == 2) {
		lightPosition[0] = 100 * sin(angleRotation);
		lightPosition[2] = 100 * cos(angleRotation);
	}
	if (keyPress == 3) {
		lightPosition[0] = 100 * sin(angleRotation);
		lightPosition[1] = 100 * cos(angleRotation);
	}

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

	if (vao_sphere) {
		glDeleteVertexArrays(1, &vao_sphere);
		vao_sphere = 0;
	}

	if (vbo_sphere_element) {
		glDeleteVertexArrays(1, &vbo_sphere_element);
		vbo_sphere_element = 0;
	}
	if (vbo_sphere_position) {
		glDeleteVertexArrays(1, &vbo_sphere_position);
		vbo_sphere_position = 0;
	}
	if (vbo_sphere_normal) {
		glDeleteVertexArrays(1, &vbo_sphere_normal);
		vbo_sphere_normal = 0;
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

void draw24Sphere()
{

	//Declaration of Matrices
	mat4 modelMatrix;
	mat4 viewMatrix;
	mat4 translationMatrix;
	mat4 rotationMatrix;
	mat4 scaleMatrix;

	GLfloat materialAmbient[4];
	GLfloat materialDiffuse[4];
	GLfloat materialSpecular[4];
	GLfloat materialShininess;
	// ***** 1st sphere on 1st column, emerald *****
	// ambient material
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	
	materialAmbient[0] = 0.0215; // r
	materialAmbient[1] = 0.1745; // g
	materialAmbient[2] = 0.0215; // b
	materialAmbient[3] = 1.0f;   // a
	 
	// diffuse material
	materialDiffuse[0] = 0.07568; // r
	materialDiffuse[1] = 0.61424; // g
	materialDiffuse[2] = 0.07568; // b
	materialDiffuse[3] = 1.0f;    // a
	 
	// specular material
	materialSpecular[0] = 0.633;    // r
	materialSpecular[1] = 0.727811; // g
	materialSpecular[2] = 0.633;    // b
	materialSpecular[3] = 1.0f;     // a
	 
	// shininess
	materialShininess = 0.6 * 128;
	
	glViewport(gWidth*(-0.45f), gHeight*(0.35f), gWidth , gHeight);

	//translationMatrix = translate(-6.0f, 3.5f, -12.0f);
	translationMatrix = translate(0.0f, 0.0f, -12.0f);

	//glViewport(0, (gHeight-(gHeight/1.1)), gWidth/1.10, gHeight/1.1);

	drawSphere(modelMatrix, translationMatrix, scaleMatrix, rotationMatrix, viewMatrix, materialAmbient, materialDiffuse, materialSpecular, materialShininess);


	// ***** 2nd sphere on 1st column, jade *****
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	// ambient material
	materialAmbient[0] = 0.135;  // r
	materialAmbient[1] = 0.2225; // g
	materialAmbient[2] = 0.1575; // b
	materialAmbient[3] = 1.0f;   // a
	
	// diffuse material
	materialDiffuse[0] = 0.54; // r
	materialDiffuse[1] = 0.89; // g
	materialDiffuse[2] = 0.63; // b
	materialDiffuse[3] = 1.0f; // a
	 
	// specular material
	materialSpecular[0] = 0.316228; // r
	materialSpecular[1] = 0.316228; // g
	materialSpecular[2] = 0.316228; // b
	materialSpecular[3] = 1.0f;     // a
	 
	// shininess
	materialShininess = 0.1 * 128;
	
	glViewport(gWidth*(-0.45f), gHeight*(0.20f), gWidth, gHeight);

	//translationMatrix = translate(-6.0f, 2.0f, -12.0f);
	translationMatrix = translate(0.0f, 0.0f, -12.0f);

	drawSphere(modelMatrix, translationMatrix, scaleMatrix, rotationMatrix, viewMatrix, materialAmbient, materialDiffuse, materialSpecular, materialShininess);


	// ***** 3rd sphere on 1st column, obsidian *****
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	// ambient material
	materialAmbient[0] = 0.05375; // r
	materialAmbient[1] = 0.05;    // g
	materialAmbient[2] = 0.06625; // b
	materialAmbient[3] = 1.0f;    // a
	 
	// diffuse material
	materialDiffuse[0] = 0.18275; // r
	materialDiffuse[1] = 0.17;    // g
	materialDiffuse[2] = 0.22525; // b
	materialDiffuse[3] = 1.0f;    // a
	 
	// specular material
	materialSpecular[0] = 0.332741; // r
	materialSpecular[1] = 0.328634; // g
	materialSpecular[2] = 0.346435; // b
	materialSpecular[3] = 1.0f;     // a
	 
	// shininess
	materialShininess = 0.3 * 128;
	
	glViewport(gWidth*(-0.45f), gHeight*(0.05f), gWidth, gHeight);


	translationMatrix = translate(0.0f, 0.0f, -12.0f);
	drawSphere(modelMatrix, translationMatrix, scaleMatrix, rotationMatrix, viewMatrix, materialAmbient, materialDiffuse, materialSpecular, materialShininess);
	// ***** 4th sphere on 1st column, pearl *****
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	// ambient material
	materialAmbient[0] = 0.25;    // r
	materialAmbient[1] = 0.20725; // g
	materialAmbient[2] = 0.20725; // b
	materialAmbient[3] = 1.0f;    // a
	 
	// diffuse material
	materialDiffuse[0] = 1.0;   // r
	materialDiffuse[1] = 0.829; // g
	materialDiffuse[2] = 0.829; // b
	materialDiffuse[3] = 1.0f;  // a
	 
	// specular material
	materialSpecular[0] = 0.296648; // r
	materialSpecular[1] = 0.296648; // g
	materialSpecular[2] = 0.296648; // b
	materialSpecular[3] = 1.0f;     // a
	 
	// shininess
	materialShininess = 0.088 * 128;
	 
	glViewport(gWidth*(-0.45f), gHeight*(-0.10), gWidth, gHeight);

	 
	translationMatrix = translate(0.0f, 0.0f, -12.0f);
	drawSphere(modelMatrix, translationMatrix, scaleMatrix, rotationMatrix, viewMatrix, materialAmbient, materialDiffuse, materialSpecular, materialShininess);
	// ***** 5th sphere on 1st column, ruby *****
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	// ambient material
	materialAmbient[0] = 0.1745;  // r
	materialAmbient[1] = 0.01175; // g
	materialAmbient[2] = 0.01175; // b
	materialAmbient[3] = 1.0f;    // a
	 
	// diffuse material
	materialDiffuse[0] = 0.61424; // r
	materialDiffuse[1] = 0.04136; // g
	materialDiffuse[2] = 0.04136; // b
	materialDiffuse[3] = 1.0f;    // a
	 
	// specular material
	materialSpecular[0] = 0.727811; // r
	materialSpecular[1] = 0.626959; // g
	materialSpecular[2] = 0.626959; // b
	materialSpecular[3] = 1.0f;     // a
	 
	// shininess
	materialShininess = 0.6 * 128;
	 
	glViewport(gWidth*(-0.45f), gHeight*(-0.25f), gWidth, gHeight);

	 
	translationMatrix = translate(0.0f, 0.0f, -12.0f);
	drawSphere(modelMatrix, translationMatrix, scaleMatrix, rotationMatrix, viewMatrix, materialAmbient, materialDiffuse, materialSpecular, materialShininess);
	// ***** 6th sphere on 1st column, turquoise *****
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	// ambient material
	materialAmbient[0] = 0.1;     // r
	materialAmbient[1] = 0.18725; // g
	materialAmbient[2] = 0.1745;  // b
	materialAmbient[3] = 1.0f;    // a
	 
	// diffuse material
	materialDiffuse[0] = 0.396;   // r
	materialDiffuse[1] = 0.74151; // g
	materialDiffuse[2] = 0.69102; // b
	materialDiffuse[3] = 1.0f;    // a
	 
	// specular material
	materialSpecular[0] = 0.297254; // r
	materialSpecular[1] = 0.30829;  // g
	materialSpecular[2] = 0.306678; // b
	materialSpecular[3] = 1.0f;     // a
	 
	// shininess
	materialShininess = 0.1 * 128;
	 
	glViewport(gWidth*(-0.45f), gHeight*(-0.40f), gWidth, gHeight);

	 
	translationMatrix = translate(0.0f, 0.0f, -12.0f);
	drawSphere(modelMatrix, translationMatrix, scaleMatrix, rotationMatrix, viewMatrix, materialAmbient, materialDiffuse, materialSpecular, materialShininess);


	// ***** 1st sphere on 2nd column, brass *****
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	// ambient material
	materialAmbient[0] = 0.329412; // r
	materialAmbient[1] = 0.223529; // g
	materialAmbient[2] = 0.027451; // b
	materialAmbient[3] = 1.0f;     // a
	 
	// diffuse material
	materialDiffuse[0] = 0.780392; // r
	materialDiffuse[1] = 0.568627; // g
	materialDiffuse[2] = 0.113725; // b
	materialDiffuse[3] = 1.0f;     // a
	 
	// specular material
	materialSpecular[0] = 0.992157; // r
	materialSpecular[1] = 0.941176; // g
	materialSpecular[2] = 0.807843; // b
	materialSpecular[3] = 1.0f;     // a
	 
	// shininess
	materialShininess = 0.21794872 * 128;
	 
	glViewport(gWidth*(-0.15f), gHeight*(0.35f), gWidth, gHeight);

	 
	translationMatrix = translate(0.0f, 0.0f, -12.0f);
	drawSphere(modelMatrix, translationMatrix, scaleMatrix, rotationMatrix, viewMatrix, materialAmbient, materialDiffuse, materialSpecular, materialShininess);

	glViewport(0,0, gWidth, gHeight);

	// ***** 2nd sphere on 2nd column, bronze *****
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	// ambient material
	materialAmbient[0] = 0.2125; // r
	materialAmbient[1] = 0.1275; // g
	materialAmbient[2] = 0.054;  // b
	materialAmbient[3] = 1.0f;   // a
	 
	// diffuse material
	materialDiffuse[0] = 0.714;   // r
	materialDiffuse[1] = 0.4284;  // g
	materialDiffuse[2] = 0.18144; // b
	materialDiffuse[3] = 1.0f;    // a
	 
	// specular material
	materialSpecular[0] = 0.393548; // r
	materialSpecular[1] = 0.271906; // g
	materialSpecular[2] = 0.166721; // b
	materialSpecular[3] = 1.0f;     // a
	 
	// shininess
	materialShininess = 0.2 * 128;
	 
	glViewport(gWidth*(-0.15f), gHeight*(0.20f), gWidth, gHeight);

	 
	translationMatrix = translate(0.0f, 0.0f, -12.0f);
	drawSphere(modelMatrix, translationMatrix, scaleMatrix, rotationMatrix, viewMatrix, materialAmbient, materialDiffuse, materialSpecular, materialShininess);
	// ***** 3rd sphere on 2nd column, chrome *****
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	// ambient material
	materialAmbient[0] = 0.25; // r
	materialAmbient[1] = 0.25; // g
	materialAmbient[2] = 0.25; // b
	materialAmbient[3] = 1.0f; // a
	 
	// diffuse material
	materialDiffuse[0] = 0.4;  // r
	materialDiffuse[1] = 0.4;  // g
	materialDiffuse[2] = 0.4;  // b
	materialDiffuse[3] = 1.0f; // a
	 
	// specular material
	materialSpecular[0] = 0.774597; // r
	materialSpecular[1] = 0.774597; // g
	materialSpecular[2] = 0.774597; // b
	materialSpecular[3] = 1.0f;     // a
	 
	// shininess
	materialShininess = 0.6 * 128;
	 
	glViewport(gWidth*(-0.15f), gHeight*(0.05f), gWidth, gHeight);

	 
	translationMatrix = translate(0.0f, 0.0f, -12.0f);
	drawSphere(modelMatrix, translationMatrix, scaleMatrix, rotationMatrix, viewMatrix, materialAmbient, materialDiffuse, materialSpecular, materialShininess);
	// ***** 4th sphere on 2nd column, copper *****
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	// ambient material
	materialAmbient[0] = 0.19125; // r
	materialAmbient[1] = 0.0735;  // g
	materialAmbient[2] = 0.0225;  // b
	materialAmbient[3] = 1.0f;    // a
	 
	// diffuse material
	materialDiffuse[0] = 0.7038;  // r
	materialDiffuse[1] = 0.27048; // g
	materialDiffuse[2] = 0.0828;  // b
	materialDiffuse[3] = 1.0f;    // a
	 
	// specular material
	materialSpecular[0] = 0.256777; // r
	materialSpecular[1] = 0.137622; // g
	materialSpecular[2] = 0.086014; // b
	materialSpecular[3] = 1.0f;     // a
	 
	// shininess
	materialShininess = 0.1 * 128;
	 
	glViewport(gWidth*(-0.15f), gHeight*(-0.1f), gWidth, gHeight);

	 
	translationMatrix = translate(0.0f, 0.0f, -12.0f);
	drawSphere(modelMatrix, translationMatrix, scaleMatrix, rotationMatrix, viewMatrix, materialAmbient, materialDiffuse, materialSpecular, materialShininess);
	// ***** 5th sphere on 2nd column, gold *****
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	// ambient material
	materialAmbient[0] = 0.24725; // r
	materialAmbient[1] = 0.1995;  // g
	materialAmbient[2] = 0.0745;  // b
	materialAmbient[3] = 1.0f;    // a
	 
	// diffuse material
	materialDiffuse[0] = 0.75164; // r
	materialDiffuse[1] = 0.60648; // g
	materialDiffuse[2] = 0.22648; // b
	materialDiffuse[3] = 1.0f;    // a
	 
	// specular material
	materialSpecular[0] = 0.628281; // r
	materialSpecular[1] = 0.555802; // g
	materialSpecular[2] = 0.366065; // b
	materialSpecular[3] = 1.0f;     // a
	 
	// shininess
	materialShininess = 0.4 * 128;
	 
	glViewport(gWidth*(-0.15f), gHeight*(-0.25f), gWidth, gHeight);

	 
	translationMatrix = translate(0.0f, 0.0f, -12.0f);
	drawSphere(modelMatrix, translationMatrix, scaleMatrix, rotationMatrix, viewMatrix, materialAmbient, materialDiffuse, materialSpecular, materialShininess);
	// ***** 6th sphere on 2nd column, silver *****
	// ambient material
	materialAmbient[0] = 0.19225; // r
	materialAmbient[1] = 0.19225; // g
	materialAmbient[2] = 0.19225; // b
	materialAmbient[3] = 1.0f;    // a
	 
	// diffuse material
	materialDiffuse[0] = 0.50754; // r
	materialDiffuse[1] = 0.50754; // g
	materialDiffuse[2] = 0.50754; // b
	materialDiffuse[3] = 1.0f;    // a
	 
	// specular material
	materialSpecular[0] = 0.508273; // r
	materialSpecular[1] = 0.508273; // g
	materialSpecular[2] = 0.508273; // b
	materialSpecular[3] = 1.0f;     // a
	 
	// shininess
	materialShininess = 0.4 * 128;
	 
	glViewport(gWidth*(-0.15f), gHeight*(-0.40f), gWidth, gHeight);

	 
	translationMatrix = translate(0.0f, 0.0f, -12.0f);
	drawSphere(modelMatrix, translationMatrix, scaleMatrix, rotationMatrix, viewMatrix, materialAmbient, materialDiffuse, materialSpecular, materialShininess);
	//// ***** 1st sphere on 3rd column, black *****
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	// ambient material
	materialAmbient[0] = 0.0;  // r
	materialAmbient[1] = 0.0;  // g
	materialAmbient[2] = 0.0;  // b
	materialAmbient[3] = 1.0f; // a
	 
	// diffuse material
	materialDiffuse[0] = 0.1; // r
	materialDiffuse[1] = 0.1; // g
	materialDiffuse[2] = 0.1; // b
	materialDiffuse[3] = 1.0f; // a
	 
	// specular material
	materialSpecular[0] = 0.50; // r
	materialSpecular[1] = 0.50; // g
	materialSpecular[2] = 0.50; // b
	materialSpecular[3] = 1.0f; // a
	 
	// shininess
	materialShininess = 0.25 * 128;
	 
	glViewport(gWidth*(0.15f), gHeight*(0.35f), gWidth, gHeight);

	 
	translationMatrix = translate(0.0f, 0.0f, -12.0f);
	drawSphere(modelMatrix, translationMatrix, scaleMatrix, rotationMatrix, viewMatrix, materialAmbient, materialDiffuse, materialSpecular, materialShininess);
	// ***** 2nd sphere on 3rd column, cyan *****
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	// ambient material
	materialAmbient[0] = 0.0;  // r
	materialAmbient[1] = 0.1;  // g
	materialAmbient[2] = 0.06; // b
	materialAmbient[3] = 1.0f; // a
	 
	// diffuse material
	materialDiffuse[0] = 0.0;        // r
	materialDiffuse[1] = 0.50980392; // g
	materialDiffuse[2] = 0.50980392; // b
	materialDiffuse[3] = 1.0f;       // a
	 
	// specular material
	materialSpecular[0] = 0.50196078; // r
	materialSpecular[1] = 0.50196078; // g
	materialSpecular[2] = 0.50196078; // b
	materialSpecular[3] = 1.0f;       // a
	 
	// shininess
	materialShininess = 0.25 * 128;
	 
	glViewport(gWidth*(0.15f), gHeight*(0.20f), gWidth, gHeight);

	 
	translationMatrix = translate(0.0f, 0.0f, -12.0f);
	drawSphere(modelMatrix, translationMatrix, scaleMatrix, rotationMatrix, viewMatrix, materialAmbient, materialDiffuse, materialSpecular, materialShininess);
	// ***** 3rd sphere on 2nd column, green *****
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	// ambient material
	materialAmbient[0] = 0.0;  // r
	materialAmbient[1] = 0.0;  // g
	materialAmbient[2] = 0.0;  // b
	materialAmbient[3] = 1.0f; // a
	 
	// diffuse material
	materialDiffuse[0] = 0.1;  // r
	materialDiffuse[1] = 0.35; // g
	materialDiffuse[2] = 0.1;  // b
	materialDiffuse[3] = 1.0f; // a
	 
	// specular material
	materialSpecular[0] = 0.45; // r
	materialSpecular[1] = 0.55; // g
	materialSpecular[2] = 0.45; // b
	materialSpecular[3] = 1.0f; // a
	 
	// shininess
	materialShininess = 0.25 * 128;
	 
	glViewport(gWidth*(0.15f), gHeight*(0.05f), gWidth, gHeight);

	 
	translationMatrix = translate(0.0f, 0.0f, -12.0f);
	drawSphere(modelMatrix, translationMatrix, scaleMatrix, rotationMatrix, viewMatrix, materialAmbient, materialDiffuse, materialSpecular, materialShininess);
	// ***** 4th sphere on 3rd column, red *****
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	// ambient material
	materialAmbient[0] = 0.0;  // r
	materialAmbient[1] = 0.0;  // g
	materialAmbient[2] = 0.0;  // b
	materialAmbient[3] = 1.0f; // a
	 
	// diffuse material
	materialDiffuse[0] = 0.5;  // r
	materialDiffuse[1] = 0.0;  // g
	materialDiffuse[2] = 0.0;  // b
	materialDiffuse[3] = 1.0f; // a
	 
	// specular material
	materialSpecular[0] = 0.7;  // r
	materialSpecular[1] = 0.6;  // g
	materialSpecular[2] = 0.6;  // b
	materialSpecular[3] = 1.0f; // a
	 
	// shininess
	materialShininess = 0.25 * 128;
	 
	glViewport(gWidth*(0.15f), gHeight*(-0.10f), gWidth, gHeight);

	 
	translationMatrix = translate(0.0f, 0.0f, -12.0f);
	drawSphere(modelMatrix, translationMatrix, scaleMatrix, rotationMatrix, viewMatrix, materialAmbient, materialDiffuse, materialSpecular, materialShininess);
	// ***** 5th sphere on 3rd column, white *****
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	// ambient material
	materialAmbient[0] = 0.0;  // r
	materialAmbient[1] = 0.0;  // g
	materialAmbient[2] = 0.0;  // b
	materialAmbient[3] = 1.0f; // a
	 
	// diffuse material
	materialDiffuse[0] = 0.55; // r
	materialDiffuse[1] = 0.55; // g
	materialDiffuse[2] = 0.55; // b
	materialDiffuse[3] = 1.0f; // a
	 
	// specular material
	materialSpecular[0] = 0.70; // r
	materialSpecular[1] = 0.70; // g
	materialSpecular[2] = 0.70; // b
	materialSpecular[3] = 1.0f; // a
	 
	// shininess
	materialShininess = 0.25 * 128;
	 
	glViewport(gWidth*(0.15f), gHeight*(-0.25f), gWidth, gHeight);

	 
	translationMatrix = translate(0.0f, 0.0f, -12.0f);
	drawSphere(modelMatrix, translationMatrix, scaleMatrix, rotationMatrix, viewMatrix, materialAmbient, materialDiffuse, materialSpecular, materialShininess);
	// ***** 6th sphere on 3rd column, yellow plastic *****
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	// ambient material
	materialAmbient[0] = 0.0;  // r
	materialAmbient[1] = 0.0;  // g
	materialAmbient[2] = 0.0;  // b
	materialAmbient[3] = 1.0f; // a
	 
	// diffuse material
	materialDiffuse[0] = 0.5;  // r
	materialDiffuse[1] = 0.5;  // g
	materialDiffuse[2] = 0.0;  // b
	materialDiffuse[3] = 1.0f; // a
	 
	// specular material
	materialSpecular[0] = 0.60; // r
	materialSpecular[1] = 0.60; // g
	materialSpecular[2] = 0.50; // b
	materialSpecular[3] = 1.0f; // a
	 
	// shininess
	materialShininess = 0.25 * 128;
	 
	glViewport(gWidth*(0.15f), gHeight*(-0.40f), gWidth, gHeight);

	 
	translationMatrix = translate(0.0f, 0.0f, -12.0f);
	drawSphere(modelMatrix, translationMatrix, scaleMatrix, rotationMatrix, viewMatrix, materialAmbient, materialDiffuse, materialSpecular, materialShininess);


	// ***** 1st sphere on 4th column, black *****
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	// ambient material
	materialAmbient[0] = 0.02; // r
	materialAmbient[1] = 0.02; // g
	materialAmbient[2] = 0.02; // b
	materialAmbient[3] = 1.0f; // a
	 
	// diffuse material
	materialDiffuse[0] = 0.1; // r
	materialDiffuse[1] = 0.1; // g
	materialDiffuse[2] = 0.1; // b
	materialDiffuse[3] = 1.0f; // a
	 
	// specular material
	materialSpecular[0] = 0.4;  // r
	materialSpecular[1] = 0.4;  // g
	materialSpecular[2] = 0.4;  // b
	materialSpecular[3] = 1.0f; // a
	 
	// shininess
	materialShininess = 0.078125 * 128;
	 
	 
	glViewport(gWidth*(0.40f), gHeight*(0.35f), gWidth, gHeight);

	translationMatrix = translate(0.0f, 0.0f, -12.0f);
	drawSphere(modelMatrix, translationMatrix, scaleMatrix, rotationMatrix, viewMatrix, materialAmbient, materialDiffuse, materialSpecular, materialShininess);
	// ***** 2nd sphere on 4th column, cyan *****
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	// ambient material
	materialAmbient[0] = 0.0;  // r
	materialAmbient[1] = 0.05; // g
	materialAmbient[2] = 0.05; // b
	materialAmbient[3] = 1.0f; // a
	 
	// diffuse material
	materialDiffuse[0] = 0.4;  // r
	materialDiffuse[1] = 0.5;  // g
	materialDiffuse[2] = 0.5;  // b
	materialDiffuse[3] = 1.0f; // a
	 
	// specular material
	materialSpecular[0] = 0.04; // r
	materialSpecular[1] = 0.7;  // g
	materialSpecular[2] = 0.7;  // b
	materialSpecular[3] = 1.0f; // a
	 
	// shininess
	materialShininess = 0.078125 * 128;
	 
	glViewport(gWidth*(0.40f), gHeight*(0.20f), gWidth, gHeight);

	 
	translationMatrix = translate(0.0f, 0.0f, -12.0f);
	drawSphere(modelMatrix, translationMatrix, scaleMatrix, rotationMatrix, viewMatrix, materialAmbient, materialDiffuse, materialSpecular, materialShininess);
	// ***** 3rd sphere on 4th column, green *****
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	// ambient material
	materialAmbient[0] = 0.0;  // r
	materialAmbient[1] = 0.05; // g
	materialAmbient[2] = 0.0;  // b
	materialAmbient[3] = 1.0f; // a
	 
	// diffuse material
	materialDiffuse[0] = 0.4;  // r
	materialDiffuse[1] = 0.5;  // g
	materialDiffuse[2] = 0.4;  // b
	materialDiffuse[3] = 1.0f; // a
	 
	// specular material
	materialSpecular[0] = 0.04; // r
	materialSpecular[1] = 0.7;  // g
	materialSpecular[2] = 0.04; // b
	materialSpecular[3] = 1.0f; // a
	 
	// shininess
	materialShininess = 0.078125 * 128;
	 
	glViewport(gWidth*(0.40f), gHeight*(0.05f), gWidth, gHeight);

	 
	translationMatrix = translate(0.0f, 0.0f, -12.0f);
	drawSphere(modelMatrix, translationMatrix, scaleMatrix, rotationMatrix, viewMatrix, materialAmbient, materialDiffuse, materialSpecular, materialShininess);
	// ***** 4th sphere on 4th column, red *****
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	// ambient material
	materialAmbient[0] = 0.05; // r
	materialAmbient[1] = 0.0;  // g
	materialAmbient[2] = 0.0;  // b
	materialAmbient[3] = 1.0f; // a
	 
	// diffuse material
	materialDiffuse[0] = 0.5;  // r
	materialDiffuse[1] = 0.4;  // g
	materialDiffuse[2] = 0.4;  // b
	materialDiffuse[3] = 1.0f; // a
	 
	// specular material
	materialSpecular[0] = 0.7;  // r
	materialSpecular[1] = 0.04; // g
	materialSpecular[2] = 0.04; // b
	materialSpecular[3] = 1.0f; // a
	 
	// shininess
	materialShininess = 0.078125 * 128;
	 
	glViewport(gWidth*(0.40f), gHeight*(-0.10f), gWidth, gHeight);

	 
	translationMatrix = translate(0.0f, 0.0f, -12.0f);
	drawSphere(modelMatrix, translationMatrix, scaleMatrix, rotationMatrix, viewMatrix, materialAmbient, materialDiffuse, materialSpecular, materialShininess);
	// ***** 5th sphere on 4th column, white *****
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	// ambient material
	materialAmbient[0] = 0.05; // r
	materialAmbient[1] = 0.05; // g
	materialAmbient[2] = 0.05; // b
	materialAmbient[3] = 1.0f; // a
	 
	// diffuse material
	materialDiffuse[0] = 0.5;  // r
	materialDiffuse[1] = 0.5;  // g
	materialDiffuse[2] = 0.5;  // b
	materialDiffuse[3] = 1.0f; // a
	 
	// specular material
	materialSpecular[0] = 0.7;  // r
	materialSpecular[1] = 0.7;  // g
	materialSpecular[2] = 0.7;  // b
	materialSpecular[3] = 1.0f; // a
	 
	// shininess
	materialShininess = 0.078125 * 128;
	 
	glViewport(gWidth*(0.40f), gHeight*(-0.25f), gWidth, gHeight);

	 
	translationMatrix = translate(0.0f, 0.0f, -12.0f);
	drawSphere(modelMatrix, translationMatrix, scaleMatrix, rotationMatrix, viewMatrix, materialAmbient, materialDiffuse, materialSpecular, materialShininess);
	// ***** 6th sphere on 4th column, yellow rubber *****
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	// ambient material
	materialAmbient[0] = 0.05; // r
	materialAmbient[1] = 0.05; // g
	materialAmbient[2] = 0.0;  // b
	materialAmbient[3] = 1.0f; // a
	 
	// diffuse material
	materialDiffuse[0] = 0.5;  // r
	materialDiffuse[1] = 0.5;  // g
	materialDiffuse[2] = 0.4;  // b
	materialDiffuse[3] = 1.0f; // a
	 
	// specular material
	materialSpecular[0] = 0.7;  // r
	materialSpecular[1] = 0.7;  // g
	materialSpecular[2] = 0.04; // b
	materialSpecular[3] = 1.0f; // a
	 
	// shininess
	materialShininess = 0.078125 * 128;
	glViewport(gWidth*(0.40f), gHeight*(-0.40f), gWidth, gHeight);

	translationMatrix = translate(0.0f, 0.0f, -12.0f);
	drawSphere(modelMatrix, translationMatrix, scaleMatrix, rotationMatrix, viewMatrix, materialAmbient, materialDiffuse, materialSpecular, materialShininess);
}

void drawSphere(vmath::mat4 &modelMatrix, vmath::mat4 &translationMatrix, vmath::mat4 &scaleMatrix, vmath::mat4 &rotationMatrix, vmath::mat4 &viewMatrix, GLfloat  materialAmbient[4], GLfloat  materialDiffuse[4], GLfloat  materialSpecular[4], const GLfloat &materialShininess)
{
	modelMatrix = translationMatrix * scaleMatrix * rotationMatrix;

	glUniformMatrix4fv(mUniform,
		1,
		GL_FALSE,
		modelMatrix);
	glUniformMatrix4fv(vUniform,
		1,
		GL_FALSE,
		viewMatrix);
	glUniformMatrix4fv(pUniform,
		1,
		GL_FALSE,
		perspectiveProjectionMatrix);

	if (gb_Lighting) {
		glUniform1i(lKeyIsPressedUniform, gb_Lighting);

		glUniform3fv(ldUniform, 1, lightDiffuse);
		glUniform3fv(laUniform, 1, lightAmbient);
		glUniform3fv(lsUniform, 1, lightSpecular);

		glUniform3fv(kaUniform, 1, materialAmbient);
		glUniform3fv(kdUniform, 1, materialDiffuse);
		glUniform3fv(ksUniform, 1, materialSpecular);

		glUniform1f(shininessUniform, materialShininess);

		glUniform4fv(lightPositionUniform, 1, lightPosition);
	}
	else {
		glUniform1i(lKeyIsPressedUniform, gb_Lighting);
	}

	// *** bind vao ***
	glBindVertexArray(vao_sphere);

	// *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	// *** unbind vao ***
	glBindVertexArray(0);
}
