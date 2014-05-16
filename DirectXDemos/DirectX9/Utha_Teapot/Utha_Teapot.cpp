/*
Do you know why so many computer graphics books use the teapot?
Do you know the history of it?
Please see this wiki page http://www.sjbaker.org/wiki/index.php?title=The_History_of_The_Teapot

This demo show you how to draw a teapot manually, without using any sdk functions.
The index and vertex data are all from the wiki page above.

*/
#include <d3dx9.h>
#include <MMSystem.h>

LPDIRECT3D9             g_pD3D				= NULL ; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice		= NULL ; // Our rendering device
IDirect3DVertexBuffer9*	g_pVB				= NULL ; // vertex buffer pointer
IDirect3DIndexBuffer9*	g_pIB				= NULL ; // index buffer pointer

bool					g_bActive			= true ; // Is window active?

#define SAFE_RELEASE(P) if(P){ P->Release(); P = NULL;}
#define VERTEX_FVF D3DFVF_XYZ // vertex format

HRESULT InitVB()
{
	D3DXVECTOR3 vertex [] = {
		D3DXVECTOR3(  0.2000f,  0.0000f, 2.70000f ), D3DXVECTOR3(  0.2000f, -0.1120f, 2.70000f ),
		D3DXVECTOR3(  0.1120f, -0.2000f, 2.70000f ), D3DXVECTOR3(  0.0000f, -0.2000f, 2.70000f ),
		D3DXVECTOR3(  1.3375f,  0.0000f, 2.53125f ), D3DXVECTOR3(  1.3375f, -0.7490f, 2.53125f ),
		D3DXVECTOR3(  0.7490f, -1.3375f, 2.53125f ), D3DXVECTOR3(  0.0000f, -1.3375f, 2.53125f ),
		D3DXVECTOR3(  1.4375f,  0.0000f, 2.53125f ), D3DXVECTOR3(  1.4375f, -0.8050f, 2.53125f ),
		D3DXVECTOR3(  0.8050f, -1.4375f, 2.53125f ), D3DXVECTOR3(  0.0000f, -1.4375f, 2.53125f ),
		D3DXVECTOR3(  1.5000f,  0.0000f, 2.40000f ), D3DXVECTOR3(  1.5000f, -0.8400f, 2.40000f ),
		D3DXVECTOR3(  0.8400f, -1.5000f, 2.40000f ), D3DXVECTOR3(  0.0000f, -1.5000f, 2.40000f ),
		D3DXVECTOR3(  1.7500f,  0.0000f, 1.87500f ), D3DXVECTOR3(  1.7500f, -0.9800f, 1.87500f ),
		D3DXVECTOR3(  0.9800f, -1.7500f, 1.87500f ), D3DXVECTOR3(  0.0000f, -1.7500f, 1.87500f ),
		D3DXVECTOR3(  2.0000f,  0.0000f, 1.35000f ), D3DXVECTOR3(  2.0000f, -1.1200f, 1.35000f ),
		D3DXVECTOR3(  1.1200f, -2.0000f, 1.35000f ), D3DXVECTOR3(  0.0000f, -2.0000f, 1.35000f ),
		D3DXVECTOR3(  2.0000f,  0.0000f, 0.90000f ), D3DXVECTOR3(  2.0000f, -1.1200f, 0.90000f ),
		D3DXVECTOR3(  1.1200f, -2.0000f, 0.90000f ), D3DXVECTOR3(  0.0000f, -2.0000f, 0.90000f ),
		D3DXVECTOR3( -2.0000f,  0.0000f, 0.90000f ), D3DXVECTOR3(  2.0000f,  0.0000f, 0.45000f ),
		D3DXVECTOR3(  2.0000f, -1.1200f, 0.45000f ), D3DXVECTOR3(  1.1200f, -2.0000f, 0.45000f ),
		D3DXVECTOR3(  0.0000f, -2.0000f, 0.45000f ), D3DXVECTOR3(  1.5000f,  0.0000f, 0.22500f ),
		D3DXVECTOR3(  1.5000f, -0.8400f, 0.22500f ), D3DXVECTOR3(  0.8400f, -1.5000f, 0.22500f ),
		D3DXVECTOR3(  0.0000f, -1.5000f, 0.22500f ), D3DXVECTOR3(  1.5000f,  0.0000f, 0.15000f ),
		D3DXVECTOR3(  1.5000f, -0.8400f, 0.15000f ), D3DXVECTOR3(  0.8400f, -1.5000f, 0.15000f ),
		D3DXVECTOR3(  0.0000f, -1.5000f, 0.15000f ), D3DXVECTOR3( -1.6000f,  0.0000f, 2.02500f ),
		D3DXVECTOR3( -1.6000f, -0.3000f, 2.02500f ), D3DXVECTOR3( -1.5000f, -0.3000f, 2.25000f ),
		D3DXVECTOR3( -1.5000f,  0.0000f, 2.25000f ), D3DXVECTOR3( -2.3000f,  0.0000f, 2.02500f ),
		D3DXVECTOR3( -2.3000f, -0.3000f, 2.02500f ), D3DXVECTOR3( -2.5000f, -0.3000f, 2.25000f ),
		D3DXVECTOR3( -2.5000f,  0.0000f, 2.25000f ), D3DXVECTOR3( -2.7000f,  0.0000f, 2.02500f ),
		D3DXVECTOR3( -2.7000f, -0.3000f, 2.02500f ), D3DXVECTOR3( -3.0000f, -0.3000f, 2.25000f ),
		D3DXVECTOR3( -3.0000f,  0.0000f, 2.25000f ), D3DXVECTOR3( -2.7000f,  0.0000f, 1.80000f ),
		D3DXVECTOR3( -2.7000f, -0.3000f, 1.80000f ), D3DXVECTOR3( -3.0000f, -0.3000f, 1.80000f ),
		D3DXVECTOR3( -3.0000f,  0.0000f, 1.80000f ), D3DXVECTOR3( -2.7000f,  0.0000f, 1.57500f ),
		D3DXVECTOR3( -2.7000f, -0.3000f, 1.57500f ), D3DXVECTOR3( -3.0000f, -0.3000f, 1.35000f ),
		D3DXVECTOR3( -3.0000f,  0.0000f, 1.35000f ), D3DXVECTOR3( -2.5000f,  0.0000f, 1.12500f ),
		D3DXVECTOR3( -2.5000f, -0.3000f, 1.12500f ), D3DXVECTOR3( -2.6500f, -0.3000f, 0.93750f ),
		D3DXVECTOR3( -2.6500f,  0.0000f, 0.93750f ), D3DXVECTOR3( -2.0000f, -0.3000f, 0.90000f ),
		D3DXVECTOR3( -1.9000f, -0.3000f, 0.60000f ), D3DXVECTOR3( -1.9000f,  0.0000f, 0.60000f ),
		D3DXVECTOR3(  1.7000f,  0.0000f, 1.42500f ), D3DXVECTOR3(  1.7000f, -0.6600f, 1.42500f ),
		D3DXVECTOR3(  1.7000f, -0.6600f, 0.60000f ), D3DXVECTOR3(  1.7000f,  0.0000f, 0.60000f ),
		D3DXVECTOR3(  2.6000f,  0.0000f, 1.42500f ), D3DXVECTOR3(  2.6000f, -0.6600f, 1.42500f ),
		D3DXVECTOR3(  3.1000f, -0.6600f, 0.82500f ), D3DXVECTOR3(  3.1000f,  0.0000f, 0.82500f ),
		D3DXVECTOR3(  2.3000f,  0.0000f, 2.10000f ), D3DXVECTOR3(  2.3000f, -0.2500f, 2.10000f ),
		D3DXVECTOR3(  2.4000f, -0.2500f, 2.02500f ), D3DXVECTOR3(  2.4000f,  0.0000f, 2.02500f ),
		D3DXVECTOR3(  2.7000f,  0.0000f, 2.40000f ), D3DXVECTOR3(  2.7000f, -0.2500f, 2.40000f ),
		D3DXVECTOR3(  3.3000f, -0.2500f, 2.40000f ), D3DXVECTOR3(  3.3000f,  0.0000f, 2.40000f ),
		D3DXVECTOR3(  2.8000f,  0.0000f, 2.47500f ), D3DXVECTOR3(  2.8000f, -0.2500f, 2.47500f ),
		D3DXVECTOR3(  3.5250f, -0.2500f, 2.49375f ), D3DXVECTOR3(  3.5250f,  0.0000f, 2.49375f ),
		D3DXVECTOR3(  2.9000f,  0.0000f, 2.47500f ), D3DXVECTOR3(  2.9000f, -0.1500f, 2.47500f ),
		D3DXVECTOR3(  3.4500f, -0.1500f, 2.51250f ), D3DXVECTOR3(  3.4500f,  0.0000f, 2.51250f ),
		D3DXVECTOR3(  2.8000f,  0.0000f, 2.40000f ), D3DXVECTOR3(  2.8000f, -0.1500f, 2.40000f ),
		D3DXVECTOR3(  3.2000f, -0.1500f, 2.40000f ), D3DXVECTOR3(  3.2000f,  0.0000f, 2.40000f ),
		D3DXVECTOR3(  0.0000f,  0.0000f, 3.15000f ), D3DXVECTOR3(  0.8000f,  0.0000f, 3.15000f ),
		D3DXVECTOR3(  0.8000f, -0.4500f, 3.15000f ), D3DXVECTOR3(  0.4500f, -0.8000f, 3.15000f ),
		D3DXVECTOR3(  0.0000f, -0.8000f, 3.15000f ), D3DXVECTOR3(  0.0000f,  0.0000f, 2.85000f ),
		D3DXVECTOR3(  1.4000f,  0.0000f, 2.40000f ), D3DXVECTOR3(  1.4000f, -0.7840f, 2.40000f ),
		D3DXVECTOR3(  0.7840f, -1.4000f, 2.40000f ), D3DXVECTOR3(  0.0000f, -1.4000f, 2.40000f ),
		D3DXVECTOR3(  0.4000f,  0.0000f, 2.55000f ), D3DXVECTOR3(  0.4000f, -0.2240f, 2.55000f ),
		D3DXVECTOR3(  0.2240f, -0.4000f, 2.55000f ), D3DXVECTOR3(  0.0000f, -0.4000f, 2.55000f ),
		D3DXVECTOR3(  1.3000f,  0.0000f, 2.55000f ), D3DXVECTOR3(  1.3000f, -0.7280f, 2.55000f ),
		D3DXVECTOR3(  0.7280f, -1.3000f, 2.55000f ), D3DXVECTOR3(  0.0000f, -1.3000f, 2.55000f ),
		D3DXVECTOR3(  1.3000f,  0.0000f, 2.40000f ), D3DXVECTOR3(  1.3000f, -0.7280f, 2.40000f ),
		D3DXVECTOR3(  0.7280f, -1.3000f, 2.40000f ), D3DXVECTOR3(  0.0000f, -1.3000f, 2.40000f ),
	} ;

	// Create vertex buffer
	HRESULT hr = g_pd3dDevice->CreateVertexBuffer(
	sizeof(vertex),
	D3DUSAGE_WRITEONLY,
	VERTEX_FVF,
	D3DPOOL_MANAGED,
	&g_pVB,
	NULL);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// Lock vertex buffer and copy data
	void* pVertices = NULL;
	g_pVB->Lock(0, 0, &pVertices, 0);
	memcpy(pVertices, vertex, sizeof(vertex));
	g_pVB->Unlock();

	return D3D_OK;
}

