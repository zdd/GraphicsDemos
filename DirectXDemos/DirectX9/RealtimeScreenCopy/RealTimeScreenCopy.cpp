/*
This Demo show you how to synchronize a desktop content to a window
for the sake of brevity, we create a popup window at the right half of the screen
and synchronize the content of the left screen to it
*/
#define D3D_DEBUG_INFO
#include <d3dx9.h> 
#include <DxErr.h>
#include <dxdiag.h>

LPDIRECT3D9             g_pD3D			= NULL ; // Used to create the D3DDevice 
LPDIRECT3DDEVICE9       g_pd3dDevice	= NULL ; // Our rendering device 
IDirect3DSurface9*		g_pBackBuffer	= NULL ; // Back buffer for current device
IDirect3DSurface9*		g_pSourceSurface = NULL ; // Surface to hold the image
IDirect3DSwapChain9*	g_pSwapChain	= NULL ; 

D3DSURFACE_DESC pd3dsdBackBuffer ;
D3DPRESENT_PARAMETERS d3dpp ;
int ScreenWidth = -1;
int ScreenHeight = -1 ;

#define SAFE_RELEASE(p) if (p){p->Release() ; p = NULL ;}

HRESULT InitD3D( HWND hWnd ) 
{ 
	// Create the D3D object, which is needed to create the D3DDevice. 
	if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) ) 
	{ 
		MessageBoxA(NULL, "Create D3D9 object failed!", "Error", 0) ; 
		return E_FAIL; 
	} 

	D3DDISPLAYMODE	ddm;
	if(FAILED(g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&ddm)))
	{
		MessageBoxA(NULL, "Unable to Get Adapter Display Mode", "Error", 0) ; 
		return E_FAIL;
	}

	ZeroMemory( &d3dpp, sizeof(d3dpp) ); 

	d3dpp.Windowed = TRUE;  
	d3dpp.BackBufferCount = 1 ;
	d3dpp.BackBufferWidth = ddm.Width ;
	d3dpp.BackBufferHeight = ddm.Height ;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; 
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8; 

	// Create device 
	if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, 
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, 
		&d3dpp, &g_pd3dDevice ) ) ) 
	{ 
		MessageBoxA(NULL, "Create D3D9 device failed!", "Error", 0) ; 
		return E_FAIL; 
	} 

	// Get backbuffer
	g_pd3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &g_pBackBuffer) ;

	HRESULT hr ;
	if (FAILED(hr = g_pd3dDevice->CreateOffscreenPlainSurface(d3dpp.BackBufferWidth, 
	d3dpp.BackBufferHeight, d3dpp.BackBufferFormat, D3DPOOL_SYSTEMMEM, &g_pSourceSurface, NULL)))
	{
		return hr;
	}

	return S_OK; 
} 

VOID Cleanup() 
{ 
	// Release D3D 
	SAFE_RELEASE(g_pD3D) ; 

	// Release device 
	SAFE_RELEASE(g_pd3dDevice) ; 

	// Release surface
	SAFE_RELEASE(g_pSourceSurface) ;

	// Release backbuffer
	SAFE_RELEASE(g_pBackBuffer) ;
} 

BOOL ScreenShot()
{
	HRESULT hr;
	if (FAILED(hr = g_pd3dDevice->GetFrontBufferData(0, g_pSourceSurface))) 
	{
		return hr ;
	}
	
	// area to capture
	RECT rect ;
	rect.left = 0 ;
	rect.right =  ScreenWidth / 2;
	rect.top = 0 ;
	rect.bottom = ScreenHeight ;

	hr = D3DXLoadSurfaceFromSurface(g_pBackBuffer, NULL, NULL, g_pSourceSurface, NULL, NULL, D3DX_DEFAULT, NULL) ;
	//hr = g_pd3dDevice->UpdateSurface(g_pSourceSurface, &rect, g_pBackBuffer, NULL) ;
	if (FAILED(hr)) 
	{
		return hr ;
	}

	return hr ;
}

