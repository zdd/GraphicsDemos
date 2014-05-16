/*
This is a question from Baidu, http://zhidao.baidu.com/question/472758923.html?fr=im100400#reply-box-1181667578
Draw a texture, then wait 2 seconds and draw another texture
*/

/*
There are two way to flip two textures

=== First way
1. draw first texture
2. sleep few seconds
3. draw another texture

== Second way
Setup a windows timmer
update the texture with the timmer
*/


#include <d3dx9.h>

LPDIRECT3D9             g_pD3D				= NULL ; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice		= NULL; // Our rendering device
LPDIRECT3DVERTEXBUFFER9 g_pVB				= NULL; // Vertex buffer
LPDIRECT3DTEXTURE9		g_pTexture1			= NULL; // Texture 1
LPDIRECT3DTEXTURE9      g_pTexture2			= NULL; // Texture 2

#define SAFE_RELEASE(P) if(P){ P->Release(); P = NULL;}

struct Vertex
{
	float x, y, z ; // Vertex position
	float u, v ;	// Texture coordinates
};

#define VertexFVF D3DFVF_XYZ | D3DFVF_TEX1

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
	/*d3dpp.EnableAutoDepthStencil = TRUE ;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16 ;*/

	// Create device
	if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice ) ) )
	{
		MessageBoxA(NULL, "Create D3D9 device failed!", "Error", 0) ;
		return E_FAIL;
	}

	// Disable lighting
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING , FALSE );  

	// Create texture 1
	HRESULT hr ;
	hr = D3DXCreateTextureFromFile(g_pd3dDevice, "../Common/Media/chessboard.jpg", &g_pTexture1) ;
	if (FAILED(hr))
	{
		MessageBoxA(NULL, "Create Texture 1 failed!", "Error", 0) ;
	}
	
	// Create texture 2
	hr = D3DXCreateTextureFromFile(g_pd3dDevice, "../Common/Media/crate.jpg", &g_pTexture2) ;
	if (FAILED(hr))
	{
		MessageBoxA(NULL, "Create Texture 2 failed!", "Error", 0) ;
	}
	return S_OK;
}

// Prepare vertex buffer
void InitVB()
{
	Vertex Quad[] = 
	{
		{-5.0f,  5.0f, 0,    0,    0},	// 1
		{ 5.0f,  5.0f, 0, 1.0f,    0},	// 2
		{-5.0f, -5.0f, 0,    0, 1.0f},	// 4
		{ 5.0f, -5.0f, 0, 1.0f, 1.0f},	// 3
	} ;

	// Create vertex buffer
	HRESULT hr ;
	hr = g_pd3dDevice->CreateVertexBuffer(6 * sizeof(Vertex), D3DUSAGE_WRITEONLY, 
		VertexFVF, D3DPOOL_MANAGED, &g_pVB, NULL) ;
	if (FAILED(hr))
	{
		MessageBoxA(NULL, "Create vertex buffer failed!", "Error", 0) ;
	}

	// Copy data
	Vertex* v ;
	g_pVB->Lock(0, 0, (void**)&v, 0) ;
	memcpy(v, Quad, 6 * sizeof(Vertex)) ;
	g_pVB->Unlock() ;
}

VOID Cleanup()
{
	SAFE_RELEASE(g_pTexture1) ;

	SAFE_RELEASE(g_pTexture2);

	SAFE_RELEASE(g_pVB) ;

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
	D3DXVECTOR3 eyePt(0.0f, 0.0f, -15.0f) ;
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

void RenderTexture(LPDIRECT3DTEXTURE9 texture)
{
	// Setup texture
	g_pd3dDevice->SetTexture(0, texture) ;
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU,  D3DTADDRESS_WRAP );
	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV,  D3DTADDRESS_WRAP );

	// Begin the scene
	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		// render
		g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(Vertex) );
		g_pd3dDevice->SetFVF(VertexFVF) ;
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2) ;

		// End the scene
		g_pd3dDevice->EndScene();
	}

	// Present the back-buffer contents to the display
	g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}

VOID Render()
{
	SetupMatrix() ;

	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, 0, 1.0f, 0 );

	RenderTexture(g_pTexture1);

	Sleep(1000);

	RenderTexture(g_pTexture2);

	Sleep(1000);
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

	winClass.lpszClassName = "DrawTwoTextures";
	winClass.cbSize        = sizeof(WNDCLASSEX);
	winClass.style         = CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc   = MsgProc;
	winClass.hInstance     = hInstance;
	winClass.hIcon	       = NULL ;
	winClass.hIconSm	   = NULL ;
	winClass.hCursor       = LoadCursor(NULL, IDC_ARROW) ; // to avoid busy cursor
	winClass.hbrBackground = NULL ;
	winClass.lpszMenuName  = NULL ;
	winClass.cbClsExtra    = 0;
	winClass.cbWndExtra    = 0;

	RegisterClassEx (&winClass) ;  

	HWND hWnd = CreateWindowEx(NULL,  
		winClass.lpszClassName,		// window class name
		"DrawTwoTextures",			// window caption
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
		InitVB() ;

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

	UnregisterClass(winClass.lpszClassName, hInstance) ;
	return 0;
}