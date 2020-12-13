#include"Source.h"
#include"Font.h"
#ifndef UNICODE  
typedef std::string String;
#else
typedef std::wstring String;
#endif

//Method Declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

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
	TCHAR szAppName[] = TEXT("RTR");
	int iRet = 0;
	bool bDone = false;
	//Code
	if (fopen_s(&gpFile, "Log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Log File Can't Be Created"), TEXT("ERROR"), MB_OK);
		exit(0);
	}
	else {
		fprintf_s(gpFile, "Log File Created Successfully\n");
	}

	if (fopen_s(&gpFileForFPS, "LogForFPS.txt", "w") != 0)
	{
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
		TEXT("RTR"),
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
			
			if (updateCallPerSecond[int(deltaTime / 16.66)] == false) {
				Update();
				updateCallPerSecond[(int)(deltaTime / 16.66)] = true;
				ActualFPS++;
			}

			Display();
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
		//ShowCursor(TRUE);
		ShowCursor(FALSE);
		bFullScreen = false;
	}
}

//Initialize OpenGl
int Initialize(void) {
	//Function Declaration
	void Resize(int, int);
	void UnInitialize(void);

	showLoading();

	TCHAR szModName[256];
	GetModulePath(szModName, 256);
	string str(szModName);
	fprintf_s(gpFile, str.c_str(), sizeof(str));

	//char modulePath[255];
	strcpy(modulePath, str.c_str());

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

	for (int i = 0; i < 60; i++) {
		updateCallPerSecond[i] = false;
	}

	//----------------------------------------------------------------------------------------------------------//
	LoadTexture();
	//----------------------------------------------------------------------------------------------------------//
	vector<std::string> faces
	{
		("right.jpg"),
		("left.jpg"),
		("top.jpg"),
		("bottom.jpg"),
		("front.jpg"),
		("back.jpg")
	};
	textures.push_back(loadCubemapTexture(faces));
	//----------------------------------------------------------------------------------------------------------//
	loadktxTextures(modulePath);
	//----------------------------------------------------------------------------------------------------------//
	faces.clear();
	faces.push_back("Aditi.png");
	textures.push_back(loadTextureUsingSOIL(faces));

	faces.clear();
	faces.push_back("Amita.png");
	textures.push_back(loadTextureUsingSOIL(faces));

	faces.clear();
	faces.push_back("Gayatri.png");
	textures.push_back(loadTextureUsingSOIL(faces));

	faces.clear();
	faces.push_back("Chaitanya.png");
	textures.push_back(loadTextureUsingSOIL(faces));

	faces.clear();
	faces.push_back("Rain.jpg");
	textures.push_back(loadTextureUsingSOIL(faces));

	//----------------------------------------------------------------------------------------------------------//
	objSceneNight = new SceneNight();
	objSceneNight->InitScene();
	//----------------------------------------------------------------------------------------------------------//
	objSceneLeaf = new SceneLeaf();
	objSceneLeaf->InitScene();
	//
	//----------------------------------------------------------------------------------------------------------//
	objSceneHearts = new SceneHearts();
	objSceneHearts->InitScene();
	//----------------------------------------------------------------------------------------------------------//
	objSceneStarField = new SceneStarField();
	objSceneStarField->InitScene();
	//----------------------------------------------------------------------------------------------------------//
	objSceneCubemap = new SceneCubemap();
	objSceneCubemap->InitScene();
	//----------------------------------------------------------------------------------------------------------//
	objSceneCloth = new SceneCloth();
	objSceneCloth->InitScene();
	//----------------------------------------------------------------------------------------------------------//
	objSceneCouple = new SceneCouple();
	objSceneCouple->InitScene();
	//----------------------------------------------------------------------------------------------------------//
	objSceneTerrain = new SceneTerrain();
	objSceneTerrain->InitScene();
	//----------------------------------------------------------------------------------------------------------//
	objSceneDay = new SceneDay();
	objSceneDay->InitScene();
	//InitDayScene();

	/////////////////////////////////////////////////// Fonts ///////////////////////////////////////////////////

	//int retval = InitLetter(modulePath);
	objSceneAMC = new SceneAMC();
	objSceneAMC->InitScene();

	//----------------------------------------------------------------------------------------------------------//

	objSceneComputer.InitScene();
	//----------------------------------------------------------------------------------------------------------//
	objSceneStudyRoom = new SceneStudyRoom();
	objSceneStudyRoom->InitScene();
	//----------------------------------------------------------------------------------------------------------//
	objSceneRTR = new SceneRTR();
	objSceneRTR->InitScene();
	//----------------------------------------------------------------------------------------------------------//
	objSceneCollage = new SceneCollage();
	objSceneCollage->InitScene();
	//----------------------------------------------------------------------------------------------------------//
	objSceneEndCredits = new SceneEndCredits();
	objSceneEndCredits->InitScene();
	//----------------------------------------------------------------------------------------------------------//

	//obj->InitScene1();

	ModelLoading(modulePath, "\\Resources\\tree\\tree.obj");

	ModelLoading(modulePath, "\\Resources\\Rain\\rock.obj");

	ModelLoading(modulePath, "\\Resources\\CartoonChair\\Animation_Cartoon_ForChair.obj");

	ModelLoading(modulePath, "\\Resources\\CartoonChair\\chair_made_of_metal_and_wood.obj");

	ModelLoading(modulePath, "\\Resources\\Heart.obj");

	AnimationLoading(modulePath, 300);

	//----------------------------------------------------------------------------------------------------------//
	objSceneRain = new SceneRain();
	objSceneRain->InitScene();
	//----------------------------------------------------------------------------------------------------------//
	objSceneRipple = new SceneRipple();
	objSceneRipple->InitScene();

	//----------------------------------------------------------------------------------------------------------//
	objSceneInterop = new SceneInterop();
	objSceneInterop->InitScene();
	//----------------------------------------------------------------------------------------------------------//
	InitAudio(modulePath);
	//----------------------------------------------------------------------------------------------------------//
	sceneFeatures = new Font();
	sceneFeatures->InitFont();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_TEXTURE_2D);

	SetWindowText(ghwnd, TEXT("RTR"));//Remove Loading Text
	SetCursor(LoadCursor(NULL, IDC_ARROW));//Set Normal Cursor

	Resize(WIN_WIDTH, WIN_HEIGHT);
	return 0;
}