VOID Render(FLOAT timeElapse) 
{ 
	//// Clear the back-buffer to a RED color 
	//HRESULT hr ;
	//hr = g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0); 
	//if (FAILED(hr))
	//{
	//	DXTRACE_ERR_MSGBOX(DXGetErrorDescription(hr), hr) ;
	//}

	ScreenShot() ;

	// Present the back-buffer contents to the display 
	g_pd3dDevice->Present( NULL, NULL, NULL, NULL ); 
} 

LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) 
{ 
	switch( msg ) 
	{ 
	case WM_KEYDOWN: 
		{ 
			switch( wParam ) 
			{ 
			case VK_ESCAPE: 
				SendMessage( hWnd, WM_CLOSE, 0, 0 ); 
				break ; 
			default: 
				break ; 
			} 
		} 
		break ; 

	case WM_DESTROY: 
		Cleanup(); 
		PostQuitMessage( 0 ); 
		return 0; 
	} 

	return DefWindowProc( hWnd, msg, wParam, lParam ); 
} 

INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR szCmdLine, int iCmdShow) 
{ 
	WNDCLASSEX winClass ; 

	winClass.lpszClassName = "Teapot"; 
	winClass.cbSize        = sizeof(WNDCLASSEX); 
	winClass.style         = CS_HREDRAW | CS_VREDRAW; 
	winClass.lpfnWndProc   = MsgProc; 
	winClass.hInstance     = hInstance; 
	winClass.hIcon         = NULL ; 
	winClass.hIconSm       = NULL ; 
	winClass.hCursor       = LoadCursor(NULL, IDC_ARROW) ; 
	winClass.hbrBackground = NULL ; 
	winClass.lpszMenuName  = NULL ; 
	winClass.cbClsExtra    = 0; 
	winClass.cbWndExtra    = 0; 

	RegisterClassEx (&winClass) ;   

	ScreenWidth = GetSystemMetrics(SM_CXSCREEN) ;
	ScreenHeight = GetSystemMetrics(SM_CYSCREEN) ;

	HWND hWnd = CreateWindowEx(NULL,   
		winClass.lpszClassName,	// window class name 
		"Teapot",				// window caption 
		WS_POPUP,				// window style 
		ScreenWidth / 2,		// initial x position 
		0,						// initial y position 
		ScreenWidth,			// initial window width 
		ScreenHeight,			// initial window height 
		NULL,					// parent window handle 
		NULL,					// window menu handle 
		hInstance,				// program instance handle 
		NULL) ;					// creation parameters 

	// Create window failed 
	if(hWnd == NULL) 
	{ 
		MessageBoxA(hWnd, "Create Window failed!", "Error", 0) ; 
		return -1 ; 
	} 

	// Initialize Direct3D 
	if( SUCCEEDED(InitD3D(hWnd))) 
	{  
		// Show the window 
		ShowWindow( hWnd, SW_SHOWDEFAULT ); 
		UpdateWindow( hWnd ); 

		// Enter the message loop 
		MSG    msg ;  
		ZeroMemory( &msg, sizeof(msg) ); 
		PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE ); 

		// Get last time 
		static DWORD lastTime = timeGetTime(); 

		while (msg.message != WM_QUIT)   
		{ 
			if( PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) != 0) 
			{ 
				TranslateMessage (&msg) ; 
				DispatchMessage (&msg) ; 
			} 
			else // Render the game if there is no message to process 
			{ 
				// Get current time 
				DWORD currTime  = timeGetTime(); 

				// Calculate time elapsed 
				float timeDelta = (currTime - lastTime)*0.001f; 

				// Render 
				Render(timeDelta) ; 

				// Update last time to current time for next loop 
				lastTime = currTime; 
			} 
		} 
	} 

	UnregisterClass(winClass.lpszClassName, hInstance) ; 
	return 0; 
} 
