#include <d3dx9.h>
#include "Camera.h"

LPDIRECT3D9             g_pD3D				= NULL ; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice		= NULL ; // Our rendering device
IDirect3DTexture9*		g_pRenderTexture	= NULL ; // Texture to render to
IDirect3DSurface9*		g_pRenderSurface	= NULL ; // Surface of the texture
IDirect3DSurface9*		g_pOldRenderTarget	= NULL ; // store old back buffer
Camera*					g_ModelViewCamera	= NULL ; // Model view camera
ID3DXMesh*				g_pTeapotMesh		= NULL ; // Hold the teapot

LPDIRECT3DVERTEXBUFFER9 g_pVB				= NULL ; // Vertex buffer pointer
LPDIRECT3DINDEXBUFFER9  g_pIB				= NULL ; // Index buffer pointer

struct Vertex
{
	float x, y, z ;		// Position
	float nx, ny, nz ;	// Normal
	float u, v ;		// Texture
};

#define VERTEX_FVF (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)

#define SAFE_RELEASE(P) if(P){ P->Release(); P = NULL;}

HRESULT CreateCube()
{
	// The vertex order in each face is in the following order when you 
	// look at the face perpendicular against it's normal
	// bottom left -> bottom right -> top right -> top left
	// So you need to set the cull mode as D3DCULL_CW, since the default mode is D3DCULL_CCW
	Vertex vertices[] =
	{
		// Front face
		{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f }, // 0
		{ 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f }, // 1
		{ 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f }, // 2
		{ 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f }, // 3

		// Back face
		{ 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f },
		{ 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f },
		{ 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },

		// Left face
		{ 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f },
		{ 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f },
		{ 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f },
		{ 0.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f },

		// Right face
		{ 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f },
		{ 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f },
		{ 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f },

		// Top face
		{ 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f },
		{ 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f },
		{ 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f },

		// Bottom face
		{ 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f },
		{ 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f },
		{ 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f },
		{ 1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f },
	} ;

	// Index buffer
	WORD indices[] = 
	{
		// Front side
		0, 1, 2, 0, 2, 3,

		// Back side
		4, 5, 6, 4, 6, 7, 

		// Top side
		8, 9, 10, 8, 10, 11,

		// Bottom side
		12, 13, 14, 12, 14, 15, 

		// Left side
		16, 17, 18, 16, 18, 19,

		// Right side
		20, 21, 22, 20, 22, 23, 
	} ;

	// Create vertex buffer
	if( FAILED( g_pd3dDevice->CreateVertexBuffer( sizeof(vertices) * sizeof(Vertex),
		D3DUSAGE_WRITEONLY, 
		VERTEX_FVF,
		D3DPOOL_MANAGED, 
		&g_pVB, 
		NULL ) ) )
	{
		return E_FAIL;
	}

	// Copy vertex data
	VOID* pVertices;
	if( FAILED( g_pVB->Lock( 0, sizeof(vertices), (void**)&pVertices, 0 ) ) )
		return E_FAIL;
	memcpy( pVertices, vertices, sizeof(vertices) );
	g_pVB->Unlock();

	// Create index buffer
	if( FAILED( g_pd3dDevice->CreateIndexBuffer( sizeof(indices) * sizeof(WORD), 
		D3DUSAGE_WRITEONLY, 
		D3DFMT_INDEX16, 
		D3DPOOL_MANAGED, 
		&g_pIB, 
		0) ) )
	{
		return E_FAIL ;
	}

	// Copy index data
	VOID *pIndices;
	if( FAILED( g_pIB->Lock( 0, sizeof(indices), (void **)&pIndices, 0) ) )
		return E_FAIL;
	memcpy(pIndices, indices, sizeof(indices) );
	g_pIB->Unlock() ;

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

	// Create texture
	HRESULT hr = g_pd3dDevice->CreateTexture(
		256,
		256,
		1, 
		D3DUSAGE_RENDERTARGET, 
		//d3dpp.BackBufferFormat,
		D3DFMT_R5G6B5,
		D3DPOOL_DEFAULT, 
		&g_pRenderTexture, 
		NULL) ;
	if (FAILED(hr))
	{
		MessageBox(NULL, "Create texture failed!", "Error", 0) ;
		return E_FAIL ;
	}

	// Get texture surface
	hr = g_pRenderTexture->GetSurfaceLevel(0, &g_pRenderSurface) ;
	if (FAILED(hr))
	{
		MessageBox(NULL, "Get surface on texture failed!", "Error", 0) ;
		return E_FAIL ;
	}

	// Create teapot
	D3DXCreateTeapot(g_pd3dDevice, &g_pTeapotMesh, NULL) ;

	// Create Cube
	CreateCube() ;

	// Create camera
	g_ModelViewCamera = new Camera() ;

	// Initialize view matrix
	D3DXVECTOR3 eyePt(0.0f, 0.0f, -5.0f) ;
	D3DXVECTOR3 upVec(0.0f, 1.0f, 0.0f) ;
	D3DXVECTOR3 lookCenter(0.0f, 0.0f, 0.0f) ;
	g_ModelViewCamera->SetViewParams(&eyePt, &lookCenter, &upVec) ;

	g_ModelViewCamera->SetProjParams(D3DX_PI / 4, 1.0f, 1.0f, 1000.0f) ;

	return S_OK;
}

