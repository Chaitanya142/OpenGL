#include<Windows.h>
#include<stdio.h>
#include<gl\GL.h>
#include<gl\GLU.h>
#include<math.h>
#include"Dynamic_INDIA.h"

//#include <time.h>

#pragma comment(lib,"OpenGL32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"Winmm.lib")

//Constants
#define WIN_WIDTH 800
#define WIN_HEIGHT 600
//Global Variables
HWND ghwnd = NULL;
DWORD dwStyle;
bool bFullScreen = false;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
HDC ghdc = NULL;
HGLRC ghrc = NULL;
bool gbActiveWindow = false;
FILE *gpFile = NULL;

GLfloat D_R_OrangeColor = 0.0f;
GLfloat D_G_OrangeColor = 0.0f;
GLfloat D_B_OrangeColor = 0.0f;
GLfloat D_R_GreenColor = 0.0f;
GLfloat D_G_GreenColor = 0.0f;
GLfloat D_B_GreenColor = 0.0f;
GLfloat Plane1_X_Position = -2.5f;
GLfloat angle = 0.0f;
GLfloat Line_X_Position = -1.28f;

GLfloat Line_Orange_R = 1.0f;
GLfloat Line_Orange_G = 0.6f;
GLfloat Line_Orange_B = 0.2f;
GLfloat Line_White_R = 1.00f;
GLfloat Line_White_G = 1.00f;
GLfloat Line_White_B = 1.00f;
GLfloat Line_Green_R = 0.074f;
GLfloat Line_Green_G = 0.533f;
GLfloat Line_Green_B = 0.2f;



//Method Declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//void PrintTime();
//WinMain Function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevinstance, LPSTR lpszCmdLine, int iCmdShow) {
	//Method Declaration
	int Initialize(void);
	void Display(void);
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
	PlaySound(MAKEINTRESOURCE(Hai_Preet_Jahan_Ki_Reet_Sada), NULL, SND_RESOURCE | SND_ASYNC);
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
				//Here Call Update
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

	case WM_CLOSE:
		DestroyWindow(0);
		break;
	case WM_CHAR:
		switch (wParam)
		{
		case 'F':
		case 'f':
			//ToggleFullScreen();
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
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;
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
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	Resize(WIN_WIDTH, WIN_HEIGHT);
	ToggleFullScreen();

	return 0;
}
//Function Resize
void Resize(int width, int height) {
	if (height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,
		(GLfloat)width / (GLfloat)height,
		0.1f,
		100.0f);

}

