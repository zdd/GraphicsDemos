#include <Windows.h>
#include <windowsx.h>
#pragma comment (lib, "Winmm.lib")

#include <d3d11.h>
#pragma comment (lib, "d3d11.lib")
#include <d3dcompiler.h>
#pragma comment(lib, "D3DCompiler.lib")

#include <dwmapi.h>
#pragma comment (lib, "Dwmapi.lib")


// Globals
IDXGISwapChain *swapChain;
ID3D11Device *device;
ID3D11DeviceContext *deviceContext;
ID3D11RenderTargetView *backBuffer;
ID3D11InputLayout *inputLayout;            // the pointer to the input layout
ID3D11VertexShader *vertexShader;               // the pointer to the vertex shader
ID3D11PixelShader *pixelShader;                // the pointer to the pixel shader
ID3D11Buffer *vertexBuffer;                // the pointer to the vertex buffer

void InitializeDirectX(HWND hWnd);
void LoadTriangle();

struct Vertex {
	float x, y, z;
	float color[4];
};

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		{
			// close the application entirely
			PostQuitMessage(0);
			return 0;
		}
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

// Globals
const LPCWSTR ClassName = L"className";


// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine,
				   int nCmdShow)
{
	// the handle for the window, filled by a function
	HWND hWnd;
	// this struct holds information for the window class
	WNDCLASSEX wc;

	// Pick out window size
	int windowHeight = 400;
	int windowWidth = 400;

	// clear out the window class for use
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	// fill in the struct with the needed information
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = ClassName;

	// register the window class
	RegisterClassEx(&wc);

	// Window style
	DWORD exStyle = WS_EX_LAYERED | WS_EX_TOPMOST;
	DWORD style = WS_POPUP;

	// create the window and use the result as the handle
	hWnd = CreateWindowEx(
		exStyle,        // extended styles
		ClassName,    // name of the window class
		L"TestOverlay",   // title of the window
		style,    // window style
		0,    // x-position of the window
		0,    // y-position of the window
		400,    // width of the window
		400,    // height of the window
		NULL,    // we have no parent window, NULL
		NULL,    // we aren't using menus, NULL
		hInstance,    // application handle
		NULL);    // used with multiple windows, NULL

	//// display the window on the screen
	//ShowWindow(hWnd, nCmdShow);

	//SetWindowPos(hWnd,
	//	HWND_TOPMOST,
	//	0, 0,
	//	windowWidth, windowHeight,
	//	SWP_SHOWWINDOW);

	// Show the window
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);

	// Make the window transparent
	SetWindowLong(hWnd, GWL_EXSTYLE, WS_EX_LAYERED);
	SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 255, LWA_ALPHA | LWA_COLORKEY);

	// NOTE: If I uncomment DwmExtendFrameIntoClientArea then it becomes transparent, otherwise its a black screen
	MARGINS dwmMargin = { -1 };
	DwmExtendFrameIntoClientArea(hWnd, &dwmMargin);

	// DirectX
	InitializeDirectX(hWnd);
	LoadTriangle();

	// enter the main loop:
	MSG msg;
	while (TRUE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				break;

			// DirectX stuff
			float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
			deviceContext->ClearRenderTargetView(backBuffer, color);

			// Draw triangle
			UINT stride = sizeof(Vertex);
			UINT offset = 0;
			deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
			deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			deviceContext->Draw(3, 0);

			swapChain->Present(0, 0);
		}
	}

	return msg.wParam;
}


void InitializeDirectX(HWND hWnd)
{
	// create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC scd;

	// clear out the struct for use
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	// fill the swap chain description struct
	scd.BufferCount = 1;                                    // one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	scd.OutputWindow = hWnd;                                // the window to be used
	scd.SampleDesc.Count = 4;                               // how many multisamples
	scd.Windowed = TRUE;                                    // windowed/full-screen mode

	// create a device, device context and swap chain using the information in the scd struct
	D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&swapChain,
		&device,
		NULL,
		&deviceContext);


	// get the address of the back buffer
	ID3D11Texture2D *pBackBuffer;
	swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	// use the back buffer address to create the render target
	device->CreateRenderTargetView(pBackBuffer, NULL, &backBuffer);
	pBackBuffer->Release();

	// set the render target as the back buffer
	deviceContext->OMSetRenderTargets(1, &backBuffer, NULL);

	// Set the viewport size
	RECT cRect;
	GetClientRect(hWnd, &cRect);

	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)(cRect.right - cRect.left);
	viewport.Height = (float)(cRect.bottom - cRect.top);

	deviceContext->RSSetViewports(1, &viewport);


	// Initialize pipeline stuff
	ID3DBlob *VS, *PS;

	// compile shader
	D3DCompileFromFile(L"shaders.shader", 0, 0, "VS", "vs_4_0", 0, 0, &VS, 0);
	D3DCompileFromFile(L"shaders.shader", 0, 0, "PS", "ps_4_0", 0, 0, &PS, 0);

	// encapsulate both shaders into shader objects
	device->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &vertexShader);
	device->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &pixelShader);

	deviceContext->VSSetShader(vertexShader, 0, 0);
	deviceContext->PSSetShader(pixelShader, 0, 0);

	// create input layout object
	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	device->CreateInputLayout(ied, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &inputLayout);
	deviceContext->IASetInputLayout(inputLayout);

}


void LoadTriangle()
{
	// create a triangle using the VERTEX struct
	Vertex OurVertices[] =
	{
		{ 0.0f, 0.5f, 0.0f, { 1.0f, 0.0f, 0.0f, 1.0f } },
		{ 0.45f, -0.5, 0.0f, { 0.0f, 1.0f, 0.0f, 1.0f } },
		{ -0.45f, -0.5f, 0.0f, { 0.0f, 0.0f, 1.0f, 1.0f } }
	};


	// create the vertex buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
	bd.ByteWidth = sizeof(Vertex)* 3;             // size is the VERTEX struct * 3
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

	device->CreateBuffer(&bd, NULL, &vertexBuffer);       // create the buffer


	// copy the vertices into the buffer
	D3D11_MAPPED_SUBRESOURCE ms;
	deviceContext->Map(vertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);    // map the buffer
	memcpy(ms.pData, OurVertices, sizeof(OurVertices));                 // copy the data
	deviceContext->Unmap(vertexBuffer, NULL);                                      // unmap the buffer
}