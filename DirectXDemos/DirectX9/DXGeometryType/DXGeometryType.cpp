/*
This demo show how to draw with different geometry types, see list below
D3DPT_POINTLIST      
D3DPT_LINELIST        
D3DPT_LINESTRIP      
D3DPT_TRIANGLELIST    
D3DPT_TRIANGLESTRIP   
D3DPT_TRIANGLEFAN     

keyboard usage
1 D3DPT_POINTLIST
2 D3DPT_LINELIST
3 D3DPT_LINESTRIP
4 D3DPT_TRIANGLELIST
5 D3DPT_TRIANGLESTRIP
6 D3DPT_TRIANGLEFAN

n no back face culling
c clock wise culling(CW)
w counter-clock wise culling(CCW)

TODO: it's better to add font for at the top-left of the window
to indicate the geometry type and the cull mode
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

LPDIRECT3D9             g_pD3D			 = NULL ; // Used to create the D3DDevice 
LPDIRECT3DDEVICE9       g_pd3dDevice	 = NULL ; // Our rendering device 
LPDIRECT3DVERTEXBUFFER9 g_pVB			 = NULL ; // Vertex buffer pointer 
D3DPRIMITIVETYPE		g_GeometryType   = D3DPT_POINTLIST ; // default
D3DCULL					g_CullMode		 = D3DCULL_NONE ;
const int				g_vertexCount    = 8;
int						g_primitiveCount = 8; // default

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
	ScreenVertex Vertices[g_vertexCount] =
	{
		{ 50.0f, 100.0f, 0.0f, 1.0f, 0xffff0000}, // 0
		{100.0f,  50.0f, 0.0f, 1.0f, 0xffff0000}, // 1
		{150.0f, 100.0f, 0.0f, 1.0f, 0xffff0000}, // 2
		{200.0f,  50.0f, 0.0f, 1.0f, 0xffff0000}, // 3
		{250.0f, 100.0f, 0.0f, 1.0f, 0xffff0000}, // 4
		{300.0f,  50.0f, 0.0f, 1.0f, 0xffff0000}, // 5
		{350.0f, 100.0f, 0.0f, 1.0f, 0xffff0000}, // 6
		{400.0f,  50.0f, 0.0f, 1.0f, 0xffff0000}, // 7
	};

	if( FAILED( g_pd3dDevice->CreateVertexBuffer( 8* sizeof( ScreenVertex ),
		0, SCREEN_SPACE_FVF,
		D3DPOOL_DEFAULT, &g_pVB, NULL ) ) )
	{
		return E_FAIL;
	}

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

VOID Render() 
{ 
	// Clear the back-buffer to a RED color 
	HRESULT hr ;

	// D3DCOLOR
	hr = g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0, 1.0f, 0);

	if (FAILED(hr))
	{
		DXTRACE_ERR_MSGBOX(DXGetErrorDescription(hr), hr) ;
	}

	// Begin the scene 
	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) ) 
	{
		// Set cull mode
		g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, g_CullMode) ;

		g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof( ScreenVertex ) );
		g_pd3dDevice->SetFVF( SCREEN_SPACE_FVF );
		g_pd3dDevice->DrawPrimitive( g_GeometryType, 0, g_primitiveCount );

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
				// Geometry type
			case '1':
				g_GeometryType = D3DPT_POINTLIST ;
				g_primitiveCount = g_vertexCount;
				break ;
			case '2':
				g_GeometryType = D3DPT_LINELIST ;
				g_primitiveCount = g_vertexCount / 2;
				break ;
			case '3':
				g_GeometryType = D3DPT_LINESTRIP ;
				g_primitiveCount = g_vertexCount - 1;
				break ;
			case '4':
				g_GeometryType = D3DPT_TRIANGLELIST ;
				g_primitiveCount = g_vertexCount / 3;
				break ;
			case '5':
				g_GeometryType = D3DPT_TRIANGLESTRIP ;
				g_primitiveCount = g_vertexCount - 2;
				break ;
			case '6':
				g_GeometryType = D3DPT_TRIANGLEFAN ;
				g_primitiveCount = g_vertexCount - 2;
				break ;

				// Cull mode
			case 'N':
				g_CullMode = D3DCULL_NONE ;
				break ;
			case 'C':
				g_CullMode = D3DCULL_CW ;
				break ;
			case 'W':
				g_CullMode = D3DCULL_CCW ;
				break ;
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

	winClass.lpszClassName = "ScreenQuad"; 
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
		"ScreenQuad",			// window caption 
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
