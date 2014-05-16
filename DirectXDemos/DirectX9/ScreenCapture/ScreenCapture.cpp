/*

Capture screen and save to file
Usage: Press left button to capture, the image file was save under current program folder named "aaa.bmp"
ScreenShot(g_pd3dDevice, NULL, "aaa.bmp") ;

if you want to capture a window only, pass the window handle to function ScreenShot, like the code below
ScreenShot(g_pd3dDevice, hWnd, "aaa.bmp") ;

if you want to capture the full screen, just pass NULL
ScreenShot(g_pd3dDevice, NULL, "aaa.bmp") ;

if you want to capture part of the screen, just pass the rect to the last parameter of the function D3DXSaveSurfaceToFile, like below
D3DXSaveSurfaceToFile(fileName, D3DXIFF_BMP, surf, NULL, &rect);

*/

#include <d3dx9.h>
#include <DxErr.h>

LPDIRECT3D9             g_pD3D			= NULL ; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice	= NULL ; // Our rendering device
ID3DXMesh*				g_pTeapotMesh			= NULL ; // Mesh to hold the teapot

int left = 0 ;
int right = 0 ;
int top = 0 ;
int bottom = 0 ;
RECT rect ;

HRESULT InitD3D( HWND hWnd )
{
	// Create the D3D object, which is needed to create the D3DDevice.
	if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		return E_FAIL;

	D3DDISPLAYMODE	ddm;
	if(FAILED(g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&ddm)))
	{
		//DXTRACE_ERR_MSGBOX("Unable to Get Adapter Display Mode");
		return E_FAIL;
	}

	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	d3dpp.Windowed = TRUE;
	d3dpp.BackBufferWidth = ddm.Width ;
	d3dpp.BackBufferHeight = ddm.Width ;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	// Create device
	if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice ) ) )
	{
		return E_FAIL;
	}

	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE) ;
	g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME) ;

	D3DXCreateTeapot(g_pd3dDevice, &g_pTeapotMesh, NULL) ;

	return S_OK;
}

VOID Cleanup()
{
	if( g_pd3dDevice != NULL) 
		g_pd3dDevice->Release() ;

	if( g_pD3D != NULL)
		g_pD3D->Release() ;

	if(g_pTeapotMesh != NULL)
		g_pTeapotMesh->Release() ;
}

void SetupMatrix()
{

	D3DXVECTOR3 eyePt(0.0f, 0.0f, -10.0f) ;
	D3DXVECTOR3 upVec(0.0f, 1.0f, 0.0f) ;
	D3DXVECTOR3 lookAt(0.0f, 0.0f, 0.0f) ;

	D3DXMATRIX view ;
	D3DXMatrixLookAtLH(&view, &eyePt, &lookAt, &upVec) ;

	g_pd3dDevice->SetTransform(D3DTS_VIEW, &view) ;

	D3DXMATRIX proj ;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI / 4, 1.0f, 1.0f, 1000.0f) ;
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &proj) ;
}

VOID Render()
{
	if( NULL == g_pd3dDevice )
		return;

	SetupMatrix() ;

	// Clear the back-buffer to a RED color
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );

	// Begin the scene
	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		// Rendering of scene objects can happen here

		g_pTeapotMesh->DrawSubset(0) ;

		// End the scene
		g_pd3dDevice->EndScene();
	}

	// Present the back-buffer contents to the display
	g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}

BOOL ScreenShot(LPDIRECT3DDEVICE9 lpDevice, HWND hWnd, TCHAR* fileName)
{
	HRESULT hr;
	
	// Get adapter display mode
	D3DDISPLAYMODE mode;
	if (FAILED(hr = lpDevice->GetDisplayMode(0, &mode)))
		return hr;

	// Create the surface to hold the screen image data
	LPDIRECT3DSURFACE9 surf;
	if (FAILED(hr = lpDevice->CreateOffscreenPlainSurface(mode.Width, 
		mode.Height, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &surf, NULL))) //注意第四个参数不能是D3DPOOL_DEFAULT
	{
		return hr;
	}

	// Get the screen data
	if (FAILED(hr = lpDevice->GetFrontBufferData(0, surf))) 
	{
		surf->Release() ;
		return hr ;
	}

	// area to capture
	RECT *rect = NULL ;

	WINDOWINFO windowInfo ;
	windowInfo.cbSize = sizeof(WINDOWINFO) ;

	if(hWnd) // capture window
	{
		GetWindowInfo(hWnd, &windowInfo) ;
		rect = &windowInfo.rcWindow ;
	}

	// Save the screen date to file
	hr = D3DXSaveSurfaceToFile(fileName, D3DXIFF_BMP, surf, NULL, rect);

	surf->Release() ;

	return hr ;
}

