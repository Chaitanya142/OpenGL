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

GLuint gVertexShaderObject;
//GLuint gTessellationControlShaderObject;
//GLuint gTessellationEvaluationShaderObject;

GLuint gGeometryShaderObject;
GLuint gFragementShaderObject;
GLuint gShaderProgramObject;

GLuint gVao;
GLuint gVbo;
//GLuint gNumberOfSegmentsUniform;
//GLuint gNumberOfStripsUniform;
//GLuint gLineColorUniform;

enum {
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXCOORD0
};


GLuint mvpUniform;
mat4 perspectiveProjectionMatrix;

//int gNumberOfLineSegments;

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
	/*case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_UP:
			gNumberOfLineSegments++;
			if (gNumberOfLineSegments >= 50)
				gNumberOfLineSegments = 50;
			break;
		case VK_DOWN:
			gNumberOfLineSegments--;
			if (gNumberOfLineSegments <= 0)
				gNumberOfLineSegments = 1;
			break;
		}
		break;*/
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

	//Vertex Shader
	//Define Shader Object
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	//Vertex Shader Code
	const GLchar* vertextShaderSourceCode =
		"#version 430 core " \
		"\n " \
		"in vec4 vPosition;" \
		"uniform mat4 u_mvp_matrix;" \
		"void main(void)" \
		"{" \
		"gl_Position=u_mvp_matrix * vPosition;" \
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


	////Tessellation Control Shader
	////Define Shader Object
	//gTessellationControlShaderObject = glCreateShader(GL_TESS_CONTROL_SHADER);

	////Fragement Shader Code
	//const GLchar* tessellationControlShaderSourceCode =
	//	"#version 430 core " \
	//	"\n " \
	//	"layout(vertices=4)out;"\
	//	"uniform int numberOfSegments;"\
	//	"uniform int numberOfStrips;"\
	//	"void main(void)" \
	//	"{" \
	//	"gl_out[gl_InvocationID].gl_Position=gl_in[gl_InvocationID].gl_Position;"\
	//	"gl_TessLevelOuter[0]=float(numberOfStrips);"\
	//	"gl_TessLevelOuter[1]=float(numberOfSegments);"\
	//	"} ";

	////Shader object and source code mapping
	//glShaderSource(gTessellationControlShaderObject, 1, &tessellationControlShaderSourceCode, NULL);

	////Compile Shader
	//glCompileShader(gTessellationControlShaderObject);

	////Error Checking
	//iShaderCompileStatus = 0;
	//iInfoLogLength = 0;
	//szInfoLog = NULL;

	//glGetShaderiv(gTessellationControlShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);

	//if (iShaderCompileStatus == GL_FALSE) {
	//	glGetShaderiv(gTessellationControlShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
	//	if (iInfoLogLength > 0) {
	//		szInfoLog = (GLchar *)malloc(iInfoLogLength);
	//		if (szInfoLog != NULL) {
	//			GLsizei written;
	//			glGetShaderInfoLog(gTessellationControlShaderObject,
	//				iInfoLogLength,
	//				&written,
	//				szInfoLog);
	//			fprintf_s(gpFile, "Tesselation Control Shader Compilation Log %s\n", szInfoLog);
	//			free(szInfoLog);
	//			UnInitialize();
	//			DestroyWindow(0);
	//			exit(0);
	//		}
	//	}
	//}




	////Tessellation Evaluation Shader
	////Define Shader Object
	//gTessellationEvaluationShaderObject = glCreateShader(GL_TESS_EVALUATION_SHADER);

	////Fragement Shader Code
	//const GLchar* tessellationEvaluationShaderSourceCode =
	//	"#version 430 core " \
	//	"\n " \
	//	"layout(isolines)in;"\
	//	"uniform mat4 u_mvp_matrix;" \
	//	"void main(void)" \
	//	"{" \
	//	"float u=gl_TessCoord.x;"\
	//	"vec3 p0=gl_in[0].gl_Position.xyz;"\
	//	"vec3 p1=gl_in[1].gl_Position.xyz;"\
	//	"vec3 p2=gl_in[2].gl_Position.xyz;"\
	//	"vec3 p3=gl_in[3].gl_Position.xyz;"\
	//	"float u1=	(1.0	-	u);"\
	//	"float u2=	u	*	u;"\
	//	"float b3=	u2	*	u;"\
	//	"float b2=	3.0	*	u2	*	u1;"\
	//	"float b1=	3.0	*	u	*	u1	*	u1;"\
	//	"float b0=	u1	*	u1	*	u1;"\
	//	"vec3 p=	p0*b0	+	p1*b1	+	p2*b2	+	p3*b3;"\
	//	"gl_Position=u_mvp_matrix * vec4(	p,	1.0);" \
	//	"} ";

	////Shader object and source code mapping
	//glShaderSource(gTessellationEvaluationShaderObject, 1, &tessellationEvaluationShaderSourceCode, NULL);

	////Compile Shader
	//glCompileShader(gTessellationEvaluationShaderObject);

	////Error Checking
	//iShaderCompileStatus = 0;
	//iInfoLogLength = 0;
	//szInfoLog = NULL;

	//glGetShaderiv(gTessellationEvaluationShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);

	//if (iShaderCompileStatus == GL_FALSE) {
	//	glGetShaderiv(gTessellationEvaluationShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
	//	if (iInfoLogLength > 0) {
	//		szInfoLog = (GLchar *)malloc(iInfoLogLength);
	//		if (szInfoLog != NULL) {
	//			GLsizei written;
	//			glGetShaderInfoLog(gTessellationEvaluationShaderObject,
	//				iInfoLogLength,
	//				&written,
	//				szInfoLog);
	//			fprintf_s(gpFile, "Tesselation Evaluaiton Shader Compilation Log %s\n", szInfoLog);
	//			free(szInfoLog);
	//			UnInitialize();
	//			DestroyWindow(0);
	//			exit(0);
	//		}
	//	}
	//}

	//Geometry Shader
	//Define Shader Object
	gGeometryShaderObject = glCreateShader(GL_GEOMETRY_SHADER);

	//Fragement Shader Code
	const GLchar* gGeometryShaderSourceCode =
		"#version 430 core " \
		"\n " \
		"layout(triangles)in;"\
		"layout(triangle_strip,max_vertices=9)out;"\
		"uniform mat4 u_mvp_matrix;" \
		"void main(void)" \
		"{" \
		"for(int vertex=0;vertex<3;vertex++)"\
		"{"\
		"gl_Position=u_mvp_matrix*(gl_in[vertex].gl_Position+vec4(0.0,1.0,0.0,0.0));"\
		"EmitVertex();"\

		"gl_Position=u_mvp_matrix*(gl_in[vertex].gl_Position+vec4(-1.0,-1.0,0.0,0.0));"\
		"EmitVertex();"\

		"gl_Position=u_mvp_matrix*(gl_in[vertex].gl_Position+vec4(1.0,-1.0,0.0,0.0));"\
		"EmitVertex();"\

		"EndPrimitive();"\

		"}"\
		"} ";

	//Shader object and source code mapping
	glShaderSource(gGeometryShaderObject, 1, &gGeometryShaderSourceCode, NULL);

	//Compile Shader
	glCompileShader(gGeometryShaderObject);

	//Error Checking
	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(gGeometryShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);

	if (iShaderCompileStatus == GL_FALSE) {
		glGetShaderiv(gGeometryShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0) {
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL) {
				GLsizei written;
				glGetShaderInfoLog(gGeometryShaderObject,
					iInfoLogLength,
					&written,
					szInfoLog);
				fprintf_s(gpFile, "Geometry Shader Compilation Log %s\n", szInfoLog);
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
		"out vec4 FragColor;"
		"void main(void)" \
		"{" \
		"FragColor=vec4(1.0,1.0,1.0,1.0);" \
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
	//glAttachShader(gShaderProgramObject, gTessellationControlShaderObject);
	//glAttachShader(gShaderProgramObject, gTessellationEvaluationShaderObject);
	glAttachShader(gShaderProgramObject, gGeometryShaderObject);

	glAttachShader(gShaderProgramObject, gFragementShaderObject);

	//Prelinking
	//Binding to attribute
	glBindAttribLocation(gShaderProgramObject,
		AMC_ATTRIBUTE_POSITION,
		"vPosition");

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

	
	const GLfloat triangleVertices[] = {
		0.0f,	1.0f,	0.0f,
		-1.0f,	-1.0f,	0.0f,
		1.0f,	-1.0f,	0.0f
	};



	glGenVertexArrays(1, &gVao);
	glBindVertexArray(gVao);

	glGenBuffers(1, &gVbo);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices),
		triangleVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT,
		GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_CULL_FACE);
	//glDisable(GL_CULL_FACE);

	glLineWidth(3.0f);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);

	glEnable(GL_TEXTURE_2D);

	perspectiveProjectionMatrix = mat4::identity();

	//gNumberOfLineSegments = 1;

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

	glUseProgram(gShaderProgramObject);

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
	//Transformation

	translationMatrix = translate(0.0f, 0.0f, -5.0f);
	//rotationMatrix = rotate(anglePyramid, 0.0f, 1.0f, 0.0f);

	//Matrix Multiplication
	modelViewMatrix = translationMatrix * rotationMatrix;

	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	//Send necessary matrices to shader in resp. Uniforms

	glUniformMatrix4fv(mvpUniform,
		1,
		GL_FALSE,
		modelViewProjectionMatrix);

	//glUniform1i(gNumberOfSegmentsUniform, gNumberOfLineSegments);

	/*TCHAR str[255];

	wsprintf(str,TEXT("OpenGL Programmable PipeLine Window : [ Segments = %d ]",gNumberOfLineSegments ));

	SetWindowText(ghwnd,str);*/

	//glUniform1i(gNumberOfStripsUniform, 1);

	//vec4 colorVec = vec4(1.0f, 1.0f, 0.0f, 1.0f);

	//glUniform4fv(gLineColorUniform, 1, colorVec);

	//Bind with vao

	glBindVertexArray(gVao);

	//glPatchParameteri(GL_PATCH_VERTICES, 4);

	//Bind with textures if any

	//Draw

	glDrawArrays(GL_TRIANGLES,
		0,
		3);

	//Unbind vao

	glBindVertexArray(0);


	glUseProgram(0);

	SwapBuffers(ghdc);

}
void Update(void)
{

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

	if (gVao) {
		glDeleteVertexArrays(1, &gVao);
		gVao = 0;
	}

	if (gVbo) {
		glDeleteBuffers(1, &gVbo);
		gVbo = 0;
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

