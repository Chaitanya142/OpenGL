#include<Windows.h>
#include<stdio.h>

#include<GL\glew.h>
#include<gl\GL.h>

//#include <time.h>

#include"vmath.h"
#include<math.h>
#include"Sphere.h"

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

//GLubyte checkImage[checkImageWidth][checkImageHeight][4];
//GLuint txtImage;

//Shader Program Objects
GLint gShaderProgramObject;

enum {
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXCOORD0
};

GLuint vao_quad;
GLuint vao_triangle;
GLuint vao_point;

GLuint vbo_position_quad;
GLuint vbo_position_triangle;
GLuint vbo_position_point;

GLuint vbo_color_quad;
GLuint vbo_color_triangle;
GLuint vbo_color_point;

//GLuint vbo_texCoord_rectangle;

GLuint mvpUniform;
//GLuint samplerUniform;

mat4 perspectiveProjectionMatrix;

GLfloat D_R_OrangeColor = 1.0f;
GLfloat D_G_OrangeColor = 0.6f;
GLfloat D_B_OrangeColor = 0.2f;
GLfloat D_R_GreenColor = 0.074f;
GLfloat D_G_GreenColor = 0.533f;
GLfloat D_B_GreenColor = 0.2f;

GLfloat Line_Orange_R = 1.0f;
GLfloat Line_Orange_G = 0.6f;
GLfloat Line_Orange_B = 0.2f;
GLfloat Line_White_R = 1.00f;
GLfloat Line_White_G = 1.00f;
GLfloat Line_White_B = 1.00f;
GLfloat Line_Green_R = 0.074f;
GLfloat Line_Green_G = 0.533f;
GLfloat Line_Green_B = 0.2f;

GLint stage = 1;
GLfloat I_1_XPosition = -2.25f;
GLfloat A_XPosition = 2.25f;
GLfloat N_YPosition = 2.25f;
GLfloat I_2_YPosition = -2.25f;
GLfloat Plane1_Y_Position = 0.00f;
GLfloat Plane1_Z_Rotation_Angle = -90.0f;

GLfloat Plane1_X_Position = -2.5f;
GLfloat angle = 0.0f;
GLfloat Line_X_Position = -1.28f;

Sphere planeFrontSphere;

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

	//"in vec4 vColor;" \
	//"out vec4 out_color;" \
		//"out_color=vColor;"\

	//"in vec2 vTexCoord;" \
	//"out vec2 out_TexCoord;" \
	//"out_TexCoord=vTexCoord;"\


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
		"in vec4 out_color;"\
		"out vec4 fragColor;"
		"void main(void)" \
		"{" \
		"fragColor=out_color;" \
		"} ";


	//"in vec4 out_color;"\
	//"fragColor=out_color;" \

	//"uniform sampler2D u_sampler;"
	//"in vec2 out_TexCoord;"
	//"fragColor=texture(u_sampler,out_TexCoord);" \


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

	//glBindAttribLocation(gShaderProgramObject,
	//	AMC_ATTRIBUTE_TEXCOORD0,
	//	"vTexCoord");

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

	/*samplerUniform = glGetUniformLocation(gShaderProgramObject,
		"u_sampler");*/


		//RECTANGLE
	glGenVertexArrays(1, &vao_quad);

	glBindVertexArray(vao_quad);

	//Generate Buffer
	glGenBuffers(1, &vbo_position_quad);
	//Bind Generated Buffer
	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_position_quad);
	//Fill Buffer
	//glBufferData(GL_ARRAY_BUFFER,
	//	sizeof(rectangleVertices),
	//	rectangleVertices,
	//	GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER,
		4 * 3 * sizeof(float),
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

	//COLOR Generate Buffer
	glGenBuffers(1, &vbo_color_quad);
	//Bind Generated Buffer
	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_color_quad);
	//Fill Buffer
	//glBufferData(GL_ARRAY_BUFFER,
	//	sizeof(rectangleColors),
	//	rectangleColors,
	//	GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER,
		4 * 3 * sizeof(float),
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



	//Create vao
	//Save everying in single set
	glGenVertexArrays(1, &vao_triangle);

	glBindVertexArray(vao_triangle);


	//TRIANGLE
	//Generate Buffer
	glGenBuffers(1, &vbo_position_triangle);
	//Bind Generated Buffer
	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_position_triangle);
	//Fill Buffer
	glBufferData(GL_ARRAY_BUFFER,
		3 * 3 * sizeof(float),
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

	glGenBuffers(1, &vbo_color_triangle);
	//Bind Generated Buffer
	glBindBuffer(GL_ARRAY_BUFFER,
		vbo_color_triangle);
	//Fill Buffer
	glBufferData(GL_ARRAY_BUFFER,
		3 * 3 * sizeof(float),
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

	planeFrontSphere.setCenter(0.21f,0.0f);
	planeFrontSphere.setTextureColor(0.729f , 0.886f , 0.933f);//(1.0f*255.0f, 1.0f*255.0f, 1.0f * 255); 
	planeFrontSphere.set(0.07f, 30, 30);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0f);

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

	perspectiveProjectionMatrix = perspective(60.0f,
		(GLfloat)width / (GLfloat)height,
		1.0f,
		30.0f);

}