BOOL ScreenShot1(LPDIRECT3DDEVICE9 lpDevice, HWND hWnd, TCHAR* fileName)
{
	HRESULT hr;

	// get display dimensions
	// this will be the dimensions of the front buffer
	DWORD SurfaceWidth = 0 ;
	DWORD SurfaceHeight = 0 ;

	D3DDISPLAYMODE mode;
	if (FAILED(hr = lpDevice->GetDisplayMode(0, &mode)))
		return hr;

	if(hWnd == NULL) // Capture full screen
	{
		D3DDISPLAYMODE mode;
		if (FAILED(hr = lpDevice->GetDisplayMode(0, &mode)))
			return hr;
		SurfaceWidth = mode.Width ;
		SurfaceHeight = mode.Height ;
	}

	else // Capture current window, not worked yet!
	{
		RECT rect ;
		GetWindowRect(hWnd, &rect) ;
		SurfaceWidth = rect.right ;
		SurfaceHeight = rect.bottom ;
	}
	

	// create the image surface to store the front buffer image
	// note that call to GetFrontBuffer will always convert format to A8R8G8B8
	LPDIRECT3DSURFACE9 surf;
	if (FAILED(hr = lpDevice->CreateOffscreenPlainSurface(SurfaceWidth, 
		SurfaceHeight, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &surf, NULL))) //注意第四个参数不能是D3DPOOL_DEFAULT
	{
		return hr;
	}

	LPDIRECT3DSURFACE9 surf1;
	if (FAILED(hr = lpDevice->CreateOffscreenPlainSurface(mode.Width, 
		mode.Height, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &surf1, NULL))) //注意第四个参数不能是D3DPOOL_DEFAULT
	{
		return hr;
	}

	// read the front buffer into the image surface
	if (FAILED(hr = lpDevice->GetFrontBufferData(0, surf1))) 
	{
		int i = 0 ;
		if(hr == D3DERR_DRIVERINTERNALERROR)
		{
			i = 1 ;
		}
		if(hr == D3DERR_DEVICELOST)
		{
			i = 2 ;
		}
		if(hr == D3DERR_INVALIDCALL)
		{
			i = 3 ;
		}

		surf->Release();
		return hr;
	}

	
	RECT rect = {0, 0, SurfaceWidth, SurfaceHeight} ;

	if(hWnd)
	{
		POINT point = {0, 0} ;

		lpDevice->UpdateSurface(surf1, &rect, surf, &point) ;
	}

	// write the entire surface to the requested file
	hr = D3DXSaveSurfaceToFile(fileName, D3DXIFF_BMP, surf1, NULL, &rect);

	// release the image surface
	surf->Release();

	// return status of save operation to caller
	return hr;
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

			case 'S':
				//ScreenShot(g_pd3dDevice, hWnd, "Screenshot.bmp");
				break ;

			default:
				break ;
			}
		}
		break ;
	case WM_LBUTTONDOWN:
		left = ( short )LOWORD( lParam );
		top = ( short )HIWORD( lParam );
		break ;

	case WM_LBUTTONUP:
		right = ( short )LOWORD( lParam );
		bottom = ( short )HIWORD( lParam );

		rect.left = min(left, right) ;
		rect.right = max(left, right) ;
		rect.top = min(top, bottom) ;
		rect.bottom = max(top, bottom) ;
		// 调用截图函数

		//rect = {left, right, top, bottom} ;
		ScreenShot(g_pd3dDevice, NULL, "aaa.jpg") ;
		break ;

	case WM_DESTROY:
		Cleanup();
		PostQuitMessage( 0 );
		return 0;
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}

INT WINAPI wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, INT )
{
	// Register the window class
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L, 
		GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
		"D3D Tutoria", NULL };
	RegisterClassEx( &wc );

	// Create the application's window
	HWND hWnd = CreateWindow( "D3D Tutoria", "D3D Tutorial 01: CreateDevice", 
		WS_OVERLAPPEDWINDOW , 0, 0, 800, 600,
		NULL, NULL, wc.hInstance, NULL );

	// Initialize Direct3D
	if( SUCCEEDED( InitD3D( hWnd ) ) )
	{ 
		// Show the window
		ShowWindow( hWnd, SW_SHOWDEFAULT );
		UpdateWindow( hWnd );

		// Enter the message loop
		MSG    msg ; 
		ZeroMemory( &msg, sizeof(msg) );
		PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );


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
	}

	UnregisterClass( "D3D Tutoria", wc.hInstance );
	return 0;
}



