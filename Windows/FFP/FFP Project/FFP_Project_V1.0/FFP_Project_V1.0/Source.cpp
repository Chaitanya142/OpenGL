#include<Windows.h>
#include<stdio.h>
#include<gl\GL.h>
#include<gl\GLU.h>
#include"resource.h"
#include<windowsx.h>
#define _USE_MATH_DEFINES 1
#include<math.h>
#include "Globe.h"
#include <stdlib.h>
#include <limits.h>
#include"FontOGL.h"
#include"VertexToWindow.h"

struct AirportPoint {
	int AirportIndex;
	char AirportName[500];
	char AirportCode[10];
	float Latitude;
	float Longitude;
	struct AirportPoint *ptrNextAirport;
};
typedef struct AirportPoint *ptrAirport;
ptrAirport AirportPointer;//Head pointer
struct AirportPointOnWindow {
	int AirportIndex;
	float Latitude;
	float Longitude;
	float glx;//Opengl vertex point
	float gly;
	float glz;
	float wx;//Windows point
	float wy;
	float wz;
	struct AirportPointOnWindow *ptrNextAirport;
};
typedef struct AirportPointOnWindow *ptrAirportWindow;
ptrAirportWindow AirportWindowPointer;//Head pointer
struct Route {
	char Airport1Code[10];
	char Airport2Code[10];
	int Airport1Index;
	int Airport2Index;
	char RouteName[20];
	//char AlternativeRouteName[20];
	float distance;
	struct Route *ptrNextRoute;
};
typedef struct Route *ptrRoute;
ptrRoute RoutePointer;//Head Pointer
struct ShortRoutePoints {
	float Latitude;
	float Longitude;
	int AirportIndex;
	struct ShortRoutePoints *ptrNextRoutePoint;
};
typedef struct ShortRoutePoints *ptrShortRoute;
ptrShortRoute ShortRoutePointer;//Head Pointer
//#include <time.h>
#pragma comment(lib,"OpenGL32.lib")
#pragma comment(lib,"glu32.lib")
																																																																																																																																																																																																																																																																//Constants
#define WIN_WIDTH 1000
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
FILE *gpLocationFile = NULL;
FILE *gpRoutesFile = NULL;
GLfloat angleGlobe = 0.0f;
GLuint texture_WorldMap;
GLuint texture_Airport;
GLuint texture_Galaxy;

int xPos;
int yPos;
Globe s;
void addShortRoute(int j);
int stage = 0;
int Airport1Index = -1;
int Airport2Index = -1;
char Airport1Name[255];
char Airport2Name[255];
float tempTestVariable = 1.0f;
float gz = 5.0f;
float gx = 0.0f;
float gy = 0.0f;
int airportCount = 0;
struct Graph* createGraph(int V);
struct Graph* graph;
int gWidth;
int gHeight;
GLuint   base;
FontOGL f1;
FontOGL f2;

void dijkstra(struct Graph* graph, int src);
void getShortRoute(int DestinationIndex);
void boardLetterPrint(char *line1, char *line2);
//Method Declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void GetAirportDetails();
void GetRouteDetails();
//void getWindowCoords(float x, float y, float z, float RCy, float *wx, float *wy, float *wz);
int getAirportFromWindowsCoordinate(int wx, int wy);
void getShortestRoute();
void getAirportWindowDetails(RECT rc);
void getDistance(char Airport1Code[10], char Airport2Code[10], float *dist, Route*);
void addEdge(struct Graph* graph, int src, int dest, float weight);
double distance(double lat1, double lon1, double lat2, double lon2);
double deg2rad(double deg);
double rad2deg(double rad);


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
	float static fwKeys;
	float static zDelta;
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
	case WM_LBUTTONDOWN:
		xPos = GET_X_LPARAM(lParam);
		yPos = GET_Y_LPARAM(lParam);
		/*data = (GLubyte*)malloc(4 * 1 * 1);
		glReadPixels(xPos,
		WIN_WIDTH - yPos,
		1,
		1,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		data);
		R = (int)data[0];
		G = (int)data[1];
		B = (int)data[2];
		free(data);*/
		if (stage == 1 || stage == 2) {
			RECT rc;
			GetClientRect(ghwnd, &rc);
			getAirportWindowDetails(rc);
			if (stage == 1)
			{
				Airport1Index = getAirportFromWindowsCoordinate(xPos, yPos);
				ptrAirport tempAirportPoint = AirportPointer;
				while (tempAirportPoint != NULL)
				{
					if (tempAirportPoint->AirportIndex == Airport1Index) {
						strcpy_s(Airport1Name, tempAirportPoint->AirportName);
					}
					tempAirportPoint = tempAirportPoint->ptrNextAirport;
				}
			}
			else {
				Airport2Index = getAirportFromWindowsCoordinate(xPos, yPos);
				ptrAirport tempAirportPoint = AirportPointer;
				while (tempAirportPoint != NULL)
				{
					if (tempAirportPoint->AirportIndex == Airport2Index) {
						strcpy_s(Airport2Name, tempAirportPoint->AirportName);
					}
					tempAirportPoint = tempAirportPoint->ptrNextAirport;
				}
			}
		}
		break;
	case WM_MOUSEWHEEL:
		fwKeys = GET_KEYSTATE_WPARAM(wParam);
		zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		if (zDelta > 0) {
			if (gz > 1.7f)//To control max Zoom
				gz = gz - 0.2f;
			//tempTestVariable += 0.1f;
		}
		else {
			gz = gz + 0.2f;
			//tempTestVariable -= 0.1f;
		}
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
		case 's':
		case 'S':
			getShortestRoute();
			break;
		case 'r':
		case 'R':
			ptrShortRoute temp = ShortRoutePointer;
			ptrShortRoute temp1 = ShortRoutePointer;

			while (temp != NULL) {
				temp1 = temp;
				temp = temp->ptrNextRoutePoint;
				free(temp1);
			}

			ShortRoutePointer = NULL;
			stage = 1;
			Airport1Index = -1;
			Airport2Index = -1;

			break;
		}
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_NUMPAD4:
			gx -= 0.2;
			break;
		case VK_NUMPAD6:
			gx += 0.2;
			break;
		case VK_NUMPAD8:
			gy += 0.2;
			break;
		case VK_NUMPAD2:
			gy -= 0.2;
			break;
		default:
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
				//ShowCursor(FALSE);
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
		//ShowCursor(TRUE);
		bFullScreen = false;
	}
}
//Initialize OpenGl
int Initialize(void) {
	//Function Declaration
	void Resize(int, int);
	BOOL LoadTexture(GLuint *texture, TCHAR imageResourceID[]);
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
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_TEXTURE_2D);
	LoadTexture(&texture_WorldMap, MAKEINTRESOURCE(IDBITMAP_WORLD));
	LoadTexture(&texture_Airport, MAKEINTRESOURCE(IDBITMAP_AIRPORT));

	LoadTexture(&texture_Galaxy, MAKEINTRESOURCE(IDBITMAP_GALAXY));

	//void makeSphere(GLfloat fRadius, GLint iSlices, GLint iStacks);
	//makeSphere(1.0f, 360, 180);
	//To Run Code From File
	//Latitudes 360 Longitude 180
	//s.Set(1.5f, 360, 180);
	//s.Set(1.5f, 180, 90);//Reduced to increase frame rate

	s.Set(1.5f, 90, 90);//Reduced to increase frame rate


	if (fopen_s(&gpLocationFile, "Locations.txt", "r+") != 0)
	{
		MessageBox(NULL, TEXT("Can't Open Locations File"), TEXT("ERROR"), MB_OK);
		exit(0);
	}
	else 
	{
		//PrintTime();
		fprintf_s(gpFile, "Opened Locations File");
	}
	GetAirportDetails();
	fclose(gpLocationFile);
	if (fopen_s(&gpRoutesFile, "Routes.txt", "r+") != 0)
	{
		MessageBox(NULL, TEXT("Can't Open Routes File"), TEXT("ERROR"), MB_OK);
		exit(0);
	}
	else 
	{
		//PrintTime();
		fprintf_s(gpFile, "Opened Locations File");
	}
	GetRouteDetails();
	fclose(gpRoutesFile);
	ptrAirport tempAirportPtr = AirportPointer;
	while (tempAirportPtr != NULL)
	{
		airportCount++;
		tempAirportPtr = tempAirportPtr->ptrNextAirport;
	}
	graph = createGraph(airportCount);
	ptrRoute tempRoute = RoutePointer;
	while (tempRoute != NULL) 
	{
		addEdge(graph, tempRoute->Airport1Index, tempRoute->Airport2Index, tempRoute->distance);
		tempRoute = tempRoute->ptrNextRoute;
	}
	
	Resize(WIN_WIDTH, WIN_HEIGHT);

	//Set font heights
	f1.set(ghdc, 20.0f);
	f2.set(ghdc, 15.0f);

	stage = 1;
	return 0;
}

