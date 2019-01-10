#include<Windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HWND ghwnd = NULL;
bool bFullScreen = false;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev = {sizeof(WINDOWPLACEMENT)};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	HWND hwnd;
	MSG msg;
	WNDCLASSEX wndClass;
	TCHAR szAppName[] = TEXT("FullScreenApplication");

	wndClass.cbClsExtra = 0;
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.cbWndExtra = 0;
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClass.hCursor = LoadCursor(NULL,IDC_ARROW);
	wndClass.hIcon = LoadIcon(NULL,IDI_APPLICATION);
	wndClass.hIconSm = LoadIcon(NULL,IDI_APPLICATION);
	wndClass.hInstance = hInstance;
	wndClass.lpfnWndProc = WndProc;
	wndClass.lpszClassName = szAppName;
	wndClass.lpszMenuName = NULL;
	wndClass.style = CS_VREDRAW | CS_HREDRAW;

	RegisterClassEx(&wndClass);

	hwnd = CreateWindow(szAppName,
		TEXT("Full Screen Window"),
		WS_OVERLAPPEDWINDOW,
		100,
		100,
		800,
		600,
		NULL,
		NULL,
		hInstance,
		NULL
		);

	ghwnd = hwnd;

	ShowWindow(hwnd,iCmdShow);
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
	/*case WM_PAINT:
		HDC hdc;
		PAINTSTRUCT ps;
		RECT rc;
		hdc = BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rc);
		SetBkColor(hdc, RGB(0, 0, 0));
		EndPaint(hwnd, &ps);
		break;
	*/
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 0x46:
			ToggleFullScreen();
			break;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return (DefWindowProc(hwnd,iMsg,wParam,lParam));
}

void ToggleFullScreen(void) {
	MONITORINFO mi;
	if (bFullScreen == false) {
		dwStyle = GetWindowLong(ghwnd,GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi = { sizeof(MONITORINFO) };
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi)) {
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd,
					HWND_TOP,
					mi.rcMonitor.left,
					mi.rcMonitor.top,
					mi.rcMonitor.right-mi.rcMonitor.left,
					mi.rcMonitor.bottom-mi.rcMonitor.top,
					SWP_NOZORDER|SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
		bFullScreen = true;
	
	}
	else {
		SetWindowLong(ghwnd,GWL_STYLE,dwStyle|WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd,&wpPrev);
		SetWindowPos(ghwnd,
			HWND_TOP,
			0,
			0,
			0,
			0,
			SWP_NOZORDER|SWP_FRAMECHANGED|SWP_NOMOVE|SWP_NOSIZE|SWP_NOOWNERZORDER);
		ShowCursor(TRUE);
		bFullScreen = false;
	}
}
