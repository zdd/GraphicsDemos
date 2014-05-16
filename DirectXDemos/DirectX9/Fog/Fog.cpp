#include <d3dx9.h>
#include "Camera.h"

// Release COM
#define SAFE_RELEASE(p) { if (p) { (p)->Release(); (p) = NULL; } }


LPDIRECT3D9             g_pD3D				= NULL ; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice		= NULL ; // Our rendering device
LPDIRECT3DVERTEXBUFFER9 g_pVB				= NULL ; // Vertex buffer

ID3DXMesh*				g_pTeapotMesh		= NULL ; // Teapot mesh
ID3DXMesh*				g_pPillarMesh		= NULL ; // Pillar mesh

Camera*					g_pCamera			= new Camera() ;	// First person view camera

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

	// Set view matrix
	D3DXVECTOR3 eyePt(0, 1.0f, -5.0f) ;
	D3DXVECTOR3 lookAt(0, 0.0f, 0) ;
	D3DXVECTOR3 upVec(0, 1.0f, 0) ;
	g_pCamera->SetViewParams(eyePt, lookAt, upVec) ;

	// Set projection matrix
	g_pCamera->SetProjParams(D3DX_PI / 4, 1.0f, 1.0f, 1000.0f) ;

	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU,  D3DTADDRESS_WRAP );
	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV,  D3DTADDRESS_WRAP );

	// Create teapot
	D3DXCreateTeapot(g_pd3dDevice, &g_pTeapotMesh, NULL) ;

	return S_OK;
}

VOID InitVertexBuffer()
{
	// Define vertex data
	Vertex Quad[] =
	{
		{-64.0f, 0.0f, -64.0f,  0.0f, 16.0f},	// 0
		{-64.0f, 0.0f,  64.0f,  0.0f,  0.0f},	// 1
		{ 64.0f, 0.0f, -64.0f, 16.0f, 16.0f},   // 3
		{ 64.0f, 0.0f,  64.0f, 16.0f,  0.0f},	// 2
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

VOID RenderGround()
{
	//// Disable lighting
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE) ;

	// Set texture
	g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(Vertex)) ;
	g_pd3dDevice->SetFVF(Vertex_FVF) ;

	g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID) ;
	g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2) ;
}

VOID RenderFog(DWORD Color, DWORD Mode, BOOL UseRange, FLOAT Density)
{
	float Start = 1.0f ;   // Linear fog distances
	float End   = 5.f;

	// Enable fog blending.
	g_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);

	// Set the fog color.
	g_pd3dDevice->SetRenderState(D3DRS_FOGCOLOR, Color);

	// Set fog parameters.
	if(D3DFOG_LINEAR == Mode)
	{
		g_pd3dDevice->SetRenderState(D3DRS_FOGVERTEXMODE, Mode);
		g_pd3dDevice->SetRenderState(D3DRS_FOGSTART, *(DWORD *)(&Start));
		g_pd3dDevice->SetRenderState(D3DRS_FOGEND,   *(DWORD *)(&End));
	}
	else
	{
		g_pd3dDevice->SetRenderState(D3DRS_FOGVERTEXMODE, Mode);
		g_pd3dDevice->SetRenderState(D3DRS_FOGDENSITY, *(DWORD *)(&Density));
	}

	// Enable range-based fog if desired (only supported for
	//   vertex fog). For this example, it is assumed that UseRange
	//   is set to a nonzero value only if the driver exposes the 
	//   D3DPRASTERCAPS_FOGRANGE capability.
	// Note: This is slightly more performance intensive
	//   than non-range-based fog.
	if(UseRange)
		g_pd3dDevice->SetRenderState(D3DRS_RANGEFOGENABLE, TRUE);
}

VOID Cleanup()
{
	// Release vertex buffer
	SAFE_RELEASE(g_pVB) ;

	// Release teapot mesh
	SAFE_RELEASE(g_pTeapotMesh) ;

	// Release device
	SAFE_RELEASE(g_pd3dDevice) ;

	// Release D3D
	SAFE_RELEASE(g_pD3D) ;
}

void SetMaterial()
{
	// Set white material
	D3DMATERIAL9 redMaterial ;
	ZeroMemory(&redMaterial, sizeof(redMaterial)) ;
	redMaterial.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f) ;
	redMaterial.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f) ;
	redMaterial.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f) ;
	redMaterial.Emissive = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f) ;
	g_pd3dDevice->SetMaterial(&redMaterial) ;
}

void SetupLight()
{
	// Enable lighting
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING , TRUE );

	D3DLIGHT9 light;
	ZeroMemory( &light, sizeof(light) );

	light.Type      = D3DLIGHT_DIRECTIONAL;
	light.Ambient   = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	light.Diffuse   = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	light.Specular  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	light.Direction = D3DXVECTOR3(10.0f, 10.0f, -10.0f);
	g_pd3dDevice->SetLight(0, &light);
	g_pd3dDevice->LightEnable(0, true);

	g_pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	g_pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE, true);
};

VOID SetupMatrix()
{
	D3DXMATRIX view = g_pCamera->GetViewMatrix() ;
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &view) ;

	D3DXMATRIX proj = g_pCamera->GetProjMatrix() ;
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &proj) ;
}

VOID Render()
{
	SetupLight() ;
	SetupLight() ;
	SetupMatrix() ;

	g_pCamera->Update(0.1f, 1.0f) ;

	// Clear the back-buffer to a RED color
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255,255,255), 1.0f, 0 );

	// Begin the scene
	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		// Draw teapot
		g_pTeapotMesh->DrawSubset(0) ;

		RenderFog(0xff00ff00, D3DFOG_LINEAR, TRUE, 10.f) ;

		//RenderGround() ;

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