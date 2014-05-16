
#include "stdafx.h"

#include <Windows.h>
#include <d3dx9.h>
#include <DxErr.h>

#include "Camera.h"
#include "DXMultiViewWin32.h"
#include "SwapChain.h"

IDirect3DSwapChain9* g_pDefaultSwapChain = NULL ;
SwapChain* g_pSwapChain1 = new SwapChain() ;
SwapChain* g_pSwapChain2 = new SwapChain() ;
SwapChain* g_pSwapChain3 = new SwapChain() ;
SwapChain* g_pSwapChain4 = new SwapChain() ;

Camera*	g_pCamera1 = new Camera() ; // for top-left window
Camera*	g_pCamera2 = new Camera() ;	// for top-right window
Camera*	g_pCamera3 = new Camera() ;	// for bottom-left window
Camera*	g_pCamera4 = new Camera() ; // for bottom-right window


#define MAX_LOADSTRING 100
#define SAFE_RELEASE(p) if(p){p->Release(); p = NULL ;}

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

LPDIRECT3D9			g_pD3D		= NULL ;
LPDIRECT3DDEVICE9	g_pDevice	= NULL ;
ID3DXMesh*			g_pTeapot	= NULL ; // Mesh for teapot
ID3DXMesh*			g_pSphere	= NULL ; // Mesh for sphere
ID3DXMesh*			g_pCylinder = NULL ; // Mesh for cylinder
ID3DXMesh*			g_pTorus	= NULL ; // Mesh for torus

void InitD3D(HWND hWnd) ;
void InitializeCamera(HWND hWnd, Camera* pCamera) ;
void Render() ;
void Release() ;

VOID CreateChildWindows(HWND hWnd, HINSTANCE hInst) ;
VOID CreateChildWindow(HWND hWnd, HWND& childHwnd, HINSTANCE hInst, LPCSTR className, int x, int y, int width, int height) ;
void CreateSwapChains(HWND hWnd, int width, int height) ;
LRESULT CALLBACK TopLeftWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) ;
LRESULT CALLBACK TopRightWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) ;
LRESULT CALLBACK BottomLeftWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) ;
LRESULT CALLBACK BottomRightWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) ;

const int BorderWidth = 0 ;

HWND g_topLefthWnd ;
HWND g_topRighthWnd ;
HWND g_bottomLefthWnd ;
HWND g_bottomRighthWnd ;

