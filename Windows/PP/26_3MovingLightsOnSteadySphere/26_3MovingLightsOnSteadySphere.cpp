#include<Windows.h>
#include<stdio.h>

#include<GL\glew.h>
#include<gl\GL.h>
#include"Sphere.h"
//#include <time.h>

#include"vmath.h"

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

//Shader Program Objects
GLint gShaderProgramObject_PV;
GLint gShaderProgramObject_PF;

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

struct Light {
	GLfloat lightAmbient[4];
	GLfloat lightDiffuse[4];
	GLfloat lightSpecular[4];
	GLfloat lightPosition[4];
};
struct Material {
	GLfloat materialAmbient[4];
	GLfloat materialDiffuse[4];
	GLfloat materialSpecular[4];
	GLfloat materialShininess;
};

const int noLights = 3;
float arrayPosition[4 * noLights];
float arrayAmbient[3 * noLights];
float arrayDiffuse[3 * noLights];
float arraySpecular[3 * noLights];
float tempArray[3 * noLights];

struct Light lights[noLights];
struct Material material;
float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];
int gNumVertices;
int gNumElements;

int perVertexOrFragement = 0;//0 Vertex 1 Fragment

//Method Declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void getUniformLocation(GLint gShaderProgramObject);

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
		case VK_ESCAPE:
			ToggleFullScreen();
			break;
		case 'e':
		case 'E':
			DestroyWindow(hwnd);
			break;
		case 'l':
		case 'L':
			if (gb_Lighting == 0)
				gb_Lighting = 1;
			else
				gb_Lighting = 0;
			break;
		case 'F':
		case 'f':
			getUniformLocation(gShaderProgramObject_PF);
			perVertexOrFragement = 1;
			break;
		case 'v':
		case 'V':
			getUniformLocation(gShaderProgramObject_PV);
			perVertexOrFragement = 0;
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
	GLint gVertexShaderObject_PF;
	GLint gFragementShaderObject_PF;

	//Vertex Shader
	//Define Shader Object
	gVertexShaderObject_PF = glCreateShader(GL_VERTEX_SHADER);

	//Vertex Shader Code
	const GLchar* vertextShaderSourceCode_PF =
		"#version 430 core " \
		"\n " \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
		"uniform mat4 u_m_matrix;" \
		"uniform mat4 u_v_matrix;" \
		"uniform mat4 u_p_matrix;" \
		"uniform int u_lkeyispressed;" \
		"uniform vec4 u_lightposition[3];" \
		"out vec3 tnorm;"\
		"out vec3 lightDirection[3];"\
		"out vec3 reflectionVector[3];"\
		"out vec3 viewerVector;"\
		"void main(void)" \
		"{" \
		"gl_Position=u_p_matrix*u_v_matrix*u_m_matrix * vPosition;" \
		"if(u_lkeyispressed==1)"\
		"{"\
		"vec4 eyecoordinates=u_v_matrix*u_m_matrix*vPosition;"\
		"tnorm=mat3(u_v_matrix*u_m_matrix)*vNormal;"\
		"viewerVector=vec3(-eyecoordinates);"\
		
		"for(int i=0;i<3;i++)"\
		"{"\
		"lightDirection[i]=vec3(u_lightposition[i]-eyecoordinates);"\
		"reflectionVector[i]=reflect(-lightDirection[i],tnorm);" \
		"}"\

		"}"\
		"}";

	//Shader object and source code mapping
	glShaderSource(gVertexShaderObject_PF, 1, &vertextShaderSourceCode_PF, NULL);

	//Compile Shader
	glCompileShader(gVertexShaderObject_PF);

	//Error Checking
	GLint iShaderCompileStatus = 0;
	GLint iInfoLogLength = 0;
	GLchar * szInfoLog = NULL;

	glGetShaderiv(gVertexShaderObject_PF, GL_COMPILE_STATUS, &iShaderCompileStatus);

	if (iShaderCompileStatus == GL_FALSE) {
		glGetShaderiv(gVertexShaderObject_PF, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0) {
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL) {
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject_PF,
					iInfoLogLength,
					&written,
					szInfoLog);
				fprintf_s(gpFile, "Vertex Shader PF Compilation Log %s\n", szInfoLog);
				free(szInfoLog);
				UnInitialize();
				DestroyWindow(0);
				exit(0);
			}
		}
	}

	//Fragement Shader
	//Define Shader Object
	gFragementShaderObject_PF = glCreateShader(GL_FRAGMENT_SHADER);

	//Fragement Shader Code
	const GLchar* fragementShaderSourceCode_PF =
		"#version 430 core " \
		"\n " \
		"out vec4 fragColor;"
		"uniform vec3 u_ld[3];" \
		"uniform vec3 u_kd;" \
		"uniform vec3 u_la[3];" \
		"uniform vec3 u_ka;" \
		"uniform vec3 u_ls[3];" \
		"uniform vec3 u_ks;" \
		"uniform float u_shininess;" \
		"uniform int u_lkeyispressed;" \
		"in vec3 tnorm;"\
		"in vec3 lightDirection[3];"\
		"in vec3 reflectionVector[3];"\
		"in vec3 viewerVector;"\
		"void main(void)" \
		"{" \
		"vec3 phongADSLight;"\
		"if(u_lkeyispressed==1)"\
		"{"\
		"vec3 tnorm_normalize=normalize(tnorm);"\
		"vec3 viewerVector_normalize=normalize(viewerVector);"\

		"phongADSLight = vec3(0.0f, 0.0f, 0.0f);"\
		"for(int i=0;i<3;i++)"\
		"{"\
		"vec3 lightDirection_normalize=normalize(lightDirection[i]);"\
		"vec3 reflectionVector_normalize=normalize(reflectionVector[i]);"\
		"float tn_dot_ld=max(dot(lightDirection_normalize,tnorm_normalize),0.0);"\
		"vec3 ambient=u_la[i]*u_ka;"\
		"vec3 diffuse=u_ld[i]*u_kd*tn_dot_ld;"
		"vec3 specular=u_ls[i]*u_ks*pow(max(dot(reflectionVector_normalize,viewerVector_normalize),0.0),u_shininess);"\
		"phongADSLight=phongADSLight+ambient+diffuse+specular;"\
		"}"\

		"}"\
		"else"\
		"{"\
		"phongADSLight=vec3(1.0f,1.0f,1.0f);"\
		"}"\
		"fragColor=vec4(phongADSLight,1.0f);"\
		"} ";

	//Shader object and source code mapping
	glShaderSource(gFragementShaderObject_PF, 1, &fragementShaderSourceCode_PF, NULL);

	//Compile Shader
	glCompileShader(gFragementShaderObject_PF);

	//Error Checking
	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(gFragementShaderObject_PF, GL_COMPILE_STATUS, &iShaderCompileStatus);

	if (iShaderCompileStatus == GL_FALSE) {
		glGetShaderiv(gFragementShaderObject_PF, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0) {
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL) {
				GLsizei written;
				glGetShaderInfoLog(gFragementShaderObject_PF,
					iInfoLogLength,
					&written,
					szInfoLog);
				fprintf_s(gpFile, "Fragement Shader Per Fragement Compilation Log %s\n", szInfoLog);
				free(szInfoLog);
				UnInitialize();
				DestroyWindow(0);
				exit(0);
			}
		}
	}

	//Program Object
	gShaderProgramObject_PF = glCreateProgram();
	//Attach Shaders
	glAttachShader(gShaderProgramObject_PF, gVertexShaderObject_PF);
	glAttachShader(gShaderProgramObject_PF, gFragementShaderObject_PF);

	//Prelinking
	//Binding to attribute
	glBindAttribLocation(gShaderProgramObject_PF,
		AMC_ATTRIBUTE_POSITION,
		"vPosition");

	glBindAttribLocation(gShaderProgramObject_PF,
		AMC_ATTRIBUTE_NORMAL,
		"vNormal");

	//Link Shader Program
	glLinkProgram(gShaderProgramObject_PF);

	//Error Checking
	GLint iShaderLinkStatus = 0;

	glGetProgramiv(gShaderProgramObject_PF, GL_LINK_STATUS, &iShaderLinkStatus);

	if (iShaderLinkStatus == GL_FALSE) {
		glGetProgramiv(gShaderProgramObject_PF, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0) {
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL) {
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject_PF,
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

	if (perVertexOrFragement == 1) {
		getUniformLocation(gShaderProgramObject_PF);
	}

	//----------------------------------------------------------------------------------------------------------------

	//Shader Objects
	GLint gVertexShaderObject_PV;
	GLint gFragementShaderObject_PV;

	//Vertex Shader
	//Define Shader Object
	gVertexShaderObject_PV = glCreateShader(GL_VERTEX_SHADER);

	//Vertex Shader Code
	const GLchar* vertextShaderSourceCode_PV =
		"#version 430 core " \
		"\n " \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
		"uniform mat4 u_m_matrix;" \
		"uniform mat4 u_v_matrix;" \
		"uniform mat4 u_p_matrix;" \
		"uniform int u_lkeyispressed;" \

		"uniform vec3 u_ld[3];" \
		"uniform vec3 u_kd;" \
		"uniform vec3 u_la[3];" \
		"uniform vec3 u_ka;" \
		"uniform vec3 u_ls[3];" \
		"uniform vec3 u_ks;" \
		"uniform float u_shininess;" \
		"uniform vec4 u_lightposition[3];" \

		"out vec3 phongADSLight;" \
		"void main(void)" \
		"{" \
		"gl_Position=u_p_matrix*u_v_matrix*u_m_matrix * vPosition;" \
		"if(u_lkeyispressed==1)"\
		"{"\
		"vec4 eyecoordinates=u_v_matrix*u_m_matrix*vPosition;"\
		"vec3 tnorm=normalize(mat3(u_v_matrix*u_m_matrix)*vNormal);"\
		"vec3 viewerVector=normalize(vec3(-eyecoordinates));"\
		"phongADSLight = vec3(0.0f, 0.0f, 0.0f);"\
		"for(int i=0;i<3;i++)"\
		"{"\

		"vec3 lightDirection=normalize(vec3(u_lightposition[i]-eyecoordinates));"\
		"float tn_dot_ld=max(dot(lightDirection,tnorm),0.0);"\
		"vec3 reflectionVector=reflect(-lightDirection,tnorm);"\
		"vec3 ambient=u_la[i]*u_ka;"\
		"vec3 diffuse=u_ld[i]*u_kd*tn_dot_ld;"
		"vec3 specular=u_ls[i]*u_ks*pow(max(dot(reflectionVector,viewerVector),0.0),u_shininess);"\

		"phongADSLight=phongADSLight+ambient+diffuse+specular;"\
		"}"\

		"}"\
		"else"\
		"{"\
		"phongADSLight=vec3(1.0f,1.0f,1.0f);"\
		"}"\
		"}";

	//Shader object and source code mapping
	glShaderSource(gVertexShaderObject_PV, 1, &vertextShaderSourceCode_PV, NULL);

	//Compile Shader
	glCompileShader(gVertexShaderObject_PV);

	//Error Checking
	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(gVertexShaderObject_PV, GL_COMPILE_STATUS, &iShaderCompileStatus);

	if (iShaderCompileStatus == GL_FALSE) {
		glGetShaderiv(gVertexShaderObject_PV, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0) {
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL) {
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject_PV,
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
	gFragementShaderObject_PV = glCreateShader(GL_FRAGMENT_SHADER);

	//Fragement Shader Code
	const GLchar* fragementShaderSourceCode_PV =
		"#version 430 core " \
		"\n " \
		"out vec4 fragColor;"
		"in vec3 phongADSLight;" \
		"void main(void)" \
		"{" \
		"fragColor=vec4(phongADSLight,1.0f);"\
		"} ";

	//Shader object and source code mapping
	glShaderSource(gFragementShaderObject_PV, 1, &fragementShaderSourceCode_PV, NULL);

	//Compile Shader
	glCompileShader(gFragementShaderObject_PV);

	//Error Checking
	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(gFragementShaderObject_PV, GL_COMPILE_STATUS, &iShaderCompileStatus);

	if (iShaderCompileStatus == GL_FALSE) {
		glGetShaderiv(gFragementShaderObject_PV, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0) {
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL) {
				GLsizei written;
				glGetShaderInfoLog(gFragementShaderObject_PV,
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
	gShaderProgramObject_PV = glCreateProgram();
	//Attach Shaders
	glAttachShader(gShaderProgramObject_PV, gVertexShaderObject_PV);
	glAttachShader(gShaderProgramObject_PV, gFragementShaderObject_PV);

	//Prelinking
	//Binding to attribute
	glBindAttribLocation(gShaderProgramObject_PV,
		AMC_ATTRIBUTE_POSITION,
		"vPosition");

	glBindAttribLocation(gShaderProgramObject_PV,
		AMC_ATTRIBUTE_NORMAL,
		"vNormal");

	//Link Shader Program
	glLinkProgram(gShaderProgramObject_PV);

	//Error Checking
	iShaderLinkStatus = 0;

	glGetProgramiv(gShaderProgramObject_PV, GL_LINK_STATUS, &iShaderLinkStatus);

	if (iShaderLinkStatus == GL_FALSE) {
		glGetProgramiv(gShaderProgramObject_PV, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0) {
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL) {
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject_PV,
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

	if (perVertexOrFragement == 0) {
		getUniformLocation(gShaderProgramObject_PV);
	}


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


	lights[0].lightAmbient[0] = 0.0f;
	lights[0].lightAmbient[1] = 0.0f;
	lights[0].lightAmbient[2] = 0.0f;
	lights[0].lightAmbient[3] = 1.0f;

	lights[0].lightDiffuse[0] = 1.0f;
	lights[0].lightDiffuse[1] = 0.0f;
	lights[0].lightDiffuse[2] = 0.0f;
	lights[0].lightDiffuse[3] = 1.0f;

	lights[0].lightSpecular[0] = 1.0f;
	lights[0].lightSpecular[1] = 0.0f;
	lights[0].lightSpecular[2] = 0.0f;
	lights[0].lightSpecular[3] = 1.0f;

	lights[0].lightPosition[0] = 0.0f;
	lights[0].lightPosition[1] = 0.0f;
	lights[0].lightPosition[2] = 0.0f;
	lights[0].lightPosition[3] = 1.0f;

	lights[1].lightAmbient[0] = 0.0f;
	lights[1].lightAmbient[1] = 0.0f;
	lights[1].lightAmbient[2] = 0.0f;
	lights[1].lightAmbient[3] = 1.0f;

	lights[1].lightDiffuse[0] = 0.0f;
	lights[1].lightDiffuse[1] = 1.0f;
	lights[1].lightDiffuse[2] = 0.0f;
	lights[1].lightDiffuse[3] = 1.0f;

	lights[1].lightSpecular[0] = 0.0f;
	lights[1].lightSpecular[1] = 1.0f;
	lights[1].lightSpecular[2] = 0.0f;
	lights[1].lightSpecular[3] = 1.0f;

	lights[1].lightPosition[0] = 0.0f;
	lights[1].lightPosition[1] = 0.0f;
	lights[1].lightPosition[2] = 0.0f;
	lights[1].lightPosition[3] = 1.0f;

	lights[2].lightAmbient[0] = 0.0f;
	lights[2].lightAmbient[1] = 0.0f;
	lights[2].lightAmbient[2] = 0.0f;
	lights[2].lightAmbient[3] = 1.0f;

	lights[2].lightDiffuse[0] = 0.0f;
	lights[2].lightDiffuse[1] = 0.0f;
	lights[2].lightDiffuse[2] = 1.0f;
	lights[2].lightDiffuse[3] = 1.0f;

	lights[2].lightSpecular[0] = 0.0f;
	lights[2].lightSpecular[1] = 0.0f;
	lights[2].lightSpecular[2] = 1.0f;
	lights[2].lightSpecular[3] = 1.0f;

	lights[2].lightPosition[0] = 0.0f;
	lights[2].lightPosition[1] = 0.0f;
	lights[2].lightPosition[2] = 0.0f;
	lights[2].lightPosition[3] = 1.0f;

	material.materialAmbient[0] = 0.5f;
	material.materialAmbient[1] = 0.5f;
	material.materialAmbient[2] = 0.5f;
	material.materialAmbient[3] = 1.0f;
	material.materialDiffuse[0] = 1.0f;
	material.materialDiffuse[1] = 1.0f;
	material.materialDiffuse[2] = 1.0f;
	material.materialDiffuse[3] = 1.0f;
	material.materialSpecular[0] = 1.0f;
	material.materialSpecular[1] = 1.0f;
	material.materialSpecular[2] = 1.0f;
	material.materialSpecular[3] = 1.0f;
	material.materialShininess = 128.0f;

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0f);

	//glEnable(GL_CULL_FACE);
	//glDisable(GL_CULL_FACE);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glEnable(GL_TEXTURE_2D);

	for (int i = 0; i < noLights; i++) {
		arrayDiffuse[3 * i] = lights[i].lightDiffuse[0];
		arrayDiffuse[3 * i + 1] = lights[i].lightDiffuse[1];
		arrayDiffuse[3 * i + 2] = lights[i].lightDiffuse[2];
	}

	for (int i = 0; i < noLights; i++) {
		arrayAmbient[3 * i] = lights[i].lightAmbient[0];
		arrayAmbient[3 * i + 1] = lights[i].lightAmbient[1];
		arrayAmbient[3 * i + 2] = lights[i].lightAmbient[2];
	}

	for (int i = 0; i < noLights; i++) {
		arraySpecular[3 * i] = lights[i].lightSpecular[0];
		arraySpecular[3 * i + 1] = lights[i].lightSpecular[1];
		arraySpecular[3 * i + 2] = lights[i].lightSpecular[2];
	}

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

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	if (perVertexOrFragement == 0)
		glUseProgram(gShaderProgramObject_PV);
	else
		glUseProgram(gShaderProgramObject_PF);


	//Declaration of Matrices
	mat4 modelMatrix;
	mat4 viewMatrix;

	mat4 translationMatrix;
	mat4 rotationMatrix;
	mat4 scaleMatrix;

	//CUBE
	//Initialize matrices

	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	scaleMatrix = mat4::identity();

	translationMatrix = translate(0.0f, 0.0f, -3.0f);

	//Matrix Multiplication
	modelMatrix = translationMatrix * scaleMatrix * rotationMatrix;
	//perspectiveProjectionMatrix

	//Send necessary matrices to shader in resp. Uniforms

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

	for (int i = 0; i < noLights; i++) {
		arrayPosition[4 * i] = lights[i].lightPosition[0];
		arrayPosition[4 * i + 1] = lights[i].lightPosition[1];
		arrayPosition[4 * i + 2] = lights[i].lightPosition[2];
		arrayPosition[4 * i + 3] = lights[i].lightPosition[3];
	}

	if (gb_Lighting) {
		glUniform1i(lKeyIsPressedUniform, gb_Lighting);

		glUniform3fv(ldUniform, noLights, arrayDiffuse);
		glUniform3fv(laUniform, noLights, arrayAmbient);
		glUniform3fv(lsUniform, noLights, arraySpecular);

		glUniform4fv(lightPositionUniform, noLights, arrayPosition);

		glUniform3fv(kaUniform, 1, material.materialAmbient);
		glUniform3fv(kdUniform, 1, material.materialDiffuse);
		glUniform3fv(ksUniform, 1, material.materialSpecular);
		glUniform1f(shininessUniform, material.materialShininess);
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

	glUseProgram(0);

	SwapBuffers(ghdc);

}

float angle1 = 0.0f;

void Update(void)
{
	angle1 = angle1 + 0.001f;
	if (angle1 > 2 * 3.14159265)
		angle1 = 0.0f;

	lights[0].lightPosition[1] = 100 * sin(angle1);
	lights[0].lightPosition[2] = 100 * cos(angle1);

	lights[1].lightPosition[0] = 100 * sin(angle1);
	lights[1].lightPosition[2] = 100 * cos(angle1);

	lights[2].lightPosition[0] = 100 * sin(angle1);
	lights[2].lightPosition[1] = 100 * cos(angle1);

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

	if (gShaderProgramObject_PV) {
		GLsizei shaderCount;
		GLsizei shaderNumber;
		glUseProgram(gShaderProgramObject_PV);

		glGetProgramiv(gShaderProgramObject_PV,
			GL_ATTACHED_SHADERS,
			&shaderCount);

		GLuint * pShaders = (GLuint *)malloc(shaderCount * sizeof(GLuint));

		if (pShaders) {
			glGetAttachedShaders(gShaderProgramObject_PV,
				shaderCount,
				&shaderCount,
				pShaders);

			for (shaderNumber = 0; shaderNumber < shaderCount; shaderNumber++) {
				glDetachShader(gShaderProgramObject_PV, pShaders[shaderNumber]);
				glDeleteShader(pShaders[shaderNumber]);
				pShaders[shaderNumber] = 0;
			}
			free(pShaders);
		}
		glDeleteProgram(gShaderProgramObject_PV);
		gShaderProgramObject_PV = 0;
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

void getUniformLocation(GLint gShaderProgramObject) {
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

