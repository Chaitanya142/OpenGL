#include <Windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow) {
	HWND hwnd;
	MSG msg;
	WNDCLASSEX wndClass;
	TCHAR szAppName[] = TEXT("MyApplication");

	wndClass.cbClsExtra = 0;
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.cbWndExtra = 0;
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClass.hCursor = LoadCursor(NULL,IDC_ARROW);
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hIconSm = LoadIcon(NULL,IDI_APPLICATION);
	wndClass.hInstance = hInstance;
	wndClass.lpfnWndProc = WndProc;
	wndClass.lpszClassName = szAppName;
	wndClass.lpszMenuName = NULL;
	wndClass.style = CS_VREDRAW | CS_HREDRAW;

	RegisterClassEx(&wndClass);

	hwnd = CreateWindow(szAppName,
		TEXT("Window With Message Box"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,NULL,
		hInstance,NULL);

	ShowWindow(hwnd,iCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return(msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd,UINT iMsg,WPARAM wParam,LPARAM lParam) {
	switch (iMsg)
	{
	case WM_CREATE:
		MessageBox(hwnd,TEXT("This Is WM_CREATE Message"),TEXT("My Message"),MB_OK);
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			MessageBox(hwnd,TEXT("This Is Escape Key Is Pressed"),TEXT("My Message"),MB_OK);
			DestroyWindow(hwnd);
			break;
		case 0x46:
			MessageBox(hwnd, TEXT("This Is f Key Is Pressed"), TEXT("My Message"), MB_OK);
			break;
		}
		break;
	case WM_LBUTTONDOWN:
		MessageBox(hwnd, TEXT("This Is Left Mouse Button Click"), TEXT("My Message"), MB_OK);
		break;
	case WM_RBUTTONDOWN:
		MessageBox(hwnd, TEXT("This Is Right Mouse Button Click"), TEXT("My Message"), MB_OK);
		break;
	case WM_DESTROY:
		MessageBox(hwnd, TEXT("This Is WM_DESTROY Message"), TEXT("My Message"), MB_OK);
		PostQuitMessage(0);
		break;
	}
	return (DefWindowProc(hwnd,iMsg,wParam,lParam));
}