void DrawLetter(char);
//Function Display
void Display(void) {
	static GLint stage = 1;

	static GLfloat I_1_XPosition = -2.25f;
	static GLfloat A_XPosition = 2.25f;
	static GLfloat N_YPosition = 2.25f;
	static GLfloat I_2_YPosition = -2.25f;
	//static GLfloat Plane1_X_Position = -2.5f;
	static GLfloat Plane1_Y_Position = 0.00f;
	static GLfloat Plane1_Z_Rotation_Angle = -90.0f;


	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	//glLoadIdentity();
	//glTranslatef(0.0f, 0.0f, -3.0f);//glTranslatef(-1.0f, 0.0f, -3.0f);	
	//DrawLetter('A');
	//DrawLetter('T');


	if (stage > 0) {
		glLoadIdentity();
		glTranslatef(I_1_XPosition, 0.0f, -3.0f);//glTranslatef(-1.5f, 0.0f, -3.0f);
		DrawLetter('I');
	}

	if (stage > 2) {
		glLoadIdentity();
		glTranslatef(-1.0f, N_YPosition, -3.0f);//glTranslatef(-1.0f, 0.0f, -3.0f);
		DrawLetter('N');
	}

	if (stage > 4) {
		glLoadIdentity();
		glTranslatef(-0.25f, 0.0f, -3.0f);
		DrawLetter('D');
	}

	if (stage > 3) {
		glLoadIdentity();
		glTranslatef(0.25f, I_2_YPosition, -3.0f);//glTranslatef(0.25f, 0.0f, -3.0f);
		DrawLetter('I');
	}
	if (stage > 1) {
		glLoadIdentity();
		glTranslatef(A_XPosition, 0.0f, -3.0f);////glTranslatef(0.75f, 0.0f, -3.0f);
		DrawLetter('A');
	}


	if (stage == 8) {
		glLoadIdentity();
		Plane1_Y_Position = ((Plane1_X_Position - 1.3)*(Plane1_X_Position - 1.3) / (4 * 0.15)) - 0.0f;
		glTranslatef(Plane1_X_Position, Plane1_Y_Position, -3.0f);//glTranslatef(-1.0f, 0.0f, -3.0f);	
		glRotatef(Plane1_Z_Rotation_Angle, 0.0f, 0.0f, 1.0f);
		DrawLetter('P');

		glLoadIdentity();
		glTranslatef(Plane1_X_Position, 0.0f, -3.0f);//glTranslatef(-1.0f, 0.0f, -3.0f);	
		DrawLetter('P');

		glLoadIdentity();
		glTranslatef(Plane1_X_Position, -Plane1_Y_Position, -3.0f);//glTranslatef(-1.0f, 0.0f, -3.0f);	
		glRotatef(-Plane1_Z_Rotation_Angle, 0.0f, 0.0f, 1.0f);
		DrawLetter('P');

	}

	if (stage == 7) {
		glLoadIdentity();
		glTranslatef(Plane1_X_Position, 0.0f, -3.0f);//glTranslatef(-1.0f, 0.0f, -3.0f);
		DrawLetter('P');
	}

	if ((stage == 7 || stage == 8) && Plane1_X_Position > -1.02f && Plane1_X_Position < 1.40f) {
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -3.0f);//glTranslatef(-1.0f, 0.0f, -3.0f);
		DrawLetter('L');
	}

	if (stage > 7 && Plane1_X_Position > 1.40f) {
		glLoadIdentity();
		glTranslatef(A_XPosition, 0.0f, -3.0f);////glTranslatef(0.75f, 0.0f, -3.0f);
		DrawLetter('T');
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


		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -3.0f);//glTranslatef(-1.0f, 0.0f, -3.0f);
		DrawLetter('L');
	}

	if (stage > 7 && Plane1_X_Position > 1.40f) {
		glLoadIdentity();
		glTranslatef(A_XPosition, 0.0f, -3.0f);////glTranslatef(0.75f, 0.0f, -3.0f);
		DrawLetter('T');
	}


	if (stage == 6)
	{
		Plane1_Y_Position = ((Plane1_X_Position + 1.3)*(Plane1_X_Position + 1.3) / (4 * 0.15)) - 0.0f;
		glLoadIdentity();
		glTranslatef(Plane1_X_Position, Plane1_Y_Position, -3.0f);//glTranslatef(-1.0f, 0.0f, -3.0f);
		glRotatef(Plane1_Z_Rotation_Angle, 0.0f, 0.0f, 1.0f);
		DrawLetter('P');

		glLoadIdentity();
		glTranslatef(Plane1_X_Position, 0.0f, -3.0f);//glTranslatef(-1.0f, 0.0f, -3.0f);
		DrawLetter('P');

		glLoadIdentity();
		glTranslatef(Plane1_X_Position, -1 * (Plane1_Y_Position), -3.0f);//glTranslatef(-1.0f, 0.0f, -3.0f);
		glRotatef(-Plane1_Z_Rotation_Angle, 0.0f, 0.0f, 1.0f);

		DrawLetter('P');
	}

	SwapBuffers(ghdc);

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

