/*
This demo show how to handle device lost, when device was lost, the only
function can reset the device is reset, before call reset, all resources 
that created using D3DPOOL_DEFAULT must be released, if you want to render
after reset device, you must re-create all the resources created by using
D3DPOOL_DEFAULT

To check the device lost function, run this demo and when the cube show in
the rendering window, press Ctrl + Alt + Delete to launch the task manager
then click cancel to return back, the cube should be rendered correctly, if you
didn't handle the device lost or didn't create the vertex buffer again after reset 
the device, you will got an error.
*/

#include <d3dx9.h>
#include <DxErr.h>
#include <MMSystem.h>

LPDIRECT3D9             g_pD3D				= NULL ; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice		= NULL ; // Our rendering device
ID3DXMesh*				g_pTeapotMesh		= NULL ; // Hold the teapot
LPDIRECT3DVERTEXBUFFER9 g_pVB        = NULL; // Buffer to hold vertices
D3DPRESENT_PARAMETERS	d3dpp; 
bool					g_bActive			= true ; // Is window active?

#define SAFE_RELEASE(P) if(P){ P->Release(); P = NULL;}

// A structure for our custom vertex type
struct CUSTOMVERTEX
{
	FLOAT x, y, z;	// vertex position
	DWORD color;	// vertex color
};
#define D3DFVF_CUSTOMVERTEX D3DFVF_DIFFUSE | D3DFVF_XYZ

HRESULT InitVB();

HRESULT InitD3D( HWND hWnd )
{
	// Create the D3D object, which is needed to create the D3DDevice.
	if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
	{
		MessageBoxA(NULL, "Create D3D9 object failed!", "Error", 0) ;
		return E_FAIL;
	}

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

	// Create teapot
	D3DXCreateTeapot(g_pd3dDevice, &g_pTeapotMesh, NULL) ;

	// Create vertex buffer
	InitVB();

	return S_OK;
}

HRESULT InitVB()
{
	CUSTOMVERTEX A = { -1.0f, -1.0f, -1.0f, 0xffff0000 }; // position and color
	CUSTOMVERTEX B = {  1.0f, -1.0f, -1.0f, 0x0000ffff };
	CUSTOMVERTEX C = {  1.0f, -1.0f,  1.0f, 0x00ffff00 };
	CUSTOMVERTEX D = { -1.0f, -1.0f,  1.0f, 0xff0000ff };
	CUSTOMVERTEX E = { -1.0f,  1.0f, -1.0f, 0x00ffff00 };
	CUSTOMVERTEX F = {  1.0f,  1.0f, -1.0f, 0xff0000ff };
	CUSTOMVERTEX G = {  1.0f,  1.0f,  1.0f, 0xffff0000 };
	CUSTOMVERTEX H = { -1.0f,  1.0f,  1.0f, 0x0000ffff };

	// 3 points build a triangle, totally 12 triangles
	// two triangles forms a square, totally 6 squares
	// 6 square forms a cube
	CUSTOMVERTEX vertices[] =
	{
		A, E, H, A, H, D, // front
		C, G, F, C, F, B, // back
		B, F, E, B, E, A, // left
		D, H, G, D, G, C, // right
		E, F, G, E, G, H, // top
		D, C, B, D, B, A, // bottom
	};

	// create vertex buffer
	if( FAILED( g_pd3dDevice->CreateVertexBuffer( 36 * sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX,
		D3DPOOL_DEFAULT, &g_pVB, NULL ) ) )
	{
		return E_FAIL;
	}

	// copy date to vertex buffer
	VOID* pVertices;
	if( FAILED( g_pVB->Lock( 0, sizeof(vertices), (void**)&pVertices, 0 ) ) )
		return E_FAIL;
	memcpy( pVertices, vertices, sizeof(vertices) );
	g_pVB->Unlock();

	return S_OK;
}

HRESULT ResetDevice(D3DPRESENT_PARAMETERS d3dpp)
{
	// Check device state
	HRESULT hr = g_pd3dDevice->TestCooperativeLevel() ;

	// Device can be reset now
	if (SUCCEEDED(hr) || hr == D3DERR_DEVICENOTRESET)
	{
		// Release resource allocated as D3DPOOL_DEFAULT
		g_pVB->Release();

		// Reset device
		HRESULT hr = g_pd3dDevice->Reset(&d3dpp) ;
		if (FAILED(hr))
		{
			const CHAR* errorString = DXGetErrorString(hr) ;
			DXTRACE_ERR_MSGBOX(errorString, hr) ;
		}

		// Recreate the vertex buffer, since it is create by using
		// D3DPOOL_DEFAULT, the function MUST placed here!!! why?
		// you cann't place it before the return clause of this function
		// what's the inner cause?
		InitVB();
	}
	// Device is still in lost state, wait
	else if (hr == D3DERR_DEVICELOST)
	{
		Sleep(25) ;
	}
	else // Other error, Show error box
	{
		const CHAR* errorString = DXGetErrorString(hr) ;
		DXTRACE_ERR_MSGBOX(errorString, hr) ;
	}

	return hr ;
}

VOID Cleanup()
{
	SAFE_RELEASE(g_pVB);
	SAFE_RELEASE(g_pd3dDevice) ;
	SAFE_RELEASE(g_pD3D) ;
}

void SetupMatrix()
{
	// translate model to origin
	D3DXMATRIX world ;
	D3DXMatrixTranslation(&world, 0.0f, 0.0f, 0.0f) ;
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &world) ;

	// set view
	D3DXVECTOR3 eyePt(0.0f, 5.0f, -5.0f) ;
	D3DXVECTOR3 upVec(0.0f, 1.0f, 0.0f) ;
	D3DXVECTOR3 lookCenter(0.0f, 0.0f, 0.0f) ;

	D3DXMATRIX view ;
	D3DXMatrixLookAtLH(&view, &eyePt, &lookCenter, &upVec) ;
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &view) ;

	// set projection
	D3DXMATRIX proj ;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI / 4, 1.0f, 1.0f, 1000.0f) ;
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &proj) ;
}

void Render(float timeDelta)
{
	if (!g_bActive)
	{
		Sleep(50) ;
	}

	SetupMatrix() ;

	// Clear the back-buffer to a RED color
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );

	// Disable lighting, since we didn't specify color for vertex
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING , FALSE );   

	// Begin the scene
	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		// Draw teapot 
		g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof(CUSTOMVERTEX) );
		g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );

		// 12 triangles forms six faces and a cube
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 12 );


		// End the scene
		g_pd3dDevice->EndScene();
	}

	// Present the back-buffer contents to the display
	HRESULT hr = g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	if (FAILED(hr))
	{
		d3dpp.Windowed = TRUE; // use window mode, not full screen
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
		ResetDevice(d3dpp);
	}
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
		return 0 ;

	case WM_SIZE:
		if(wParam == SIZE_MAXHIDE || wParam == SIZE_MINIMIZED)
			g_bActive = false;
		else
			g_bActive = true;
		return 0;

	case WM_ACTIVATEAPP:
		if(wParam == TRUE)
			g_bActive = true ;
		else
			g_bActive = false ;
		return 0 ;

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
		"Teapot",					// window caption
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

		MSG msg ; 
		ZeroMemory( &msg, sizeof(msg) );
		PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );

		// Get last time
		static DWORD lastTime = timeGetTime();

		while (msg.message != WM_QUIT)  
		{
			if(PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) != 0)
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
				Render(timeDelta) ;

				// Update last time to current time for next loop
				lastTime = currTime;
			}
		}
	}

	UnregisterClass(winClass.lpszClassName, hInstance) ;
	return 0;
}