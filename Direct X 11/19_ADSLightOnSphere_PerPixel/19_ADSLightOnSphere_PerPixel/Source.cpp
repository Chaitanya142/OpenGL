#include<windows.h>
#include<stdio.h>//for File I/O
#include <string>

#include<d3d11.h>
#pragma comment(lib,"d3d11.lib")
#include<d3dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")

#pragma comment(lib,"Sphere.lib")

#pragma warning(disable:4838)
#include"XNAMath\xnamath.h"

#include"Sphere.h"

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

ID3D11Buffer *gpID3D11Buffer_VertexBufferPositionSphere = NULL;
ID3D11Buffer *gpID3D11Buffer_VertexBufferNormalSphere = NULL;

ID3D11InputLayout *gpID3D11InputLayout = NULL;
ID3D11Buffer *gpID3D11Buffer_ConstantBuffer = NULL;

ID3D11RasterizerState *gpID3D11RasterizerState = NULL;
ID3D11DepthStencilView *gpID3D11DepthStencilView = NULL;

ID3D11Buffer *gpID3D11Buffer_IndexBuffer = NULL;

float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];
unsigned int gNumElements;
unsigned int gNumVertices;

float angleSphere = 0;

void update();

bool gbLight = false;
bool gb_Animation = false;
struct CBUFFER {
	XMMATRIX WorldMatrix;
	XMMATRIX ViewMatrix;
	XMMATRIX ProjectionMatrix;

	XMVECTOR Ld;
	XMVECTOR Kd;
	XMVECTOR La;
	XMVECTOR Ka;
	XMVECTOR Ls;
	XMVECTOR Ks;

	XMVECTOR LightPosition;
	FLOAT MaterialShininess;

	UINT LKeyIsPressed;
};

XMMATRIX gPerspectiveProjectionMatrix;

float lightAmbient[4];
float lightDiffuse[4];
float lightSpecular[4];
float lightPosition[4];