HRESULT InitIB()
{
	DWORD index[] = {
		// Rim:
		  102, 103, 104, 105,   4,   5,   6,   7,
		    8,   9,  10,  11,  12,  13,  14,  15,

		// Body:
		   12,  13,  14,  15,  16,  17,  18,  19,
		   20,  21,  22,  23,  24,  25,  26,  27,
		   24,  25,  26,  27,  29,  30,  31,  32,
		   33,  34,  35,  36,  37,  38,  39,  40,

		// Lid:
		   96,  96,  96,  96,  97,  98,  99, 100,
		  101, 101, 101, 101,   0,   1,   2,   3,
		    0,   1,   2,   3, 106, 107, 108, 109,
		  110, 111, 112, 113, 114, 115, 116, 117,

		// Handle:
		   41,  42,  43,  44,  45,  46,  47,  48,
		   49,  50,  51,  52,  53,  54,  55,  56,
		   53,  54,  55,  56,  57,  58,  59,  60,
		   61,  62,  63,  64,  28,  65,  66,  67,

		// Spout:
		   68,  69,  70,  71,  72,  73,  74,  75,
		   76,  77,  78,  79,  80,  81,  82,  83,
		   80,  81,  82,  83,  84,  85,  86,  87,
		   88,  89,  90,  91,  92,  93,  94,  95 
	} ;

	// Create index buffer
	HRESULT hr = g_pd3dDevice->CreateIndexBuffer(
		sizeof(index),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX32,
		D3DPOOL_MANAGED,
		&g_pIB,
		0);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// Lock index buffer and copy data
	DWORD* pIndices = NULL;
	hr = g_pIB->Lock(0, 0, (void**)&pIndices, 0);
	if (FAILED(hr))
	{
		return E_FAIL;
	}
	memcpy(pIndices, index, sizeof(index));
	g_pIB->Unlock();

	return D3D_OK;
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

	// Disable lighting, since we didn't specify color for vertex
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING , FALSE );   

	// Wire frame mode
	g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	// Inti vertex buffer
	InitVB();

	// Init index buffer
	InitIB();

	return S_OK;
}

VOID Cleanup()
{
	SAFE_RELEASE(g_pVB);
	SAFE_RELEASE(g_pIB);
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
	D3DXVECTOR3 eyePt(0.0f, 0.0f, -10.0f) ;
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

	// Begin the scene
	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(D3DXVECTOR3));
		g_pd3dDevice->SetIndices(g_pIB);
		g_pd3dDevice->SetFVF(VERTEX_FVF);
		g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 118, 0, 48);
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