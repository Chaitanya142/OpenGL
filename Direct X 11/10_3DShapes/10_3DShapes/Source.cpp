#include<windows.h>
#include<stdio.h>//for File I/O
#include <string>

#include<d3d11.h>
#pragma comment(lib,"d3d11.lib")
#include<d3dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")

#pragma warning(disable:4838)
#include"XNAMath\xnamath.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void WriteToFile(std::string str);

FILE *gpFile = NULL;

char gszLogFileName[] = "Log.txt";

HWND ghwnd;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

bool gbActiveWindow = false;
bool gbEscapeKeyIsPressed = false;
bool gbFullScreen = false;

float gClearColor[4];
IDXGISwapChain *gpIDXGISwapChain = NULL;
ID3D11Device *gpID3D11Device = NULL;
ID3D11DeviceContext *gpID3D11DeviceContext = NULL;
ID3D11RenderTargetView *gpID3D11RenderTargetView = NULL;

ID3D11VertexShader *gpID3D11VertexShader = NULL;
ID3D11PixelShader *gpID3D11PixelShader = NULL;

ID3D11Buffer *gpID3D11Buffer_VertexBufferPositionPyramid = NULL;
ID3D11Buffer *gpID3D11Buffer_VertexBufferColorPyramid = NULL;

ID3D11Buffer *gpID3D11Buffer_VertexBufferPositionCube = NULL;
ID3D11Buffer *gpID3D11Buffer_VertexBufferColorCube = NULL;

ID3D11InputLayout *gpID3D11InputLayout = NULL;
ID3D11Buffer *gpID3D11Buffer_ConstantBuffer = NULL;

ID3D11RasterizerState *gpID3D11RasterizerState = NULL;
ID3D11DepthStencilView *gpID3D11DepthStencilView = NULL;

float anglePyramid = 0;
float angleCube = 0;


void update();

struct CBUFFER {
	XMMATRIX WorldViewProjectionMatrix;
};