#define TOPLEFT_WINDOW_CLASS "TopLeftWindowClass"
#define TOPRIGHT_WINDOW_CLASS "TopRightWindowClass"
#define BOTTOMLEFT_WINDOW_CLASS "BottomLeftWindowClass"
#define BOTTOMRIGHT_WINDOW_CLASS "BottomRightWindowClass"

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	ZeroMemory(&msg, sizeof(msg)) ;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_DXMULTIVIEWWIN32, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	while (msg.message != WM_QUIT)  
	{
		if( PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) != 0)
		{
			TranslateMessage (&msg) ;
			DispatchMessage (&msg) ;
		}
		else // Render the game if no message to process
		{
			Render() ;
		}
	}

	return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	UINT returnValue ;

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style			= CS_VREDRAW | CS_HREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DXMULTIVIEWWIN32));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= NULL ;/*(HBRUSH)(COLOR_WINDOW + 1);*/
	wcex.lpszMenuName	= NULL ;/*MAKEINTRESOURCE(IDC_DXMULTIVIEWWIN32);*/
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	// Register the main window
	returnValue = RegisterClassEx(&wcex);
	if (returnValue == 0)
	{
		return 0 ;
	}

	WNDCLASSEX winClassTopleft ;
	winClassTopleft.cbSize			= sizeof(winClassTopleft) ;
	winClassTopleft.style			= CS_VREDRAW | CS_HREDRAW;
	winClassTopleft.lpfnWndProc		= TopLeftWndProc;
	winClassTopleft.cbClsExtra		= 0;
	winClassTopleft.cbWndExtra		= 0;
	winClassTopleft.hInstance		= hInstance;
	winClassTopleft.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DXMULTIVIEWWIN32)) ;
	winClassTopleft.hCursor			= LoadCursor(NULL, IDC_ARROW);
	winClassTopleft.hbrBackground	= NULL ;
	winClassTopleft.lpszMenuName	= NULL ;
	winClassTopleft.lpszClassName	= TOPLEFT_WINDOW_CLASS;
	winClassTopleft.hIconSm			= LoadIcon(winClassTopleft.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	
	// Register the top-left window
	/*wcex.lpfnWndProc		= TopLeftWndProc;
	wcex.lpszClassName	= TOPLEFT_WINDOW_CLASS;*/
	returnValue = RegisterClassEx(&winClassTopleft) ;
	if (returnValue == 0)
	{
		return 0 ;
	}

	// Register the top-right window
	wcex.lpfnWndProc = TopRightWndProc ;
	wcex.lpszClassName = TOPRIGHT_WINDOW_CLASS ;
	returnValue = RegisterClassEx(&wcex) ;
	if (returnValue == 0)
	{
		return 0 ;
	}

	// Register the bottom-left window
	wcex.lpfnWndProc = BottomLeftWndProc ;
	wcex.lpszClassName = BOTTOMLEFT_WINDOW_CLASS ;
	returnValue = RegisterClassEx(&wcex) ;
	if (returnValue == 0)
	{
		return 0 ;
	}

	// Register the bottom-right window
	wcex.lpfnWndProc = BottomRightWndProc ;
	wcex.lpszClassName = BOTTOMRIGHT_WINDOW_CLASS ;
	returnValue = RegisterClassEx(&wcex) ;
	if (returnValue == 0)
	{
		return 0 ;
	}

	return 1 ;
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, 
	   szTitle, 
	   WS_OVERLAPPEDWINDOW | WS_SYSMENU,
	   0, 
	   0, 
	   600, 
	   600, 
	   NULL, 
	   NULL, 
	   hInstance, 
	   NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	// Mouse position(screen coordinates)
	UINT x = -1 ;
	UINT y = -1 ;

	// Mouse position(client coordinates)
	UINT mouseX = -1 ;
	UINT mouseY = -1 ;

	RECT rect ;
	GetClientRect(hWnd, &rect) ;

	switch (message)
	{
	case WM_CREATE:
		InitD3D(hWnd);
		CreateChildWindows(hWnd, hInst) ;
		CreateSwapChains(hWnd, (rect.right - BorderWidth) / 2, (rect.bottom - BorderWidth) / 2) ;
		break ;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_ERASEBKGND:
		UpdateWindow(hWnd) ;
		break ;

		// Mouse wheel message need additional processing
	case WM_MOUSEWHEEL:
		/*x = LOWORD(lParam) ;
		y = HIWORD(lParam) ;
		mouseX = ScreenToClient() ;
		if ((x >= 0 && x <= (rect.right - BorderWidth) / 2) && 
			(y >= 0 && y <= (rect.bottom - BorderWidth) / 2))
		{
			SendMessage(g_topLefthWnd, message, wParam, lParam) ;
		}
		*/
		break ;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		/// !!! Do not use return TRUE, or there is no title bar for main window !!!
		//return TRUE ; 
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void SetupMatrix(Camera* pCamera)
{
	// Get world matrix
	D3DXMATRIX world = *pCamera->GetWorldMatrix() ;
	g_pDevice->SetTransform(D3DTS_WORLD, &world) ;

	// Get view matrix
	D3DXMATRIX view = *pCamera->GetViewMatrix() ;
	g_pDevice->SetTransform(D3DTS_VIEW, &view) ;

	// Get Projection matrix
	D3DXMATRIX proj = *pCamera->GetProjMatrix() ;
	g_pDevice->SetTransform(D3DTS_PROJECTION, &proj) ;
}

// Create all 4 child windows
VOID CreateChildWindows(HWND hWnd, HINSTANCE hInst)
{
	// Get parent window client area
	RECT rect ;
	GetClientRect(hWnd, &rect) ;

	int width = (rect.right - BorderWidth) / 2 ;
	int height = (rect.bottom - BorderWidth) / 2 ;

	// Create top-left window
	CreateChildWindow(hWnd, 
		g_topLefthWnd,
		hInst, 
		TOPLEFT_WINDOW_CLASS, 
		rect.left, 
		rect.top, 
		width, 
		height) ;

	// Create top-right window
	CreateChildWindow(hWnd, 
		g_topRighthWnd,
		hInst, 
		TOPRIGHT_WINDOW_CLASS, 
		rect.left + width + BorderWidth, 
		rect.top, 
		width, 
		height) ;
	
	// Create bottom-left window
	CreateChildWindow(hWnd, 
		g_bottomLefthWnd,
		hInst, 
		BOTTOMLEFT_WINDOW_CLASS, 
		rect.left, 
		rect.top + height + BorderWidth, 
		width, 
		height) ;

	// Create bottom-right window
	CreateChildWindow(hWnd, 
		g_bottomRighthWnd,
		hInst, 
		BOTTOMRIGHT_WINDOW_CLASS, 
		rect.left + width + BorderWidth, 
		rect.top + height + BorderWidth, 
		width, 
		height) ;
}

// Create a child window
// hWnd: parent window handle
// childHwnd: child window handle
// hInst: Instance
// x, y: position
// width, height: size
VOID CreateChildWindow(HWND hWnd, 
					   HWND& childHwnd, 
					   HINSTANCE hInst, 
					   LPCSTR className, 
					   int x, 
					   int y, 
					   int width, 
					   int height)
{
	// Create window
	childHwnd = CreateWindowEx(WS_EX_CLIENTEDGE, 
		className, 
		"", 
		WS_CHILD | WS_VISIBLE | SS_SUNKEN,
		x, 
		y,
		width, 
		height,
		hWnd, 
		NULL, 
		hInst, 
		NULL);

	// Show window
	if(childHwnd != NULL)
	{
		ShowWindow(childHwnd, SW_SHOW);
		UpdateWindow(childHwnd);
	}
}

LRESULT CALLBACK TopLeftWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rect;
	static HBRUSH	hleft_Wnd_brush = NULL;

	switch (message)
	{
	case WM_CREATE :
		GetClientRect(hWnd, &rect) ;
		InitializeCamera(hWnd, g_pCamera1) ;
		g_pCamera1->SetWindow(rect.right, rect.bottom) ;
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		GetClientRect(hWnd, &rect);

		//Paint the window with the color specified by the user
		hleft_Wnd_brush = CreateSolidBrush(RGB(1, 1, 1));

		FillRect(hdc, &rect, hleft_Wnd_brush);

		DeleteObject(hleft_Wnd_brush);

		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
	//	break;
	}

	g_pCamera1->HandleMessages(hWnd, message, wParam, lParam) ;
	OutputDebugString("top-left\n") ;

	return TRUE ;

	// !!! Do not use the following clause, or the WM_MOUSEWHEEL Message will not be processed by child window !!!
	//return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT CALLBACK TopRightWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rect;
	static HBRUSH	hleft_Wnd_brush = NULL;

	switch (message)
	{
	case WM_CREATE :
		GetClientRect(hWnd, &rect) ;
		InitializeCamera(hWnd, g_pCamera2) ;
		g_pCamera2->SetWindow(rect.right, rect.bottom) ;
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		GetClientRect(hWnd, &rect);

		//Paint the window with the color specified by the user
		hleft_Wnd_brush = CreateSolidBrush(RGB(1, 1, 1));

		FillRect(hdc, &rect, hleft_Wnd_brush);

		DeleteObject(hleft_Wnd_brush);

		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	}

	g_pCamera2->HandleMessages(hWnd, message, wParam, lParam) ;
	OutputDebugString("top-right\n") ;

	return TRUE ;

	//return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT CALLBACK BottomLeftWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rect;
	static HBRUSH	hleft_Wnd_brush = NULL;

	switch (message)
	{
	case WM_CREATE :
		GetClientRect(hWnd, &rect) ;
		InitializeCamera(hWnd, g_pCamera3) ;
		g_pCamera3->SetWindow(rect.right, rect.bottom) ;
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		GetClientRect(hWnd, &rect);

		//Paint the window with the color specified by the user
		hleft_Wnd_brush = CreateSolidBrush(RGB(1, 1, 1));

		FillRect(hdc, &rect, hleft_Wnd_brush);

		DeleteObject(hleft_Wnd_brush);

		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	}

	g_pCamera3->HandleMessages(hWnd, message, wParam, lParam) ;
	OutputDebugString("Bottom-left\n") ;

	return TRUE ;

	//return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT CALLBACK BottomRightWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rect;
	static HBRUSH	hleft_Wnd_brush = NULL;

	switch (message)
	{
	case WM_CREATE :
		GetClientRect(hWnd, &rect) ;
		g_pCamera4->SetWindow(rect.right, rect.bottom) ;
		InitializeCamera(hWnd, g_pCamera4) ;

		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		GetClientRect(hWnd, &rect);

		//Paint the window with the color specified by the user
		hleft_Wnd_brush = CreateSolidBrush(RGB(1, 1, 1));

		FillRect(hdc, &rect, hleft_Wnd_brush);

		DeleteObject(hleft_Wnd_brush);

		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	}

	g_pCamera4->HandleMessages(hWnd, message, wParam, lParam) ;
	OutputDebugString("Bottom-right\n") ;

	return TRUE ;

	//return DefWindowProc(hWnd, message, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////
void InitD3D(HWND hWnd)
{
	RECT rect ;
	GetClientRect(hWnd, &rect) ;

	HRESULT hr ;

	// Create D3D
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (g_pD3D == NULL)
	{
		DXTRACE_MSG("Create D3D failed!") ;
		return ; 
	}

	// Get display mode
	D3DDISPLAYMODE d3ddm ;
	hr = g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm) ;
	if (FAILED(hr))
	{
		DXTRACE_ERR_MSGBOX("Get display mode failed!", hr) ;
		return ; 
	}

	// Create Device
	D3DPRESENT_PARAMETERS d3dpp ;
	ZeroMemory(&d3dpp, sizeof(d3dpp)) ;

	d3dpp.Windowed = TRUE ;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD ;
	d3dpp.BackBufferCount = 1 ;
	d3dpp.BackBufferWidth = rect.right ;
	d3dpp.BackBufferHeight = rect.bottom ;
	d3dpp.BackBufferFormat = d3ddm.Format ;
	d3dpp.EnableAutoDepthStencil = TRUE ;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16 ;

	hr = g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, 
		D3DDEVTYPE_HAL, 
		hWnd, 
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, 
		&d3dpp, 
		&g_pDevice) ;
	if (FAILED(hr))
	{
		DXTRACE_ERR_MSGBOX("Create device failed!", hr) ;
		return ;
	}

	// Enable z-buffer
	g_pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE) ;

	D3DXCreateTeapot(g_pDevice, &g_pTeapot, NULL) ;
	D3DXCreateSphere(g_pDevice, 1, 20, 20, &g_pSphere, NULL) ;
	D3DXCreateCylinder(g_pDevice, 1, 1, 3, 20, 10, &g_pCylinder, NULL) ;
	D3DXCreateTorus(g_pDevice, 0.5, 1, 20, 20, &g_pTorus, NULL) ;
}

