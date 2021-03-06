/*
Draw a quad on screen space directly
We use the transformed and lit vertex format, so we specify the screen coordinates 
and color directly and didn't use d3d transform and lighting engine
#define SCREEN_SPACE_FVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)
*/

#include <d3dx9.h> 
#include <DxErr.h>

struct ScreenVertex
{
	float x, y, z ;
	float rhw ;
	D3DCOLOR color ;
};

// This vertex format contains a transformed position and color
#define SCREEN_SPACE_FVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

LPDIRECT3D9             g_pD3D			= NULL ; // Used to create the D3DDevice 
LPDIRECT3DDEVICE9       g_pd3dDevice	= NULL ; // Our rendering device 
LPDIRECT3DVERTEXBUFFER9 g_pVB			= NULL ; // Vertex buffer pointer 

#define SAFE_RELEASE(p) if (p){p->Release() ; p = NULL ;}

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
	d3dpp.BackBufferCount = 1 ;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; 
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; 
	d3dpp.EnableAutoDepthStencil = TRUE ;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16 ;

	// Create device 
	if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, 
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, 
		&d3dpp, &g_pd3dDevice ) ) ) 
	{ 
		MessageBoxA(NULL, "Create D3D9 device failed!", "Error", 0) ; 
		return E_FAIL; 
	} 

	return S_OK; 
} 

// Prepare quad data, we build up a quad by combine two triangles
HRESULT InitVB()
{
	// Initialize three Vertices for rendering a triangle
	ScreenVertex Vertices[] =
	{
		// Triangle 1
		{ 150.0f, 150.0f, 0, 1.0f, 0xffff0000, }, // x, y, z, rhw, color
		{ 350.0f, 150.0f, 0, 1.0f, 0xff00ff00, },
		{ 350.0f, 350.0f, 0, 1.0f, 0xff00ffff, },

		// Triangle 2
		{ 150.0f, 150.0f, 0, 1.0f, 0xffff0000, }, 
		{ 350.0f, 350.0f, 0, 1.0f, 0xff00ffff, },
		{ 150.0f, 350.0f, 0, 1.0f, 0xff00ffff, },
	};

	// Create the vertex buffer. Here we are allocating enough memory
	// (from the default pool) to hold all our 6 custom Vertices. We also
	// specify the FVF, so the vertex buffer knows what data it contains.
	if( FAILED( g_pd3dDevice->CreateVertexBuffer( 6 * sizeof( ScreenVertex ),
		0, SCREEN_SPACE_FVF,
		D3DPOOL_DEFAULT, &g_pVB, NULL ) ) )
	{
		return E_FAIL;
	}

	// Now we fill the vertex buffer. To do this, we need to Lock() the VB to
	// gain access to the Vertices. This mechanism is required because vertex
	// buffers may be in device memory.
	VOID* pVertices;
	if( FAILED( g_pVB->Lock( 0, sizeof( Vertices ), ( void** )&pVertices, 0 ) ) )
		return E_FAIL;
	memcpy( pVertices, Vertices, sizeof( Vertices ) );
	g_pVB->Unlock();

	return S_OK;
}

VOID Cleanup() 
{ 
	// Release vertex buffer
	SAFE_RELEASE(g_pVB) ;

	// Release device 
	SAFE_RELEASE(g_pd3dDevice) ; 

	// Release D3D 
	SAFE_RELEASE(g_pD3D) ; 
}

// Draw quad
VOID DrawScreenQuad()
{
	g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof( ScreenVertex ) );
	g_pd3dDevice->SetFVF( SCREEN_SPACE_FVF );
	g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 );
}

VOID Render() 
{ 
	// Clear the back-buffer to a RED color 
	HRESULT hr ;

	// D3DCOLOR
	hr = g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
	
	if (FAILED(hr))
	{
		DXTRACE_ERR_MSGBOX(DXGetErrorDescription(hr), hr) ;
	}

	// Begin the scene 
	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) ) 
	{
		// Draw quad
		DrawScreenQuad() ;

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

	winClass.lpszClassName = "Transformed and Lit"; 
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

	HWND hWnd = CreateWindowEx(NULL,   
		winClass.lpszClassName,	// window class name 
		"Transformed and Lit",	// window caption 
		WS_OVERLAPPEDWINDOW,	// window style 
		32,						// initial x position 
		32,						// initial y position 
		512,					// initial window width 
		512,					// initial window height 
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
		if (SUCCEEDED(InitVB()))
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
				else // Render the game if there is no message to process 
				{ 
					Render() ; 
				} 
			} 
		}
	} 

	UnregisterClass(winClass.lpszClassName, hInstance) ; 
	return 0; 
} 