XMMATRIX gPerspectiveProjectionMatrix;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	HRESULT initialize(void);
	void uninitialize(void);
	void display(void);

	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szClassName[] = TEXT("Direct3D11");

	bool bDone = false;
	if (fopen_s(&gpFile, gszLogFileName, "w") != 0) {
		MessageBox(NULL, TEXT("Log File Cannot Be Created\n"), TEXT("Error"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
		exit(0);
	}
	else {
		fprintf_s(gpFile, "Log File Is Successfully Opened.\n");
		fclose(gpFile);
	}

	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = szClassName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassEx(&wndclass);

	hwnd = CreateWindow(szClassName,
		TEXT("Direct3D11Window"),
		WS_OVERLAPPEDWINDOW,
		100,
		100,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;

	ShowWindow(hwnd, iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	HRESULT hr;
	hr = initialize();
	if (FAILED(hr)) {
		WriteToFile("Initialize() Failed. Exiting Now...\n");
		DestroyWindow(hwnd);
		hwnd = NULL;
	}
	else {
		WriteToFile("Initialize() Succeeded.\n");
	}

	while (bDone == false) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT)
				bDone = true;
			else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else {
			display();
			if (gbActiveWindow == true) {
				if (gbEscapeKeyIsPressed == true) {
					bDone = true;
				}

				update();
			}
		}
	}

	uninitialize();

	return((int)msg.wParam);

}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	HRESULT resize(int, int);
	void ToggleFullScreen(void);
	void uninitialize(void);

	HRESULT hr;

	switch (iMsg)
	{
	case WM_ACTIVATE:
		if (HIWORD(wParam) == 0)
			gbActiveWindow = true;
		else
			gbActiveWindow = false;
		break;
	case WM_ERASEBKGND:
		return(0);
	case WM_SIZE:
		if (gpID3D11DeviceContext) {
			hr = resize(LOWORD(lParam), HIWORD(lParam));
			if (FAILED(hr)) {
				WriteToFile("Resize() Failed.\n");
				return(hr);
			}
			else {
				WriteToFile("Resize() Succeeded.\n");
			}
		}
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			if (gbEscapeKeyIsPressed == false)
				gbEscapeKeyIsPressed = true;
			break;
		case 0x46:
			if (gbFullScreen == false) {
				ToggleFullScreen();
				gbFullScreen = true;
			}
			else {
				ToggleFullScreen();
				gbFullScreen = false;
			}
			break;
		default:
			break;
		}
		break;
	case WM_LBUTTONDOWN:
		break;
	case WM_CLOSE:
		uninitialize();
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		break;
	}
	return (DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void ToggleFullScreen(void)
{
	MONITORINFO mi;
	if (gbFullScreen == false) {
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi = { sizeof(MONITORINFO) };
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left,
					mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left,
					mi.rcMonitor.bottom - mi.rcMonitor.top,
					SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
	}
	else {
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle & WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}
}

HRESULT initialize() {
	void uninitialize(void);
	HRESULT resize(int, int);

	HRESULT hr;

	D3D_DRIVER_TYPE d3dDriverType;
	D3D_DRIVER_TYPE d3dDriverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP,D3D_DRIVER_TYPE_REFERENCE
	};
	D3D_FEATURE_LEVEL d3dFeatureLevel_requred = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL d3dFeatureLevel_acquired = D3D_FEATURE_LEVEL_10_0;

	UINT createDeviceFlags = 0;
	UINT numDriverTypes = 0;
	UINT numFeatureLevels = 1;


	numDriverTypes = sizeof(d3dDriverTypes) / sizeof(d3dDriverTypes[0]);

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	ZeroMemory((void *)&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	dxgiSwapChainDesc.BufferCount = 1;
	dxgiSwapChainDesc.BufferDesc.Width = WIN_WIDTH;
	dxgiSwapChainDesc.BufferDesc.Height = WIN_HEIGHT;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 0;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.OutputWindow = ghwnd;
	dxgiSwapChainDesc.SampleDesc.Count = 1;
	dxgiSwapChainDesc.SampleDesc.Quality = 0;
	dxgiSwapChainDesc.Windowed = TRUE;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		d3dDriverType = d3dDriverTypes[driverTypeIndex];

		hr = D3D11CreateDeviceAndSwapChain(
			NULL,
			d3dDriverType,
			NULL,
			createDeviceFlags,
			&d3dFeatureLevel_requred,
			numFeatureLevels,
			D3D11_SDK_VERSION,
			&dxgiSwapChainDesc,
			&gpIDXGISwapChain,
			&gpID3D11Device,
			&d3dFeatureLevel_acquired,
			&gpID3D11DeviceContext
		);

		if (SUCCEEDED(hr))
			break;

	}
	if (FAILED(hr))
	{
		WriteToFile("D3D11CreateDeviceAndSwapChain() Failed.\n");
		return hr;
	}
	else {
		WriteToFile("D3D11CreateDeviceAndSwapChain() Succeeded.\n");
		WriteToFile("The Choosed Driver is Of ");
		if (d3dDriverType == D3D_DRIVER_TYPE_HARDWARE)
			WriteToFile("Hardware Type.\n");
		else if (d3dDriverType == D3D_DRIVER_TYPE_WARP)
			WriteToFile("WRAP Type.\n");
		else if (d3dDriverType == D3D_DRIVER_TYPE_REFERENCE)
			WriteToFile("Reference Type.\n");
		else
			WriteToFile("Unknown Type.\n");

		WriteToFile("The Supported Highest Feature Level Is ");

		if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_11_0)
			WriteToFile("11.0\n");
		else if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_10_1)
			WriteToFile("10.1\n");
		else if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_10_0)
			WriteToFile("10.0\n");
		else
			WriteToFile("Unknown\n");
	}

	const char *vertexShaderSouceCode =
		"cbuffer ConstantBuffer"\
		"{"\
		"float4x4 worldViewProjectionMatrix;"\
		"}"\
		"struct vertex_output"\
		"{"\
		"float4 position : SV_POSITION;"\
		"float4 color : COLOR;"\
		"};"\
		"vertex_output main(float4 pos : POSITION,float4 color:COLOR)"\
		"{"\
		"vertex_output output;"\
		"output.position=mul(worldViewProjectionMatrix,pos);"\
		"output.color=color;"\
		"return(output);"\
		"}";

	ID3DBlob *pID3DBlob_VertexShaderCode = NULL;
	ID3DBlob *pID3DBlob_Error = NULL;

	hr = D3DCompile(vertexShaderSouceCode,
		lstrlenA(vertexShaderSouceCode) + 1,
		"VS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"vs_5_0",
		0,
		0,
		&pID3DBlob_VertexShaderCode,
		&pID3DBlob_Error
	);

	if (FAILED(hr))
	{
		if (pID3DBlob_Error != NULL)
		{
			fopen_s(&gpFile, gszLogFileName, "a+");
			fprintf_s(gpFile, "D3DCompile() Failed For Vertex Shader : %s.\n", (char*)pID3DBlob_Error->GetBufferPointer());
			fclose(gpFile);
			//WriteToFile("D3DCompile() Failed For Vertex Shader : %s.\n"+(char*)pID3DBlob_Error->GetBufferPointer());
			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
			return(hr);
		}
	}
	else {
		WriteToFile("D3DCompile() Succeeded For Vertex Shader.\n");
	}

	hr = gpID3D11Device->CreateVertexShader(pID3DBlob_VertexShaderCode->GetBufferPointer(),
		pID3DBlob_VertexShaderCode->GetBufferSize(),
		NULL,
		&gpID3D11VertexShader
	);

	if (FAILED(hr))
	{
		WriteToFile("ID3D11Device::CreateVertexShader() Failed .\n");
		return(hr);
	}
	else {
		WriteToFile("ID3D11Device::CreateVertexShader() Succeeded.\n");
	}

	gpID3D11DeviceContext->VSSetShader(gpID3D11VertexShader, 0, 0);

	const char * pixelShaderSourceCode =
		"float4 main(float4 pos:SV_POSITION,float4 color:COLOR) : SV_TARGET"\
		"{"\
		"return(color);"\
		"}";

	ID3DBlob *pID3DBlob_PixelShaderCode = NULL;
	pID3DBlob_Error = NULL;

	hr = D3DCompile(pixelShaderSourceCode,
		lstrlenA(pixelShaderSourceCode) + 1,
		"PS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"ps_5_0",
		0,
		0,
		&pID3DBlob_PixelShaderCode,
		&pID3DBlob_Error
	);

	if (FAILED(hr))
	{
		if (pID3DBlob_Error != NULL)
		{
			fopen_s(&gpFile, gszLogFileName, "a+");
			fprintf_s(gpFile, "D3DCompile() Failed For Pixel Shader : %s.\n", (char*)pID3DBlob_Error->GetBufferPointer());
			fclose(gpFile);
			//WriteToFile("D3DCompile() Failed For Vertex Shader : %s.\n"+(char*)pID3DBlob_Error->GetBufferPointer());
			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
			return(hr);
		}
	}
	else {
		WriteToFile("D3DCompile() Succeeded For Pixel Shader.\n");
	}

	hr = gpID3D11Device->CreatePixelShader(pID3DBlob_PixelShaderCode->GetBufferPointer(),
		pID3DBlob_PixelShaderCode->GetBufferSize(),
		NULL,
		&gpID3D11PixelShader
	);

	if (FAILED(hr))
	{
		WriteToFile("ID3D11Device::CreatePixelShader() Failed .\n");
		return(hr);
	}
	else {
		WriteToFile("ID3D11Device::CreatePixelShader() Succeeded.\n");
	}

	gpID3D11DeviceContext->PSSetShader(gpID3D11PixelShader, 0, 0);
	pID3DBlob_PixelShaderCode->Release();
	pID3DBlob_PixelShaderCode = NULL;

	float vertices_Pyramid[] = {
		0.0f,1.0f,0.0f,
		-1.0f,-1.0f,1.0f,
		1.0f,-1.0f,1.0f,

		0.0f,1.0f,0.0f,
		1.0f,-1.0f,1.0f,
		1.0f,-1.0f,-1.0f,

		0.0f,1.0f,0.0f,
		1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,

		0.0f,1.0f,0.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,1.0f
	};

	float colors_Pyramid[] = {
		1.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,0.0f,1.0f,

		1.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,0.0f,1.0f,

		1.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,0.0f,1.0f,

		1.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,0.0f,1.0f,
	};


	D3D11_BUFFER_DESC bufferDescPosition;
	ZeroMemory(&bufferDescPosition, sizeof(D3D11_BUFFER_DESC));

	bufferDescPosition.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescPosition.ByteWidth = sizeof(float)*ARRAYSIZE(vertices_Pyramid);
	bufferDescPosition.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDescPosition.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = gpID3D11Device->CreateBuffer(&bufferDescPosition, NULL,
		&gpID3D11Buffer_VertexBufferPositionPyramid);

	if (FAILED(hr)) {
		WriteToFile("ID3D11Device::CreateBuffer() Failed For Vertex Buffer.\n");
		return(hr);
	}
	else {
		WriteToFile("ID3D11Device::CreateBuffer() Succeeded For Vertex Buffer.\n");
	}

	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBufferPositionPyramid, 0,
		D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	memcpy(mappedSubresource.pData, vertices_Pyramid, sizeof(vertices_Pyramid));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBufferPositionPyramid, NULL);

	D3D11_BUFFER_DESC bufferDescColor;
	ZeroMemory(&bufferDescColor, sizeof(D3D11_BUFFER_DESC));

	bufferDescColor.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescColor.ByteWidth = sizeof(float)*ARRAYSIZE(colors_Pyramid);
	bufferDescColor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDescColor.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = gpID3D11Device->CreateBuffer(&bufferDescColor, NULL,
		&gpID3D11Buffer_VertexBufferColorPyramid);

	if (FAILED(hr)) {
		WriteToFile("ID3D11Device::CreateBuffer() Failed For Color Buffer.\n");
		return(hr);
	}
	else {
		WriteToFile("ID3D11Device::CreateBuffer() Succeeded For Color Buffer.\n");
	}

	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBufferColorPyramid, 0,
		D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	memcpy(mappedSubresource.pData, colors_Pyramid, sizeof(colors_Pyramid));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBufferColorPyramid, NULL);





	float vertices_Cube[] = {
		//top - triangle 1
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,

		//triangle 2
		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,

		//bottom - triangle 1
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,

		//triangle 2
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		//front - triangle 1
		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,

		//triangle 2
		-1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		//back - triangle 1
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		//triangle 2
		-1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,

		//left - triangle 1
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,

		//triangle 2
		-1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,

		//right - triangle 1
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,

		//triangle 2
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f
	};

	float colors_Cube[] = {
		1.0f,0.0f,0.0f,
		1.0f,0.0f,0.0f,
		1.0f,0.0f,0.0f,

		1.0f,0.0f,0.0f,
		1.0f,0.0f,0.0f,
		1.0f,0.0f,0.0f,

		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f,

		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,1.0f,0.0f,

		0.0f,0.0f,1.0f,
		0.0f,0.0f,1.0f,
		0.0f,0.0f,1.0f,

		0.0f,0.0f,1.0f,
		0.0f,0.0f,1.0f,
		0.0f,0.0f,1.0f,

		1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f,

		1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f,

		0.0f,1.0f,1.0f,
		0.0f,1.0f,1.0f,
		0.0f,1.0f,1.0f,

		0.0f,1.0f,1.0f,
		0.0f,1.0f,1.0f,
		0.0f,1.0f,1.0f,

		1.0f,0.0f,1.0f,
		1.0f,0.0f,1.0f,
		1.0f,0.0f,1.0f,

		1.0f,0.0f,1.0f,
		1.0f,0.0f,1.0f,
		1.0f,0.0f,1.0f,

	};

	bufferDescPosition;
	ZeroMemory(&bufferDescPosition, sizeof(D3D11_BUFFER_DESC));

	bufferDescPosition.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescPosition.ByteWidth = sizeof(float)*ARRAYSIZE(vertices_Cube);
	bufferDescPosition.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDescPosition.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = gpID3D11Device->CreateBuffer(&bufferDescPosition, NULL,
		&gpID3D11Buffer_VertexBufferPositionCube);

	if (FAILED(hr)) {
		WriteToFile("ID3D11Device::CreateBuffer() Failed For Vertex Buffer.\n");
		return(hr);
	}
	else {
		WriteToFile("ID3D11Device::CreateBuffer() Succeeded For Vertex Buffer.\n");
	}

	mappedSubresource;
	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBufferPositionCube, 0,
		D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	memcpy(mappedSubresource.pData, vertices_Cube, sizeof(vertices_Cube));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBufferPositionCube, NULL);

	bufferDescColor;
	ZeroMemory(&bufferDescColor, sizeof(D3D11_BUFFER_DESC));

	bufferDescColor.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescColor.ByteWidth = sizeof(float)*ARRAYSIZE(colors_Cube);
	bufferDescColor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDescColor.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = gpID3D11Device->CreateBuffer(&bufferDescColor, NULL,
		&gpID3D11Buffer_VertexBufferColorCube);

	if (FAILED(hr)) {
		WriteToFile("ID3D11Device::CreateBuffer() Failed For Color Buffer.\n");
		return(hr);
	}
	else {
		WriteToFile("ID3D11Device::CreateBuffer() Succeeded For Color Buffer.\n");
	}

	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBufferColorCube, 0,
		D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	memcpy(mappedSubresource.pData, colors_Cube, sizeof(colors_Cube));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBufferColorCube, NULL);




	D3D11_BUFFER_DESC bufferDesc_ConstantBuffer;
	ZeroMemory(&bufferDesc_ConstantBuffer, sizeof(D3D11_BUFFER_DESC));
	bufferDesc_ConstantBuffer.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc_ConstantBuffer.ByteWidth = sizeof(CBUFFER);
	bufferDesc_ConstantBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = gpID3D11Device->CreateBuffer(&bufferDesc_ConstantBuffer, nullptr,
		&gpID3D11Buffer_ConstantBuffer);

	if (FAILED(hr)) {
		WriteToFile("ID3D11Device::CreateBuffer() Failed for Constant Buffer.\n");
		return (hr);
	}
	else {
		WriteToFile("ID3D11Device::CreateBuffer() Succeeded for Constant Buffer.\n");
	}

	gpID3D11DeviceContext->VSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer);

	D3D11_INPUT_ELEMENT_DESC inputElementDesc[2];
	inputElementDesc[0].SemanticName = "POSITION";
	inputElementDesc[0].SemanticIndex = 0;
	inputElementDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc[0].InputSlot = 0;
	inputElementDesc[0].AlignedByteOffset = 0;
	inputElementDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc[0].InstanceDataStepRate = 0;

	inputElementDesc[1].SemanticName = "COLOR";
	inputElementDesc[1].SemanticIndex = 0;
	inputElementDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc[1].InputSlot = 1;
	inputElementDesc[1].AlignedByteOffset = 0;
	inputElementDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc[1].InstanceDataStepRate = 0;

	hr = gpID3D11Device->CreateInputLayout(inputElementDesc, _ARRAYSIZE(inputElementDesc),
		pID3DBlob_VertexShaderCode->GetBufferPointer(),
		pID3DBlob_VertexShaderCode->GetBufferSize(),
		&gpID3D11InputLayout);

	if (FAILED(hr)) {
		WriteToFile("ID3D11Device::CreateInput() Failed.\n");
		return(hr);
	}
	else {
		WriteToFile("ID3D11Device::CreateInput() Succeeded.\n");
	}

	gpID3D11DeviceContext->IASetInputLayout(gpID3D11InputLayout);
	pID3DBlob_VertexShaderCode->Release();
	pID3DBlob_VertexShaderCode = NULL;


	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory((void *)&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerDesc.AntialiasedLineEnable = FALSE;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f,
		rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;

	hr = gpID3D11Device->CreateRasterizerState(&rasterizerDesc,
		&gpID3D11RasterizerState);

	if (FAILED(hr)) {
		WriteToFile("ID3D11Device::CreateRasterizerState() Failed For Culling.\n");
		return(hr);
	}
	else {
		WriteToFile("ID3D11Device::CreateRasterizerState() Succeeded For Culling.\n");
	}

	gpID3D11DeviceContext->RSSetState(gpID3D11RasterizerState);

	gClearColor[0] = 0.0f;
	gClearColor[1] = 0.0f;
	gClearColor[2] = 0.0f;
	gClearColor[3] = 1.0f;

	gPerspectiveProjectionMatrix = XMMatrixIdentity();

	hr = resize(WIN_WIDTH, WIN_HEIGHT);

	if (FAILED(hr)) {
		WriteToFile("Resize() Failed.\n");
		return(hr);
	}
	else {
		WriteToFile("Resize() Succeeded.\n");
	}
	return(S_OK);
}