void InitializeCamera(HWND hWnd, Camera* pCamera)
{
	// Set world matrix
	D3DXMATRIX world ;
	D3DXMatrixTranslation(&world, 0.0f, 0.0f, 0.0f) ;
	pCamera->SetWorldMatrix(&world) ;

	// Set view matrix
	D3DXMATRIX view ;
	D3DXVECTOR3 eyePt(0.0f, 0.0f, -10.0f) ;
	D3DXVECTOR3 lookAt(0.0f, 0.0f, 0.0f) ;
	D3DXVECTOR3 upVector(0.0f, 1.0f, 0.0f) ;
	pCamera->SetViewParams(&eyePt, &lookAt, &upVector) ;

	RECT rect ;
	GetClientRect(hWnd, &rect) ;
	float aspect = (float)rect.right / rect.bottom ;
	pCamera->SetProjParams(D3DX_PI / 4, aspect, 1.0f, 1000.f) ;
}

void CreateSwapChains(HWND hWnd, int width, int height)
{
	g_pSwapChain1->Create(g_pD3D, g_pDevice, g_topLefthWnd, 0xffff0000, FALSE, width, height) ;
	
	/*g_pDevice->GetSwapChain(0, &g_pDefaultSwapChain) ;
	g_pSwapChain1->SetSwapChain(g_pDefaultSwapChain) ;*/

	g_pSwapChain2->Create(g_pD3D, g_pDevice, g_topRighthWnd, 0xffffff00, FALSE, width, height) ;
	g_pSwapChain3->Create(g_pD3D, g_pDevice, g_bottomLefthWnd, 0xff0000ff, FALSE, width, height) ;
	g_pSwapChain4->Create(g_pD3D, g_pDevice, g_bottomRighthWnd, 0xff00ff00, FALSE, width, height) ;
}