void ModelLoading(char modulePath[255], const char modelPath[255])
{
	//Model Loadings
	char  char_array[255];
	strcpy(char_array, modulePath);
	strcat(char_array, modelPath);
	Model model(char_array);
	models.push_back(model);
}

//Function Resize
void Resize(int width, int height) {
	if (height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	gWidth = (float)width;
	gHeight = (float)height;
}

double clockToMilliseconds(clock_t ticks) {
	return (ticks / (double)CLOCKS_PER_SEC)*1000.0;
}
double clockToseconds(clock_t ticks) {
	return ((ticks / (double)CLOCKS_PER_SEC));
}

unsigned int frames = 0;
double  averageFrameTimeMilliseconds = 16.666;
clock_t lastTimeFrame = 0;

int firstCall = 1;

//Function Display
void Display(void) {

	if (firstCall == 1) {
		ToggleFullScreen();
		Sleep(5000);

		alSourcePlay(audioSourceid);

		lastTimeFrame = clock();
		firstCall = 0;
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	switch (scene)
	{
	case Scene_AstroMediComp:
		objSceneAMC->DisplayScene();
		sceneFeatures->DisplayText("Font Rendering", 0.0f, 0.0f, vec3(1.0, 1.0f, 1.0f));
		break;
	case Scene_Leaf:
		objSceneLeaf->DisplayScene();
		sceneFeatures->DisplayText("Point Sprites", 0.0f, 0.0f, vec3(1.0,1.0f,0.0f));
		break;
	case Scene_Star:
		objSceneStarField->DisplayScene();
		sceneFeatures->DisplayText("Particle System",0.0f, 0.0f, vec3(1.0, 1.0f, 1.0f));
		break;
	case Scene_Night:
		objSceneNight->DisplayScene();
		sceneFeatures->DisplayText("Depth of Field using Compute Shader", -0.1f, 0.0f, vec3(1.0, 1.0f, 1.0f));
		break;
	case Scene_Couple:
		objSceneCouple->DisplayScene();
		break;
	case Scene_Hearts:
		camera.ResetCamera();
		objSceneHearts->DisplayScene();
		sceneFeatures->DisplayText("Point Sprites using Particle System", -0.1f, 0.0f, vec3(1.0, 0.0f, 0.0f));
		break;
	case Scene_CubeMap:
		objSceneCubemap->DisplayScene();
		sceneFeatures->DisplayText("Cubemap and Glass Rendering", 0.0f, 0.0f, vec3(0.5, 0.5f, 0.5f));
		break;
	case Scene_Flag:
		objSceneCloth->DisplayScene();
		sceneFeatures->DisplayText("Cloth Rendering using Compute Shader", -0.15f, 0.0f, vec3(1.0, 1.0f, 1.0f));
		break;
	case Scene_Day:
		camera.ResetCamera();
		objSceneDay->DisplayScene();
		sceneFeatures->DisplayText("Instancing and Noise", 0.0f, 0.0f, vec3(1.0, 1.0f, 1.0f));
		break;
	case Scene_Terrain:
		objSceneTerrain->DisplayScene();
		sceneFeatures->DisplayText("Terrain and Geometry Shader", -0.12f, 0.0f, vec3(0.0, 0.0f, 0.0f));
		break;
	case Scene_FrontText:
		objSceneComputer.SetSceneNo(0);
		objSceneComputer.DisplayScene();
		objSceneComputer.DisplayModelAndChair();
		break;
	case Scene_StudyRoom:
		objSceneStudyRoom->DisplayScene();
		DrawModelStudyRoom();
		sceneFeatures->DisplayText("Model Loading", 0.0f, 0.0f, vec3(1.0, 1.0f, 1.0f));
		break;
	case Scene_LastText:
		objSceneComputer.SetSceneNo(1);
		objSceneComputer.DisplayScene();
		objSceneComputer.DisplayModelAndChair();
		break;
	case Scene_RTR:
		objSceneRTR->DisplayScene();
		sceneFeatures->DisplayText("Bloom Effect", -0.2f, 0.0f, vec3(1.0, 1.0f, 1.0f));
		break;
	case Scene_Rain:
		objSceneRain->DisplayScene();
		sceneFeatures->DisplayText("Rain using Instance Matrix", 0.0f, 0.0f, vec3(1.0, 1.0f, 1.0f));
		DrawModelRain();
		break;
	case Scene_Collage:
		objSceneCollage->DisplayScene();
		break;
	case Scene_Ripple:
		objSceneRipple->DisplayScene();
		break;
	case Scene_Interop:
		objSceneInterop->DisplayScene();
		sceneFeatures->DisplayText("OpenCL-OpenGL Interoperability", -0.1f, 0.0f, vec3(1.0, 1.0f, 1.0f));
		break;
	case Scene_Assignment:
		((SceneAMC*)objSceneAMC)->changeToAssignment();
		objSceneAMC->DisplayScene();
		break;
	case Scene_EndCredits:
		objSceneEndCredits->DisplayScene();
		//objSceneEndCredits->UpdateScene();
		break;
	default:
		break;
	}
	
	glUseProgram(0);
	SwapBuffers(ghdc);

	clock_t endFrame = clock();

	currentFrameTime = endFrame - lastTimeFrame;

	deltaTime += currentFrameTime;
	gTimeInMS += deltaTime;

	lastTimeFrame = endFrame;

	UpdateScene();

	frames++;

	if (clockToMilliseconds(deltaTime) > 1000.0) { //every second
		gSec++;

		char buffer[10];
		_itoa(scene, buffer, 10);
		fprintf(gpFileForFPS, buffer);
		fprintf(gpFileForFPS, "\t");

		_itoa(ActualFPS, buffer, 10);
		fprintf(gpFileForFPS, buffer);
		fprintf(gpFileForFPS, "\n");

		ActualFPS = 0;
		for (int i = 0; i < FPS; i++)
		{
			updateCallPerSecond[i] = false;
		}

		frameRate = (double)frames*0.5 + frameRate * 0.5; //more stable
		frames = 0;
		deltaTime -= CLOCKS_PER_SEC;
		//averageFrameTimeMilliseconds = 1000.0 / (frameRate == 0 ? 0.001 : frameRate);
	}
}

void UpdateScene()
{
	if (gSec >= 0 && gSec < 13) { scene = Scene_AstroMediComp; }
	else if (gSec >= 13 && gSec < 17) { scene = Scene_Leaf; }
	else if (gSec >= 17 && gSec < 30) { scene = Scene_StudyRoom; }
	else if (gSec >= 30 && gSec < 52) { scene = Scene_FrontText; }
	else if (gSec >= 52 && gSec < 55) { scene = Scene_Star; }
	else if (gSec >= 55 && gSec < 61) { scene = Scene_Night; }
	else if (gSec >= 61 && gSec < 90) { scene = Scene_Couple; }
	else if (gSec >= 90 && gSec < 95) { scene = Scene_Hearts; }
	else if (gSec >= 95 && gSec < 101) { scene = Scene_Terrain; }
	else if (gSec >= 101 && gSec < 110) { scene = Scene_Day; }
	else if (gSec >= 110 && gSec < 132) { scene = Scene_CubeMap; }
	else if (gSec >= 132 && gSec < 143) { scene = Scene_Rain; }
	else if (gSec >= 143 && gSec < 149) { scene = Scene_Assignment; }
	else if (gSec >= 149 && gSec < 158) { scene = Scene_LastText; }
	else if (gSec >= 158 && gSec < 180) { scene = Scene_FrontText; }
	else if (gSec >= 180 && gSec < 191) { scene = Scene_Interop; }
	else if (gSec >= 191 && gSec < 195) { scene = Scene_RTR; }
	else if (gSec >= 195 && gSec < 199) { scene = Scene_Collage; }
	else if (gSec >= 199 && gSec < 220) { scene = Scene_Flag; }
	else if (gSec >= 220) { scene = Scene_EndCredits; }
}

void Update(void)
{
	switch (scene)
	{
	case Scene_AstroMediComp:
		objSceneAMC->UpdateScene();
		break;
	case Scene_Leaf:
		objSceneLeaf->UpdateScene();
		break;
	case Scene_Star:
		objSceneStarField->UpdateScene();
		break;
	case Scene_Night:
		objSceneNight->UpdateScene();
		break;
	case Scene_Couple:
		objSceneCouple->UpdateScene();
		break;
	case Scene_Hearts:
		objSceneHearts->UpdateScene();
		break;
	case Scene_Day:
		objSceneDay->UpdateScene();
		break;
	case Scene_StudyRoom:
		UpdateModelStudyRoom();
		break;
	case Scene_FrontText:
		objSceneComputer.UpdateScene();
		break;
	case Scene_LastText:
		objSceneComputer.UpdateScene();
		break;
	case Scene_Rain:
		objSceneRain->UpdateScene();
		UpdateModelRain();
		break;
	case Scene_Collage:
		objSceneCollage->UpdateScene();
		break;
	case Scene_Ripple:
		objSceneRipple->UpdateScene();
		break;
	case Scene_CubeMap:
		objSceneCubemap->UpdateScene();
		break;
	case Scene_Interop:
		objSceneInterop->UpdateScene();
		break;
	case Scene_Assignment:
		objSceneAMC->UpdateScene();
		break;
	case Scene_Terrain:
		objSceneTerrain->UpdateScene();									 
		break;
	case Scene_EndCredits:
		objSceneEndCredits->UpdateScene();
		break;
	default:
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

	uninitializeAudio();

	objSceneRipple->UninitializeScene();
	objSceneAMC->UninitializeScene();
	objSceneCloth->UninitializeScene();
	objSceneCollage->UninitializeScene();
	objSceneComputer.UninitializeScene();
	objSceneCouple->UninitializeScene();
	objSceneCubemap->UninitializeScene();
	objSceneHearts->UninitializeScene();
	objSceneLeaf->UninitializeScene();
	objSceneNight->UninitializeScene();
	objSceneRain->UninitializeScene();
	objSceneRTR->UninitializeScene();
	objSceneStarField->UninitializeScene();
	objSceneStudyRoom->UninitializeScene();
	objSceneTerrain->UninitializeScene();
	objSceneDay->UninitializeScene();
	objSceneInterop->UninitializeScene();
	objSceneEndCredits->UninitializeScene();
	/*for (int count = 0; count < shaders.size; count++) {
		unsigned int * id = &(shaders.at(count).ID);
		DeleteShaderProgram(id);
	}*/
	shaders.clear();
	models.clear();
	textures.clear();

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
	if (gpFileForFPS) {
		fprintf_s(gpFileForFPS, "Log File Closed Successfully");
		fclose(gpFileForFPS);
		gpFileForFPS = NULL;
	}
}

void InitAudio(char modulePath[255]) {
	char  char_array[255];
	strcpy(char_array, modulePath);
	strcat(char_array, "\\Resources\\Man_mix.wav");

	////////////////////////////
	/*AUDIO*/
	audioData = loadWAV(char_array, audioChannel, audioSampleRate, audiobps, audioSize);
	audioDevice = alcOpenDevice(NULL);
	if (audioDevice == NULL)
	{
		std::cout << "cannot open sound card" << std::endl;
		printf("failed to open sound card, Exitting!!!\n");
		//uninitialize();
		exit(0);
	}
	audioContext = alcCreateContext(audioDevice, NULL);
	if (audioContext == NULL)
	{
		std::cout << "cannot open device context" << std::endl;
		printf("can not open sound context, Exitting!!!\n");
		//uninitialize();
		exit(0);
	}
	alcMakeContextCurrent(audioContext);

	alGenBuffers(1, &audioBufferid);
	if (audioChannel == 1)
	{
		if (audiobps == 8)
		{
			audioFormat = AL_FORMAT_MONO8;
		}
		else {
			audioFormat = AL_FORMAT_MONO16;
		}
	}
	else {
		if (audiobps == 8)
		{
			audioFormat = AL_FORMAT_STEREO8;
		}
		else {
			audioFormat = AL_FORMAT_STEREO16;
		}
	}
	alBufferData(audioBufferid, audioFormat, audioData, audioSize, audioSampleRate);

	alGenSources(1, &audioSourceid);
	alSourcei(audioSourceid, AL_BUFFER, audioBufferid);
	//alSourcePlay(sourceid);
	////////////////////////////
}

void uninitializeAudio(void)
{
	alDeleteSources(1, &audioSourceid);
	alDeleteBuffers(1, &audioBufferid);

	alcDestroyContext(audioContext);
	alcCloseDevice(audioDevice);
	delete[] audioData;
}

void showLoading() {

	if (loadingCount == 0)
		SetWindowText(ghwnd, TEXT("RTR Loading"));
	if (loadingCount == 1)
		SetWindowText(ghwnd, TEXT("RTR Loading."));
	if (loadingCount == 2)
		SetWindowText(ghwnd, TEXT("RTR Loading.."));
	if (loadingCount == 3)
		SetWindowText(ghwnd, TEXT("RTR Loading..."));
	if (loadingCount == 4)
		SetWindowText(ghwnd, TEXT("RTR Loading...."));
	if (loadingCount == 5)
		SetWindowText(ghwnd, TEXT("RTR Loading....."));
	if (loadingCount == 6)
		SetWindowText(ghwnd, TEXT("RTR Loading......"));
	if (loadingCount == 7)
		SetWindowText(ghwnd, TEXT("RTR Loading......."));
	if (loadingCount == 8)
		SetWindowText(ghwnd, TEXT("RTR Loading........"));
	if (loadingCount == 9)
		SetWindowText(ghwnd, TEXT("RTR Loading........."));

	loadingCount++;

	if (loadingCount == 10)
		loadingCount = 0;
}