HRESULT resize(int width, int height)
{
	HRESULT hr = S_OK;

	if (gpID3D11DepthStencilView) {
		gpID3D11DepthStencilView->Release();
		gpID3D11DepthStencilView = NULL;
	}

	if (gpID3D11RenderTargetView)
	{
		gpID3D11RenderTargetView->Release();
		gpID3D11RenderTargetView = NULL;
	}

	gpIDXGISwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

	ID3D11Texture2D *pID3D11Texture2D_BackBuffer;
	gpIDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pID3D11Texture2D_BackBuffer);

	hr = gpID3D11Device->CreateRenderTargetView(pID3D11Texture2D_BackBuffer,
		NULL,
		&gpID3D11RenderTargetView);

	if (FAILED(hr)) {
		WriteToFile("IDD11Device::CreateRenderTargetView() Failed\n");
		return hr;
	}
	else {
		WriteToFile("IDD11Device::CreateRenderTargetView() Succeeded\n");
	}

	pID3D11Texture2D_BackBuffer->Release();
	pID3D11Texture2D_BackBuffer = NULL;

	//Create depth stencil view from above depth stencil buffer

	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc,sizeof(D3D11_TEXTURE2D_DESC));
	textureDesc.Width = (UINT)width;
	textureDesc.Height = (UINT)height;
	textureDesc.ArraySize = 1;
	textureDesc.MipLevels = 1;
	textureDesc.SampleDesc.Count=1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Format = DXGI_FORMAT_D32_FLOAT;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	ID3D11Texture2D *pID3D11Texture2D_DepthBuffer;
	gpID3D11Device->CreateTexture2D(&textureDesc,NULL,
		&pID3D11Texture2D_DepthBuffer);

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc,sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;

	hr = gpID3D11Device->CreateDepthStencilView(pID3D11Texture2D_DepthBuffer,&depthStencilViewDesc,
		&gpID3D11DepthStencilView);

	if (FAILED(hr)) {
		WriteToFile("IDD11Device::CreateDepthStencilView() Failed\n");
		return hr;
	}
	else {
		WriteToFile("IDD11Device::CreateDepthStencilView() Succeeded\n");
	}

	pID3D11Texture2D_DepthBuffer->Release();
	pID3D11Texture2D_DepthBuffer = NULL;

	gpID3D11DeviceContext->OMSetRenderTargets(1, &gpID3D11RenderTargetView, gpID3D11DepthStencilView);

	D3D11_VIEWPORT d3dViewPort;

	d3dViewPort.TopLeftX = 0;
	d3dViewPort.TopLeftY = 0;
	d3dViewPort.Width = (float)width;
	d3dViewPort.Height = (float)height;
	d3dViewPort.MinDepth = 0.0f;
	d3dViewPort.MaxDepth = 1.0f;
	gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort);

	gPerspectiveProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0), (float)width / (float)height, 0.1f, 100.0f);

	return(hr);
}