VOID Cleanup()
{
	SAFE_RELEASE(g_pTeapotMesh) ;
	SAFE_RELEASE(g_pOldRenderTarget) ;
	SAFE_RELEASE(g_pRenderSurface) ;
	SAFE_RELEASE(g_pRenderTexture) ;
	SAFE_RELEASE(g_pVB) ;
	SAFE_RELEASE(g_pIB) ;
	SAFE_RELEASE(g_pd3dDevice) ;
	SAFE_RELEASE(g_pD3D) ;
}

void SetupMatrix()
{
	// Set world matrix
	D3DXMATRIX world = *g_ModelViewCamera->GetWorldMatrix() ;
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &world) ;

	// Set view matrix
	D3DXMATRIX view = *g_ModelViewCamera->GetViewMatrix() ;
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &view) ;

	// Set projection matrix
	D3DXMATRIX proj = *g_ModelViewCamera->GetProjMatrix() ;
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &proj) ;
}

void SetupTeapotMatrix()
{

}

void RenderCube(float timeDelta)
{
	// Clear the back-buffer to a RED color
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,255,0), 1.0f, 0 );

	// Begin the scene
	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		HRESULT hr = g_pd3dDevice->SetTexture(NULL, g_pRenderTexture) ;
		if (FAILED(hr))
		{
			MessageBox(NULL, "Set texture failed", "Error", 0) ;
			return ;
		}

		g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		// Set stream source, and index buffer
		g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof(Vertex) );
		g_pd3dDevice->SetIndices(g_pIB) ;
		g_pd3dDevice->SetFVF(VERTEX_FVF) ;

		g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW) ;
		g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID) ;

		D3DXMATRIX world ;
		D3DXMatrixTranslation(&world, -0.5f, -0.5f, -0.5f) ;
		g_pd3dDevice->SetTransform(D3DTS_WORLD, &world) ;

		// Totally 24 points and 12 triangles
		g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12) ;

		// End the scene
		g_pd3dDevice->EndScene();
	}
}

VOID Render(float timeDelta)
{
	SetupMatrix() ;

	g_ModelViewCamera->OnFrameMove();

	// Save render target
	g_pd3dDevice->GetRenderTarget(0, &g_pOldRenderTarget) ;

	g_pd3dDevice->SetRenderTarget(0, g_pRenderSurface) ;
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff0000ff, 1.0f, 0 );

	// Begin the scene
	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		// Without this line, the teapot won't show up, what's the inner stuff of SetTexture?
		g_pd3dDevice->SetTexture(0, NULL) ;

		// Disable lighting, since we didn't specify color for vertex
		g_pd3dDevice->SetRenderState( D3DRS_LIGHTING , FALSE ); 
		g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME) ;

		D3DXMATRIX word ;
		D3DXMatrixTranslation(&word, 0.0f, 0.0f, 0.0f) ;

		static float totalTime = 0.0f ;
		totalTime += timeDelta * 2 ;

		D3DXMATRIX rot ;
		D3DXMatrixRotationY(&rot, totalTime) ;

		word *= rot ;
		g_pd3dDevice->SetTransform(D3DTS_WORLD, &word) ;

		g_pTeapotMesh->DrawSubset(0) ;

		// End the scene
		g_pd3dDevice->EndScene();
	}

	// Restore the back buffer
	g_pd3dDevice->SetRenderTarget(0, g_pOldRenderTarget) ;

	RenderCube(timeDelta) ;

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

	g_ModelViewCamera->HandleMessages(hWnd, msg, wParam, lParam) ;

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
		"ScreenQuad",				// window caption
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
		//InitVB() ;

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
				Render(timeDelta) ;

				// Update last time to current time for next loop
				lastTime = currTime;
			}
		}
	}

	UnregisterClass(winClass.lpszClassName, hInstance) ;
	return 0;
}