float materialAmbient[4];
float materialDiffuse[4];
float materialSpecular[4];
float materialShininess;

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
			//Light
		case 'L':
		case 'l':
			if (gbLight == true) {
				gbLight = false;
			}
			else {
				gbLight = true;
			}

			break;
		case 'a':
		case 'A':
			if (gb_Animation == true) {
				gb_Animation = false;
			}
			else {
				gb_Animation = true;
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

	//COLOR Intershader named shader
	const char *vertexShaderSouceCode =
		"cbuffer ConstantBuffer"\
		"{"\
		"float4x4 worldMatrix;"\
		"float4x4 viewMatrix;"\
		"float4x4 projectionMatrix;"\
		"float4 ld;"\
		"float4 kd;"\
		"float4 la;"\
		"float4 ka;"\
		"float4 ls;"\
		"float4 ks;"\
		"float4 lightPosition;"\
		"float materialShininess;"\
		"uint lKeyIsPressed;"\
		"}"\
		"struct vertex_output"\
		"{"\
		"float4 position : SV_POSITION;"\
		"float3 transformedNormal : NORMAL0;"\
		"float3 lightDirection : NORMAL1;"\
		"float3 reflectionVector : NORMAL2;"\
		"float3 viewerVector : NORMAL3;"\
		"};"\
		"vertex_output main(float4 pos : POSITION,float3 normal:NORMAL)"\
		"{"\
		"vertex_output output;"\

		"if(lKeyIsPressed == 1)"\
		"{"\
			"float4 eyecoordinates=mul(viewMatrix,mul(worldMatrix,pos));"\
			"output.transformedNormal = mul((float3x3)worldMatrix, (float3)normal);"\
			"output.lightDirection=(float3)(lightPosition-eyecoordinates);"\
			"output.reflectionVector = reflect(-output.lightDirection, output.transformedNormal);"\
			"output.viewerVector=(float3)(-eyecoordinates.xyz);"\
		"}"\
		"output.position=mul(projectionMatrix,mul(viewMatrix,mul(worldMatrix,pos)));"\
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
		"cbuffer ConstantBuffer"\
		"{"\
		"float4x4 worldMatrix;"\
		"float4x4 viewMatrix;"\
		"float4x4 projectionMatrix;"\
		"float4 ld;"\
		"float4 kd;"\
		"float4 la;"\
		"float4 ka;"\
		"float4 ls;"\
		"float4 ks;"\
		"float4 lightPosition;"\
		"float materialShininess;"\
		"uint lKeyIsPressed;"\
		"}"\
		"struct vertex_output"\
		"{"\
		"float4 position : SV_POSITION;"\
		"float3 transformedNormal : NORMAL0;"\
		"float3 lightDirection : NORMAL1;"\
		"float3 reflectionVector : NORMAL2;"\
		"float3 viewerVector : NORMAL3;"\
		"};"\
		"float4 main(float4 pos:SV_POSITION,vertex_output input) : SV_TARGET"\
		"{"\
		"float4 color;"\
		"if(lKeyIsPressed == 1)"\
		"{"\
			"float3 tnorm_normalize=normalize(input.transformedNormal);"\
			"float3 lightDirection_normalize=normalize(input.lightDirection);"\
			"float3 reflectionVector_normalize=normalize(input.reflectionVector);"\
			"float3 viewerVector_normalize=normalize(input.viewerVector);"\
			"float tn_dot_ld=max(dot(lightDirection_normalize,tnorm_normalize),0.0f);"\
			"float4 ambient=la*ka;"\
			"float4 diffuse=ld*kd*tn_dot_ld;"
			"float4 specular=ls*ks*pow(max(dot(reflectionVector_normalize,viewerVector_normalize),0.0),materialShininess);"\
			"color=ambient+diffuse+specular;"\
		"}"\
		"else"\
		"{"\
			"color=float4(1.0f,1.0f,1.0f,1.0f);"\
		"}"\
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

	D3D11_BUFFER_DESC bufferDescPosition;

	D3D11_MAPPED_SUBRESOURCE mappedSubresource;

	D3D11_BUFFER_DESC bufferDescNormal;

	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();

	bufferDescPosition;
	ZeroMemory(&bufferDescPosition, sizeof(D3D11_BUFFER_DESC));

	bufferDescPosition.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescPosition.ByteWidth = sizeof(float)*ARRAYSIZE(sphere_vertices);
	bufferDescPosition.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDescPosition.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = gpID3D11Device->CreateBuffer(&bufferDescPosition, NULL,
		&gpID3D11Buffer_VertexBufferPositionSphere);

	if (FAILED(hr)) {
		WriteToFile("ID3D11Device::CreateBuffer() Failed For Vertex Buffer.\n");
		return(hr);
	}
	else {
		WriteToFile("ID3D11Device::CreateBuffer() Succeeded For Vertex Buffer.\n");
	}

	mappedSubresource;
	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBufferPositionSphere, 0,
		D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	memcpy(mappedSubresource.pData, sphere_vertices, sizeof(sphere_vertices));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBufferPositionSphere, NULL);

	bufferDescNormal;
	ZeroMemory(&bufferDescNormal, sizeof(D3D11_BUFFER_DESC));

	bufferDescNormal.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescNormal.ByteWidth = sizeof(float)*ARRAYSIZE(sphere_normals);
	bufferDescNormal.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDescNormal.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = gpID3D11Device->CreateBuffer(&bufferDescNormal, NULL,
		&gpID3D11Buffer_VertexBufferNormalSphere);

	if (FAILED(hr)) {
		WriteToFile("ID3D11Device::CreateBuffer() Failed For Normal Buffer.\n");
		return(hr);
	}
	else {
		WriteToFile("ID3D11Device::CreateBuffer() Succeeded For Normal Buffer.\n");
	}

	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBufferNormalSphere, 0,
		D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	memcpy(mappedSubresource.pData, sphere_normals, sizeof(sphere_normals));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBufferNormalSphere, NULL);


	D3D11_BUFFER_DESC bufferDesc;

	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC; // write access access by CPU and GPU
	bufferDesc.ByteWidth = gNumElements * sizeof(short);
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // allow CPU to write into this buffer
	hr = gpID3D11Device->CreateBuffer(&bufferDesc, NULL, &gpID3D11Buffer_IndexBuffer);
	if (FAILED(hr))
	{
		WriteToFile("ID3D11Device::CreateBuffer() Failed For Index Buffer.\n");
		return(hr);
	}
	else
	{
		WriteToFile("ID3D11Device::CreateBuffer() Succeeded For Index Buffer.\n");

	}

	// copy indices into above index buffer
	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_IndexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedSubresource); // map buffer
	memcpy(mappedSubresource.pData, sphere_elements, gNumElements * sizeof(short));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_IndexBuffer, NULL); // unmap buffer


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

	gpID3D11DeviceContext->PSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer);

	D3D11_INPUT_ELEMENT_DESC inputElementDesc[2];
	inputElementDesc[0].SemanticName = "POSITION";
	inputElementDesc[0].SemanticIndex = 0;
	inputElementDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc[0].InputSlot = 0;
	inputElementDesc[0].AlignedByteOffset = 0;
	inputElementDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc[0].InstanceDataStepRate = 0;

	inputElementDesc[1].SemanticName = "NORMAL";
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



	lightAmbient[0] = 0.0f;
	lightAmbient[1] = 0.0f;
	lightAmbient[2] = 0.0f;
	lightAmbient[3] = 0.0f;
	lightDiffuse[0] = 1.0f;
	lightDiffuse[1] = 1.0f;
	lightDiffuse[2] = 1.0f;
	lightDiffuse[3] = 1.0f;
	lightSpecular[0] = 1.0f;
	lightSpecular[1] = 1.0f;
	lightSpecular[2] = 1.0f;
	lightSpecular[3] = 1.0f;
	lightPosition[0] = 100.0f;
	lightPosition[1] = 100.0f;

	lightPosition[2] = -100.0f;

	lightPosition[3] = 1.0f;
	materialAmbient[0] = 0.0f;
	materialAmbient[1] = 0.0f;
	materialAmbient[2] = 0.0f;
	materialAmbient[3] = 0.0f;
	materialDiffuse[0] = 1.0f;
	materialDiffuse[1] = 1.0f;
	materialDiffuse[2] = 1.0f;
	materialDiffuse[3] = 1.0f;
	materialSpecular[0] = 1.0f;
	materialSpecular[1] = 1.0f;
	materialSpecular[2] = 1.0f;
	materialSpecular[3] = 1.0f;
	materialShininess = 50.0f;


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

	UINT stride = sizeof(float) * 3;
	UINT offset = 0;

	XMMATRIX worldMatrix = XMMatrixIdentity();

	XMMATRIX translationMatrix = XMMatrixIdentity();
	XMMATRIX rotationMatrix = XMMatrixIdentity();
	XMMATRIX scaleMatrix = XMMatrixIdentity();

	XMMATRIX viewMatrix = XMMatrixIdentity();

	CBUFFER constantBuffer;

	/////////////////////////////////////////////////////

	stride = sizeof(float) * 3;
	offset = 0;

	gpID3D11DeviceContext->IASetVertexBuffers(0, 1, &gpID3D11Buffer_VertexBufferPositionSphere,
		&stride, &offset);

	stride = sizeof(float) * 3;
	offset = 0;

	gpID3D11DeviceContext->IASetVertexBuffers(1, 1, &gpID3D11Buffer_VertexBufferNormalSphere,
		&stride, &offset);

	gpID3D11DeviceContext->IASetIndexBuffer(gpID3D11Buffer_IndexBuffer, DXGI_FORMAT_R16_UINT, 0); // R16 maps with 'short'


	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	worldMatrix = XMMatrixIdentity();

	translationMatrix = XMMatrixIdentity();
	rotationMatrix = XMMatrixIdentity();
	scaleMatrix = XMMatrixIdentity();

	translationMatrix = XMMatrixTranslation(0.0f, 0.0f, 3.0f);
	rotationMatrix = XMMatrixRotationRollPitchYaw(angleSphere, angleSphere, angleSphere);
	//scaleMatrix = XMMatrixScaling(0.75, 0.75, 0.75);

	worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

	viewMatrix = XMMatrixIdentity();

	//constantBuffer;
	if (gbLight) {
		constantBuffer.WorldMatrix = worldMatrix;
		constantBuffer.ViewMatrix = viewMatrix;
		constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;

		constantBuffer.La = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
		constantBuffer.Ld = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
		constantBuffer.Ls = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);

		constantBuffer.Ka = XMVectorSet(materialAmbient[0], materialAmbient[1], materialAmbient[2], materialAmbient[3]);
		constantBuffer.Kd = XMVectorSet(materialDiffuse[0], materialDiffuse[1], materialDiffuse[2], materialDiffuse[3]);
		constantBuffer.Ks = XMVectorSet(materialSpecular[0], materialSpecular[1], materialSpecular[2], materialSpecular[3]);

		constantBuffer.LightPosition= XMVectorSet(lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);

		constantBuffer.MaterialShininess = materialShininess;

		constantBuffer.LKeyIsPressed = 1;
	}
	else {
		constantBuffer.WorldMatrix = worldMatrix;
		constantBuffer.ViewMatrix = viewMatrix;
		constantBuffer.ProjectionMatrix = gPerspectiveProjectionMatrix;
		constantBuffer.LKeyIsPressed = 0;
	}
	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer, 0,
		NULL, &constantBuffer, 0, 0);

	gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);

	gpIDXGISwapChain->Present(0, 0);

}

void update() {
	if (gb_Animation) {
		if (angleSphere < 360)
			angleSphere = angleSphere + 0.0005;
		else
			angleSphere = 0.0f;
	}
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
	if (gpID3D11Buffer_IndexBuffer)
	{
		gpID3D11Buffer_IndexBuffer->Release();
		gpID3D11Buffer_IndexBuffer = NULL;
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