//File IO to get Flight Route Details
void GetRouteDetails()
{
	ptrRoute Route;
	ptrRoute temp = NULL;
	char tempString[255];
	if (gpRoutesFile != NULL)
	{
		char line[128]; /* or other suitable maximum line size */
		while (fgets(line, sizeof line, gpRoutesFile) != NULL) /* read a line */
		{
			fprintf_s(gpFile, line);
			Route = (ptrRoute)malloc(sizeof(struct Route));
			Route->ptrNextRoute = NULL;
			char *tokenString = NULL;
			char *next_token = NULL;
			char seps[] = ",";
			tokenString = strtok_s(line, seps, &next_token);
			strcpy_s(Route->Airport1Code, 10, tokenString);
			tokenString = strtok_s(NULL, seps, &next_token);
			strcpy_s(tempString, 255, tokenString);
			tokenString = strtok_s(tempString, ";", &next_token);
			strcpy_s(Route->Airport2Code, 10, tokenString);
			strcpy_s(Route->RouteName, 10, Route->Airport1Code);
			strcat_s(Route->RouteName, 10, "-");
			strcat_s(Route->RouteName, 10, Route->Airport2Code);
			getDistance(Route->Airport1Code, Route->Airport2Code, &Route->distance, Route);
			if (RoutePointer == NULL) {
				RoutePointer = Route;
				temp = RoutePointer;
			}
			else {
				temp->ptrNextRoute = Route;
				temp = temp->ptrNextRoute;
			}
		}
	}
}

//File IO to get Airport Details
void GetAirportDetails()
{
	int AirpotIndex = 0;
	ptrAirport Airport;
	ptrAirport temp = NULL;
	char tempString[255];
	if (gpLocationFile != NULL)
	{
		char line[128]; /* or other suitable maximum line size */
		while (fgets(line, sizeof line, gpLocationFile) != NULL) /* read a line */
		{
			fprintf_s(gpFile, line);
			Airport = (ptrAirport)malloc(sizeof(struct AirportPoint));
			//strcpy_s(Airport->AirportName, sizeof(line), line);
			Airport->ptrNextAirport = NULL;
			//char string1[] =line;
			char *tokenString = NULL;
			char *next_token = NULL;
			char seps[] = ",";
			tokenString = strtok_s(line, seps, &next_token);
			strcpy_s(Airport->AirportCode, 10, tokenString);
			tokenString = strtok_s(NULL, seps, &next_token);
			strcpy_s(Airport->AirportName, 255, tokenString);
			tokenString = strtok_s(NULL, seps, &next_token);
			strcpy_s(tempString, 255, tokenString);
			Airport->Latitude = strtof(tempString, NULL);//  (float)tempString;
			tokenString = strtok_s(NULL, seps, &next_token);
			strcpy_s(tempString, 255, tokenString);
			//tokenString = strtok_s(tempString, ";", &next_token);
			//strcpy_s(tempString, 255, tokenString);
			Airport->Longitude = strtof(tempString, NULL);
			Airport->AirportIndex = AirpotIndex++;
			if (AirportPointer == NULL) {
				AirportPointer = Airport;
				temp = AirportPointer;
			}
			else {
				temp->ptrNextAirport = Airport;
				temp = temp->ptrNextAirport;
			}
		}
	}
}
//Function Resize
void Resize(int width, int height) {
	/*if (height == 0)
	height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,
	(GLfloat)width / (GLfloat)height,
	0.1f,
	100.0f);*/
	gWidth = width;
	gHeight = height;
}
//void Resize(int width, int height) {
//            if (height == 0)
//                            height = 1;
//            glViewport(0, 0, (GLsizei)width, (GLsizei)height);
//            glMatrixMode(GL_PROJECTION);
//            glLoadIdentity();
//            if (width <= height) {
//                            glOrtho(-10.0f,
//                                            10.0f,
//                                            (-10.0f*(GLfloat)width / (GLfloat)height),
//                                            (10.0f*(GLfloat)width / (GLfloat)height),
//                                            -10.0f,
//                                            10.0f
//                            );
//            }
//            else
//            {
//                            glOrtho(
//                                            (-10.0f*(GLfloat)width / (GLfloat)height),
//                                            (10.0f*(GLfloat)width / (GLfloat)height),
//                                            -10.0f,
//                                            10.0f,
//                                            -10.0f,
//                                            10.0f);
//            }
//
//}
//Function Display

//int draw=0;

SYSTEMTIME st1;
SYSTEMTIME st2;
float fps;

