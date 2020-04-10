//3D Color animation as template

#include<Windows.h>
#include<stdio.h>

#include<GL\glew.h>
#include<gl\GL.h>

#include"resource.h"

//#include <time.h>

#include"vmath.h"

#include<stdlib.h>
#include<math.h>
#include<gl\glew.h>
#include<gl\gl.h>
#include"vmath.h"
#include"VectorofFloat.h"
#include"VectorofIntegres.h"

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

GLfloat anglePyramid = 0.0;
GLfloat angleCube = 0.0;

GLuint texture_Marble;
//Shader Program Objects
GLint gShaderProgramObject;

//Addition
GLint gShaderProgramObject_FBO;
GLuint mvpUniform_FBO;
GLuint samplerUniform_FBO;
GLuint vbo_texCoord_Cube;
GLuint FBO;
GLuint texture_FBO;

int windowWidth;
int windowHeight;

enum {
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXCOORD0
};

//GLuint vao_pyramid;
GLuint vao_cube;

//GLuint vbo_position_pyramid;
GLuint vbo_position_cube;

//GLuint vbo_color_pyramid;
GLuint vbo_color_cube;

GLuint mvpUniform;
mat4 perspectiveProjectionMatrix;

GLuint samplerUniform_ModelLoading;


/*Global Variables for Model*/
struct vec_float *gp_vec_vertices, *gp_vec_normals, *gp_vec_textures;
struct vec_float *gp_vec_vertex_sorted, *gp_vec_normals_sorted, *gp_vec_textures_sorted;
struct vec_int *gp_vertex_indices, *gp_normals_indces, *gp_texture_indices;
GLuint gElementBuffer;
char buffer[1024];
FILE *gpMesh = NULL;

GLuint glShaderProgramObject_ModelLoading;
GLuint vao_ModelLoading;
GLuint vbo_ModelLoading;
GLuint vbo_ModelLoadingTexCoord;

GLuint mvpUniform_ModelLoading;
/*Shader object variables*/
GLuint glVertexShaderObject;
GLuint glFragmentShaderObject;


void LoadMesh();


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
		windowWidth = LOWORD(lParam);
		windowHeight = HIWORD(lParam);
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

