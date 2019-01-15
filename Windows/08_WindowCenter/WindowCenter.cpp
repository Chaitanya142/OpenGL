#include<Windows.h>

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	HWND hwnd;
	MSG msg;
	WNDCLASSEX wndClass;
	TCHAR szAppName[] = TEXT("MyApplication");

	wndClass.cbClsExtra = 0;
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.cbWndExtra = 0;
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hInstance = hInstance;
	wndClass.lpfnWndProc = WndProc;
	wndClass.lpszClassName = szAppName;
	wndClass.lpszMenuName = NULL;
	wndClass.style = CS_VREDRAW | CS_HREDRAW;

	RegisterClassEx(&wndClass);

	MONITORINFO mi;
	mi = { sizeof(MONITORINFO) };
	GetMonitorInfo(MonitorFromWindow(NULL, MONITORINFOF_PRIMARY), &mi);

	hwnd = CreateWindow(szAppName,
		TEXT("Center Window"),
		WS_OVERLAPPEDWINDOW,
		((mi.rcMonitor.right-mi.rcMonitor.left)/2)-(WIN_WIDTH/2),
		((mi.rcMonitor.bottom-mi.rcMonitor.top)/2)-(WIN_HEIGHT/2),
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return(msg.wParam);

}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	void ToggleFullScreen(void);
	switch (iMsg) {
		
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return (DefWindowProc(hwnd, iMsg, wParam, lParam));
}