//Function Display
void Display(void) {

	void DrawLetter(char letter);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(gShaderProgramObject);

	//Declaration of Matrices
	mat4 modelViewMatrix;
	mat4 modelViewProjectionMatrix;
	mat4 translationMatrix;
	mat4 rotationMatrix;


	if (stage > 0) {
		//Initialize matrices
		modelViewMatrix = mat4::identity();
		modelViewProjectionMatrix = mat4::identity();
		translationMatrix = mat4::identity();
		rotationMatrix = mat4::identity();

		//Transformation

		translationMatrix = translate(I_1_XPosition, 0.0f, -3.0f);

		//Matrix Multiplication
		modelViewMatrix = translationMatrix * rotationMatrix;

		modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

		//Send necessary matrices to shader in resp. Uniforms

		glUniformMatrix4fv(mvpUniform,
			1,
			GL_FALSE,
			modelViewProjectionMatrix);

		//Bind with vao_quad

		//Bind with textures if any

		DrawLetter('I');

		//Unbind vao_quad

	}

	if (stage > 2) {
		//Initialize matrices
		modelViewMatrix = mat4::identity();
		modelViewProjectionMatrix = mat4::identity();
		translationMatrix = mat4::identity();
		rotationMatrix = mat4::identity();

		//Transformation

		translationMatrix = translate(-1.0f, N_YPosition, -3.0f);

		//Matrix Multiplication
		modelViewMatrix = translationMatrix * rotationMatrix;

		modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

		//Send necessary matrices to shader in resp. Uniforms

		glUniformMatrix4fv(mvpUniform,
			1,
			GL_FALSE,
			modelViewProjectionMatrix);

		//Bind with vao_quad

		//Bind with textures if any

		DrawLetter('N');

		//Unbind vao_quad

	}

	if (stage > 4) {
		//Initialize matrices
		modelViewMatrix = mat4::identity();
		modelViewProjectionMatrix = mat4::identity();
		translationMatrix = mat4::identity();
		rotationMatrix = mat4::identity();

		//Transformation

		translationMatrix = translate(-0.25f, 0.0f, -3.0f);

		//Matrix Multiplication
		modelViewMatrix = translationMatrix * rotationMatrix;

		modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

		//Send necessary matrices to shader in resp. Uniforms

		glUniformMatrix4fv(mvpUniform,
			1,
			GL_FALSE,
			modelViewProjectionMatrix);

		//Bind with vao_quad

		//Bind with textures if any

		DrawLetter('D');

		//Unbind vao_quad

	}

	if (stage > 3) {

		//Initialize matrices
		modelViewMatrix = mat4::identity();
		modelViewProjectionMatrix = mat4::identity();
		translationMatrix = mat4::identity();
		rotationMatrix = mat4::identity();

		//Transformation

		translationMatrix = translate(0.25f, I_2_YPosition, -3.0f);

		//Matrix Multiplication
		modelViewMatrix = translationMatrix * rotationMatrix;

		modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

		//Send necessary matrices to shader in resp. Uniforms

		glUniformMatrix4fv(mvpUniform,
			1,
			GL_FALSE,
			modelViewProjectionMatrix);

		//Bind with vao_quad

		//Bind with textures if any

		DrawLetter('I');

		//Unbind vao_quad

	}
	if (stage > 1) {

		//Initialize matrices
		modelViewMatrix = mat4::identity();
		modelViewProjectionMatrix = mat4::identity();
		translationMatrix = mat4::identity();
		rotationMatrix = mat4::identity();

		//Transformation

		translationMatrix = translate(A_XPosition, 0.0f, -3.0f);

		//Matrix Multiplication
		modelViewMatrix = translationMatrix * rotationMatrix;

		modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

		//Send necessary matrices to shader in resp. Uniforms

		glUniformMatrix4fv(mvpUniform,
			1,
			GL_FALSE,
			modelViewProjectionMatrix);

		//Bind with vao_quad

		//Bind with textures if any

		DrawLetter('A');
		//DrawLetter('T');

		//Unbind vao_quad

	}
	if (stage == 8) {
		Plane1_Y_Position = ((Plane1_X_Position - 1.3)*(Plane1_X_Position - 1.3) / (4 * 0.15)) - 0.0f;

		//Initialize matrices
		modelViewMatrix = mat4::identity();
		modelViewProjectionMatrix = mat4::identity();
		translationMatrix = mat4::identity();
		rotationMatrix = mat4::identity();

		//Transformation
		translationMatrix = translate(Plane1_X_Position, Plane1_Y_Position, -3.0f);

		rotationMatrix = rotate(Plane1_Z_Rotation_Angle, 0.0f, 0.0f, 1.0f);

		//Matrix Multiplication
		modelViewMatrix = translationMatrix * rotationMatrix;

		modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

		//Send necessary matrices to shader in resp. Uniforms

		glUniformMatrix4fv(mvpUniform,
			1,
			GL_FALSE,
			modelViewProjectionMatrix);

		//Bind with vao_quad

		//Bind with textures if any

		DrawLetter('P');

		//Unbind vao_quad


		//Initialize matrices
		modelViewMatrix = mat4::identity();
		modelViewProjectionMatrix = mat4::identity();
		translationMatrix = mat4::identity();
		rotationMatrix = mat4::identity();

		//Transformation
		translationMatrix = translate(Plane1_X_Position, 0.0f, -3.0f);

		//Matrix Multiplication
		modelViewMatrix = translationMatrix * rotationMatrix;

		modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

		//Send necessary matrices to shader in resp. Uniforms

		glUniformMatrix4fv(mvpUniform,
			1,
			GL_FALSE,
			modelViewProjectionMatrix);

		//Bind with vao_quad

		//Bind with textures if any

		DrawLetter('P');

		//Unbind vao_quad


		//Initialize matrices
		modelViewMatrix = mat4::identity();
		modelViewProjectionMatrix = mat4::identity();
		translationMatrix = mat4::identity();
		rotationMatrix = mat4::identity();

		//Transformation
		translationMatrix = translate(Plane1_X_Position, -Plane1_Y_Position, -3.0f);
		rotationMatrix = rotate(-Plane1_Z_Rotation_Angle, 0.0f, 0.0f, 1.0f);

		//Matrix Multiplication
		modelViewMatrix = translationMatrix * rotationMatrix;

		modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

		//Send necessary matrices to shader in resp. Uniforms

		glUniformMatrix4fv(mvpUniform,
			1,
			GL_FALSE,
			modelViewProjectionMatrix);

		//Bind with vao_quad

		//Bind with textures if any

		DrawLetter('P');

		//Unbind vao_quad
	}

	if (stage == 7) {
		//Initialize matrices
		modelViewMatrix = mat4::identity();
		modelViewProjectionMatrix = mat4::identity();
		translationMatrix = mat4::identity();
		rotationMatrix = mat4::identity();

		//Transformation
		translationMatrix = translate(Plane1_X_Position, 0.0f, -3.0f);
		rotationMatrix = rotate(-Plane1_Z_Rotation_Angle, 0.0f, 0.0f, 1.0f);

		//Matrix Multiplication
		modelViewMatrix = translationMatrix * rotationMatrix;

		modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

		//Send necessary matrices to shader in resp. Uniforms

		glUniformMatrix4fv(mvpUniform,
			1,
			GL_FALSE,
			modelViewProjectionMatrix);

		//Bind with vao_quad

		//Bind with textures if any

		DrawLetter('P');

		//Unbind vao_quad

	}

	if ((stage == 7 || stage == 8) && Plane1_X_Position > -1.02f && Plane1_X_Position < 1.40f) {
		//Initialize matrices
		modelViewMatrix = mat4::identity();
		modelViewProjectionMatrix = mat4::identity();
		translationMatrix = mat4::identity();
		rotationMatrix = mat4::identity();

		//Transformation
		translationMatrix = translate(0.0f, 0.0f, -3.0f);

		//Matrix Multiplication
		modelViewMatrix = translationMatrix * rotationMatrix;

		modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

		//Send necessary matrices to shader in resp. Uniforms

		glUniformMatrix4fv(mvpUniform,
			1,
			GL_FALSE,
			modelViewProjectionMatrix);

		//Bind with vao_quad

		//Bind with textures if any

		DrawLetter('L');

		//Unbind vao_quad

	}

	if (stage > 7 && Plane1_X_Position > 1.40f) {
		//Initialize matrices
		modelViewMatrix = mat4::identity();
		modelViewProjectionMatrix = mat4::identity();
		translationMatrix = mat4::identity();
		rotationMatrix = mat4::identity();

		//Transformation
		translationMatrix = translate(A_XPosition, 0.0f, -3.0f);

		//Matrix Multiplication
		modelViewMatrix = translationMatrix * rotationMatrix;

		modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

		//Send necessary matrices to shader in resp. Uniforms

		glUniformMatrix4fv(mvpUniform,
			1,
			GL_FALSE,
			modelViewProjectionMatrix);

		//Bind with vao_quad

		//Bind with textures if any

		DrawLetter('T');

		//Unbind vao_quad
	}

	if (stage > 7 && Plane1_X_Position > 1.40f && Line_X_Position <= 1.10f) {
		Line_X_Position = Line_X_Position + 0.006f;

		if (Line_White_R > 0.0f)
			Line_White_R = Line_White_R - Line_White_R / 256;
		if (Line_White_G > 0.0f)
			Line_White_G = Line_White_G - Line_White_G / 256;
		if (Line_White_B > 0.0f)
			Line_White_B = Line_White_B - Line_White_B / 256;

		if (Line_Orange_R > 0.0f)
			Line_Orange_R = Line_Orange_R - Line_Orange_R / 256;
		if (Line_Orange_G > 0.0f)
			Line_Orange_G = Line_Orange_G - Line_Orange_G / 256;
		if (Line_Orange_B > 0.0f)
			Line_Orange_B = Line_Orange_B - Line_Orange_B / 256;

		if (Line_Green_R > 0.0f)
			Line_Green_R = Line_Green_R - Line_Green_R / 256;
		if (Line_Green_G > 0.0f)
			Line_Green_G = Line_Green_G - Line_Green_G / 256;
		if (Line_Green_B > 0.0f)
			Line_Green_B = Line_Green_B - Line_Green_B / 256;

		//Initialize matrices
		modelViewMatrix = mat4::identity();
		modelViewProjectionMatrix = mat4::identity();
		translationMatrix = mat4::identity();
		rotationMatrix = mat4::identity();

		//Transformation
		translationMatrix = translate(0.0f, 0.0f, -3.0f);

		//Matrix Multiplication
		modelViewMatrix = translationMatrix * rotationMatrix;

		modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

		//Send necessary matrices to shader in resp. Uniforms

		glUniformMatrix4fv(mvpUniform,
			1,
			GL_FALSE,
			modelViewProjectionMatrix);

		//Bind with vao_quad

		//Bind with textures if any

		DrawLetter('L');

		//Unbind vao_quad

	}

	if (stage > 7 && Plane1_X_Position > 1.40f) {
		//Initialize matrices
		modelViewMatrix = mat4::identity();
		modelViewProjectionMatrix = mat4::identity();
		translationMatrix = mat4::identity();
		rotationMatrix = mat4::identity();

		//Transformation
		translationMatrix = translate(A_XPosition, 0.0f, -3.0f);

		//Matrix Multiplication
		modelViewMatrix = translationMatrix * rotationMatrix;

		modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

		//Send necessary matrices to shader in resp. Uniforms

		glUniformMatrix4fv(mvpUniform,
			1,
			GL_FALSE,
			modelViewProjectionMatrix);

		//Bind with vao_quad

		//Bind with textures if any

		DrawLetter('T');

		//Unbind vao_quad

	}


	if (stage == 6)
	{
		Plane1_Y_Position = ((Plane1_X_Position + 1.3)*(Plane1_X_Position + 1.3) / (4 * 0.15)) - 0.0f;

		//Initialize matrices
		modelViewMatrix = mat4::identity();
		modelViewProjectionMatrix = mat4::identity();
		translationMatrix = mat4::identity();
		rotationMatrix = mat4::identity();

		//Transformation
		translationMatrix = translate(Plane1_X_Position, Plane1_Y_Position, -3.0f);
		rotationMatrix = rotate(Plane1_Z_Rotation_Angle, 0.0f, 0.0f, 1.0f);

		//Matrix Multiplication
		modelViewMatrix = translationMatrix * rotationMatrix;

		modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

		//Send necessary matrices to shader in resp. Uniforms

		glUniformMatrix4fv(mvpUniform,
			1,
			GL_FALSE,
			modelViewProjectionMatrix);

		//Bind with vao_quad

		//Bind with textures if any

		DrawLetter('P');

		//Unbind vao_quad

		//Initialize matrices
		modelViewMatrix = mat4::identity();
		modelViewProjectionMatrix = mat4::identity();
		translationMatrix = mat4::identity();
		rotationMatrix = mat4::identity();

		//Transformation
		translationMatrix = translate(Plane1_X_Position, 0.0f, -3.0f);

		//Matrix Multiplication
		modelViewMatrix = translationMatrix * rotationMatrix;

		modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

		//Send necessary matrices to shader in resp. Uniforms

		glUniformMatrix4fv(mvpUniform,
			1,
			GL_FALSE,
			modelViewProjectionMatrix);

		//Bind with vao_quad

		//Bind with textures if any

		DrawLetter('P');

		//Unbind vao_quad

		//Initialize matrices
		modelViewMatrix = mat4::identity();
		modelViewProjectionMatrix = mat4::identity();
		translationMatrix = mat4::identity();
		rotationMatrix = mat4::identity();

		//Transformation
		translationMatrix = translate(Plane1_X_Position, -1 * (Plane1_Y_Position), -3.0f);
		rotationMatrix = rotate(-Plane1_Z_Rotation_Angle, 0.0f, 0.0f, 1.0f);

		//Matrix Multiplication
		modelViewMatrix = translationMatrix * rotationMatrix;

		modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

		//Send necessary matrices to shader in resp. Uniforms

		glUniformMatrix4fv(mvpUniform,
			1,
			GL_FALSE,
			modelViewProjectionMatrix);

		//Bind with vao_quad

		//Bind with textures if any

		DrawLetter('P');

		//Unbind vao_quad

	}


	glUseProgram(0);

	SwapBuffers(ghdc);

}
void Update(void)
{
	switch (stage) {
	case 1:
		//Print I
		if (I_1_XPosition < -1.5f)
			I_1_XPosition = I_1_XPosition + 0.00022f;
		else {
			I_1_XPosition = -1.5f;
			stage++;
		}
		break;
	case 2:
		//Print A
		if (A_XPosition > 0.75f)
			A_XPosition = A_XPosition - 0.00022f;
		else {
			A_XPosition = 0.75f;
			stage++;
		}
		break;
	case 3:
		//Print N
		if (N_YPosition > 0.00f)
			N_YPosition = N_YPosition - 0.00022f;
		else {
			N_YPosition = 0.00f;
			stage++;
		}
		break;
	case 4:
		//Print I
		if (I_2_YPosition < 0.00f)
			I_2_YPosition = I_2_YPosition + 0.00022f;
		else {
			I_2_YPosition = 0.00f;
			stage++;
		}
		break;
	case 5:
		//Print D
		if (D_R_OrangeColor < 1.0f)
			D_R_OrangeColor = D_R_OrangeColor + 1.0f / 5050;
		if (D_G_OrangeColor < 0.6f)
			D_G_OrangeColor = D_G_OrangeColor + 0.6f / 5050;
		if (D_B_OrangeColor < 0.2f)
			D_B_OrangeColor = D_B_OrangeColor + 0.2f / 5050;

		if (D_R_GreenColor < 0.074f)
			D_R_GreenColor = D_R_GreenColor + 0.074f / 5050;
		if (D_G_GreenColor < 0.533f)
			D_G_GreenColor = D_G_GreenColor + 0.533f / 5050;
		if (D_B_GreenColor < 0.2f)
			D_B_GreenColor = D_B_GreenColor + 0.2f / 5050;

		if (D_R_OrangeColor >= 1.0f && D_G_OrangeColor >= 0.6f && D_B_OrangeColor >= 0.2f && D_R_GreenColor >= 0.074f && D_G_GreenColor >= 0.533f && D_B_GreenColor >= 0.2f)
			stage++;
		break;
	case 6:
		if (Plane1_X_Position < -1.3f) {
			Plane1_X_Position = Plane1_X_Position + 0.0008f;
			Plane1_Z_Rotation_Angle = Plane1_Z_Rotation_Angle + 90.0f / (1.2 / 0.0008);//0.015f
		}
		else
			stage++;
		break;
	case 7:
		if (Plane1_X_Position < 1.3f)
			Plane1_X_Position = Plane1_X_Position + 0.0008f;
		else
			stage++;
		break;
	case 8:
		if (Plane1_X_Position < 2.5f)
		{
			Plane1_X_Position = Plane1_X_Position + 0.0008f;
			Plane1_Z_Rotation_Angle = Plane1_Z_Rotation_Angle + 90.0f / (1.3 / 0.0008);
		}
		else
			stage++;
		break;
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

	//if (vbo_texCoord_rectangle) {
	//	glDeleteBuffers(1, &vbo_texCoord_rectangle);
	//	vbo_texCoord_rectangle = 0;
	//}
	if (vbo_color_quad) {
		glDeleteBuffers(1, &vbo_color_quad);
		vbo_color_quad = 0;
	}

	if (vbo_position_quad) {
		glDeleteBuffers(1, &vbo_position_quad);
		vbo_position_quad = 0;
	}
	if (vao_quad) {
		glDeleteVertexArrays(1, &vao_quad);
		vao_quad = 0;
	}
	if (vao_point) {
		glDeleteVertexArrays(1, &vao_point);
		vao_point = 0;
	}
	if (vbo_position_point) {
		glDeleteBuffers(1, &vbo_position_point);
		vbo_position_point = 0;
	}

	if (vbo_color_point) {
		glDeleteBuffers(1, &vbo_color_point);
		vbo_color_point = 0;
	}

	if (vbo_position_triangle) {
		glDeleteBuffers(1, &vbo_position_triangle);
		vbo_position_triangle = 0;
	}

	if (vbo_color_triangle) {
		glDeleteBuffers(1, &vbo_color_triangle);
		vbo_color_triangle = 0;
	}

	if (vao_triangle) {
		glDeleteVertexArrays(1, &vao_triangle);
		vao_triangle = 0;
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
	//if (txtImage) {
	//	glDeleteTextures(1, &txtImage);
	//}
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

GLfloat rectangleVertices[] = {
		0.275f, 1.0f, 0.0f,
		0.225f, 1.0f, 0.0f,
		0.225f, -1.0f, 0.0f,
		0.275f, -1.0f, 0.0f
};

GLfloat rectangleColorCoords[] = {
1.0f, 0.6f, 0.2f,
1.0f, 0.6f, 0.2f,
0.074f, 0.533f, 0.2f,
0.074f, 0.533f, 0.2f
};



void DrawLetter(char letter) {
	switch (letter)
	{
	case 'I':
		glBindVertexArray(vao_quad);

		rectangleVertices[0] = 0.275f;
		rectangleVertices[1] = 1.0f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = 0.225f;
		rectangleVertices[4] = 1.0f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = 0.225f;
		rectangleVertices[7] = -1.0f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = 0.275f;
		rectangleVertices[10] = -1.0f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = 1.0f;
		rectangleColorCoords[1] = 0.6f;
		rectangleColorCoords[2] = 0.2f;
		rectangleColorCoords[3] = 1.0f;
		rectangleColorCoords[4] = 0.6f;
		rectangleColorCoords[5] = 0.2f;
		rectangleColorCoords[6] = 0.074f;
		rectangleColorCoords[7] = 0.533f;
		rectangleColorCoords[8] = 0.2f;
		rectangleColorCoords[9] = 0.074f;
		rectangleColorCoords[10] = 0.533f;
		rectangleColorCoords[11] = 0.2f;



		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_position_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleVertices,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_color_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleColorCoords,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLE_FAN,
			0,
			4);

		glBindVertexArray(0);

		break;

	case 'N':
		glBindVertexArray(vao_quad);

		//Left side of N
		rectangleVertices[0] = 0.05f;
		rectangleVertices[1] = 1.0f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = 0.0f;
		rectangleVertices[4] = 1.0f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = 0.00f;
		rectangleVertices[7] = -1.0f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = 0.05f;
		rectangleVertices[10] = -1.0f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = 1.0f;
		rectangleColorCoords[1] = 0.6f;
		rectangleColorCoords[2] = 0.2f;
		rectangleColorCoords[3] = 1.0f;
		rectangleColorCoords[4] = 0.6f;
		rectangleColorCoords[5] = 0.2f;
		rectangleColorCoords[6] = 0.074f;
		rectangleColorCoords[7] = 0.533f;
		rectangleColorCoords[8] = 0.2f;
		rectangleColorCoords[9] = 0.074f;
		rectangleColorCoords[10] = 0.533f;
		rectangleColorCoords[11] = 0.2f;


		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_position_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleVertices,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_color_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleColorCoords,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLE_FAN,
			0,
			4);

		//Middle part of N
		rectangleVertices[0] = 0.05f;
		rectangleVertices[1] = 1.0f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = 0.00f;
		rectangleVertices[4] = 1.0f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = 0.45f;
		rectangleVertices[7] = -1.0f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = 0.50f;
		rectangleVertices[10] = -1.0f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = 1.0f;
		rectangleColorCoords[1] = 0.6f;
		rectangleColorCoords[2] = 0.2f;
		rectangleColorCoords[3] = 1.0f;
		rectangleColorCoords[4] = 0.6f;
		rectangleColorCoords[5] = 0.2f;
		rectangleColorCoords[6] = 0.074f;
		rectangleColorCoords[7] = 0.533f;
		rectangleColorCoords[8] = 0.2f;
		rectangleColorCoords[9] = 0.074f;
		rectangleColorCoords[10] = 0.533f;
		rectangleColorCoords[11] = 0.2f;

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_position_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleVertices,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_color_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleColorCoords,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLE_FAN,
			0,
			4);

		//Right Side of N
		rectangleVertices[0] = 0.50f;
		rectangleVertices[1] = 1.0f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = 0.45f;
		rectangleVertices[4] = 1.0f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = 0.45f;
		rectangleVertices[7] = -1.0f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = 0.50f;
		rectangleVertices[10] = -1.0f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = 1.0f;
		rectangleColorCoords[1] = 0.6f;
		rectangleColorCoords[2] = 0.2f;
		rectangleColorCoords[3] = 1.0f;
		rectangleColorCoords[4] = 0.6f;
		rectangleColorCoords[5] = 0.2f;
		rectangleColorCoords[6] = 0.074f;
		rectangleColorCoords[7] = 0.533f;
		rectangleColorCoords[8] = 0.2f;
		rectangleColorCoords[9] = 0.074f;
		rectangleColorCoords[10] = 0.533f;
		rectangleColorCoords[11] = 0.2f;


		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_position_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleVertices,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_color_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleColorCoords,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLE_FAN,
			0,
			4);

		glBindVertexArray(0);

		break;

	case 'D':
		glBindVertexArray(vao_quad);

		rectangleVertices[0] = 0.05f;
		rectangleVertices[1] = 1.0f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = 0.0f;
		rectangleVertices[4] = 1.0f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = 0.00f;
		rectangleVertices[7] = -1.0f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = 0.05f;
		rectangleVertices[10] = -1.0f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = D_R_OrangeColor;
		rectangleColorCoords[1] = D_G_OrangeColor;
		rectangleColorCoords[2] = D_B_OrangeColor;
		rectangleColorCoords[3] = D_R_OrangeColor;
		rectangleColorCoords[4] = D_G_OrangeColor;
		rectangleColorCoords[5] = D_B_OrangeColor;
		rectangleColorCoords[6] = D_R_GreenColor;
		rectangleColorCoords[7] = D_G_GreenColor;
		rectangleColorCoords[8] = D_B_GreenColor;
		rectangleColorCoords[9] = D_R_GreenColor;
		rectangleColorCoords[10] = D_G_GreenColor;
		rectangleColorCoords[11] = D_B_GreenColor;




		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_position_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleVertices,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_color_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleColorCoords,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLE_FAN,
			0,
			4);


		rectangleVertices[0] = 0.5f;
		rectangleVertices[1] = 1.0f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = -0.01f;
		rectangleVertices[4] = 1.0f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = -0.01f;
		rectangleVertices[7] = 0.95f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = 0.5f;
		rectangleVertices[10] = 0.95f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = D_R_OrangeColor;
		rectangleColorCoords[1] = D_G_OrangeColor;
		rectangleColorCoords[2] = D_B_OrangeColor;
		rectangleColorCoords[3] = D_R_OrangeColor;
		rectangleColorCoords[4] = D_G_OrangeColor;
		rectangleColorCoords[5] = D_B_OrangeColor;
		rectangleColorCoords[6] = D_R_OrangeColor;
		rectangleColorCoords[7] = D_G_OrangeColor;
		rectangleColorCoords[8] = D_B_OrangeColor;
		rectangleColorCoords[9] = D_R_OrangeColor;
		rectangleColorCoords[10] = D_G_OrangeColor;
		rectangleColorCoords[11] = D_B_OrangeColor;


		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_position_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleVertices,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_color_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleColorCoords,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLE_FAN,
			0,
			4);

		rectangleVertices[0] = 0.50f;
		rectangleVertices[1] = 1.0f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = 0.45f;
		rectangleVertices[4] = 1.0f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = 0.45f;
		rectangleVertices[7] = -1.0f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = 0.50f;
		rectangleVertices[10] = -1.0f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = D_R_OrangeColor;
		rectangleColorCoords[1] = D_G_OrangeColor;
		rectangleColorCoords[2] = D_B_OrangeColor;
		rectangleColorCoords[3] = D_R_OrangeColor;
		rectangleColorCoords[4] = D_G_OrangeColor;
		rectangleColorCoords[5] = D_B_OrangeColor;
		rectangleColorCoords[6] = D_R_GreenColor;
		rectangleColorCoords[7] = D_G_GreenColor;
		rectangleColorCoords[8] = D_B_GreenColor;
		rectangleColorCoords[9] = D_R_GreenColor;
		rectangleColorCoords[10] = D_G_GreenColor;
		rectangleColorCoords[11] = D_B_GreenColor;


		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_position_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleVertices,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_color_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleColorCoords,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLE_FAN,
			0,
			4);

		rectangleVertices[0] = 0.50f;
		rectangleVertices[1] = -0.95f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = -0.01f;
		rectangleVertices[4] = -0.95f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = -0.01f;
		rectangleVertices[7] = -1.0f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = 0.50f;
		rectangleVertices[10] = -1.0f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = D_R_GreenColor;
		rectangleColorCoords[1] = D_G_GreenColor;
		rectangleColorCoords[2] = D_B_GreenColor;
		rectangleColorCoords[3] = D_R_GreenColor;
		rectangleColorCoords[4] = D_G_GreenColor;
		rectangleColorCoords[5] = D_B_GreenColor;
		rectangleColorCoords[6] = D_R_GreenColor;
		rectangleColorCoords[7] = D_G_GreenColor;
		rectangleColorCoords[8] = D_B_GreenColor;
		rectangleColorCoords[9] = D_R_GreenColor;
		rectangleColorCoords[10] = D_G_GreenColor;
		rectangleColorCoords[11] = D_B_GreenColor;

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_position_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleVertices,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_color_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleColorCoords,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLE_FAN,
			0,
			4);

		glBindVertexArray(0);

		break;
	case 'A':
		glBindVertexArray(vao_quad);

		rectangleVertices[0] = 0.275f;
		rectangleVertices[1] = 1.0f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = 0.225f;
		rectangleVertices[4] = 1.0f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = 0.00f;
		rectangleVertices[7] = -1.0f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = 0.05f;
		rectangleVertices[10] = -1.0f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = 1.0f;
		rectangleColorCoords[1] = 0.6f;
		rectangleColorCoords[2] = 0.2f;
		rectangleColorCoords[3] = 1.0f;
		rectangleColorCoords[4] = 0.6f;
		rectangleColorCoords[5] = 0.2f;
		rectangleColorCoords[6] = 0.074f;
		rectangleColorCoords[7] = 0.533f;
		rectangleColorCoords[8] = 0.2f;
		rectangleColorCoords[9] = 0.074f;
		rectangleColorCoords[10] = 0.533f;
		rectangleColorCoords[11] = 0.2f;

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_position_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleVertices,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_color_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleColorCoords,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLE_FAN,
			0,
			4);


		rectangleVertices[0] = 0.275f;
		rectangleVertices[1] = 1.0f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = 0.225f;
		rectangleVertices[4] = 1.0f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = 0.45f;
		rectangleVertices[7] = -1.0f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = 0.50f;
		rectangleVertices[10] = -1.0f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = 1.0f;
		rectangleColorCoords[1] = 0.6f;
		rectangleColorCoords[2] = 0.2f;
		rectangleColorCoords[3] = 1.0f;
		rectangleColorCoords[4] = 0.6f;
		rectangleColorCoords[5] = 0.2f;
		rectangleColorCoords[6] = 0.074f;
		rectangleColorCoords[7] = 0.533f;
		rectangleColorCoords[8] = 0.2f;
		rectangleColorCoords[9] = 0.074f;
		rectangleColorCoords[10] = 0.533f;
		rectangleColorCoords[11] = 0.2f;

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_position_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleVertices,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_color_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleColorCoords,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLE_FAN,
			0,
			4);
		break;

		glBindVertexArray(0);

	case 'T':
		glBindVertexArray(vao_quad);

		//Tricolor of A
		rectangleVertices[0] = 0.380f;
		rectangleVertices[1] = 0.06f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = 0.12f;
		rectangleVertices[4] = 0.06f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = 0.12f;
		rectangleVertices[7] = 0.02f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = 0.380f;
		rectangleVertices[10] = 0.02f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = 1.0f;
		rectangleColorCoords[1] = 0.6f;
		rectangleColorCoords[2] = 0.2f;
		rectangleColorCoords[3] = 1.0f;
		rectangleColorCoords[4] = 0.6f;
		rectangleColorCoords[5] = 0.2f;
		rectangleColorCoords[6] = 1.0f;
		rectangleColorCoords[7] = 0.6f;
		rectangleColorCoords[8] = 0.2f;
		rectangleColorCoords[9] = 1.0f;
		rectangleColorCoords[10] = 0.6f;
		rectangleColorCoords[11] = 0.2f;

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_position_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleVertices,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_color_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleColorCoords,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLE_FAN,
			0,
			4);

		rectangleVertices[0] = 0.380f;
		rectangleVertices[1] = 0.02f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = 0.12f;
		rectangleVertices[4] = 0.02f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = 0.12f;
		rectangleVertices[7] = -0.02f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = 0.380f;
		rectangleVertices[10] = -0.02f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = 1.0f;
		rectangleColorCoords[1] = 1.0f;
		rectangleColorCoords[2] = 1.0f;
		rectangleColorCoords[3] = 1.0f;
		rectangleColorCoords[4] = 1.0f;
		rectangleColorCoords[5] = 1.0f;
		rectangleColorCoords[6] = 1.0f;
		rectangleColorCoords[7] = 1.0f;
		rectangleColorCoords[8] = 1.0f;
		rectangleColorCoords[9] = 1.0f;
		rectangleColorCoords[10] = 1.0f;
		rectangleColorCoords[11] = 1.0f;

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_position_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleVertices,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_color_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleColorCoords,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLE_FAN,
			0,
			4);

		rectangleVertices[0] = 0.380f;
		rectangleVertices[1] = -0.02f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = 0.12f;
		rectangleVertices[4] = -0.02f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = 0.12f;
		rectangleVertices[7] = -0.06f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = 0.380f;
		rectangleVertices[10] = -0.06f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = 0.074f;
		rectangleColorCoords[1] = 0.533f;
		rectangleColorCoords[2] = 0.2f;
		rectangleColorCoords[3] = 0.074f;
		rectangleColorCoords[4] = 0.533f;
		rectangleColorCoords[5] = 0.2f;
		rectangleColorCoords[6] = 0.074f;
		rectangleColorCoords[7] = 0.533f;
		rectangleColorCoords[8] = 0.2f;
		rectangleColorCoords[9] = 0.074f;
		rectangleColorCoords[10] = 0.533f;
		rectangleColorCoords[11] = 0.2f;

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_position_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleVertices,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_color_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleColorCoords,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLE_FAN,
			0,
			4);

		glBindVertexArray(0);

		break;

	case 'L':
		glBindVertexArray(vao_quad);

		if (Plane1_X_Position < 1.40) {
			//Line Red
			rectangleVertices[0] = Plane1_X_Position - 0.27f;
			rectangleVertices[1] = 0.06f;
			rectangleVertices[2] = 0.0f;
			rectangleVertices[3] = Line_X_Position;
			rectangleVertices[4] = 0.06f;
			rectangleVertices[5] = 0.0f;
			rectangleVertices[6] = Line_X_Position;
			rectangleVertices[7] = 0.02f;
			rectangleVertices[8] = 0.0f;
			rectangleVertices[9] = Plane1_X_Position - 0.27f;
			rectangleVertices[10] = 0.02f;
			rectangleVertices[11] = 0.0f;

			rectangleColorCoords[0] = 1.0f;
			rectangleColorCoords[1] = 0.6f;
			rectangleColorCoords[2] = 0.2f;
			rectangleColorCoords[3] = 1.0f;
			rectangleColorCoords[4] = 0.6f;
			rectangleColorCoords[5] = 0.2f;
			rectangleColorCoords[6] = 1.0f;
			rectangleColorCoords[7] = 0.6f;
			rectangleColorCoords[8] = 0.2f;
			rectangleColorCoords[9] = 1.0f;
			rectangleColorCoords[10] = 0.6f;
			rectangleColorCoords[11] = 0.2f;

			glBindBuffer(GL_ARRAY_BUFFER,
				vbo_position_quad);
			glBufferData(GL_ARRAY_BUFFER,
				4 * 3 * sizeof(float),
				rectangleVertices,
				GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ARRAY_BUFFER,
				vbo_color_quad);
			glBufferData(GL_ARRAY_BUFFER,
				4 * 3 * sizeof(float),
				rectangleColorCoords,
				GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_TRIANGLE_FAN,
				0,
				4);

			// W
			rectangleVertices[0] = Plane1_X_Position - 0.27f;
			rectangleVertices[1] = 0.02f;
			rectangleVertices[2] = 0.0f;
			rectangleVertices[3] = Line_X_Position;
			rectangleVertices[4] = 0.02f;
			rectangleVertices[5] = 0.0f;
			rectangleVertices[6] = Line_X_Position;
			rectangleVertices[7] = -0.02f;
			rectangleVertices[8] = 0.0f;
			rectangleVertices[9] = Plane1_X_Position - 0.27f;
			rectangleVertices[10] = -0.02f;
			rectangleVertices[11] = 0.0f;

			rectangleColorCoords[0] = 1.0f;
			rectangleColorCoords[1] = 1.0f;
			rectangleColorCoords[2] = 1.0f;
			rectangleColorCoords[3] = 1.0f;
			rectangleColorCoords[4] = 1.0f;
			rectangleColorCoords[5] = 1.0f;
			rectangleColorCoords[6] = 1.0f;
			rectangleColorCoords[7] = 1.0f;
			rectangleColorCoords[8] = 1.0f;
			rectangleColorCoords[9] = 1.0f;
			rectangleColorCoords[10] = 1.0f;
			rectangleColorCoords[11] = 1.0f;

			glBindBuffer(GL_ARRAY_BUFFER,
				vbo_position_quad);
			glBufferData(GL_ARRAY_BUFFER,
				4 * 3 * sizeof(float),
				rectangleVertices,
				GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ARRAY_BUFFER,
				vbo_color_quad);
			glBufferData(GL_ARRAY_BUFFER,
				4 * 3 * sizeof(float),
				rectangleColorCoords,
				GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_TRIANGLE_FAN,
				0,
				4);


			// G		
			rectangleVertices[0] = Plane1_X_Position - 0.27f;
			rectangleVertices[1] = -0.02f;
			rectangleVertices[2] = 0.0f;
			rectangleVertices[3] = Line_X_Position;
			rectangleVertices[4] = -0.02f;
			rectangleVertices[5] = 0.0f;
			rectangleVertices[6] = Line_X_Position;
			rectangleVertices[7] = -0.06f;
			rectangleVertices[8] = 0.0f;
			rectangleVertices[9] = Plane1_X_Position - 0.27f;
			rectangleVertices[10] = -0.06f;
			rectangleVertices[11] = 0.0f;

			rectangleColorCoords[0] = 0.074f;
			rectangleColorCoords[1] = 0.533f;
			rectangleColorCoords[2] = 0.2f;
			rectangleColorCoords[3] = 0.074f;
			rectangleColorCoords[4] = 0.533f;
			rectangleColorCoords[5] = 0.2f;
			rectangleColorCoords[6] = 0.074f;
			rectangleColorCoords[7] = 0.533f;
			rectangleColorCoords[8] = 0.2f;
			rectangleColorCoords[9] = 0.074f;
			rectangleColorCoords[10] = 0.533f;
			rectangleColorCoords[11] = 0.2f;

			glBindBuffer(GL_ARRAY_BUFFER,
				vbo_position_quad);
			glBufferData(GL_ARRAY_BUFFER,
				4 * 3 * sizeof(float),
				rectangleVertices,
				GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ARRAY_BUFFER,
				vbo_color_quad);
			glBufferData(GL_ARRAY_BUFFER,
				4 * 3 * sizeof(float),
				rectangleColorCoords,
				GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_TRIANGLE_FAN,
				0,
				4);

		}
		else {

			//R
			rectangleVertices[0] = 1.1f;
			rectangleVertices[1] = 0.06f;
			rectangleVertices[2] = 0.0f;
			rectangleVertices[3] = Line_X_Position;
			rectangleVertices[4] = 0.06f;
			rectangleVertices[5] = 0.0f;
			rectangleVertices[6] = Line_X_Position;
			rectangleVertices[7] = 0.02f;
			rectangleVertices[8] = 0.0f;
			rectangleVertices[9] = 1.1f;
			rectangleVertices[10] = 0.02f;
			rectangleVertices[11] = 0.0f;

			rectangleColorCoords[0] = 1.0f;
			rectangleColorCoords[1] = 0.6f;
			rectangleColorCoords[2] = 0.2f;
			rectangleColorCoords[3] = 1.0f;
			rectangleColorCoords[4] = 0.6f;
			rectangleColorCoords[5] = 0.2f;
			rectangleColorCoords[6] = 1.0f;
			rectangleColorCoords[7] = 0.6f;
			rectangleColorCoords[8] = 0.2f;
			rectangleColorCoords[9] = 1.0f;
			rectangleColorCoords[10] = 0.6f;
			rectangleColorCoords[11] = 0.2f;

			glBindBuffer(GL_ARRAY_BUFFER,
				vbo_position_quad);
			glBufferData(GL_ARRAY_BUFFER,
				4 * 3 * sizeof(float),
				rectangleVertices,
				GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ARRAY_BUFFER,
				vbo_color_quad);
			glBufferData(GL_ARRAY_BUFFER,
				4 * 3 * sizeof(float),
				rectangleColorCoords,
				GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_TRIANGLE_FAN,
				0,
				4);

			// W
			rectangleVertices[0] = 1.1f;
			rectangleVertices[1] = 0.02f;
			rectangleVertices[2] = 0.0f;
			rectangleVertices[3] = Line_X_Position;
			rectangleVertices[4] = 0.02f;
			rectangleVertices[5] = 0.0f;
			rectangleVertices[6] = Line_X_Position;
			rectangleVertices[7] = -0.02f;
			rectangleVertices[8] = 0.0f;
			rectangleVertices[9] = 1.1f;
			rectangleVertices[10] = -0.02f;
			rectangleVertices[11] = 0.0f;

			rectangleColorCoords[0] = 1.0f;
			rectangleColorCoords[1] = 1.0f;
			rectangleColorCoords[2] = 1.0f;
			rectangleColorCoords[3] = 1.0f;
			rectangleColorCoords[4] = 1.0f;
			rectangleColorCoords[5] = 1.0f;
			rectangleColorCoords[6] = 1.0f;
			rectangleColorCoords[7] = 1.0f;
			rectangleColorCoords[8] = 1.0f;
			rectangleColorCoords[9] = 1.0f;
			rectangleColorCoords[10] = 1.0f;
			rectangleColorCoords[11] = 1.0f;

			glBindBuffer(GL_ARRAY_BUFFER,
				vbo_position_quad);
			glBufferData(GL_ARRAY_BUFFER,
				4 * 3 * sizeof(float),
				rectangleVertices,
				GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ARRAY_BUFFER,
				vbo_color_quad);
			glBufferData(GL_ARRAY_BUFFER,
				4 * 3 * sizeof(float),
				rectangleColorCoords,
				GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_TRIANGLE_FAN,
				0,
				4);

			// G		
			rectangleVertices[0] = 1.1f;
			rectangleVertices[1] = -0.02f;
			rectangleVertices[2] = 0.0f;
			rectangleVertices[3] = Line_X_Position;
			rectangleVertices[4] = -0.02f;
			rectangleVertices[5] = 0.0f;
			rectangleVertices[6] = Line_X_Position;
			rectangleVertices[7] = -0.06f;
			rectangleVertices[8] = 0.0f;
			rectangleVertices[9] = 1.1f;
			rectangleVertices[10] = -0.06f;
			rectangleVertices[11] = 0.0f;

			rectangleColorCoords[0] = 0.074f;
			rectangleColorCoords[1] = 0.533f;
			rectangleColorCoords[2] = 0.2f;
			rectangleColorCoords[3] = 0.074f;
			rectangleColorCoords[4] = 0.533f;
			rectangleColorCoords[5] = 0.2f;
			rectangleColorCoords[6] = 0.074f;
			rectangleColorCoords[7] = 0.533f;
			rectangleColorCoords[8] = 0.2f;
			rectangleColorCoords[9] = 0.074f;
			rectangleColorCoords[10] = 0.533f;
			rectangleColorCoords[11] = 0.2f;

			glBindBuffer(GL_ARRAY_BUFFER,
				vbo_position_quad);
			glBufferData(GL_ARRAY_BUFFER,
				4 * 3 * sizeof(float),
				rectangleVertices,
				GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ARRAY_BUFFER,
				vbo_color_quad);
			glBufferData(GL_ARRAY_BUFFER,
				4 * 3 * sizeof(float),
				rectangleColorCoords,
				GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_TRIANGLE_FAN,
				0,
				4);
		}
		glBindVertexArray(0);

		break;
		//Plane
	case 'P':
		glBindVertexArray(vao_quad);

		//Smoke R
		rectangleVertices[0] = -0.24f;
		rectangleVertices[1] = 0.06f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = -0.40f;
		rectangleVertices[4] = 0.06f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = -0.40;
		rectangleVertices[7] = -0.02f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = -0.24;
		rectangleVertices[10] = -0.02f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = 1.0f;
		rectangleColorCoords[1] = 0.6f;
		rectangleColorCoords[2] = 0.2f;
		rectangleColorCoords[3] = 0.0f;
		rectangleColorCoords[4] = 0.0f;
		rectangleColorCoords[5] = 0.0f;
		rectangleColorCoords[6] = 0.0f;
		rectangleColorCoords[7] = 0.0f;
		rectangleColorCoords[8] = 0.0f;
		rectangleColorCoords[9] = 1.0f;
		rectangleColorCoords[10] = 0.6f;
		rectangleColorCoords[11] = 0.2f;

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_position_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleVertices,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_color_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleColorCoords,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLE_FAN,
			0,
			4);

		//Smoke W

		rectangleVertices[0] = -0.24f;
		rectangleVertices[1] = 0.02f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = -0.40f;
		rectangleVertices[4] = 0.02f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = -0.40;
		rectangleVertices[7] = -0.02f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = -0.24;
		rectangleVertices[10] = -0.02f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = 1.0f;
		rectangleColorCoords[1] = 1.0f;
		rectangleColorCoords[2] = 1.0f;
		rectangleColorCoords[3] = 0.0f;
		rectangleColorCoords[4] = 0.0f;
		rectangleColorCoords[5] = 0.0f;
		rectangleColorCoords[6] = 0.0f;
		rectangleColorCoords[7] = 0.0f;
		rectangleColorCoords[8] = 0.0f;
		rectangleColorCoords[9] = 1.0f;
		rectangleColorCoords[10] = 1.0f;
		rectangleColorCoords[11] = 1.0f;

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_position_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleVertices,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_color_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleColorCoords,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLE_FAN,
			0,
			4);


		//Smoke G

		rectangleVertices[0] = -0.24f;
		rectangleVertices[1] = -0.02f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = -0.40f;
		rectangleVertices[4] = -0.02f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = -0.40;
		rectangleVertices[7] = -0.06f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = -0.24;
		rectangleVertices[10] = -0.06f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = 0.074f;
		rectangleColorCoords[1] = 0.533f;
		rectangleColorCoords[2] = 0.2f;
		rectangleColorCoords[3] = 0.0f;
		rectangleColorCoords[4] = 0.0f;
		rectangleColorCoords[5] = 0.0f;
		rectangleColorCoords[6] = 0.0f;
		rectangleColorCoords[7] = 0.0f;
		rectangleColorCoords[8] = 0.0f;
		rectangleColorCoords[9] = 0.074f;
		rectangleColorCoords[10] = 0.533f;
		rectangleColorCoords[11] = 0.2f;

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_position_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleVertices,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_color_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleColorCoords,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLE_FAN,
			0,
			4);





		rectangleVertices[0] = 0.20f;
		rectangleVertices[1] = 0.07f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = -0.20f;
		rectangleVertices[4] = 0.07f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = -0.20f;
		rectangleVertices[7] = -0.07f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = 0.20;
		rectangleVertices[10] = -0.07f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = 0.729f;
		rectangleColorCoords[1] = 0.886f;
		rectangleColorCoords[2] = 0.933f;
		rectangleColorCoords[3] = 0.729f;
		rectangleColorCoords[4] = 0.886f;
		rectangleColorCoords[5] = 0.933f;
		rectangleColorCoords[6] = 0.729f;
		rectangleColorCoords[7] = 0.886f;
		rectangleColorCoords[8] = 0.933f;
		rectangleColorCoords[9] = 0.729f;
		rectangleColorCoords[10] = 0.886f;
		rectangleColorCoords[11] = 0.933f;

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_position_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleVertices,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_color_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleColorCoords,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLE_FAN,
			0,
			4);


		//Back Wings

		rectangleVertices[0] = -0.10f;
		rectangleVertices[1] = 0.0f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = -0.27f;
		rectangleVertices[4] = 0.12f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = -0.31f;
		rectangleVertices[7] = 0.12f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = -0.24f;
		rectangleVertices[10] = 0.0f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = 0.729f;
		rectangleColorCoords[1] = 0.886f;
		rectangleColorCoords[2] = 0.933f;
		rectangleColorCoords[3] = 0.729f;
		rectangleColorCoords[4] = 0.886f;
		rectangleColorCoords[5] = 0.933f;
		rectangleColorCoords[6] = 0.729f;
		rectangleColorCoords[7] = 0.886f;
		rectangleColorCoords[8] = 0.933f;
		rectangleColorCoords[9] = 0.729f;
		rectangleColorCoords[10] = 0.886f;
		rectangleColorCoords[11] = 0.933f;

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_position_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleVertices,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_color_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleColorCoords,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLE_FAN,
			0,
			4);

		rectangleVertices[0] = -0.10f;
		rectangleVertices[1] = 0.0f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = -0.27f;
		rectangleVertices[4] = -0.12f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = -0.31f;
		rectangleVertices[7] = -0.12f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = -0.24f;
		rectangleVertices[10] = 0.0f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = 0.729f;
		rectangleColorCoords[1] = 0.886f;
		rectangleColorCoords[2] = 0.933f;
		rectangleColorCoords[3] = 0.729f;
		rectangleColorCoords[4] = 0.886f;
		rectangleColorCoords[5] = 0.933f;
		rectangleColorCoords[6] = 0.729f;
		rectangleColorCoords[7] = 0.886f;
		rectangleColorCoords[8] = 0.933f;
		rectangleColorCoords[9] = 0.729f;
		rectangleColorCoords[10] = 0.886f;
		rectangleColorCoords[11] = 0.933f;

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_position_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleVertices,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_color_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleColorCoords,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLE_FAN,
			0,
			4);

		//Letter I
		rectangleVertices[0] = -0.05f;
		rectangleVertices[1] = 0.05f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = -0.07f;
		rectangleVertices[4] = 0.05f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = -0.07f;
		rectangleVertices[7] = -0.05f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = -0.05f;
		rectangleVertices[10] = -0.05f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = 1.0f;
		rectangleColorCoords[1] = 0.6f;
		rectangleColorCoords[2] = 0.2f;
		rectangleColorCoords[3] = 1.0f;
		rectangleColorCoords[4] = 0.6f;
		rectangleColorCoords[5] = 0.2f;
		rectangleColorCoords[6] = 0.074f;
		rectangleColorCoords[7] = 0.533f;
		rectangleColorCoords[8] = 0.2f;
		rectangleColorCoords[9] = 0.074f;
		rectangleColorCoords[10] = 0.533f;
		rectangleColorCoords[11] = 0.2f;

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_position_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleVertices,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_color_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleColorCoords,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLE_FAN,
			0,
			4);


		//Letter A			
		rectangleVertices[0] = 0.00f;
		rectangleVertices[1] = 0.05f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = -0.02f;
		rectangleVertices[4] = 0.05f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = -0.04f;
		rectangleVertices[7] = -0.05f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = -0.02f;
		rectangleVertices[10] = -0.05f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = 0.0f;
		rectangleColorCoords[1] = 0.0f;
		rectangleColorCoords[2] = 0.530f;
		rectangleColorCoords[3] = 0.0f;
		rectangleColorCoords[4] = 0.0f;
		rectangleColorCoords[5] = 0.530f;
		rectangleColorCoords[6] = 0.0f;
		rectangleColorCoords[7] = 0.0f;
		rectangleColorCoords[8] = 0.530f;
		rectangleColorCoords[9] = 0.0f;
		rectangleColorCoords[10] = 0.0f;
		rectangleColorCoords[11] = 0.530f;

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_position_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleVertices,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_color_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleColorCoords,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLE_FAN,
			0,
			4);

		rectangleVertices[0] = 0.00f;
		rectangleVertices[1] = 0.05f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = 0.02f;
		rectangleVertices[4] = 0.05f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = 0.04f;
		rectangleVertices[7] = -0.05f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = 0.02f;
		rectangleVertices[10] = -0.05f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = 0.0f;
		rectangleColorCoords[1] = 0.0f;
		rectangleColorCoords[2] = 0.530f;
		rectangleColorCoords[3] = 0.0f;
		rectangleColorCoords[4] = 0.0f;
		rectangleColorCoords[5] = 0.530f;
		rectangleColorCoords[6] = 0.0f;
		rectangleColorCoords[7] = 0.0f;
		rectangleColorCoords[8] = 0.530f;
		rectangleColorCoords[9] = 0.0f;
		rectangleColorCoords[10] = 0.0f;
		rectangleColorCoords[11] = 0.530f;

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_position_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleVertices,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_color_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleColorCoords,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLE_FAN,
			0,
			4);


		rectangleVertices[0] = 0.02f;
		rectangleVertices[1] = 0.01f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = -0.02f;
		rectangleVertices[4] = 0.01f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = -0.02f;
		rectangleVertices[7] = -0.01f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = 0.02f;
		rectangleVertices[10] = -0.01f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = 0.0f;
		rectangleColorCoords[1] = 0.0f;
		rectangleColorCoords[2] = 0.530f;
		rectangleColorCoords[3] = 0.0f;
		rectangleColorCoords[4] = 0.0f;
		rectangleColorCoords[5] = 0.530f;
		rectangleColorCoords[6] = 0.0f;
		rectangleColorCoords[7] = 0.0f;
		rectangleColorCoords[8] = 0.530f;
		rectangleColorCoords[9] = 0.0f;
		rectangleColorCoords[10] = 0.0f;
		rectangleColorCoords[11] = 0.530f;

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_position_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleVertices,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_color_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleColorCoords,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLE_FAN,
			0,
			4);


		//Letter F

		rectangleVertices[0] = 0.06f;
		rectangleVertices[1] = 0.05f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = 0.04f;
		rectangleVertices[4] = 0.05f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = 0.04f;
		rectangleVertices[7] = -0.05f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = 0.06f;
		rectangleVertices[10] = -0.05f;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = 0.074f;
		rectangleColorCoords[1] = 0.533f;
		rectangleColorCoords[2] = 0.2f;
		rectangleColorCoords[3] = 0.074f;
		rectangleColorCoords[4] = 0.533f;
		rectangleColorCoords[5] = 0.2f;
		rectangleColorCoords[6] = 0.074f;
		rectangleColorCoords[7] = 0.533f;
		rectangleColorCoords[8] = 0.2f;
		rectangleColorCoords[9] = 0.074f;
		rectangleColorCoords[10] = 0.533f;
		rectangleColorCoords[11] = 0.2f;

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_position_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleVertices,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_color_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleColorCoords,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLE_FAN,
			0,
			4);

		rectangleVertices[0] = 0.09f;
		rectangleVertices[1] = 0.05f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = 0.06f;
		rectangleVertices[4] = 0.05f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = 0.06f;
		rectangleVertices[7] = 0.035f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = 0.09f;
		rectangleVertices[10] = 0.035;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = 0.074f;
		rectangleColorCoords[1] = 0.533f;
		rectangleColorCoords[2] = 0.2f;
		rectangleColorCoords[3] = 0.074f;
		rectangleColorCoords[4] = 0.533f;
		rectangleColorCoords[5] = 0.2f;
		rectangleColorCoords[6] = 0.074f;
		rectangleColorCoords[7] = 0.533f;
		rectangleColorCoords[8] = 0.2f;
		rectangleColorCoords[9] = 0.074f;
		rectangleColorCoords[10] = 0.533f;
		rectangleColorCoords[11] = 0.2f;

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_position_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleVertices,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_color_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleColorCoords,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLE_FAN,
			0,
			4);

		rectangleVertices[0] = 0.09f;
		rectangleVertices[1] = 0.02f;
		rectangleVertices[2] = 0.0f;
		rectangleVertices[3] = 0.06f;
		rectangleVertices[4] = 0.02f;
		rectangleVertices[5] = 0.0f;
		rectangleVertices[6] = 0.06f;
		rectangleVertices[7] = 0.005f;
		rectangleVertices[8] = 0.0f;
		rectangleVertices[9] = 0.09f;
		rectangleVertices[10] = 0.005;
		rectangleVertices[11] = 0.0f;

		rectangleColorCoords[0] = 0.074f;
		rectangleColorCoords[1] = 0.533f;
		rectangleColorCoords[2] = 0.2f;
		rectangleColorCoords[3] = 0.074f;
		rectangleColorCoords[4] = 0.533f;
		rectangleColorCoords[5] = 0.2f;
		rectangleColorCoords[6] = 0.074f;
		rectangleColorCoords[7] = 0.533f;
		rectangleColorCoords[8] = 0.2f;
		rectangleColorCoords[9] = 0.074f;
		rectangleColorCoords[10] = 0.533f;
		rectangleColorCoords[11] = 0.2f;

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_position_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleVertices,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_color_quad);
		glBufferData(GL_ARRAY_BUFFER,
			4 * 3 * sizeof(float),
			rectangleColorCoords,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLE_FAN,
			0,
			4);

		glBindVertexArray(0);

		GLfloat triangleVertices[] = {
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		};

		GLfloat triangleColorCoords[] = {
			0.729f,
			0.886f,
			0.933f,
			0.729f,
			0.886f,
			0.933f,
			0.729f,
			0.886f,
			0.933f
		};


		glBindVertexArray(vao_triangle);
		//Upper Wing
		triangleVertices[0] = -0.05f;
		triangleVertices[1] = 0.07f;
		triangleVertices[2] = 0.0f;
		triangleVertices[3] = 0.05f;
		triangleVertices[4] = 0.07f;
		triangleVertices[5] = 0.0f;
		triangleVertices[6] = -0.15f;
		triangleVertices[7] = 0.22f;
		triangleVertices[8] = 0.0f;

		/*triangleColorCoords[0] = 0.729f;
		triangleColorCoords[1] = 0.886f;
		triangleColorCoords[2] = 0.933f;
		triangleColorCoords[3] = 1.0f;
		triangleColorCoords[4] = 0.6f;
		triangleColorCoords[5] = 0.2f;
		triangleColorCoords[6] = 0.074f;
		triangleColorCoords[7] = 0.533f;
		triangleColorCoords[8] = 0.2f;*/

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_position_triangle);
		glBufferData(GL_ARRAY_BUFFER,
			3 * 3 * sizeof(float),
			triangleVertices,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_color_triangle);
		glBufferData(GL_ARRAY_BUFFER,
			3 * 3 * sizeof(float),
			triangleColorCoords,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLES,
			0,
			3);

		glBindVertexArray(0);


		glBindVertexArray(vao_triangle);
		triangleVertices[0] = -0.05f;
		triangleVertices[1] = -0.07f;
		triangleVertices[2] = 0.0f;
		triangleVertices[3] = 0.05f;
		triangleVertices[4] = -0.07f;
		triangleVertices[5] = 0.0f;
		triangleVertices[6] = -0.15f;
		triangleVertices[7] = -0.22f;
		triangleVertices[8] = 0.0f;

		/*triangleColorCoords[0] = 0.729f;
		triangleColorCoords[1] = 0.886f;
		triangleColorCoords[2] = 0.933f;
		triangleColorCoords[3] = 1.0f;
		triangleColorCoords[4] = 0.6f;
		triangleColorCoords[5] = 0.2f;
		triangleColorCoords[6] = 0.074f;
		triangleColorCoords[7] = 0.533f;
		triangleColorCoords[8] = 0.2f;*/

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_position_triangle);
		glBufferData(GL_ARRAY_BUFFER,
			3 * 3 * sizeof(float),
			triangleVertices,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_color_triangle);
		glBufferData(GL_ARRAY_BUFFER,
			3 * 3 * sizeof(float),
			triangleColorCoords,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLES,
			0,
			3);

		glBindVertexArray(0);

		//Lower Wing

		glBindVertexArray(vao_triangle);
		triangleVertices[0] = 0.05f;
		triangleVertices[1] = 0.07f;
		triangleVertices[2] = 0.0f;
		triangleVertices[3] = -0.10f;
		triangleVertices[4] = 0.22f;
		triangleVertices[5] = 0.0f;
		triangleVertices[6] = -0.15f;
		triangleVertices[7] = 0.22f;
		triangleVertices[8] = 0.0f;

		/*triangleColorCoords[0] = 0.729f;
		triangleColorCoords[1] = 0.886f;
		triangleColorCoords[2] = 0.933f;
		triangleColorCoords[3] = 1.0f;
		triangleColorCoords[4] = 0.6f;
		triangleColorCoords[5] = 0.2f;
		triangleColorCoords[6] = 0.074f;
		triangleColorCoords[7] = 0.533f;
		triangleColorCoords[8] = 0.2f;*/

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_position_triangle);
		glBufferData(GL_ARRAY_BUFFER,
			3 * 3 * sizeof(float),
			triangleVertices,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_color_triangle);
		glBufferData(GL_ARRAY_BUFFER,
			3 * 3 * sizeof(float),
			triangleColorCoords,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLES,
			0,
			3);

		glBindVertexArray(0);


		glBindVertexArray(vao_triangle);
		triangleVertices[0] = 0.05f;
		triangleVertices[1] = -0.07f;
		triangleVertices[2] = 0.0f;
		triangleVertices[3] = -0.10f;
		triangleVertices[4] = -0.22f;
		triangleVertices[5] = 0.0f;
		triangleVertices[6] = -0.15f;
		triangleVertices[7] = -0.22f;
		triangleVertices[8] = 0.0f;

		/*triangleColorCoords[0] = 0.729f;
		triangleColorCoords[1] = 0.886f;
		triangleColorCoords[2] = 0.933f;
		triangleColorCoords[3] = 1.0f;
		triangleColorCoords[4] = 0.6f;
		triangleColorCoords[5] = 0.2f;
		triangleColorCoords[6] = 0.074f;
		triangleColorCoords[7] = 0.533f;
		triangleColorCoords[8] = 0.2f;*/

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_position_triangle);
		glBufferData(GL_ARRAY_BUFFER,
			3 * 3 * sizeof(float),
			triangleVertices,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_color_triangle);
		glBufferData(GL_ARRAY_BUFFER,
			3 * 3 * sizeof(float),
			triangleColorCoords,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLES,
			0,
			3);

		glBindVertexArray(0);



		//glBegin(GL_TRIANGLES);