//GLuint colorRenderBuffer;
GLuint depthRenderBuffer;
//GLint backingWidth, backingHeight;
BOOL LoadTexture(GLuint *texture, TCHAR imageResourceID[]);

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




	//-------------------------------------------------ModelLoading---------------------------------------------------------------


	///*Define Vertex Shader object*/
	glVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	///*Write Vertex Shader code*/
	const GLchar* vertexShaderSourceCode[] =
	{
		"#version 430 core"\
		"\n"\
		"in vec4 vPos;"\
		"in vec2 vTexCoord;" \
		"out vec2 out_TexCoord;" \
		"uniform mat4 u_mvp_matrix;"\
		"void main(void)"\
		"{"\
		"gl_Position = u_mvp_matrix * vPos;"\
		"out_TexCoord=vTexCoord;"\
		"}"
	};


	////specifiy above source code to the Vertex Shader Object
	glShaderSource(glVertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);

	////compile the vertex shader program
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
				//UnInitialize();
				DestroyWindow(ghwnd);
				exit(0);
			}
		}
	}

	///*Fragment Shader code*/
	glFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	//Write Fragment shader code
	const GLchar* fragmentShaderSourceCode[] =
	{
		"#version 430 core"\
		"\n"\
		"out vec4 FragColor;"\
		"in vec2 out_TexCoord;"
		"uniform sampler2D u_sampler;"
		"void main(void)"\
		"{"\
		"FragColor=texture(u_sampler,out_TexCoord);" \
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
				//UnInitialize();
				DestroyWindow(ghwnd);
				exit(0);
			}
		}
	}

	//Create Shader program object
	glShaderProgramObject_ModelLoading = glCreateProgram();
	//Attach Vertex shader to shader program object
	glAttachShader(glShaderProgramObject_ModelLoading, glVertexShaderObject);
	glAttachShader(glShaderProgramObject_ModelLoading, glFragmentShaderObject);



	/*Prelinking Binding to vaertex attributes*/
	glBindAttribLocation(glShaderProgramObject_ModelLoading, AMC_ATTRIBUTE_POSITION, "vPos");
	glBindAttribLocation(glShaderProgramObject_ModelLoading,
		AMC_ATTRIBUTE_TEXCOORD0,
		"vTexCoord");

	//Link the shader program
	glLinkProgram(glShaderProgramObject_ModelLoading);

	//Error checking
	GLint iProgramLinkStatus = 0;
	szInfoLog = NULL;
	iInfoLength = 0;

	glGetProgramiv(glShaderProgramObject_ModelLoading, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(glShaderProgramObject_ModelLoading, GL_INFO_LOG_LENGTH, &iInfoLength);
		if (iInfoLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLength * sizeof(GLchar));
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(glShaderProgramObject_ModelLoading, iInfoLength, &written, szInfoLog);
				fprintf_s(gpFile, "Link error Log: \n%s", szInfoLog);
				free(szInfoLog);
				//UnInitialize();
				DestroyWindow(ghwnd);
				exit(0);
			}
		}
	}

	/*Post linking retriving uniform loactions*/
	mvpUniform_ModelLoading = glGetUniformLocation(glShaderProgramObject_ModelLoading, "u_mvp_matrix");
	samplerUniform_ModelLoading = glGetUniformLocation(gShaderProgramObject_FBO,
		"u_sampler");
	LoadMesh();


	/*Create vao_ModelLoading*/
	glGenVertexArrays(1, &vao_ModelLoading);
	glBindVertexArray(vao_ModelLoading);

	glGenBuffers(1, &vbo_ModelLoading);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_ModelLoading);

	glBufferData(GL_ARRAY_BUFFER, gp_vec_vertices->size * sizeof(float), gp_vec_vertices->p, GL_STATIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, gp_vec_vertices->size * sizeof(float), gp_vec_vertices->p, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, &vbo_ModelLoadingTexCoord);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_ModelLoadingTexCoord);

	glBufferData(GL_ARRAY_BUFFER, gp_vec_textures->size * sizeof(float), gp_vec_textures->p, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &gElementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gElementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, gp_vertex_indices->size * sizeof(unsigned int), gp_vertex_indices->p, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		//-------------------------------------------------ModelLoading-----------------------------------------------------------------



	//Addition
	GLint gVertexShaderObject_FBO;
	GLint gFragementShaderObject_FBO;

	gVertexShaderObject_FBO = glCreateShader(GL_VERTEX_SHADER);

	//Vertex Shader Code
	const GLchar* vertextShaderSourceCodeN =
		"#version 430 core " \
		"\n " \
		"in vec4 vPosition;" \
		"in vec2 vTexCoord;" \
		"uniform mat4 u_mvp_matrix;" \
		"out vec2 out_TexCoord;" \
		"void main(void)" \
		"{" \
		"gl_Position=u_mvp_matrix * vPosition;" \
		"out_TexCoord=vTexCoord;"\
		"} ";

	//"in vec4 vColor;" \
	//"out vec4 out_color;" \
		//"out_color=vColor;"\

	//Shader object and source code mapping
	glShaderSource(gVertexShaderObject_FBO, 1, &vertextShaderSourceCodeN, NULL);

	//Compile Shader
	glCompileShader(gVertexShaderObject_FBO);

	//Error Checking
	GLint iShaderCompileStatusN = 0;
	GLint iInfoLogLengthN = 0;
	GLchar * szInfoLogN = NULL;

	glGetShaderiv(gVertexShaderObject_FBO, GL_COMPILE_STATUS, &iShaderCompileStatusN);

	if (iShaderCompileStatusN == GL_FALSE) {
		glGetShaderiv(gVertexShaderObject_FBO, GL_INFO_LOG_LENGTH, &iInfoLogLengthN);
		if (iInfoLogLengthN > 0) {
			szInfoLogN = (GLchar *)malloc(iInfoLogLengthN);
			if (szInfoLogN != NULL) {
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject_FBO,
					iInfoLogLengthN,
					&written,
					szInfoLogN);
				fprintf_s(gpFile, "Vertex Shader Compilation Log %s\n", szInfoLogN);
				free(szInfoLogN);
				UnInitialize();
				DestroyWindow(0);
				exit(0);
			}
		}
	}

	//Fragement Shader
	//Define Shader Object
	gFragementShaderObject_FBO = glCreateShader(GL_FRAGMENT_SHADER);

	//Fragement Shader Code
	const GLchar* fragementShaderSourceCodeN =
		"#version 430 core " \
		"\n " \
		"in vec2 out_TexCoord;"
		"uniform sampler2D u_sampler;"
		"out vec4 fragColor;"
		"void main(void)" \
		"{" \
		"fragColor=texture(u_sampler,out_TexCoord);" \
		"} ";


	//"in vec4 out_color;"\
	//"fragColor=out_color;" \
		
	//Shader object and source code mapping
	glShaderSource(gFragementShaderObject_FBO, 1, &fragementShaderSourceCodeN, NULL);

	//Compile Shader
	glCompileShader(gFragementShaderObject_FBO);

	//Error Checking
	iShaderCompileStatusN = 0;
	iInfoLogLengthN = 0;
	szInfoLogN = NULL;

	glGetShaderiv(gFragementShaderObject_FBO, GL_COMPILE_STATUS, &iShaderCompileStatusN);

	if (iShaderCompileStatusN == GL_FALSE) {
		glGetShaderiv(gFragementShaderObject_FBO, GL_INFO_LOG_LENGTH, &iInfoLogLengthN);
		if (iInfoLogLengthN > 0) {
			szInfoLogN = (GLchar *)malloc(iInfoLogLengthN);
			if (szInfoLogN != NULL) {
				GLsizei written;
				glGetShaderInfoLog(gFragementShaderObject_FBO,
					iInfoLogLengthN,
					&written,
					szInfoLogN);
				fprintf_s(gpFile, "Fragement Shader Compilation Log %s\n", szInfoLogN);
				free(szInfoLogN);
				UnInitialize();
				DestroyWindow(0);
				exit(0);
			}
		}
	}

	//Program Object
	gShaderProgramObject_FBO = glCreateProgram();
	//Attach Shaders
	glAttachShader(gShaderProgramObject_FBO, gVertexShaderObject_FBO);
	glAttachShader(gShaderProgramObject_FBO, gFragementShaderObject_FBO);

	//Prelinking
	//Binding to attribute
	glBindAttribLocation(gShaderProgramObject_FBO,
		AMC_ATTRIBUTE_POSITION,
		"vPosition");

	glBindAttribLocation(gShaderProgramObject_FBO,
		AMC_ATTRIBUTE_TEXCOORD0,
		"vTexCoord");
	/*glBindAttribLocation(gShaderProgramObject_FBO,
		AMC_ATTRIBUTE_COLOR,
		"vColor");*/

		//Link Shader Program
	glLinkProgram(gShaderProgramObject_FBO);

	//Error Checking
	GLint iShaderLinkStatusN = 0;

	glGetProgramiv(gShaderProgramObject_FBO, GL_LINK_STATUS, &iShaderLinkStatusN);

	if (iShaderLinkStatusN == GL_FALSE) {
		glGetProgramiv(gShaderProgramObject_FBO, GL_INFO_LOG_LENGTH, &iInfoLogLengthN);
		if (iInfoLogLengthN > 0) {
			szInfoLogN = (GLchar *)malloc(iInfoLogLengthN);
			if (szInfoLogN != NULL) {
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject_FBO,
					iInfoLogLengthN,
					&written,
					szInfoLogN);
				fprintf_s(gpFile, "Program Link Log %s\n", szInfoLogN);
				free(szInfoLogN);
				UnInitialize();
				DestroyWindow(0);
				exit(0);
			}
		}
	}

	//Post Linking
	//Retriving Uniform Location
	mvpUniform_FBO = glGetUniformLocation(gShaderProgramObject_FBO,
		"u_mvp_matrix");

	samplerUniform_FBO = glGetUniformLocation(gShaderProgramObject_FBO,
		"u_sampler");





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
	//GLint iShaderCompileStatus = 0;
	GLint iInfoLogLength = 0;
	szInfoLog = NULL;

	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

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


	//Vertices
	//const GLfloat pyramidVertices[] = {
	//	0.0f, 1.0f, 0.0f,
	//	-1.0f, -1.0f, 1.0f,
	//	1.0f, -1.0f, 1.0f,
	//	0.0f, 1.0f, 0.0f,
	//	1.0f, -1.0f, 1.0f,
	//	1.0f, -1.0f, -1.0f,
	//	0.0f, 1.0f, 0.0f,
	//	1.0f, -1.0f, -1.0f,
	//	-1.0f, -1.0f, -1.0f,
	//	0.0f, 1.0f, 0.0f,
	//	-1.0f, -1.0f, -1.0f,
	//	-1.0f, -1.0f, 1.0f

	//	,-1,-1,-1,
	//	-1,-1,1,
	//	1,-1,-1,

	//	1,-1,-1,
	//	1,-1,1,
	//	-1,-1,1


	//};

	const GLfloat cubeVertices[] = {
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f
	};

	//const GLfloat pyramidColors[] = {
	//	1.0f, 0.0f, 0.0f,
	//	0.0f, 1.0f, 0.0f,
	//	0.0f, 0.0f, 1.0f,
	//	1.0f, 0.0f, 0.0f,
	//	0.0f, 0.0f, 1.0f,
	//	0.0f, 1.0f, 0.0f,
	//	1.0f, 0.0f, 0.0f,
	//	0.0f, 1.0f, 0.0f,
	//	0.0f, 0.0f, 1.0f,
	//	1.0f, 0.0f, 0.0f,
	//	0.0f, 0.0f, 1.0f,
	//	0.0f, 1.0f, 0.0f

	//	,1.0f,0.0f,0.0f,
	//	0.0f,1.0f,0.0f,
	//	0.0f,0.0f,1.0f,

	//	0.0f,0.0f,1.0f,
	//	1.0f,0.0f,0.0f,
	//	0.0f,1.0f,0.0f
	//};

	const GLfloat cubeColors[] = {
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f
	};

	const GLfloat cubeTexCoord[] =
	{ 0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f };


	////Create vao
	////Save everying in single set
	//glGenVertexArrays(1, &vao_pyramid);

	//glBindVertexArray(vao_pyramid);


	////TRIANGLE
	////Generate Buffer
	//glGenBuffers(1, &vbo_position_pyramid);
	////Bind Generated Buffer
	//glBindBuffer(GL_ARRAY_BUFFER,
	//	vbo_position_pyramid);
	////Fill Buffer
	//glBufferData(GL_ARRAY_BUFFER,
	//	sizeof(pyramidVertices),
	//	pyramidVertices,
	//	GL_STATIC_DRAW);
	////Set Vertex Attrib Pointer
	//glVertexAttribPointer(AMC_ATTRIBUTE_POSITION,
	//	3,
	//	GL_FLOAT,
	//	GL_FALSE,
	//	0,
	//	NULL);
	////Enable Vertex Attrib Array
	//glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	////Unbind Buffer
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glGenBuffers(1, &vbo_color_pyramid);
	////Bind Generated Buffer
	//glBindBuffer(GL_ARRAY_BUFFER,
	//	vbo_color_pyramid);
	////Fill Buffer
	//glBufferData(GL_ARRAY_BUFFER,
	//	sizeof(pyramidColors),
	//	pyramidColors,
	//	GL_STATIC_DRAW);
	////Set Vertex Attrib Pointer
	//glVertexAttribPointer(AMC_ATTRIBUTE_COLOR,
	//	3,
	//	GL_FLOAT,
	//	GL_FALSE,
	//	0,
	//	NULL);
	////Enable Vertex Attrib Array
	//glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	////Unbind Buffer
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	////Unbind array
	//glBindVertexArray(0);

	//RECTANGLE
	glGenVertexArrays(1, &vao_cube);

	glBindVertexArray(vao_cube);

	//Generate Buffer
	glGenBuffers(1, &vbo_position_cube);
	//Bind Generated Buffer
	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_position_cube);
	//Fill Buffer
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(cubeVertices),
		cubeVertices,
		GL_STATIC_DRAW);
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

	//Generate Buffer
	glGenBuffers(1, &vbo_color_cube);
	//Bind Generated Buffer
	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_color_cube);
	//Fill Buffer
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(cubeColors),
		cubeColors,
		GL_STATIC_DRAW);
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


	//Generate Buffer
	glGenBuffers(1, &vbo_texCoord_Cube);
	//Bind Generated Buffer
	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_texCoord_Cube);
	//Fill Buffer
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(cubeTexCoord),
		cubeTexCoord,
		GL_STATIC_DRAW);
	//Set Vertex Attrib Pointer
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD0,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		NULL);
	//Enable Vertex Attrib Array
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD0);
	//Unbind Buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Unbind array
	glBindVertexArray(0);

	void createFrameBuffer(GLuint*);

	LoadTexture(&texture_Marble, MAKEINTRESOURCE(IDBITMAP_MARBLE));
	//createFrameBuffer(&texture_Marble);


	glGenFramebuffers(1,&FBO);
	glBindFramebuffer(GL_FRAMEBUFFER,FBO);

	glGenTextures(1, &texture_FBO);
	glBindTexture(GL_TEXTURE_2D, texture_FBO);

	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 1024, 1024);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_FBO, 0);



	glGenRenderbuffers(1, &depthRenderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);

	glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT16,1024,1024);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);
	


	static const GLenum draw_Buffers[] = { GL_COLOR_ATTACHMENT0 };

	glDrawBuffers(1, draw_Buffers);

	glBindTexture(GL_TEXTURE_2D, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//GLenum fboStatus = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);

	if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		fprintf(gpFile, "Done with framebuffer facility");
	//36053

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0f);

	//glEnable(GL_CULL_FACE);
	//glDisable(GL_CULL_FACE);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

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

}

