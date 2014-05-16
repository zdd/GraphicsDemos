#include <d3dx9.h>
#include "Camera.h"
#include "Terrain.h"

LPDIRECT3D9             g_pD3D				= NULL ; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice		= NULL ; // Our rendering device
ID3DXMesh*				g_pTeapotMesh				= NULL ; // Hold the teapot
LPDIRECT3DVERTEXBUFFER9 g_pVB				= NULL ; // Vertex buffer
LPDIRECT3DTEXTURE9		g_pTexture			= NULL ; // Texture
D3DMATERIAL9			g_Material ;				 // Terrain material
D3DLIGHT9				g_Light ;					 // Light
D3DXMATRIX				g_matBillboardWorld ;		 // World matrix for billboard

Camera*					g_pCamera			= new Camera() ;	// First person view camera
Terrain*				g_pTerrain			= new Terrain() ;	// A flat terrain 

struct Vertex
{
	float x, y, z ; // Position
	float u, v ;	// Texture coordinates
};

#define Vertex_FVF D3DFVF_XYZ | D3DFVF_TEX1

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

	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE) ;

	// Create teapot
	D3DXCreateTeapot(g_pd3dDevice, &g_pTeapotMesh, NULL) ;

	// Initialize terrain
	g_pTerrain->BuildGridsBuffer(g_pd3dDevice) ;

	D3DXMatrixIdentity(&g_matBillboardWorld) ;

	// Set view matrix
	D3DXVECTOR3 eyePt(0, 0.0f, -20.0f) ;
	D3DXVECTOR3 lookAt(0, 1.0f, 0) ;
	D3DXVECTOR3 upVec(0, 1.0f, 0) ;
	g_pCamera->SetViewParams(eyePt, lookAt, upVec) ;

	// Set projection matrix
	g_pCamera->SetProjParams(D3DX_PI / 4, 1.0f, 1.0f, 1000.0f) ;

	// Create texture
	HRESULT hr = D3DXCreateTextureFromFile(g_pd3dDevice, "../Common/Media/autumn.jpg", &g_pTexture) ;
	if (FAILED(hr))
	{
		DXTRACE_ERR_MSGBOX(DXGetErrorString(hr), hr) ;
	}

	// Set texture sampler state
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );

	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU,  D3DTADDRESS_WRAP );
	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV,  D3DTADDRESS_WRAP );

	// Set material
	g_Material.Diffuse = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f) ; // white
	g_Material.Ambient = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f) ; // white
	g_Material.Specular = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f) ; // white

	// Set light
	g_Light.Type = D3DLIGHT_DIRECTIONAL ;
	g_Light.Direction = D3DXVECTOR3(1.0f, 0, 0) ;
	g_Light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f) ; // white
	g_Light.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f) ; // white
	g_Light.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f) ; // white
	g_Light.Range = 1000.0f ;

	return S_OK;
}

void InitVertexBuffer()
{
	// Define vertex data
	Vertex Quad[] =
	{
		{-25.0f,  0.0f, 0.0f, 0.0f, 1.0f}, // 0
		{-25.0f, 50.0f, 0.0f, 0.0f, 0.0f}, // 1
		{ 25.0f,  0.0f, 0.0f, 1.0f, 1.0f}, // 3
		{ 25.0f, 50.0f, 0.0f, 1.0f, 0.0f}, // 2
	} ;

	// Create vertex buffer
	HRESULT hr ;
	hr = g_pd3dDevice->CreateVertexBuffer(4 * sizeof(Vertex), D3DUSAGE_WRITEONLY, 
		Vertex_FVF, D3DPOOL_MANAGED, &g_pVB, NULL) ;
	if (FAILED(hr))
	{
		DXTRACE_ERR_MSGBOX(DXGetErrorString(hr), hr) ;
	}

	// Fill vertex buffer
	Vertex* v ;
	g_pVB->Lock(0, 0, (void**)&v, 0) ;
	memcpy(v, Quad, 4 * sizeof(Vertex)) ;
	g_pVB->Unlock() ;
}

void RenderTree()
{
	// Disable lighting
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE) ;

	g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
	g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	// Get current world matrix
	D3DXMATRIX matCurrentWorld ;
	g_pd3dDevice->GetTransform(D3DTS_WORLD, &matCurrentWorld) ;

	// Set world matrix for billboard
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_matBillboardWorld) ;

	// Set texture
	g_pd3dDevice->SetTexture(0, g_pTexture) ;
	g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(Vertex)) ;
	g_pd3dDevice->SetFVF(Vertex_FVF) ;

	g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID) ;
	g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2) ;

	// Restore world matrix
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matCurrentWorld) ;

	g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE );

}

void RenderTerrain()
{
	g_pTerrain->Render(g_pd3dDevice) ;
}

VOID RenderTeapot()
{
	g_pd3dDevice->SetMaterial(&g_Material) ;

	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING , TRUE );  
	g_pd3dDevice->SetLight(0, &g_Light) ;
	g_pd3dDevice->LightEnable(0, TRUE) ;

	g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME) ;

	g_pTeapotMesh->DrawSubset(0) ; // Draw teapot
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

void SetupMatrix()
{
	D3DXMATRIX view = g_pCamera->GetViewMatrix() ;
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &view) ;

	D3DXMATRIX proj = g_pCamera->GetProjMatrix() ;
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &proj) ;
}

void OnFrameMove()
{
	// Get director for line of sight
	D3DXVECTOR3 lookAt = g_pCamera->GetLookAt() ;
	D3DXVECTOR3 eyePoint = g_pCamera->GetEyePoint() ;
	D3DXVECTOR3 dir = lookAt - eyePoint ;

	// Rotate billboard according to view 
	if( dir.x >= 0.0f )
		D3DXMatrixRotationY(&g_matBillboardWorld, -atanf(dir.z / dir.x) + D3DX_PI / 2);
	else
		D3DXMatrixRotationY(&g_matBillboardWorld, -atanf(dir.z / dir.x) - D3DX_PI / 2);
}

VOID Render()
{
	SetupMatrix() ;

	OnFrameMove() ;

	g_pCamera->Update(0.1f, 1.0f) ;

	// Clear the back-buffer to a RED color
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255,255,255), 1.0f, 0 );

	// Begin the scene
	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		RenderTerrain() ;

		RenderTree() ;

		RenderTeapot() ;

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
		InitVertexBuffer() ;

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