void Display(void) {

	GetSystemTime(&st1);

	fps = ((st1.wSecond * 1000 + st1.wMilliseconds) - (st2.wSecond * 1000 + st2.wMilliseconds));
	fps = 1000 / fps;
	st2 = st1;

	//ViewPort1
	glViewport(0, 0, (GLsizei)(gWidth * 4 / 5), (GLsizei)gHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,
		(GLfloat)(gWidth * 4 / 5) / (GLfloat)gHeight,
		0.1f,
		100.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(gx, gy, gz,
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f);
	//glTranslatef(0.0f, 0.0f, -gz);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glRotatef(angleGlobe, 0.0f, 0.0f, 1.0f);
	glScalef(-1, 1, 1);
	glBindTexture(GL_TEXTURE_2D, texture_WorldMap);
	float vx = 0, vy = 0, vz = 0;


	//Draw Globe
	s.DrawGlobe();

	//Draw Airport
	ptrAirport tempAirportPointer;
	tempAirportPointer = AirportPointer;
	while (tempAirportPointer != NULL) {
		s.DrawAirport(tempAirportPointer->Latitude, tempAirportPointer->Longitude, &vx, &vy, &vz);


		glColor3f(1.0f, 0.0f, 0.0f);
		glRasterPos3f(vx - 0.01f, vy, vz - 0.01f);
		f2.glPrint(tempAirportPointer->AirportName);

		tempAirportPointer = tempAirportPointer->ptrNextAirport;
	}

	//Draw Route
	ptrShortRoute tempPtrShortRoute;
	tempPtrShortRoute = ShortRoutePointer;
	while (tempPtrShortRoute != NULL && tempPtrShortRoute->ptrNextRoutePoint != NULL) {

		glColor3f(1.0f,0.0f,0.0f);
		s.DrawRoute(tempPtrShortRoute->Latitude, tempPtrShortRoute->Longitude, (tempPtrShortRoute->ptrNextRoutePoint)->Latitude, (tempPtrShortRoute->ptrNextRoutePoint)->Longitude);
		//fprintf_s(gpFile, "%f,%f,%f,%f", tempPtrShortRoute->Latitude, tempPtrShortRoute->Longitude, (tempPtrShortRoute->ptrNextRoutePoint)->Latitude, (tempPtrShortRoute->ptrNextRoutePoint)->Longitude);
		tempPtrShortRoute = tempPtrShortRoute->ptrNextRoutePoint;
	}



	glLoadIdentity();
	glColor3f(1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, texture_Galaxy);
	glBegin(GL_QUADS);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(4.0f, 3.0f, -6.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-4.0f, 3.0f, -6.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-4.0f, -3.0f, -6.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(4.0f, -3.0f, -6.0f);

	glEnd();


	//View Port 2
	glViewport((GLsizei)(gWidth * 4 / 5), 0, (GLsizei)(gWidth * 1 / 5), (GLsizei)gHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,
		(GLfloat)(gWidth * 1 / 5) / (GLfloat)gHeight,
		0.1f,
		100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	char line1[255];
	char line2[255];
	if (stage == 1) {
		strcpy_s(line1, "Select");
		strcpy_s(line2, "Airport One");
		boardLetterPrint(line1, line2);
	}
	if (stage == 2) {
		strcpy_s(line1, "Select");
		strcpy_s(line2, "Airport Two");
		boardLetterPrint(line1, line2);
	}
	if (stage == 3) {
		strcpy_s(line1, "Press S");
		strcpy_s(line2, "For Shortest Route");
		boardLetterPrint(line1, line2);
	}
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -3.0f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glScalef(-1, -1, 1);
	glBindTexture(GL_TEXTURE_2D, texture_Airport);
	glBegin(GL_QUADS);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.0f, 2.8f, -3.5f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.0f, 2.8f, -3.5f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-1.0f, -2.8f, -3.5f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.0f, -2.8f, -3.5f);

	glEnd();


	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -3.0f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_TRIANGLES);
	glVertex3f(-0.30f, 0.5f, -1.0f);
	glVertex3f(0.30f, 0.5f, -1.0f);
	glVertex3f(0.30f, 0.35f, -1.0f);
	glVertex3f(0.30f, 0.35f, -1.0f);
	glVertex3f(-0.30f, 0.35f, -1.0f);
	glVertex3f(-0.30f, 0.5f, -1.0f);
	glVertex3f(-0.30f, 0.25f, -1.0f);
	glVertex3f(0.30f, 0.25f, -1.0f);
	glVertex3f(0.30f, 0.10f, -1.0f);
	glVertex3f(0.30f, 0.10f, -1.0f);
	glVertex3f(-0.30f, 0.10f, -1.0f);
	glVertex3f(-0.30f, 0.25f, -1.0f);
	glEnd();
	if (Airport1Index != -1) {
		glColor3f(1.0f, 0.0f, 0.0f);
		glRasterPos3f(-0.25f, 0.40f, -0.9f);
		f1.glPrint(Airport1Name);
	}
	if (Airport2Index != -1) {
		glColor3f(1.0f, 0.0f, 0.0f);
		glRasterPos3f(-0.25f, 0.15f, -0.9f);
		f1.glPrint(Airport2Name);
	}

	glRasterPos3f(-0.2f, -1.5f, -0.9f);
	char strFPS[255];
	snprintf(strFPS, sizeof strFPS, "%f", fps);
	f1.glPrint(strFPS);


	SwapBuffers(ghdc);
}
void Update(void)
{
	/*angleGlobe = angleGlobe + 10.0f;
	if (angleGlobe > 360.0f)
	angleGlobe = 0.0f;
	*/
	angleGlobe = 180.0f;
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

	glDeleteTextures(1, &texture_WorldMap);
	glDeleteTextures(1, &texture_Airport);

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
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, bmp.bmWidth, bmp.bmHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, bmp.bmBits);
		DeleteObject(hBitMap);
	}
	return bStatus;
}
void getDistance(char Airport1Code[10], char Airport2Code[10], float *dist, Route* route) {
	ptrAirport Temp = AirportPointer;
	double lat1 = -370;//DeafultLoctn
	double long1 = -370;//DeafultLoctn
	double lat2 = -370;//DeafultLoctn
	double long2 = -370;//DeafultLoctn
	*dist = 0;
	while (Temp != NULL) {
		if (strcmp(Airport1Code, Temp->AirportCode) == 0) {
			lat1 = Temp->Latitude;
			long1 = Temp->Longitude;
			route->Airport1Index = Temp->AirportIndex;
		}
		if (strcmp(Airport2Code, Temp->AirportCode) == 0) {
			lat2 = Temp->Latitude;
			long2 = Temp->Longitude;
			route->Airport2Index = Temp->AirportIndex;
		}
		Temp = Temp->ptrNextAirport;
	}
	if (lat1 != -370 && long1 != -370 && lat2 != -370 && long2 != -370) {
		*dist = distance(lat1, long1, lat2, long2);
	}
}
//https://stackoverflow.com/questions/6981916/how-to-calculate-distance-between-two-locations-using-their-longitude-and-latitu
double distance(double lat1, double lon1, double lat2, double lon2) {
	double theta = lon1 - lon2;
	double dist = sin(deg2rad(lat1))
		* sin(deg2rad(lat2))
		+ cos(deg2rad(lat1))
		* cos(deg2rad(lat2))
		* cos(deg2rad(theta));
	dist = acos(dist);
	dist = rad2deg(dist);
	dist = dist * 60 * 1.1515;
	return (dist);
}
double deg2rad(double deg) {
	return (deg * M_PI / 180.0);
}
double rad2deg(double rad) {
	return (rad * 180.0 / M_PI);
}
//https://www.geeksforgeeks.org/dijkstras-algorithm-for-adjacency-list-representation-greedy-algo-8/
//https://www.geeksforgeeks.org/printing-paths-dijkstras-shortest-path-algorithm/
// A structure to represent a node in adjacency list
struct AdjListNode
{
	int dest;
	float weight;
	struct AdjListNode* next;
};
// A structure to represent an adjacency liat
struct AdjList
{
	struct AdjListNode *head; // pointer to head node of list
};
// A structure to represent a graph. A graph is an array of adjacency lists.
// Size of array will be V (number of vertices in graph)
struct Graph
{
	int V;
	struct AdjList* array;
};
// A utility function to create a new adjacency list node
struct AdjListNode* newAdjListNode(int dest, float weight)
{
	struct AdjListNode* newNode =
		(struct AdjListNode*) malloc(sizeof(struct AdjListNode));
	newNode->dest = dest;
	newNode->weight = weight;
	newNode->next = NULL;
	return newNode;
}
// A utility function that creates a graph of V vertices
struct Graph* createGraph(int V)
{
	struct Graph* graph = (struct Graph*) malloc(sizeof(struct Graph));
	graph->V = V;
	// Create an array of adjacency lists. Size of array will be V
	graph->array = (struct AdjList*) malloc(V * sizeof(struct AdjList));
	// Initialize each adjacency list as empty by making head as NULL
	for (int i = 0; i < V; ++i)
		graph->array[i].head = NULL;
	return graph;
}
// Adds an edge to an undirected graph
void addEdge(struct Graph* graph, int src, int dest, float weight)
{
	// Add an edge from src to dest. A new node is added to the adjacency
	// list of src. The node is added at the begining
	struct AdjListNode* newNode = newAdjListNode(dest, weight);
	newNode->next = graph->array[src].head;
	graph->array[src].head = newNode;
	// Since graph is undirected, add an edge from dest to src also
	newNode = newAdjListNode(src, weight);
	newNode->next = graph->array[dest].head;
	graph->array[dest].head = newNode;
}
// Structure to represent a min heap node
struct MinHeapNode
{
	int v;
	float dist;
};
// Structure to represent a min heap
struct MinHeap
{
	int size; // Number of heap nodes present currently
	int capacity; // Capacity of min heap
	int *pos;              // This is needed for decreaseKey()
	struct MinHeapNode **array;
};
// A utility function to create a new Min Heap Node
struct MinHeapNode* newMinHeapNode(int v, float dist)
{
	struct MinHeapNode* minHeapNode =
		(struct MinHeapNode*) malloc(sizeof(struct MinHeapNode));
	minHeapNode->v = v;
	minHeapNode->dist = dist;
	return minHeapNode;
}
// A utility function to create a Min Heap
struct MinHeap* createMinHeap(int capacity)
{
	struct MinHeap* minHeap =
		(struct MinHeap*) malloc(sizeof(struct MinHeap));
	minHeap->pos = (int *)malloc(capacity * sizeof(int));
	minHeap->size = 0;
	minHeap->capacity = capacity;
	minHeap->array =
		(struct MinHeapNode**) malloc(capacity * sizeof(struct MinHeapNode*));
	return minHeap;
}
// A utility function to swap two nodes of min heap. Needed for min heapify
void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b)
{
	struct MinHeapNode* t = *a;
	*a = *b;
	*b = t;
}
// A standard function to heapify at given idx
// This function also updates position of nodes when they are swapped.
// Position is needed for decreaseKey()
void minHeapify(struct MinHeap* minHeap, int idx)
{
	int smallest, left, right;
	smallest = idx;
	left = 2 * idx + 1;
	right = 2 * idx + 2;
	if (left < minHeap->size &&
		minHeap->array[left]->dist < minHeap->array[smallest]->dist)
		smallest = left;
	if (right < minHeap->size &&
		minHeap->array[right]->dist < minHeap->array[smallest]->dist)
		smallest = right;
	if (smallest != idx)
	{
		// The nodes to be swapped in min heap
		MinHeapNode *smallestNode = minHeap->array[smallest];
		MinHeapNode *idxNode = minHeap->array[idx];
		// Swap positions
		minHeap->pos[smallestNode->v] = idx;
		minHeap->pos[idxNode->v] = smallest;
		// Swap nodes
		swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
		minHeapify(minHeap, smallest);
	}
}
// A utility function to check if the given minHeap is ampty or not
int isEmpty(struct MinHeap* minHeap)
{
	return minHeap->size == 0;
}
// Standard function to extract minimum node from heap
struct MinHeapNode* extractMin(struct MinHeap* minHeap)
{
	if (isEmpty(minHeap))
		return NULL;
	// Store the root node
	struct MinHeapNode* root = minHeap->array[0];
	// Replace root node with last node
	struct MinHeapNode* lastNode = minHeap->array[minHeap->size - 1];
	minHeap->array[0] = lastNode;
	// Update position of last node
	minHeap->pos[root->v] = minHeap->size - 1;
	minHeap->pos[lastNode->v] = 0;
	// Reduce heap size and heapify root
	--minHeap->size;
	minHeapify(minHeap, 0);
	return root;
}
// Function to decreasy dist value of a given vertex v. This function
// uses pos[] of min heap to get the current index of node in min heap
void decreaseKey(struct MinHeap* minHeap, int v, float dist)
{
	// Get the index of v in heap array
	int i = minHeap->pos[v];
	// Get the node and update its dist value
	minHeap->array[i]->dist = dist;
	// Travel up while the complete tree is not hepified.
	// This is a O(Logn) loop
	while (i && minHeap->array[i]->dist < minHeap->array[(i - 1) / 2]->dist)
	{
		// Swap this node with its parent
		minHeap->pos[minHeap->array[i]->v] = (i - 1) / 2;
		minHeap->pos[minHeap->array[(i - 1) / 2]->v] = i;
		swapMinHeapNode(&minHeap->array[i], &minHeap->array[(i - 1) / 2]);
		// move to parent index
		i = (i - 1) / 2;
	}
}
// A utility function to check if a given vertex
// 'v' is in min heap or not
bool isInMinHeap(struct MinHeap *minHeap, int v)
{
	if (minHeap->pos[v] < minHeap->size)
		return true;
	return false;
}
// A utility function used to print the solution
void printArr(float dist[], int n)
{
	printf("Vertex Distance from Source\n");
	for (int i = 0; i < n; ++i)
		printf("%d \t\t %d\n", i, dist[i]);
}
void printPath(int parent[], int j);
int printSolution(float dist[], int n, int parent[], int strart);
int *Parent;
float *dist;
// The main function that calulates distances of shortest paths from src to all
// vertices. It is a O(ELogV) function
void dijkstra(struct Graph* graph, int src)
{
	int V = graph->V;// Get the number of vertices in graph
	Parent = (int *)malloc(V * sizeof(int));
	dist = (float *)malloc(V * sizeof(float));
	//float dist[V];   // dist values used to pick minimum weight edge in cut
	// minHeap represents set E
	struct MinHeap* minHeap = createMinHeap(V);
	// Initialize min heap with all vertices. dist value of all vertices
	for (int v = 0; v < V; ++v)
	{
		Parent[v] = -1;
		dist[v] = INT_MAX;
		minHeap->array[v] = newMinHeapNode(v, dist[v]);
		minHeap->pos[v] = v;
	}
	// Make dist value of src vertex as 0 so that it is extracted first
	minHeap->array[src] = newMinHeapNode(src, dist[src]);
	minHeap->pos[src] = src;
	dist[src] = 0;
	decreaseKey(minHeap, src, dist[src]);
	// Initially size of min heap is equal to V
	minHeap->size = V;
	// In the followin loop, min heap contains all nodes
	// whose shortest distance is not yet finalized.
	while (!isEmpty(minHeap))
	{
		// Extract the vertex with minimum distance value
		struct MinHeapNode* minHeapNode = extractMin(minHeap);
		int u = minHeapNode->v; // Store the extracted vertex number
																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																		// Traverse through all adjacent vertices of u (the extracted
																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																		// vertex) and update their distance values
		struct AdjListNode* pCrawl = graph->array[u].head;
		while (pCrawl != NULL)
		{
			int v = pCrawl->dest;
			// If shortest distance to v is not finalized yet, and distance to v
			// through u is less than its previously calculated distance
			if (isInMinHeap(minHeap, v) && dist[u] != INT_MAX &&
				pCrawl->weight + dist[u] < dist[v])
			{
				dist[v] = dist[u] + pCrawl->weight;
				Parent[v] = u;
				// update distance value in min heap also
				decreaseKey(minHeap, v, dist[v]);
			}
			pCrawl = pCrawl->next;
		}
	}
	// print the calculated shortest distances
	//printArr(dist, V);
	//printSolution(dist, V, Parent, src);
}
int printSolution(float dist[], int n, int parent[], int start)
{
	int src = start;
	printf("Vertex\t Distance\tPath");
	for (int i = 0; i < n; i++)
	{
		//printf("\n%d -> %d \t\t %f\t\t%d ",
		//            src, i, dist[i], src);
		fprintf_s(gpFile, "\n%d -> %d \t\t %f\t\t%d ", src, i, dist[i], src);
		printPath(parent, i);
	}
	return 0;
}
//int main()
//{
//            // create the graph given in above fugure
//            int V = 9;
//            struct Graph* graph = createGraph(V);
//            addEdge(graph, 0, 1, 4);
//            addEdge(graph, 0, 7, 8);
//            addEdge(graph, 1, 2, 8);
//            addEdge(graph, 1, 7, 11);
//            addEdge(graph, 2, 3, 7);
//            addEdge(graph, 2, 8, 2);
//            addEdge(graph, 2, 5, 4.2);
//            addEdge(graph, 3, 4, 9);
//            addEdge(graph, 3, 5, 14.2);
//            addEdge(graph, 4, 5, 10);
//            addEdge(graph, 5, 6, 2.2);
//            addEdge(graph, 6, 7, 1);
//            addEdge(graph, 6, 8, 6);
//            addEdge(graph, 7, 8, 7);
//
//            dijkstra(graph, 1);
//
//            return 0;
//}
void printPath(int parent[], int j)
{
	// Base Case : If j is source/No Path
	if (parent[j] == -1)
		return;
	printPath(parent, parent[j]);
	fprintf_s(gpFile, "%d ", j);
	printf("%d ", j);
}
void getLongLatFromAirportIndex(int AirportIndex, float *Latitude, float *Longitude);
ptrShortRoute lastShortRoutePtr = NULL;
void getShortRoute(int DestinationIndex) {
	lastShortRoutePtr = NULL;
	ptrShortRoute temp = ShortRoutePointer;
	ptrShortRoute temp1 = ShortRoutePointer;
	while (temp != NULL) {
		temp1 = temp;
		temp = temp->ptrNextRoutePoint;
		free(temp1);
	}
	ShortRoutePointer = NULL;
	addShortRoute(DestinationIndex);
}
void addShortRoute(int AirportIndex) {
	ptrShortRoute temp = NULL;
	temp = (ptrShortRoute)malloc(sizeof(struct ShortRoutePoints));
	temp->ptrNextRoutePoint = NULL;
	temp->AirportIndex = AirportIndex;
	getLongLatFromAirportIndex(AirportIndex, &(temp->Latitude), &(temp->Longitude));
	if (Parent[AirportIndex] == -1) {
		if (lastShortRoutePtr != NULL) {
			lastShortRoutePtr->ptrNextRoutePoint = temp;
		}
		else {
			ShortRoutePointer = temp;
			temp = ShortRoutePointer;
		}
		return;
	}
	else {
		if (ShortRoutePointer == NULL) {
			ShortRoutePointer = temp;
			temp = ShortRoutePointer;
			lastShortRoutePtr = temp;
		}
		else {
			lastShortRoutePtr->ptrNextRoutePoint = temp;
			lastShortRoutePtr = temp;
		}
		addShortRoute(Parent[AirportIndex]);
	}
}
void getLongLatFromAirportIndex(int AirportIndex, float *Latitude, float *Longitude) {
	ptrAirport Temp = AirportPointer;
	*Latitude = 0;
	*Longitude = 0;
	while (Temp != NULL) {
		if (Temp->AirportIndex == AirportIndex) {
			*Latitude = Temp->Latitude;
			*Longitude = Temp->Longitude;
			break;
		}
		Temp = Temp->ptrNextAirport;
	}
}



