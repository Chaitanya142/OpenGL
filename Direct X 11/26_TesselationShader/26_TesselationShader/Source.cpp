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
ID3D11HullShader *gpID3D11HullShader = NULL;
ID3D11DomainShader *gpID3D11DomainShader = NULL;

ID3D11Buffer *gpID3D11Buffer_VertexBufferPosition = NULL;

ID3D11InputLayout *gpID3D11InputLayout = NULL;
ID3D11Buffer *gpID3D11Buffer_ConstantBuffer_HullShader = NULL;
ID3D11Buffer *gpID3D11Buffer_ConstantBuffer_DomainShader = NULL;
ID3D11Buffer *gpID3D11Buffer_ConstantBuffer_PixelShader = NULL;

ID3D11RasterizerState *gpID3D11RasterizerState = NULL;
ID3D11DepthStencilView *gpID3D11DepthStencilView = NULL;



void update();

struct CBUFFER_HULL_SHADER {
	XMVECTOR Hull_Constant_Function_Params;
};
struct CBUFFER_DOMAIN_SHADER {
	XMMATRIX WorldViewProjectionMatrix;
};
struct CBUFFER_PIXEL_SHADER {
	XMVECTOR LineColor;
};

unsigned int guiNumberOfLineSegments = 1;

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
		case VK_UP:
			guiNumberOfLineSegments++;
			if (guiNumberOfLineSegments >= 50)
				guiNumberOfLineSegments = 50;
			break;
		case VK_DOWN:
			guiNumberOfLineSegments--;
			if (guiNumberOfLineSegments <= 0)
				guiNumberOfLineSegments = 1;
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
		"struct vertex_output"\
		"{"\
		"float4 position : POSITION;"\
		"};"\
		"vertex_output main(float2 pos : POSITION)"\
		"{"\
		"vertex_output output;"\
		"output.position=float4(pos,0.0f,1.0f);"\
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





	const char *HullShaderSouceCode =
		"cbuffer ConstantBuffer"\
		"{"\
		"float4 hull_constant_function_params;"\
		"}"\
		"struct vertex_output"\
		"{"\
		"float4 position : POSITION;"\
		"};"\
		"struct hull_constant_output"\
		"{"\
		"float edges[2] : SV_TESSFACTOR;"\
		"};"\
		"hull_constant_output hull_constant_function(void)"\
		"{"\
		"hull_constant_output output;"\
		"float numberOfStrips=hull_constant_function_params[0];"\
		"float numberOfSegments=hull_constant_function_params[1];"\
		"output.edges[0]=numberOfStrips;"\
		"output.edges[1]=numberOfSegments;"\
		"return(output);"\
		"}"\
		"struct hull_output"\
		"{"\
		"float4 position : POSITION;"\
		"};"\
		"[domain(\"isoline\")]"\
		"[partitioning(\"integer\")]"\
		"[outputtopology(\"line\")]"\
		"[outputcontrolpoints(4)]"\
		"[patchconstantfunc(\"hull_constant_function\")]"\
		"hull_output main(InputPatch<vertex_output,4> input_patch, uint i : SV_OUTPUTCONTROLPOINTID)"
		"{"\
		"hull_output output;"\
		"output.position=input_patch[i].position;"\
		"return(output);"\
		"}";

	ID3DBlob *pID3DBlob_HullShaderCode = NULL;
	pID3DBlob_Error = NULL;

	hr = D3DCompile(HullShaderSouceCode,
		lstrlenA(HullShaderSouceCode) + 1,
		"HS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"hs_5_0",
		0,
		0,
		&pID3DBlob_HullShaderCode,
		&pID3DBlob_Error
	);

	if (FAILED(hr))
	{
		if (pID3DBlob_Error != NULL)
		{
			fopen_s(&gpFile, gszLogFileName, "a+");
			fprintf_s(gpFile, "D3DCompile() Failed For Hull Shader : %s.\n", (char*)pID3DBlob_Error->GetBufferPointer());
			fclose(gpFile);
			//WriteToFile("D3DCompile() Failed For Vertex Shader : %s.\n"+(char*)pID3DBlob_Error->GetBufferPointer());
			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
			return(hr);
		}
	}
	else {
		WriteToFile("D3DCompile() Succeeded For Hull Shader.\n");
	}

	hr = gpID3D11Device->CreateHullShader(pID3DBlob_HullShaderCode->GetBufferPointer(),
		pID3DBlob_HullShaderCode->GetBufferSize(),
		NULL,
		&gpID3D11HullShader
	);

	if (FAILED(hr))
	{
		WriteToFile("ID3D11Device::CreateHullShader() Failed .\n");
		return(hr);
	}
	else {
		WriteToFile("ID3D11Device::CreateHullShader() Succeeded.\n");
	}

	gpID3D11DeviceContext->HSSetShader(gpID3D11HullShader, 0, 0);







	const char *DomainShaderSouceCode =
		"cbuffer ConstantBuffer"\
		"{"\
		"float4x4 worldViewProjectionMatrix;"\
		"}"\
		"struct hull_constant_output"\
		"{"\
		"float edges[2] : SV_TESSFACTOR;"\
		"};"\
		"struct hull_output"\
		"{"\
		"float4 position:POSITION;"\
		"};"\
		"struct domain_output"\
		"{"\
		"float4 position:SV_POSITION;"\
		"};"\
		"[domain(\"isoline\")]"\
		"domain_output main(hull_constant_output input,OutputPatch<hull_output,4> output_patch, float2 tessCoord : SV_DOMAINLOCATION)"\
		"{"\
		"domain_output output;"\
		"float u=tessCoord.x;"\
		"float3 p0=output_patch[0].position.xyz;"\
		"float3 p1=output_patch[1].position.xyz;"\
		"float3 p2=output_patch[2].position.xyz;"\
		"float3 p3=output_patch[3].position.xyz;"\
		"float u1=	(1.0f	-	u);"\
		"float u2=	u	*	u;"\
		"float b3=	u2	*	u;"\
		"float b2=	3.0	*	u2	*	u1;"\
		"float b1=	3.0	*	u	*	u1	*	u1;"\
		"float b0=	u1	*	u1	*	u1;"\
		"float3 p=	p0*b0	+	p1*b1	+	p2*b2	+	p3*b3;"\
		"output.position=mul(worldViewProjectionMatrix,float4(p,1.0f));"\
		"return(output);"\
		"}";

	ID3DBlob *pID3DBlob_DomainShaderCode = NULL;
	pID3DBlob_Error = NULL;

	hr = D3DCompile(DomainShaderSouceCode,
		lstrlenA(DomainShaderSouceCode) + 1,
		"DS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"ds_5_0",
		0,
		0,
		&pID3DBlob_DomainShaderCode,
		&pID3DBlob_Error
	);

	if (FAILED(hr))
	{
		if (pID3DBlob_Error != NULL)
		{
			fopen_s(&gpFile, gszLogFileName, "a+");
			fprintf_s(gpFile, "D3DCompile() Failed For Domain Shader : %s.\n", (char*)pID3DBlob_Error->GetBufferPointer());
			fclose(gpFile);
			//WriteToFile("D3DCompile() Failed For Vertex Shader : %s.\n"+(char*)pID3DBlob_Error->GetBufferPointer());
			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
			return(hr);
		}
	}
	else {
		WriteToFile("D3DCompile() Succeeded For Domain Shader.\n");
	}

	hr = gpID3D11Device->CreateDomainShader(pID3DBlob_DomainShaderCode->GetBufferPointer(),
		pID3DBlob_DomainShaderCode->GetBufferSize(),
		NULL,
		&gpID3D11DomainShader
	);

	if (FAILED(hr))
	{
		WriteToFile("ID3D11Device::CreateDomainShader() Failed .\n");
		return(hr);
	}
	else {
		WriteToFile("ID3D11Device::CreateDomainShader() Succeeded.\n");
	}

	gpID3D11DeviceContext->DSSetShader(gpID3D11DomainShader, 0, 0);






	const char * pixelShaderSourceCode =
		"cbuffer ConstantBuffer"\
		"{"\
		"float4 lineColor;"\
		"}"\
		"float4 main(void) : SV_TARGET"\
		"{"\
		"return(lineColor);"\
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

	float vertices[] = { 
					-1.0f,-1.0f,
					-0.5f,1.0f,
					0.5f,-1.0f,
					1.0f,1.0f
	};

	D3D11_BUFFER_DESC bufferDescPosition;
	ZeroMemory(&bufferDescPosition, sizeof(D3D11_BUFFER_DESC));

	bufferDescPosition.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescPosition.ByteWidth = sizeof(float)*ARRAYSIZE(vertices);
	bufferDescPosition.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDescPosition.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = gpID3D11Device->CreateBuffer(&bufferDescPosition, NULL,
		&gpID3D11Buffer_VertexBufferPosition);

	if (FAILED(hr)) {
		WriteToFile("ID3D11Device::CreateBuffer() Failed For Vertex Buffer.\n");
		return(hr);
	}
	else {
		WriteToFile("ID3D11Device::CreateBuffer() Succeeded For Vertex Buffer.\n");
	}

	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBufferPosition, 0,
		D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	memcpy(mappedSubresource.pData, vertices, sizeof(vertices));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBufferPosition, NULL);


	D3D11_BUFFER_DESC bufferDesc_ConstantBuffer;
	ZeroMemory(&bufferDesc_ConstantBuffer, sizeof(D3D11_BUFFER_DESC));
	bufferDesc_ConstantBuffer.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc_ConstantBuffer.ByteWidth = sizeof(CBUFFER_HULL_SHADER);
	bufferDesc_ConstantBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = gpID3D11Device->CreateBuffer(&bufferDesc_ConstantBuffer, nullptr,
		&gpID3D11Buffer_ConstantBuffer_HullShader);

	if (FAILED(hr)) {
		WriteToFile("ID3D11Device::CreateBuffer() Failed for Constant Buffer.\n");
		return (hr);
	}
	else {
		WriteToFile("ID3D11Device::CreateBuffer() Succeeded for Constant Buffer.\n");
	}

	gpID3D11DeviceContext->HSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer_HullShader);



	ZeroMemory(&bufferDesc_ConstantBuffer, sizeof(D3D11_BUFFER_DESC));
	bufferDesc_ConstantBuffer.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc_ConstantBuffer.ByteWidth = sizeof(CBUFFER_DOMAIN_SHADER);
	bufferDesc_ConstantBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = gpID3D11Device->CreateBuffer(&bufferDesc_ConstantBuffer, nullptr,
		&gpID3D11Buffer_ConstantBuffer_DomainShader);

	if (FAILED(hr)) {
		WriteToFile("ID3D11Device::CreateBuffer() Failed for Constant Buffer.\n");
		return (hr);
	}
	else {
		WriteToFile("ID3D11Device::CreateBuffer() Succeeded for Constant Buffer.\n");
	}

	gpID3D11DeviceContext->DSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer_DomainShader);



	ZeroMemory(&bufferDesc_ConstantBuffer, sizeof(D3D11_BUFFER_DESC));
	bufferDesc_ConstantBuffer.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc_ConstantBuffer.ByteWidth = sizeof(CBUFFER_PIXEL_SHADER);
	bufferDesc_ConstantBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = gpID3D11Device->CreateBuffer(&bufferDesc_ConstantBuffer, nullptr,
		&gpID3D11Buffer_ConstantBuffer_PixelShader);

	if (FAILED(hr)) {
		WriteToFile("ID3D11Device::CreateBuffer() Failed for Constant Buffer.\n");
		return (hr);
	}
	else {
		WriteToFile("ID3D11Device::CreateBuffer() Succeeded for Constant Buffer.\n");
	}

	gpID3D11DeviceContext->PSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer_PixelShader);

	D3D11_INPUT_ELEMENT_DESC inputElementDesc;
	inputElementDesc.SemanticName = "POSITION";
	inputElementDesc.SemanticIndex = 0;
	inputElementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDesc.InputSlot = 0;
	inputElementDesc.AlignedByteOffset = 0;
	inputElementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc.InstanceDataStepRate = 0;

	hr = gpID3D11Device->CreateInputLayout(&inputElementDesc, 1,
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
	pID3DBlob_HullShaderCode->Release();
	pID3DBlob_HullShaderCode = NULL;
	pID3DBlob_DomainShaderCode->Release();
	pID3DBlob_DomainShaderCode = NULL;

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
	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	textureDesc.Width = (UINT)width;
	textureDesc.Height = (UINT)height;
	textureDesc.ArraySize = 1;
	textureDesc.MipLevels = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Format = DXGI_FORMAT_D32_FLOAT;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	ID3D11Texture2D *pID3D11Texture2D_DepthBuffer;
	gpID3D11Device->CreateTexture2D(&textureDesc, NULL,
		&pID3D11Texture2D_DepthBuffer);

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;

	hr = gpID3D11Device->CreateDepthStencilView(pID3D11Texture2D_DepthBuffer, &depthStencilViewDesc,
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

	gpID3D11DeviceContext->ClearDepthStencilView(gpID3D11DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	UINT stride = sizeof(float) * 2;
	UINT offset = 0;

	gpID3D11DeviceContext->IASetVertexBuffers(0, 1, &gpID3D11Buffer_VertexBufferPosition,
		&stride, &offset);

	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

	XMMATRIX worldMatrix = XMMatrixIdentity();

	XMMATRIX translationMatrix = XMMatrixIdentity();
	XMMATRIX rotationMatrix = XMMatrixIdentity();
	XMMATRIX scaleMatrix = XMMatrixIdentity();

	translationMatrix = XMMatrixTranslation(0.0f, 0.0f, 3.0f);

	worldMatrix = rotationMatrix * translationMatrix;

	XMMATRIX viewMatrix = XMMatrixIdentity();

	XMMATRIX mvpMatrix = worldMatrix * viewMatrix*gPerspectiveProjectionMatrix;

	CBUFFER_DOMAIN_SHADER constantBufferDomainShader;

	constantBufferDomainShader.WorldViewProjectionMatrix = mvpMatrix;

	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer_DomainShader, 0,
		NULL, &constantBufferDomainShader, 0, 0);


	CBUFFER_HULL_SHADER constantBufferHullShader;

	constantBufferHullShader.Hull_Constant_Function_Params = XMVectorSet(1.0f,(FLOAT)guiNumberOfLineSegments,0.0f,0.0f);

	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer_HullShader, 0,
		NULL, &constantBufferHullShader, 0, 0);


	CBUFFER_PIXEL_SHADER constantBufferPixelShader;

	constantBufferPixelShader.LineColor = XMVectorSet(1.0f,1.0f,0.0f,0.0f);

	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer_PixelShader, 0,
		NULL, &constantBufferPixelShader, 0, 0);



	gpID3D11DeviceContext->Draw(4, 0);

	gpIDXGISwapChain->Present(0, 0);

}

void update() {

}

void ReleaseResource(IUnknown *res);

void uninitialize()
{
	
	if (gpID3D11InputLayout) {
		gpID3D11InputLayout->Release();
		gpID3D11InputLayout = NULL;
	}


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