////Upper Wing
//glVertex3f(-0.05f, 0.07f, 0.0f);
//glVertex3f(0.05f, 0.07f, 0.0f);
//glVertex3f(-0.15f, 0.22f, 0.0f);

//glVertex3f(-0.05f, -0.07f, 0.0f);
//glVertex3f(0.05f, -0.07f, 0.0f);
//glVertex3f(-0.15f, -0.22f, 0.0f);
////Lower Wing
//glVertex3f(0.05f, 0.07f, 0.0f);
//glVertex3f(-0.10f, 0.22f, 0.0f);
//glVertex3f(-0.15f, 0.22f, 0.0f);

//glVertex3f(0.05f, -0.07f, 0.0f);
//glVertex3f(-0.10f, -0.22f, 0.0f);
//glVertex3f(-0.15f, -0.22f, 0.0f);

//glEnd();


////Front Circle

//glBegin(GL_POINTS);

////glColor3f(1.0f, 1.0f, 1.0f);

//for (float radius = 0.0f; radius <= 0.07f; radius = radius + 0.001f) {
//	for (angle = 0.00f; angle < 2 * 3.14159265; angle = angle + 0.01f) {
//		glVertex3f(cos(angle)*radius + 0.21f, sin(angle)*radius, 0.0f);
//	}
//}
//glEnd();

		/*glColor3f(0.729f, 0.886f, 0.933f);

		if (!initCircle) {
			for (float radius = 0.0f; radius <= 0.07f; radius = radius + 0.001f) {
				for (angle = 0.00f; angle < 2 * 3.14159265; angle = angle + 0.01f) {
					glVertex3f(cos(angle)*radius + 0.21f, sin(angle)*radius, 0.0f);
				}
			}
		}

		glBindVertexArray(vao_point);

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_position_point);
		glBufferData(GL_ARRAY_BUFFER,
			3 * 3 * sizeof(float),
			triangleVertices,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo_color_point);
		glBufferData(GL_ARRAY_BUFFER,
			3 * 3 * sizeof(float),
			triangleColorCoords,
			GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLES,
			0,
			3);

		glBindVertexArray(0);
*/

		planeFrontSphere.drawWithColor();

		break;

	}

}