void getAirportWindowDetails(RECT rc) {
	ptrAirportWindow tempAirportWindow = NULL;
	ptrAirportWindow AirportWindow;
	AirportWindow = AirportWindowPointer;
	//tempAirportWindow = AirportWindow;
	while (AirportWindow != NULL) {
		tempAirportWindow = AirportWindow->ptrNextAirport;
		free(AirportWindow);
		AirportWindow = tempAirportWindow;
	}

	AirportWindowPointer = NULL;
	ptrAirport tempAirportPtr = AirportPointer;

	VertexToWindow vw;
	vw.setRotationMatrix(90, 0, angleGlobe);
	vw.setScaleMatrix(-1, 1, 1);
	vw.setRCy(rc.bottom);
	vw.setPerspective(45, ((float)(4 * gWidth / 5) / gHeight), 0.1, 100);
	vw.setViewMatrix(gx, gy, gz, 0, 0, 0, 0, 1, 0);
	vw.setViewPortMatrix(0, 0, (4 * gWidth / 5), gHeight);
	vw.calculateMatrix();


	while (tempAirportPtr != NULL) {
		AirportWindow = (ptrAirportWindow)malloc(sizeof(struct AirportPointOnWindow));
		AirportWindow->ptrNextAirport = NULL;
		AirportWindow->AirportIndex = tempAirportPtr->AirportIndex;
		AirportWindow->Latitude = tempAirportPtr->Latitude;
		AirportWindow->Longitude = tempAirportPtr->Longitude;
		AirportWindow->glx = 0;
		AirportWindow->gly = 0;
		AirportWindow->glz = 0;
		s.GetAirportDetails(AirportWindow->Latitude, AirportWindow->Longitude, &(AirportWindow->glx), &(AirportWindow->gly), &(AirportWindow->glz));
		//getWindowCoords(AirportWindow->glx, AirportWindow->gly, AirportWindow->glz, rc.bottom, &(AirportWindow->wx), &(AirportWindow->wy), &(AirportWindow->wz));
		vw.getWindowCoordinates(AirportWindow->glx, AirportWindow->gly, AirportWindow->glz, &(AirportWindow->wx), &(AirportWindow->wy), &(AirportWindow->wz));
		if (AirportWindowPointer == NULL) {
			AirportWindowPointer = AirportWindow;
			tempAirportWindow = AirportWindow;
		}
		else {
			tempAirportWindow->ptrNextAirport = AirportWindow;
			tempAirportWindow = tempAirportWindow->ptrNextAirport;
		}
		fprintf_s(gpFile, "AirportIndex: %d Latitude: %f Longitude: %f glx: %f gly: %f glz: %f wx: %f wy: %f wz: %f\n", AirportWindow->AirportIndex, AirportWindow->Latitude, AirportWindow->Longitude, AirportWindow->glx, AirportWindow->gly, AirportWindow->glz, AirportWindow->wx, AirportWindow->wy, AirportWindow->wz);
		tempAirportPtr = tempAirportPtr->ptrNextAirport;
	}
}

