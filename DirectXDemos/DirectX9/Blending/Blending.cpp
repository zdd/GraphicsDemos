/*
This Demo show you how to use blending in DirectX, the step is
1 Draw background image
2 Set material for teapot and enable blending
3 Draw teapot
*/

#include <d3dx9.h>
#include <DxErr.h>

LPDIRECT3D9             g_pD3D				= NULL ; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice		= NULL ; // Our rendering device
ID3DXMesh*				g_pTeapotMesh		= NULL ; // Hold the teapot

LPDIRECT3DVERTEXBUFFER9	g_pVB				= NULL ; // Vertex buffer
LPDIRECT3DTEXTURE9		g_pTexture			= NULL ; // Texture
D3DMATERIAL9			g_pTeapotMaterial ;		     // Material
D3DMATERIAL9			g_pBackgroundMaterial ;		 // Background material

// The quad used to draw the background image, here it is an chessboard
struct Vertex
{
	float x, y, z ;	// Vertex position
	float tu, tv ;	// Vertex coordinate
};

Vertex g_Quad[6] =
{
	/*
   1________2
	|      /
	|     /
	|    /
	|   /
	|  /
	| /
	|/
    0
	*/

	{ -2.0f, -2.0f, 0.0f, 0.0f, 1.0f }, // 0
	{ -2.0f,  2.0f, 0.0f, 0.0f, 0.0f }, // 1
	{  2.0f,  2.0f, 0.0f, 1.0f, 0.0f }, // 2

	/*       2
 			/|
		   / |
		  /  |
		 /	 |
		/	 |
	   /	 |	
	 0/______|3
	
	*/

	{ -2.0f, -2.0f, 0.0f, 0.0f, 1.0f }, // 0
	{  2.0f,  2.0f, 0.0f, 1.0f, 0.0f }, // 2
	{  2.0f, -2.0f, 0.0f, 1.0f, 1.0f }, // 3
} ;

#define D3DFVF_VertexFVF (D3DFVF_XYZ|D3DFVF_TEX1)

void Setup() ;
void SetupLight() ;

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


	// Disable lighting, since we didn't specify color for vertex
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING , TRUE );  

	// Enable Z-Buffer
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE) ;

	// Create teapot
	D3DXCreateTeapot(g_pd3dDevice, &g_pTeapotMesh, NULL) ;

	Setup() ;

	SetupLight() ;
 
	return S_OK;
}

void Setup()
{
	HRESULT hr ;

	// Create vertex buffer
	hr = g_pd3dDevice->CreateVertexBuffer(6 * sizeof(Vertex), D3DUSAGE_WRITEONLY, 
		D3DFVF_VertexFVF, D3DPOOL_MANAGED, &g_pVB, NULL) ;
	if (FAILED(hr))
	{
		DXTRACE_ERR_MSGBOX(DXGetErrorString(hr), hr) ;
	}

	// Fill in vertex buffer
	Vertex* v ;
	g_pVB->Lock(0, 0, (void**)&v, 0) ;
	memcpy(v, g_Quad, 6 * sizeof(Vertex)) ;
	g_pVB->Unlock() ;

	// Create texture
	hr = D3DXCreateTextureFromFile(g_pd3dDevice, "../Common/Media/chessboard.jpg", &g_pTexture) ;
	if (FAILED(hr))
	{
		DXTRACE_ERR_MSGBOX(DXGetErrorString(hr), hr) ;
	}

	// Set texture render state
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE) ;
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1) ;

	// Set teapot material
	g_pTeapotMaterial.Diffuse = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f) ;
	g_pTeapotMaterial.Ambient = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f) ;
	g_pTeapotMaterial.Specular = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f) ;
	g_pTeapotMaterial.Diffuse.a = 0.6f ;

	// Set background material
	g_pBackgroundMaterial.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f) ;
	g_pBackgroundMaterial.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f) ;
	g_pBackgroundMaterial.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f) ;

	// Set blending factor
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA) ;
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA) ;
}

void RenderQuad()
{
	// Move quad to origin
	D3DXMATRIX world ;
	D3DXMatrixIdentity(&world) ;
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &world) ;

	// Set material
	g_pd3dDevice->SetMaterial(&g_pBackgroundMaterial);

	// Set texture
	g_pd3dDevice->SetTexture(0, g_pTexture) ;

	// Draw quad
	g_pd3dDevice->SetFVF(D3DFVF_VertexFVF) ;
	g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(Vertex)) ;
	g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2) ;
}

VOID Cleanup()
{
	// Release vertex buffer
	if (g_pVB != NULL)
	{
		g_pVB->Release() ;
		g_pVB = NULL ;
	}

	// Release teapot mesh
	if(g_pTeapotMesh != NULL)
		g_pTeapotMesh->Release() ;

	// Release D3D
	if( g_pD3D != NULL)
		g_pD3D->Release();

	// Release device
	if( g_pd3dDevice != NULL) 
		g_pd3dDevice->Release();
}

void SetupLight()
{
	D3DLIGHT9 light ;
	light.Type = D3DLIGHT_DIRECTIONAL ;
	light.Direction = D3DXVECTOR3(1.0f, 0, 0) ;
	light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f) ; // white
	light.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f) ; // white
	light.Range = 1000.0f ;

	g_pd3dDevice->SetLight(0, &light) ;
	g_pd3dDevice->LightEnable(0, TRUE) ;
};

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

VOID Render()
{
	SetupMatrix() ;

	// Clear the back-buffer to a RED color
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(10,100,0), 1.0f, 0 );

	// Begin the scene
	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		// Draw quad
		RenderQuad() ;
		
		// Enable blending
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE) ;

		// Set material
		g_pd3dDevice->SetMaterial(&g_pTeapotMaterial) ;

		// Draw teapot 
		g_pTeapotMesh->DrawSubset(0) ;

		// Disable blending
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE) ;
		
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

	winClass.lpszClassName = "Teapot";
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
				Render() ;

				// Update last time to current time for next loop
				lastTime = currTime;
			}
		}
	}

	UnregisterClass(winClass.lpszClassName, hInstance) ;
	return 0;
}