void display(void)
{
	gpID3D11DeviceContext->ClearRenderTargetView(gpID3D11RenderTargetView, gClearColor);

	gpID3D11DeviceContext->ClearDepthStencilView(gpID3D11DepthStencilView,D3D11_CLEAR_DEPTH,1.0f,0);

	UINT stride = sizeof(float) * 3;
	UINT offset = 0;

	gpID3D11DeviceContext->IASetVertexBuffers(0, 1, &gpID3D11Buffer_VertexBufferPositionPyramid,
		&stride, &offset);

	stride = sizeof(float) * 3;
	offset = 0;

	gpID3D11DeviceContext->IASetVertexBuffers(1, 1, &gpID3D11Buffer_VertexBufferColorPyramid,
		&stride, &offset);

	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMMATRIX worldMatrix = XMMatrixIdentity();

	XMMATRIX translationMatrix = XMMatrixIdentity();
	XMMATRIX rotationMatrix = XMMatrixIdentity();
	XMMATRIX scaleMatrix = XMMatrixIdentity();

	translationMatrix = XMMatrixTranslation(-2.0f, 0.0f, 6.0f);
	rotationMatrix = XMMatrixRotationY(anglePyramid);


	worldMatrix = rotationMatrix * translationMatrix;

	XMMATRIX viewMatrix = XMMatrixIdentity();

	XMMATRIX mvpMatrix = worldMatrix * viewMatrix*gPerspectiveProjectionMatrix;

	CBUFFER constantBuffer;

	constantBuffer.WorldViewProjectionMatrix = mvpMatrix;

	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer, 0,
		NULL, &constantBuffer, 0, 0);

	gpID3D11DeviceContext->Draw(12, 0);

	/////////////////////////////////////////////////////

	stride = sizeof(float) * 3;
	offset = 0;

	gpID3D11DeviceContext->IASetVertexBuffers(0, 1, &gpID3D11Buffer_VertexBufferPositionCube,
		&stride, &offset);

	stride = sizeof(float) * 3;
	offset = 0;

	gpID3D11DeviceContext->IASetVertexBuffers(1, 1, &gpID3D11Buffer_VertexBufferColorCube,
		&stride, &offset);

	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	worldMatrix = XMMatrixIdentity();

	translationMatrix = XMMatrixIdentity();
	rotationMatrix = XMMatrixIdentity();
	scaleMatrix = XMMatrixIdentity();

	translationMatrix = XMMatrixTranslation(2.0f, 0.0f, 6.0f);
	rotationMatrix = XMMatrixRotationRollPitchYaw(angleCube, angleCube, angleCube);
	scaleMatrix = XMMatrixScaling(0.75, 0.75, 0.75);

	worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

	viewMatrix = XMMatrixIdentity();

	mvpMatrix = worldMatrix * viewMatrix*gPerspectiveProjectionMatrix;

	//constantBuffer;

	constantBuffer.WorldViewProjectionMatrix = mvpMatrix;

	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer, 0,
		NULL, &constantBuffer, 0, 0);

	gpID3D11DeviceContext->Draw(6, 0);
	gpID3D11DeviceContext->Draw(6, 6);
	gpID3D11DeviceContext->Draw(6, 12);
	gpID3D11DeviceContext->Draw(6, 18);
	gpID3D11DeviceContext->Draw(6, 24);
	gpID3D11DeviceContext->Draw(6, 30);

	gpIDXGISwapChain->Present(0, 0);

}