void boardLetterPrint(char *line1, char *line2) {
	glDisable(GL_TEXTURE_2D);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor3f(1.0f, 0.0f, 0.0f);
	GLfloat lineLength = strlen(line1);//sizeof(line1) / sizeof(char); //length of an integer array.
	glRasterPos3f(0.0f - ((lineLength / 2)*0.040), 0.8f, -3.0f);
	f1.glPrint(line1);       // Print GL Text To The Screen

	lineLength = strlen(line2);//sizeof(line2) / sizeof(char);
	glRasterPos3f(0.0f - ((lineLength / 2)*0.040), 0.7f, -3.0f);
	f1.glPrint(line2);       // Print GL Text To The Screen
	glEnable(GL_TEXTURE_2D);
}

int getAirportFromWindowsCoordinate(int wx, int wy) {
	ptrAirportWindow tempAirportWindow = AirportWindowPointer;
	//tempAirportWindow = AirportWindow;
	while (tempAirportWindow != NULL) {
		if (((tempAirportWindow->wx - 1.5) <= wx) && (wx <= (tempAirportWindow->wx + 1.5)))
		{
			if (((tempAirportWindow->wy - 1.5) <= wy) && (wy <= (tempAirportWindow->wy + 1.5)))
			{
				if (tempAirportWindow->wz > 0) {
					stage++;
					return tempAirportWindow->AirportIndex;
				}
			}
		}
		tempAirportWindow = tempAirportWindow->ptrNextAirport;
	}
	return -1;
}

