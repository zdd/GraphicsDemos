#include <d3dx9.h>
#include <MMSystem.h>

LPDIRECT3D9             g_pD3D				= NULL ; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice		= NULL ; // Our rendering device
ID3DXMesh*				g_pTeapotMesh		= NULL ; // Hold the teapot
RECT					g_Rect ;					 // Client rect					

#define SAFE_RELEASE(P) if(P){ P->Release(); P = NULL;}

void SetupViewport(int x, int y, int width, int height, float minZ, float maxZ)
{
	D3DVIEWPORT9 viewport = {x, y, width, height, minZ, maxZ} ;
	HRESULT hr = g_pd3dDevice->SetViewport(&viewport) ;
	if (FAILED(hr))
	{
		MessageBox(NULL, "Set up view port failed!", "Error", 0) ;
	}
}

HRESULT InitD3D( HWND hWnd )
{
	// Create the D3D object, which is needed to create the D3DDevice.
	if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
	{
		MessageBoxA(NULL, "Create D3D9 object failed!", "Error", 0) ;
		return E_FAIL;
	}

	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory( &d3dpp, sizeof(d3dpp) );

	d3dpp.Windowed = TRUE; // use window mode, not full screen
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	// Create device
	if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice ) ) )
	{
		MessageBoxA(NULL, "Create D3D9 device failed!", "Error", 0) ;
		return E_FAIL;
	}

	g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME) ;

	// Create teapot
	D3DXCreateTeapot(g_pd3dDevice, &g_pTeapotMesh, NULL) ;

	// Translate model to origin
	D3DXMATRIX world ;
	D3DXMatrixTranslation(&world, 0.0f, 0.0f, 0.0f) ;
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &world) ;

	// Set up projection matrix
	D3DXMATRIX proj ;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI / 4, 1.0f, 1.0f, 1000.0f) ;
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &proj) ;

	return S_OK;
}

VOID Cleanup()
{
	SAFE_RELEASE(g_pTeapotMesh) ;
	SAFE_RELEASE(g_pd3dDevice) ;
	SAFE_RELEASE(g_pD3D) ;
}

// Set up the camera
void SetupCamera(D3DXVECTOR3* eyePt, D3DXVECTOR3* lookAt, D3DXVECTOR3* upVec)
{
	D3DXMATRIX view ;
	D3DXMatrixLookAtLH(&view, eyePt, lookAt, upVec) ;
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &view) ;
}

void Draw(D3DVIEWPORT9* viewport, DWORD color)
{
	g_pd3dDevice->SetViewport(viewport) ;
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, color, 1.0f, 0 );

	// Begin the scene
	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		// Draw teapot 
		g_pTeapotMesh->DrawSubset(0) ;

		// End the scene
		g_pd3dDevice->EndScene();
	}
}

VOID Render()
{
	int vpWidth = g_Rect.right / 2 ;		// viewport width
	int vpHeight = g_Rect.bottom / 2 ;		// viewport height

	// Setup camera, front view
	D3DXVECTOR3 eyePt(0.0f, 0.0f, -5.0f) ;
	D3DXVECTOR3 lookAt(0.0f, 0.0f, 0.0f) ;
	D3DXVECTOR3 upVec(0.0f, 1.0f, 0.0f) ;
	SetupCamera(&eyePt, &lookAt, &upVec) ;
	// Draw top-left viewport
	D3DVIEWPORT9 viewport1 = {0, 0, vpWidth, vpHeight, 0.0f, 1.0f} ;
	Draw(&viewport1, 0xffff0000) ;

	// Setup camera, left view
	eyePt = D3DXVECTOR3(-5.0f, 0.0f, 0.0f) ;
	SetupCamera(&eyePt, &lookAt, &upVec) ;

	// Draw top-right viewport
	D3DVIEWPORT9 viewport2 = {vpWidth, 0, vpWidth, vpHeight, 0.0f, 1.0f} ;
	Draw(&viewport2, 0xff00ff00) ;

	// Setup camera, top view
	eyePt = D3DXVECTOR3(0.0f, 5.0f, 0.0f) ;
	upVec = D3DXVECTOR3(0.0f, 0.0f, 1.0f) ;
	SetupCamera(&eyePt, &lookAt, &upVec) ;

	// Draw bottom-left viewport
	D3DVIEWPORT9 viewport3 = {0, vpHeight, vpWidth, vpHeight, 0.0f, 1.0f} ;
	Draw(&viewport3, 0xff0000ff) ;

	// Setup camera, perspective view
	eyePt = D3DXVECTOR3(-3.0f, 3.0f, -3.0f) ;
	upVec = D3DXVECTOR3(1.0f, 2.0f, 1.0f) ;
	SetupCamera(&eyePt, &lookAt, &upVec) ;

	// Draw bottom-right viewport
	D3DVIEWPORT9 viewport4 = {vpWidth, vpHeight, vpWidth, vpHeight, 0.0f, 1.0f} ;
	Draw(&viewport4, 0xffffff00) ;

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

	case WM_SYSCOMMAND:							
		{
			switch (wParam)						
			{
			case SC_SCREENSAVE:					
			case SC_MONITORPOWER:				
				return 0;						
			}
			break;								
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

	winClass.lpszClassName = "MultiViewports";
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
		"MultiViewports",			// window caption
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

	GetClientRect(hWnd, &g_Rect) ;

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
				float timeDelta = (currTime - lastTime) * 0.001f;

				// Render
				Render() ;

				// Update last time to current time for next loop
				lastTime = currTime;
			}
		}
	}

	UnregisterClass(winClass.lpszClassName, hInstance) ;
	return 0;
}