void update() {
	if (anglePyramid < 360)
		anglePyramid = anglePyramid + 0.001;
	else
		anglePyramid = 0.0f;

	if (angleCube < 360)
		angleCube = angleCube + 0.001;
	else
		angleCube = 0.0f;
}

void ReleaseResource(IUnknown *res);

void uninitialize()
{
	if (gpID3D11Buffer_ConstantBuffer) {
		gpID3D11Buffer_ConstantBuffer->Release();
		gpID3D11Buffer_ConstantBuffer = NULL;
	}
	if (gpID3D11InputLayout) {
		gpID3D11InputLayout->Release();
		gpID3D11InputLayout = NULL;
	}

	ReleaseResource((IUnknown *)gpID3D11Buffer_VertexBufferPositionPyramid);
	ReleaseResource((IUnknown *)gpID3D11Buffer_VertexBufferColorPyramid);
	ReleaseResource((IUnknown *)gpID3D11Buffer_VertexBufferPositionCube);
	ReleaseResource((IUnknown *)gpID3D11Buffer_VertexBufferColorCube);

	if (gpID3D11PixelShader) {
		gpID3D11PixelShader->Release();
		gpID3D11PixelShader = NULL;
	}
	if (gpID3D11VertexShader) {
		gpID3D11VertexShader->Release();
		gpID3D11VertexShader = NULL;
	}
	if (gpID3D11RenderTargetView)
	{
		gpID3D11RenderTargetView->Release();
		gpID3D11RenderTargetView = NULL;
	}
	if (gpIDXGISwapChain)
	{
		gpIDXGISwapChain->Release();
		gpIDXGISwapChain = NULL;
	}
	if (gpID3D11DeviceContext)
	{
		gpID3D11DeviceContext->Release();
		gpID3D11DeviceContext = NULL;
	}
	if (gpID3D11Device)
	{
		gpID3D11Device->Release();
		gpID3D11Device = NULL;
	}

	WriteToFile("Uninitialize() Succedded\n");
	WriteToFile("Log File Closed Successfully\n");

}

void WriteToFile(std::string str) {
	fopen_s(&gpFile, gszLogFileName, "a+");
	fprintf_s(gpFile, str.c_str());
	fclose(gpFile);
}

void ReleaseResource(IUnknown *res) {
	if (res != NULL) {
		res->Release();
		res = NULL;
	}
}