void Render()
{
	// Swap chain 1
	g_pCamera1->OnFrameMove() ;
	SetupMatrix(g_pCamera1) ;
	g_pSwapChain1->BeginScene() ;
	g_pTeapot->DrawSubset(0) ;
	g_pSwapChain1->EndScene() ;
	g_pSwapChain1->Present() ;

	// Swap chain 2
	g_pCamera2->OnFrameMove() ;
	SetupMatrix(g_pCamera2) ;
	g_pSwapChain2->BeginScene() ;
	g_pSphere->DrawSubset(0) ;
	g_pSwapChain2->EndScene() ;
	g_pSwapChain2->Present() ;

	// Swap chain 3
	g_pCamera3->OnFrameMove() ;
	SetupMatrix(g_pCamera3) ;
	g_pSwapChain3->BeginScene() ;
	g_pCylinder->DrawSubset(0) ;
	g_pSwapChain3->EndScene() ;
	g_pSwapChain3->Present() ;

	// Swap chain 4
	g_pCamera4->OnFrameMove() ;
	SetupMatrix(g_pCamera4) ;
	g_pSwapChain4->BeginScene() ;
	g_pTorus->DrawSubset(0) ;
	g_pSwapChain4->EndScene() ;
	g_pSwapChain4->Present() ;
}

void Release()
{
	SAFE_RELEASE(g_pTeapot) ;
	SAFE_RELEASE(g_pSphere) ;
	SAFE_RELEASE(g_pCylinder) ;
	SAFE_RELEASE(g_pTorus) ;
	SAFE_RELEASE(g_pDevice) ;
	SAFE_RELEASE(g_pD3D) ;
}