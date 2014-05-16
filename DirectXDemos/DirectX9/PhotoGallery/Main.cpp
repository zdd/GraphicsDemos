#include <d3dx9.h>
#include "FirstPersonCamera.h"

LPDIRECT3D9             g_pD3D				= NULL ; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice		= NULL ; // Our rendering device
ID3DXMesh*				g_pMesh				= NULL ; // Hold the teapot

FirstPersonCamera*		g_Camera			= NULL ; // First person camera
DXInput*				g_Input				= NULL ; // DirectX input

HRESULT InitD3D( HWND hWnd )
{
	// Create the D3D object, which is needed to create the D3DDevice.
	if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
	{
		MessageBoxA(NULL, "Create D3D9 object failed!", "Error", 0) ;
		return E_FAIL;
	}

	//D3DPRESENT_PARAMETERS d3dpp; 
	//ZeroMemory( &d3dpp, sizeof(d3dpp) );

	//d3dpp.Windowed = TRUE; // use window mode, not full screen
	//d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	//d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	// Enumerate Display settings for current display device

	DWORD ScreenW = 0;
	DWORD ScreenH = 0;

	DEVMODE devMode ;
	devMode.dmSize = sizeof(devMode) ;
	DWORD iModeNum = 0 ;
	DWORD r = 1 ;

	while(r != 0)
	{
		r = EnumDisplaySettings(NULL, iModeNum, &devMode) ;
		if(devMode.dmPelsWidth >= ScreenW && devMode.dmPelsHeight >= ScreenH)
		{
			ScreenW = devMode.dmPelsWidth ;
			ScreenH = devMode.dmPelsHeight ;
		}

		iModeNum++ ;
	}

	D3DPRESENT_PARAMETERS d3dpp;
	memset(&d3dpp, 0, sizeof(d3dpp));

	d3dpp.Windowed               = FALSE;
	d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferWidth        = ScreenW; // no hard code here, should set to maximum width
	d3dpp.BackBufferHeight       = ScreenH; // no hard code here, set to maximum height
	d3dpp.BackBufferFormat       = D3DFMT_X8R8G8B8;

	// Create device
	if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice ) ) )
	{
		MessageBoxA(NULL, "Create D3D9 device failed!", "Error", 0) ;
		return E_FAIL;
	}

	// Disable lighting, since we didn't specify color for vertex
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING , FALSE );   

	// Create teapot
	D3DXCreateTeapot(g_pd3dDevice, &g_pMesh, NULL) ;

	g_Camera = new FirstPersonCamera() ;
	// Setup view matrix
	D3DXVECTOR3 pos(0.0f, 0.0f, -10.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	g_Camera->SetViewParams(&pos, &target, &up) ;

	// Setup projection matrix
	g_Camera->SetProjParams(D3DX_PI / 4, 1.0f, 1.0f, 1000.0f) ;

	// 
	g_Input = new DXInput() ;
	g_Input->Init() ;

	return S_OK;
}


VOID Cleanup()
{
	// Release teapot mesh
	if(g_pMesh != NULL)
		g_pMesh->Release() ;

	// Release D3D
	if( g_pD3D != NULL)
		g_pD3D->Release();

	// Release device
	if( g_pd3dDevice != NULL) 
		g_pd3dDevice->Release();
}

void SetupMatrix()
{
	// Get view matrix from camera
	D3DXMATRIX matView = *g_Camera->GetViewMatrix() ;
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView) ;

	// Get projection matrix from camera
	D3DXMATRIX matProj = *g_Camera->GetProjMatrix() ;
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj) ;
}

void OnFrameMove(float timeDelta)
{
	g_Camera->HandleInput(g_Input) ;
	g_Camera->OnFrameMove(timeDelta) ;
}

VOID Render(float timeDelta)
{
	OnFrameMove(timeDelta) ;

	SetupMatrix() ;

	// Clear the back-buffer to a RED color
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );

	// Begin the scene
	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		// Draw teapot 
		g_pMesh->DrawSubset(0) ;

		// End the scene
		g_pd3dDevice->EndScene();
	}

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

	winClass.lpszClassName = L"Teapot";
	winClass.cbSize        = sizeof(WNDCLASSEX);
	winClass.style         = CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc   = MsgProc;
	winClass.hInstance     = hInstance;
	winClass.hIcon	       = NULL ;
	winClass.hIconSm	   = NULL ;
	winClass.hCursor       = LoadCursor(NULL, IDC_ARROW) ;
	winClass.hbrBackground = NULL ;
	winClass.lpszMenuName  = NULL ;
	winClass.cbClsExtra    = 0;
	winClass.cbWndExtra    = 0;

	RegisterClassEx (&winClass) ;  

	HWND hWnd = CreateWindowEx(NULL,  
		winClass.lpszClassName,		// window class name
		L"Teapot",					// window caption
		WS_OVERLAPPEDWINDOW, 		// window style
		32,							// initial x position
		32,							// initial y position
		600,						// initial window width
		600,						// initial window height
		NULL,						// parent window handle
		NULL,						// window menu handle
		hInstance,					// program instance handle
		NULL) ;						// creation parameters

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