void getShortestRoute() {
	dijkstra(graph, Airport1Index);
	getShortRoute(Airport2Index);
}





////To  Multiply 4 by 4 Matrix
//void multiplyMatrix4by4(float(*first)[4][4], float(*second)[4][4], float(*multiply)[4][4], int r1, int c1, int r2, int c2) {//(float **first,float **second,float **multiply,int r1,int c1,int r2,int c2) {
//	float sum = 0;
//	for (int c = 0; c < r1; c++) {
//		for (int d = 0; d < c2; d++) {
//			for (int k = 0; k < r2; k++) {//c1==r2
//				sum = sum + (*first)[c][k] * (*second)[k][d];
//			}
//			(*multiply)[c][d] = sum;
//			sum = 0;
//		}
//	}
//}
////To Create Identity matrix 4 by 4
//void makeIdentity4by4(float matrix[][4]) {
//	for (int i = 0; i < 4; i++) {
//		for (int j = 0; j < 4; j++) {
//			if (i == j) {
//				matrix[i][j] = 1;
//			}
//			else {
//				matrix[i][j] = 0;
//			}
//		}
//	}
//}
////Copy Matrix 4 by 4
//void copyMatrix4by4(float(*source)[4][4], float(*dest)[4][4]) {
//	for (int i = 0; i < 4; i++) {
//		for (int j = 0; j < 4; j++) {
//			(*dest)[i][j] = (*source)[i][j];
//		}
//	}
//}
////To  Multiply 4 by 1 Matrix
//void multiplyMatrix4by1(float first[][4], float second[][1], float multiply[][1], int r1, int c1, int r2, int c2) {//(float **first,float **second,float **multiply,int r1,int c1,int r2,int c2) {
//	float sum = 0;
//	for (int c = 0; c < r1; c++) {
//		for (int d = 0; d < c2; d++) {
//			for (int k = 0; k < r2; k++) {//c1==r2
//				sum = sum + first[c][k] * second[k][d];
//			}
//			multiply[c][d] = sum;
//			sum = 0;
//		}
//	}
//}
////Copy Matrix 4 by 1
//void copyMatrix4by1(float source[][1], float dest[][1]) {
//	for (int i = 0; i < 4; i++) {
//		for (int j = 0; j < 4; j++) {
//			dest[i][j] = source[i][j];
//		}
//	}
//}
////Zero Out matrix 4 by 1
//void zeroMatrix(float matrix[][1]) {
//	for (int i = 0; i < 4; i++) {
//		for (int j = 0; j < 0; j++) {
//			matrix[i][j] = 0;
//		}
//	}
//}
////Cross Product of array
//void crossProduct(float vect_A[], float vect_B[], float cross_P[])
//{
//	//cross_P[0] = vect_A[1] * vect_B[2] - vect_A[2] * vect_B[1];
//	//cross_P[1] = vect_A[0] * vect_B[2] - vect_A[2] * vect_B[0];
//	//cross_P[2] = vect_A[0] * vect_B[1] - vect_A[1] * vect_B[0];
//	cross_P[0] = vect_A[1] * vect_B[2] - vect_A[2] * vect_B[1];
//	cross_P[1] = vect_A[2] * vect_B[0] - vect_A[0] * vect_B[2];
//	cross_P[2] = vect_A[0] * vect_B[1] - vect_A[1] * vect_B[0];
//}
////Normalise Vector values
//void normalize(float matrix[])
//{
//	float w = sqrt(matrix[0] * matrix[0] + matrix[1] * matrix[1] + matrix[2] * matrix[2]);
//	matrix[0] /= w;
//	matrix[1] /= w;
//	matrix[2] /= w;
//}
////gluPerspective To GlFrustrum
//void perspectiveGL(float fovY, float aspect, float zNear, float zFar, float *fW, float *fH)
//{
//	//const float pi = 3.1415926535897932384626433832795;
//	//float fW, fH;
//	////fH = tan( (fovY / 2) / 180 * pi ) * zNear;
//	//fH = tan(fovY / 360 * pi) * zNear;
//	//fW = fH * aspect;
//	//glFrustum(-fW, fW, -fH, fH, zNear, zFar);
//	*fH = tan(fovY / 360 * M_PI) * zNear;
//	*fW = *fH * aspect;
//}
//
//void getWindowCoords(float x, float y, float z, float RCy, float *wx, float *wy, float *wz) {
//	void makeIdentity4by4(float matrix[][4]);
//	void copyMatrix4by4(float(*source)[4][4], float(*dest)[4][4]);
//	void multiplyMatrix4by4(float(*first)[4][4], float(*second)[4][4], float(*multiply)[4][4], int r1, int c1, int r2, int c2);
//	void zeroMatrix(float matrix[][1]);
//	void multiplyMatrix4by1(float first[][4], float second[][1], float multiply[][1], int r1, int c1, int r2, int c2);
//	void copyMatrix4by1(float source[][1], float dest[][1]);
//	void perspectiveGL(float fovY, float aspect, float zNear, float zFar, float *fW, float *fH);
//	void normalize(float matrix[]);
//	void crossProduct(float vect_A[], float vect_B[], float cross_P[]);
//	float getRadFromDegree(float);
//	float multiply4by4[4][4];
//	float temp4by4[4][4];
//	float multiply4by1[4][1];
//	float temp4by1[4][1];
//	zeroMatrix(multiply4by1);
//	zeroMatrix(temp4by1);
//	float scaleMatrix[4][4];
//	float translationMatrix[4][4];
//	float rotationX[4][4];
//	float rotationY[4][4];
//	float rotationZ[4][4];
//	float vertex[4][1];
//	float angleX = 0, angleY = 0, angleZ = 0;
//	zeroMatrix(vertex);
//	vertex[3][0] = 1;//W Always 1
//																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																	//Point whose coordinates needs to be calculated
//	vertex[0][0] = x;
//	vertex[1][0] = y;
//	vertex[2][0] = z;
//	vertex[3][0] = 1;
//	//Make all matrix identity
//	makeIdentity4by4(scaleMatrix);
//	makeIdentity4by4(translationMatrix);
//	makeIdentity4by4(rotationX);
//	makeIdentity4by4(rotationY);
//	makeIdentity4by4(rotationZ);
//	makeIdentity4by4(multiply4by4);
//	makeIdentity4by4(temp4by4);
//	//makeIdentity4by4(rotationZ);
//	angleX = 90;
//	//If values of angle of X/Y/Z axis is non zero cheange respective matrix
//	if (angleX != 0)
//	{
//		rotationX[1][1] = cos(getRadFromDegree(angleX));
//		rotationX[1][2] = -sin(getRadFromDegree(angleX));
//		rotationX[2][1] = sin(getRadFromDegree(angleX));
//		rotationX[2][2] = cos(getRadFromDegree(angleX));
//	}
//	if (angleY != 0)
//	{
//		rotationY[0][0] = cos(getRadFromDegree(angleY));
//		rotationY[0][3] = sin(getRadFromDegree(angleY));
//		rotationY[2][0] = -sin(getRadFromDegree(angleY));
//		rotationY[2][2] = cos(getRadFromDegree(angleY));
//	}
//	angleZ = angleGlobe;
//	if (angleZ != 0)
//	{
//		rotationZ[0][0] = cos(getRadFromDegree(angleZ));
//		rotationZ[0][1] = -sin(getRadFromDegree(angleZ));
//		rotationZ[1][0] = sin(getRadFromDegree(angleZ));
//		rotationZ[1][1] = cos(getRadFromDegree(angleZ));
//	}
//	float tx = 0, ty = 0, tz = 0;
//	//tx = 0, ty = 0, tz = -gz;
//	//Change translation matrix if tx/ty/tz is non zero
//	if (tx != 0) {
//		translationMatrix[0][3] = tx;
//	}
//	if (ty != 0) {
//		translationMatrix[1][3] = ty;
//	}
//	if (tz != 0) {
//		translationMatrix[2][3] = tz;
//	}
//	float sx = 0, sy = 0, sz = 0;
//	sx = -1, sy = 1, sz = 1;
//	////Change scale matrix if tx/ty/tz is non zero
//	if (sx != 0) {
//		scaleMatrix[0][0] = sx;
//	}
//	if (sy != 0) {
//		scaleMatrix[1][1] = sy;
//	}
//	if (sz != 0) {
//		scaleMatrix[2][2] = sz;
//	}
//	float viewMatrix[4][4];
//	float eyeCoordV[3];
//	float targetCoordV[3];
//	float upCoordV[3];
//	float forwardV[3];
//	float leftV[3];
//	float upV[3];
//	makeIdentity4by4(viewMatrix);
//	int viewMatrixFlag = 1;
//	//View Matrix specific changes
//	//http://www.songho.ca/opengl/gl_camera.html
//	//https://www.cs.sfu.ca/~haoz/teaching/htmlman/lookat.html
//	if (viewMatrixFlag) {
//		eyeCoordV[0] = gx;
//		eyeCoordV[1] = gy;
//		eyeCoordV[2] = gz;
//		targetCoordV[0] = 0;
//		targetCoordV[1] = 0;
//		targetCoordV[2] = 0;
//		upCoordV[0] = 0;
//		upCoordV[1] = 1;
//		upCoordV[2] = 0;
//		forwardV[0] = eyeCoordV[0] - targetCoordV[0];
//		forwardV[1] = eyeCoordV[1] - targetCoordV[1];
//		forwardV[2] = eyeCoordV[2] - targetCoordV[2];
//		normalize(forwardV);
//		crossProduct(upCoordV, forwardV, leftV);
//		normalize(leftV);
//		crossProduct(forwardV, leftV, upV);
//		viewMatrix[0][0] = leftV[0];
//		viewMatrix[0][1] = leftV[1];
//		viewMatrix[0][2] = leftV[2];
//		viewMatrix[0][3] = -leftV[0] * eyeCoordV[0] - leftV[1] * eyeCoordV[1] - leftV[2] * eyeCoordV[2];
//		viewMatrix[1][0] = upV[0];
//		viewMatrix[1][1] = upV[1];
//		viewMatrix[1][2] = upV[2];
//		viewMatrix[1][3] = -upV[0] * eyeCoordV[0] - upV[1] * eyeCoordV[1] - upV[2] * eyeCoordV[2];
//		viewMatrix[2][0] = forwardV[0];
//		viewMatrix[2][1] = forwardV[1];
//		viewMatrix[2][2] = forwardV[2];
//		viewMatrix[2][3] = -forwardV[0] * eyeCoordV[0] - forwardV[1] * eyeCoordV[1] - forwardV[2] * eyeCoordV[2];
//	}
//	//Tranlation*RotationX
//	multiplyMatrix4by4(&translationMatrix, &rotationX, &multiply4by4, 4, 4, 4, 4);
//	copyMatrix4by4(&multiply4by4, &temp4by4);
//	// Above * Rotation Y
//	multiplyMatrix4by4(&temp4by4, &rotationY, &multiply4by4, 4, 4, 4, 4);
//	copyMatrix4by4(&multiply4by4, &temp4by4);
//	// Above * Rotation Z
//	multiplyMatrix4by4(&temp4by4, &rotationZ, &multiply4by4, 4, 4, 4, 4);
//	copyMatrix4by4(&multiply4by4, &temp4by4);
//	// Above * Scale
//	multiplyMatrix4by4(&temp4by4, &scaleMatrix, &multiply4by4, 4, 4, 4, 4);
//	copyMatrix4by4(&multiply4by4, &temp4by4);
//	
//	//View *Above
//	multiplyMatrix4by4(&viewMatrix, &temp4by4, &multiply4by4, 4, 4, 4, 4);
//	copyMatrix4by4(&multiply4by4, &temp4by4);
//	
//	// Above * Vertex
////	multiplyMatrix4by1(temp4by4, vertex, multiply4by1, 4, 4, 4, 1);//Commented to test
////	copyMatrix4by1(multiply4by1, temp4by1);
//	float left = 0, right = 0, top = 0, bottom = 0, nearn = 0, farn = 0;
//	float fH = 0, fW = 0, width = 0, height = 0, fov = 0;
//	//float left = -0.1, right = 0.1, top = -0.1, bottom = 1.0, near = 1.0, far = 1000;
//	int gluPerspectiveFlag = 0;
//	gluPerspectiveFlag = 1;
//	//To calulate glFrustrum values from gluPerspective
//	if (gluPerspectiveFlag) {
//		fov = 45;
//		width = (4 * gWidth / 5);
//		height = gHeight;
//		nearn = 0.1;
//		farn = 100.0;
//		perspectiveGL(fov, width / height, nearn, farn, &fW, &fH);
//		left = -fW;
//		right = fW;
//		bottom = -fH;
//		top = fH;
//	}
//	//mat4 result = perspective(45, width / height, near, far);
//	//left = -4.18879;
//	//right = 4.18879;
//	//bottom = -tan(0.3926875)*0.1;
//	//top = tan(0.3926875)*0.1;
//	//near = 0.1;
//	//far = 100.0;
//
//	float perspectiveMatrix[4][4];
//	float orthoMatrix[4][4];
//	int perspectiveFlag = 0;//Perspective 0 ortho 1
//	int orthoFlag = 0;//Perspective 0 ortho 1
//	makeIdentity4by4(orthoMatrix);
//	makeIdentity4by4(perspectiveMatrix);
//	perspectiveMatrix[0][0] = 2 * nearn / (right - left);
//	perspectiveMatrix[0][2] = (right + left) / (right - left);
//	perspectiveMatrix[1][1] = 2 * nearn / (top - bottom);
//	perspectiveMatrix[1][2] = (top + bottom) / (top - bottom);
//	perspectiveMatrix[2][2] = -(farn + nearn) / (farn - nearn);//-1
//	perspectiveMatrix[2][3] = -2 * nearn*farn / (farn - nearn);//-1
//	perspectiveMatrix[3][2] = -1;
//	perspectiveMatrix[3][3] = 0;
//	//Add ortho support
//	//Multiply By Perspective Matrix
//
//
////	multiplyMatrix4by1(perspectiveMatrix, temp4by1, multiply4by1, 4, 4, 4, 1); //Commented to test
////	copyMatrix4by1(multiply4by1, temp4by1);
//	
//	
//	//Perspective*VM
//	multiplyMatrix4by4(&perspectiveMatrix, &temp4by4, &multiply4by4, 4, 4, 4, 4);
//	copyMatrix4by4(&multiply4by4, &temp4by4);
//
//	//Perspective*V*M*Vertex
//	multiplyMatrix4by1(temp4by4, vertex, multiply4by1, 4, 4, 4, 1);
//	copyMatrix4by1(multiply4by1, temp4by1);
//
//	
//	//Calculate Normalized Coordinates
//	float xn = temp4by1[0][0] / temp4by1[3][0];//divide by w
//	float yn = temp4by1[1][0] / temp4by1[3][0];//divide by w
//	float zn = temp4by1[2][0] / temp4by1[3][0];//divide by w
//	float windowCoordinate[3];
//	float ViewPortV[4];
//	//View Port Values
//	ViewPortV[0] = 0;//x
//	ViewPortV[1] = 0;//y
//	ViewPortV[2] = (4 * gWidth / 5);//Width
//	ViewPortV[3] = gHeight;//Height  
//																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																	   //Add matrix for view port
//																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																	   //glViewport(x,y,w,h);
//	windowCoordinate[0] = (xn * 0.5 + 0.5) * ViewPortV[2] + ViewPortV[0];
//	windowCoordinate[1] = (yn * 0.5 + 0.5) * ViewPortV[3] + ViewPortV[1];
//	// Convert to 0.0 to 1.0 range. Anything outside that range gets clipped.
//	windowCoordinate[2] = (1.0 + zn) * 0.5;
//	windowCoordinate[1] = ViewPortV[3] - windowCoordinate[1] - (ViewPortV[3] - RCy);//-1;// -GetSystemMetrics(SM_CYCAPTION);//Because of origin difference in Open GL And Windows                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          // int height1 = (GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION) +GetSystemMetrics(SM_CXPADDEDBORDER));
//	//float viewPortMatrix[4][4];
//	//float normalizedMatrix[4][1];
//	//normalizedMatrix[0][0] = xn;
//	//normalizedMatrix[1][0] = yn;
//	//normalizedMatrix[2][0] = zn;
//	//normalizedMatrix[3][0] = 1;
//	//makeIdentity4by4(viewPortMatrix);
//	//viewPortMatrix[0][0] = ViewPortV[2] / 2;
//	//viewPortMatrix[0][3] = (ViewPortV[2]-1) / 2;
//	//viewPortMatrix[1][1] = ViewPortV[3] / 2;
//	//viewPortMatrix[1][3] = (ViewPortV[3]-1) / 2;
//	//viewPortMatrix[2][2] = 0.5;
//	//viewPortMatrix[2][3] = 0.5;
//	//float windowMatix[4][1];
//	//multiplyMatrix4by1(viewPortMatrix,normalizedMatrix,windowMatix,4,4,4,1);
//	//windowCoordinate[0] = windowMatix[0][0];
//	//windowCoordinate[1] = windowMatix[1][0];
//	//// Convert to 0.0 to 1.0 range. Anything outside that range gets clipped.
//	//windowCoordinate[2] = windowMatix[2][0];
//	//windowCoordinate[1] = ViewPortV[3] - windowCoordinate[1];// -31;//opengl and window origin
//	*wx = windowCoordinate[0];
//	*wy = windowCoordinate[1];
//	*wz = windowCoordinate[2];
//}
//float getRadFromDegree(float Degree) {
//	return(M_PI*Degree / 180);
//}