//Function Display
void Display(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glUseProgram(gShaderProgramObject_FBO);

	////Declaration of Matrices
	//mat4 modelViewMatrix;
	//mat4 modelViewProjectionMatrix;
	//mat4 translationMatrix;
	//mat4 rotationMatrix;
	//mat4 scaleMatrix;

	////Pyramid
	////Initialize matrices

	//modelViewMatrix = mat4::identity();
	//modelViewProjectionMatrix = mat4::identity();
	//translationMatrix = mat4::identity();
	//rotationMatrix = mat4::identity();
	//
	//glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	//glViewport(0, 0, 1024, 1024);
	//glClearColor(0.5, 0.5, 0.5, 0.0);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//translationMatrix = translate(0.0f, 0.0f, -5.0f);
	////scaleMatrix = scale(0.75f, 0.75f, 0.75f);
	////rotationMatrix = rotate(anglePyramid, 1.0f, 1.0f, 1.0f);
	//rotationMatrix = rotate(anglePyramid,anglePyramid,anglePyramid);
	//


	////Matrix Multiplication
	//modelViewMatrix = translationMatrix * rotationMatrix;

	//modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	//glBindTexture(GL_TEXTURE_2D, texture_Marble);

	////Send necessary matrices to shader in resp. Uniforms

	//glUniformMatrix4fv(mvpUniform,//Changed
	//	1,
	//	GL_FALSE,
	//	modelViewProjectionMatrix);

	//glUniform1i(samplerUniform_FBO, 0);

	////Bind with vao

	//glBindVertexArray(vao_cube);

	////Bind with textures if any

	////Draw

	//glDrawArrays(GL_TRIANGLE_FAN,
	//	0,
	//	4);
	//glDrawArrays(GL_TRIANGLE_FAN,
	//	4,
	//	4);
	//glDrawArrays(GL_TRIANGLE_FAN,
	//	8,
	//	4);
	//glDrawArrays(GL_TRIANGLE_FAN,
	//	12,
	//	4);
	//glDrawArrays(GL_TRIANGLE_FAN,
	//	16,
	//	4);
	//glDrawArrays(GL_TRIANGLE_FAN,
	//	20,
	//	4);

	////Unbind vao

	//glBindVertexArray(0);

	////glBindRenderbuffer(GL_RENDERBUFFER, 0);

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);//Test

	//glUseProgram(0);

	glUseProgram(glShaderProgramObject_ModelLoading);

	//Declaration of Matrices
	mat4 modelViewMatrix;
	mat4 modelViewProjectionMatrix;
	mat4 translationMatrix;
	mat4 rotationMatrix;
	mat4 scaleMatrix;

	//Pyramid
	//Initialize matrices

	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glViewport(0, 0, 1024, 1024);
	glClearColor(0.5, 0.5, 0.5, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	translationMatrix = translate(0.0f, -8.0f, -45.0f);
	scaleMatrix = scale(1.0f, -1.0f, 1.0f);
	rotationMatrix = rotate(anglePyramid, 0.0f, 1.0f, 0.0f);
	//rotationMatrix = rotate(anglePyramid, anglePyramid, anglePyramid);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);
	//glFrontFace(GL_CCW);


	//Matrix Multiplication
	modelViewMatrix = scaleMatrix* translationMatrix * rotationMatrix;

	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	
	//Send necessary matrices to shader in resp. Uniforms

	glUniformMatrix4fv(mvpUniform_ModelLoading,//Changed
		1,
		GL_FALSE,
		modelViewProjectionMatrix);

	glBindTexture(GL_TEXTURE_2D, texture_Marble);

	glUniform1i(samplerUniform_ModelLoading, 0);

	glBindVertexArray(vao_ModelLoading);
	//Smilarly bind with texture if any
	//Draw necessary scene
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gElementBuffer);
	//glDrawElements(GL_TRIANGLES, gp_vertex_indices->size, GL_UNSIGNED_BYTE, NULL);
	glDrawElements(GL_TRIANGLES, gp_vertex_indices->size, GL_UNSIGNED_INT, NULL);

	/*Unbind vao*/
	glBindVertexArray(0);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);//Test

	glUseProgram(0);


	glClearColor(0.0, 0.0, 0.0, 0.0);

	glViewport(0, 0, (GLsizei)windowWidth, (GLsizei)windowHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(gShaderProgramObject_FBO);
	//CUBE
	//Initialize matrices

	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	scaleMatrix = mat4::identity();

	//Transformation

	translationMatrix = translate(0.0f, 0.0f, -5.0f);
	scaleMatrix = scale(0.75f, 0.75f, 0.75f);
	rotationMatrix = rotate(angleCube, angleCube, angleCube);

	//Matrix Multiplication

	modelViewMatrix = translationMatrix * scaleMatrix * rotationMatrix;

	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	//Send necessary matrices to shader in resp. Uniforms

	glUniformMatrix4fv(mvpUniform,
		1,
		GL_FALSE,
		modelViewProjectionMatrix);

	glUniform1i(samplerUniform_FBO, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_FBO);
	//Bind with vao

	glBindVertexArray(vao_cube);

	//Bind with textures if any

	//Draw
	//6Faces
	glDrawArrays(GL_TRIANGLE_FAN,
		0,
		4);
	glDrawArrays(GL_TRIANGLE_FAN,
		4,
		4);
	glDrawArrays(GL_TRIANGLE_FAN,
		8,
		4);
	glDrawArrays(GL_TRIANGLE_FAN,
		12,
		4);
	glDrawArrays(GL_TRIANGLE_FAN,
		16,
		4);
	glDrawArrays(GL_TRIANGLE_FAN,
		20,
		4);
	//Unbind vao

	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(0);

	SwapBuffers(ghdc);

}
void Update(void)
{
	anglePyramid = anglePyramid + 0.1f;
	if (anglePyramid > 360.0f)
		anglePyramid = 0.0f;
	angleCube = angleCube + 0.01f;
	if (angleCube > 360.0f)
		angleCube = 0.0f;
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


	/*if (vbo_position_pyramid) {
		glDeleteBuffers(1, &vbo_position_pyramid);
		vbo_position_pyramid = 0;
	}

	if (vbo_color_pyramid) {
		glDeleteBuffers(1, &vbo_color_pyramid);
		vbo_color_pyramid = 0;
	}

	if (vao_pyramid) {
		glDeleteVertexArrays(1, &vao_pyramid);
		vao_pyramid = 0;
	}*/

	if (vbo_texCoord_Cube) {
		glDeleteBuffers(1, &vbo_texCoord_Cube);
		vbo_texCoord_Cube = 0;
	}


	if (vbo_position_cube) {
		glDeleteBuffers(1, &vbo_position_cube);
		vbo_position_cube = 0;
	}
	if (vao_cube) {
		glDeleteVertexArrays(1, &vao_cube);
		vao_cube = 0;
	}


	if (texture_FBO) {
		glDeleteTextures(1, &texture_FBO);
		texture_FBO = 0;
	}

	if (FBO) {
		glDeleteFramebuffers(1, &FBO);
		FBO = 0;
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



BOOL LoadTexture(GLuint *texture, TCHAR imageResourceID[]) {

	HBITMAP hBitMap = NULL;
	BITMAP bmp;
	BOOL bStatus = FALSE;
	hBitMap = (HBITMAP)LoadImage(GetModuleHandle(NULL),
		imageResourceID,
		IMAGE_BITMAP,
		0,
		0,
		LR_CREATEDIBSECTION);
	if (hBitMap) {
		bStatus = TRUE;
		GetObject(hBitMap,
			sizeof(BITMAP),
			&bmp);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glGenTextures(1, texture);
		glBindTexture(GL_TEXTURE_2D, *texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		//gluBuild2DMipmaps(GL_TEXTURE_2D, 3, bmp.bmWidth, bmp.bmHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, bmp.bmBits);

		glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGB,
			bmp.bmWidth,
			bmp.bmHeight,
			0,
			GL_BGR,
			GL_UNSIGNED_BYTE,
			bmp.bmBits
		);

		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);


		DeleteObject(hBitMap);
	}

	return bStatus;
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

void LoadMesh()
{
	void UnInitialize(void);
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
		UnInitialize();
		exit(0);
	}
	gp_vec_normals = create_vec_float();
	if (gp_vec_normals == NULL)
	{
		fprintf(gpFile, "Failed to allocate memory to vertices vector\n");
		UnInitialize();
		exit(0);
	}
	gp_vec_textures = create_vec_float();
	if (gp_vec_textures == NULL)
	{
		fprintf(gpFile, "Failed to allocate memory to vertices vector\n");
		UnInitialize();
		exit(0);
	}
	gp_vec_vertex_sorted = create_vec_float();
	if (gp_vec_vertex_sorted == NULL)
	{
		fprintf(gpFile, "Failed to allocate memory to vertices vector\n");
		UnInitialize();
		exit(0);
	}
	gp_vec_normals_sorted = create_vec_float();
	if (gp_vec_normals_sorted == NULL)
	{
		fprintf(gpFile, "Failed to allocate memory to vertices vector\n");
		UnInitialize();
		exit(0);
	}
	gp_vec_textures_sorted = create_vec_float();
	if (gp_vec_textures_sorted == NULL)
	{
		fprintf(gpFile, "Failed to allocate memory to vertices vector\n");
		UnInitialize();
		exit(0);
	}
	gp_vertex_indices = create_vec_int();
	if (gp_vertex_indices == NULL)
	{
		fprintf(gpFile, "Failed to allocate memory to vertices vector\n");
		UnInitialize();
		exit(0);
	}
	gp_normals_indces = create_vec_int();
	if (gp_normals_indces == NULL)
	{
		fprintf(gpFile, "Failed to allocate memory to vertices vector\n");
		UnInitialize();
		exit(0);
	}
	gp_texture_indices = create_vec_int();
	if (gp_texture_indices == NULL)
	{
		fprintf(gpFile, "Failed to allocate memory to vertices vector\n");
		UnInitialize();
		exit(0);
	}

	fopen_s(&gpMesh, "teapot.obj", "r");
	if (gpMesh == NULL)
	{
		fprintf(gpFile, "Failed to load model file\n");
		UnInitialize();
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
				push_back_vec_int(gp_vertex_indices, atoi(token) - 1);
				fprintf_s(gpFile, "\n%d\t", atoi(token) - 1);
				token = strtok_s(NULL, slash, &next_token);
				push_back_vec_int(gp_texture_indices, atoi(token) - 1);
				token = strtok_s(NULL, slash, &next_token);
				push_back_vec_int(gp_normals_indces, atoi(token) - 1);
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