void DrawLetter(char letter) {
	switch (letter)
	{
	case 'I':

		glBegin(GL_QUADS);

		glColor3f(1.0f, 0.6f, 0.2f);
		glVertex3f(0.275f, 1.0f, 0.0f);
		glVertex3f(0.225f, 1.0f, 0.0f);

		glColor3f(0.074f, 0.533f, 0.2f);
		glVertex3f(0.225f, -1.0f, 0.0f);
		glVertex3f(0.275f, -1.0f, 0.0f);


		glEnd();
		break;

	case 'N':
		glBegin(GL_QUADS);

		glColor3f(1.0f, 0.6f, 0.2f);
		glVertex3f(0.05f, 1.0f, 0.0f);
		glVertex3f(0.0f, 1.0f, 0.0f);

		glColor3f(0.074f, 0.533f, 0.2f);
		glVertex3f(0.00f, -1.0f, 0.0f);
		glVertex3f(0.05f, -1.0f, 0.0f);

		glColor3f(1.0f, 0.6f, 0.2f);
		glVertex3f(0.05f, 1.0f, 0.0f);
		glVertex3f(0.00f, 1.0f, 0.0f);

		glColor3f(0.074f, 0.533f, 0.2f);
		glVertex3f(0.45f, -1.0f, 0.0f);
		glVertex3f(0.50f, -1.0f, 0.0f);

		glColor3f(1.0f, 0.6f, 0.2f);
		glVertex3f(0.50f, 1.0f, 0.0f);
		glVertex3f(0.45f, 1.0f, 0.0f);

		glColor3f(0.074f, 0.533f, 0.2f);
		glVertex3f(0.45f, -1.0f, 0.0f);
		glVertex3f(0.50f, -1.0f, 0.0f);


		glEnd();

		break;

	case 'D':

		glBegin(GL_QUADS);
		glColor3f(D_R_OrangeColor, D_G_OrangeColor, D_B_OrangeColor);
		glVertex3f(0.05f, 1.0f, 0.0f);
		glVertex3f(0.0f, 1.0f, 0.0f);

		glColor3f(D_R_GreenColor, D_G_GreenColor, D_B_GreenColor);
		glVertex3f(0.00f, -1.0f, 0.0f);
		glVertex3f(0.05f, -1.0f, 0.0f);


		glColor3f(D_R_OrangeColor, D_G_OrangeColor, D_B_OrangeColor);
		glVertex3f(0.5f, 1.0f, 0.0f);
		glVertex3f(-0.01f, 1.0f, 0.0f);

		glColor3f(D_R_OrangeColor, D_G_OrangeColor, D_B_OrangeColor);
		glVertex3f(-0.01f, 0.95f, 0.0f);
		glVertex3f(0.5f, 0.95f, 0.0f);


		glColor3f(D_R_OrangeColor, D_G_OrangeColor, D_B_OrangeColor);
		glVertex3f(0.50f, 1.0f, 0.0f);
		glVertex3f(0.45f, 1.0f, 0.0f);

		glColor3f(D_R_GreenColor, D_G_GreenColor, D_B_GreenColor);
		glVertex3f(0.45f, -1.0f, 0.0f);
		glVertex3f(0.50f, -1.0f, 0.0f);

		glColor3f(D_R_GreenColor, D_G_GreenColor, D_B_GreenColor);
		glVertex3f(0.50f, -0.95f, 0.0f);
		glVertex3f(-0.01f, -0.95f, 0.0f);

		glColor3f(D_R_GreenColor, D_G_GreenColor, D_B_GreenColor);
		glVertex3f(-0.01f, -1.0f, 0.0f);
		glVertex3f(0.50f, -1.0f, 0.0f);
		/*
		glColor3f(1.0f, 0.6f, 0.2f);
		glVertex3f(0.05f, 1.0f, 0.0f);
		glVertex3f(0.0f, 1.0f, 0.0f);

		glColor3f(0.074f, 0.533f, 0.2f);
		glVertex3f(0.00f, -1.0f, 0.0f);
		glVertex3f(0.05f, -1.0f, 0.0f);


		glColor3f(1.0f, 0.6f, 0.2f);
		glVertex3f(0.5f, 1.0f, 0.0f);
		glVertex3f(-0.01f, 1.0f, 0.0f);

		glColor3f(1.0f, 0.6f, 0.2f);
		glVertex3f(-0.01f, 0.95f, 0.0f);
		glVertex3f(0.5f, 0.95f, 0.0f);


		glColor3f(1.0f, 0.6f, 0.2f);
		glVertex3f(0.50f, 1.0f, 0.0f);
		glVertex3f(0.45f, 1.0f, 0.0f);

		glColor3f(0.074f, 0.533f, 0.2f);
		glVertex3f(0.45f, -1.0f, 0.0f);
		glVertex3f(0.50f, -1.0f, 0.0f);

		glColor3f(0.074f, 0.533f, 0.2f);
		glVertex3f(0.50f, -0.95f, 0.0f);
		glVertex3f(-0.01f, -0.95f, 0.0f);

		glColor3f(0.074f, 0.533f, 0.2f);
		glVertex3f(-0.01f, -1.0f, 0.0f);
		glVertex3f(0.50f, -1.0f, 0.0f);
		*/
		glEnd();

		break;
	case 'A':

		glBegin(GL_QUADS);

		glColor3f(1.0f, 0.6f, 0.2f);
		glVertex3f(0.275f, 1.0f, 0.0f);
		glVertex3f(0.225f, 1.0f, 0.0f);

		glColor3f(0.074f, 0.533f, 0.2f);
		glVertex3f(0.00f, -1.0f, 0.0f);
		glVertex3f(0.05f, -1.0f, 0.0f);

		glColor3f(1.0f, 0.6f, 0.2f);
		glVertex3f(0.275f, 1.0f, 0.0f);
		glVertex3f(0.225f, 1.0f, 0.0f);

		glColor3f(0.074f, 0.533f, 0.2f);
		glVertex3f(0.45f, -1.0f, 0.0f);
		glVertex3f(0.50f, -1.0f, 0.0f);


		/*glColor3f(1.0f, 0.6f, 0.2f);
		glVertex3f(0.375f, 0.075f, 0.0f);
		glVertex3f(0.125f, 0.075f, 0.0f);
		glVertex3f(0.125f, 0.025f, 0.0f);
		glVertex3f(0.375f, 0.025f, 0.0f);

		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.375f, 0.025f, 0.0f);
		glVertex3f(0.125f, 0.025f, 0.0f);
		glVertex3f(0.125f, -0.025f, 0.0f);
		glVertex3f(0.375f, -0.025f, 0.0f);

		glColor3f(0.074f, 0.533f, 0.2f);
		glVertex3f(0.375f, -0.025f, 0.0f);
		glVertex3f(0.125f, -0.025f, 0.0f);

		glVertex3f(0.125f, -0.075f, 0.0f);
		glVertex3f(0.375f, -0.075f, 0.0f);
*/

		glEnd();
		break;
	case 'P':

		glBegin(GL_QUADS);

		//Smoke R
		glColor3f(1.0f, 0.6f, 0.2f);
		glVertex3f(-0.24f, 0.06f, 0.0f);
		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3f(-0.40f, 0.06f, 0.0f);
		glVertex3f(-0.40f, 0.02f, 0.0f);
		glColor3f(1.0f, 0.6f, 0.2f);
		glVertex3f(-0.24f, 0.02f, 0.0f);

		//Smoke W
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(-0.24f, 0.02f, 0.0f);
		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3f(-0.40f, 0.02f, 0.0f);
		glVertex3f(-0.40f, -0.02f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(-0.24f, -0.02f, 0.0f);

		//Smoke G
		glColor3f(0.074f, 0.533f, 0.2f);
		glVertex3f(-0.24f, -0.02f, 0.0f);
		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3f(-0.40f, -0.02f, 0.0f);
		glVertex3f(-0.40f, -0.06f, 0.0f);
		glColor3f(0.074f, 0.533f, 0.2f);
		glVertex3f(-0.24f, -0.06f, 0.0f);


		glColor3f(0.729f, 0.886f, 0.933f);

		glVertex3f(0.20f, 0.07f, 0.0f);
		glVertex3f(-0.20f, 0.07f, 0.0f);
		glVertex3f(-0.20f, -0.07f, 0.0f);
		glVertex3f(0.20f, -0.07f, 0.0f);

		//Back Wings
		glVertex3f(-0.10f, 0.00f, 0.0f);
		glVertex3f(-0.27f, 0.12f, 0.0f);
		glVertex3f(-0.31f, 0.12f, 0.0f);
		glVertex3f(-0.24f, 0.00f, 0.0f);

		glVertex3f(-0.10f, 0.00f, 0.0f);
		glVertex3f(-0.27f, -0.12f, 0.0f);
		glVertex3f(-0.31f, -0.12f, 0.0f);
		glVertex3f(-0.24f, 0.00f, 0.0f);

		glEnd();

		glBegin(GL_TRIANGLES);
		/*glVertex3f(0.20f, 0.07f, 0.0f);
		glVertex3f(0.35f, 0.00f, 0.0f);
		glVertex3f(0.20f, -0.07f, 0.0f);
*/

//Upper Wing
		glVertex3f(-0.05f, 0.07f, 0.0f);
		glVertex3f(0.05f, 0.07f, 0.0f);
		glVertex3f(-0.15f, 0.22f, 0.0f);

		glVertex3f(-0.05f, -0.07f, 0.0f);
		glVertex3f(0.05f, -0.07f, 0.0f);
		glVertex3f(-0.15f, -0.22f, 0.0f);
		//Lower Wing
		glVertex3f(0.05f, 0.07f, 0.0f);
		glVertex3f(-0.10f, 0.22f, 0.0f);
		glVertex3f(-0.15f, 0.22f, 0.0f);

		glVertex3f(0.05f, -0.07f, 0.0f);
		glVertex3f(-0.10f, -0.22f, 0.0f);
		glVertex3f(-0.15f, -0.22f, 0.0f);

		glEnd();


		//Front Circle

		glBegin(GL_POINTS);

		//glColor3f(1.0f, 1.0f, 1.0f);

		for (float radius = 0.0f; radius <= 0.07f; radius = radius + 0.001f) {
			for (angle = 0.00f; angle < 2 * 3.14159265; angle = angle + 0.01f) {
				glVertex3f(cos(angle)*radius + 0.21f, sin(angle)*radius, 0.0f);
			}
		}
		glEnd();

		glBegin(GL_QUADS);

		//Letter I

		glColor3f(1.0f, 0.6f, 0.2f);
		glVertex3f(-0.05f, 0.05f, 0.0f);
		glVertex3f(-0.07f, 0.05f, 0.0f);
		glVertex3f(-0.07f, -0.05f, 0.0f);
		glVertex3f(-0.05f, -0.05f, 0.0f);

		//Letter A
		glColor3f(0.0f, 0.0f, 0.530f);

		glVertex3f(0.00f, 0.05f, 0.0f);
		glVertex3f(-0.02f, 0.05f, 0.0f);
		glVertex3f(-0.04f, -0.05f, 0.0f);
		glVertex3f(-0.02f, -0.05f, 0.0f);

		glVertex3f(0.00f, 0.05f, 0.0f);
		glVertex3f(0.02f, 0.05f, 0.0f);
		glVertex3f(0.04f, -0.05f, 0.0f);
		glVertex3f(0.02f, -0.05f, 0.0f);

		glVertex3f(0.02f, 0.01f, 0.0f);
		glVertex3f(-0.02f, 0.01f, 0.0f);
		glVertex3f(-0.02f, -0.01f, 0.0f);
		glVertex3f(0.02f, -0.01f, 0.0f);


		//Letter F
		glColor3f(0.074f, 0.533f, 0.2f);

		glVertex3f(0.06f, 0.05f, 0.0f);
		glVertex3f(0.04f, 0.05f, 0.0f);
		glVertex3f(0.04f, -0.05f, 0.0f);
		glVertex3f(0.06f, -0.05f, 0.0f);

		glVertex3f(0.09f, 0.05f, 0.0f);
		glVertex3f(0.06f, 0.05f, 0.0f);
		glVertex3f(0.06f, 0.035f, 0.0f);
		glVertex3f(0.09f, 0.035f, 0.0f);

		glVertex3f(0.09f, 0.020f, 0.0f);
		glVertex3f(0.06f, 0.020f, 0.0f);
		glVertex3f(0.06f, 0.005f, 0.0f);
		glVertex3f(0.09f, 0.005f, 0.0f);

		glEnd();


		break;
	case 'L':
		glBegin(GL_QUADS);
		if (Plane1_X_Position < 1.40) {
			glColor3f(1.0f, 0.6f, 0.2f);
			glVertex3f(Plane1_X_Position - 0.27f, 0.06f, 0.0f);
			glVertex3f(Line_X_Position, 0.06f, 0.0f);
			glVertex3f(Line_X_Position, 0.02f, 0.0f);
			glVertex3f(Plane1_X_Position - 0.27f, 0.02f, 0.0f);

			// W
			glColor3f(1.0f, 1.0f, 1.0f);
			glVertex3f(Plane1_X_Position - 0.27f, 0.02f, 0.0f);
			glVertex3f(Line_X_Position, 0.02f, 0.0f);
			glVertex3f(Line_X_Position, -0.02f, 0.0f);
			glVertex3f(Plane1_X_Position - 0.27f, -0.02f, 0.0f);

			// G
			glColor3f(0.074f, 0.533f, 0.2f);
			glVertex3f(Plane1_X_Position - 0.27f, -0.02f, 0.0f);
			glVertex3f(Line_X_Position, -0.02f, 0.0f);
			glVertex3f(Line_X_Position, -0.06f, 0.0f);
			glVertex3f(Plane1_X_Position - 0.27f, -0.06f, 0.0f);
		}
		else {
			glColor3f(Line_Orange_R, Line_Orange_G, Line_Orange_B);
			glVertex3f(1.1f, 0.06f, 0.0f);
			glVertex3f(Line_X_Position, 0.06f, 0.0f);
			glVertex3f(Line_X_Position, 0.02f, 0.0f);
			glVertex3f(1.1f, 0.02f, 0.0f);

			// W
			glColor3f(Line_White_R, Line_White_G, Line_White_B);
			glVertex3f(1.1f, 0.02f, 0.0f);
			glVertex3f(Line_X_Position, 0.02f, 0.0f);
			glVertex3f(Line_X_Position, -0.02f, 0.0f);
			glVertex3f(1.1f, -0.02f, 0.0f);

			// G
			glColor3f(Line_Green_R, Line_Green_G, Line_Green_B);
			glVertex3f(1.1f, -0.02f, 0.0f);
			glVertex3f(Line_X_Position, -0.02f, 0.0f);
			glVertex3f(Line_X_Position, -0.06f, 0.0f);
			glVertex3f(1.1f, -0.06f, 0.0f);

		}

		glEnd();

		break;
	case 'T'://Tricolor of A
		glBegin(GL_QUADS);

		//R
		glColor3f(1.0f, 0.6f, 0.2f);
		glVertex3f(0.380f, 0.06f, 0.0f);
		glVertex3f(0.12f, 0.06f, 0.0f);
		glVertex3f(0.12f, 0.02f, 0.0f);
		glVertex3f(0.380f, 0.02f, 0.0f);


		//W
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.380f, 0.02f, 0.0f);
		glVertex3f(0.12f, 0.02f, 0.0f);
		glVertex3f(0.12f, -0.02f, 0.0f);
		glVertex3f(0.380f, -0.02f, 0.0f);

		//G
		glColor3f(0.074f, 0.533f, 0.2f);
		glVertex3f(0.380f, -0.02f, 0.0f);
		glVertex3f(0.12f, -0.02f, 0.0f);
		glVertex3f(0.12f, -0.06f, 0.0f);
		glVertex3f(0.380f, -0.06f, 0.0f);

		glEnd();
		break;
	}
}
