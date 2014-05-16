/*
Description:
Spherical mapping
Steps:
1. Create a D3D sphere
2. Copy the mesh of the sphere created above
3. Generated texture coordinates for all the vertices in the mesh above
4. apply texture to the mesh
5. draw the mesh
*/

#include <d3dx9.h>
#include <DxErr.h>
#include "../ModelCamera/Camera.h"

LPDIRECT3D9             g_pD3D				= NULL ; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice		= NULL ; // Our rendering device
ID3DXMesh*				g_pMesh				= NULL ; // Hold the teapot
LPDIRECT3DTEXTURE9		g_pTexture			= NULL ; // Environment texture
Camera*					g_pCamera			= NULL ; // Model view camera

LPD3DXMESH GenerateBoxMesh(LPDIRECT3DDEVICE9 pDev) ;

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
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING , FALSE );  
	//g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME) ;
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE) ;

	// Initialize model view camera
	g_pCamera = new Camera() ;

	// Initialize view and projection matrix
	D3DXVECTOR3 eyePt(0.0f, 0.0f, -10.0f) ;
	D3DXVECTOR3 upVec(0.0f, 1.0f, 0.0f) ;
	D3DXVECTOR3 lookCenter(0.0f, 0.0f, 0.0f) ;
	g_pCamera->SetViewParams(&eyePt, &lookCenter, &upVec) ;
	g_pCamera->SetProjParams(D3DX_PI / 4, 1.0f, 1.0f, 1000.0f) ;

	// Generate sphere mesh
	g_pMesh = GenerateBoxMesh(g_pd3dDevice) ;

	HRESULT hr = D3DXCreateTextureFromFile(g_pd3dDevice, "../Common/Media/autumn.jpg", &g_pTexture) ;
	if (FAILED(hr))
	{
		DXTRACE_ERR_MSGBOX(DXGetErrorString(hr), hr) ;
	}

	return S_OK;
}

struct VERTEX
{
	D3DXVECTOR3 pos ;	// Vertex position
	D3DXVECTOR3 norm ;	// Vertex normal
	float tu ;			// Texture coordinate u
	float tv ;			// Texture coordinate v
} ;

#define FVF_VERTEX D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1

LPD3DXMESH GenerateBoxMesh(LPDIRECT3DDEVICE9 pDev)
{
	// Create D3D sphere
	LPD3DXMESH mesh ;
	if(FAILED(D3DXCreateBox(pDev, 1.0f, 1.0f, 1.0f, &mesh, NULL)))
	{
		return NULL ;
	}

	// Get a copy of the sphere mesh
	LPD3DXMESH texMesh ;
	if (FAILED(mesh->CloneMeshFVF(D3DXMESH_SYSTEMMEM, FVF_VERTEX, pDev, &texMesh)))
	{
		return NULL ;
	}

	// Release original mesh
	mesh->Release() ;

	// add texture coordinates
	VERTEX* pVerts ;
	if (SUCCEEDED(texMesh->LockVertexBuffer(0, (void **)&pVerts)))
	{
		// Get vertex count
		int numVerts = texMesh->GetNumVertices() ;

		for (int i = 0; i < numVerts; ++i)
		{
			D3DXVECTOR3 v = pVerts->pos ;
			D3DXVec3Normalize(&v, &v) ;

			// Calculates texture coordinates
			pVerts->tu = asinf(v.x) / D3DX_PI + 0.5f ;
			pVerts->tv = asinf(v.y) / D3DX_PI + 0.5f ;

			++pVerts ;
		}

		// Unlock the vertex buffer
		texMesh->UnlockVertexBuffer() ;
	}

	return texMesh ;
}

VOID Cleanup()
{
	// Release teapot mesh
	if(g_pMesh != NULL)
		g_pMesh->Release() ;

	// Release D3D
	if( g_pD3D != NULL)
		g_pD3D->Release();

	// Release device
	if( g_pd3dDevice != NULL) 
		g_pd3dDevice->Release();
}

void SetupMatrix()
{
	// Set world matrix
	D3DXMATRIX world = *g_pCamera->GetWorldMatrix() ;
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &world) ;

	// Set view matrix
	D3DXMATRIX view = *g_pCamera->GetViewMatrix() ;
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &view) ;

	// Set projection matrix
	D3DXMATRIX proj = *g_pCamera->GetProjMatrix() ;
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &proj) ;
}

VOID Render()
{
	SetupMatrix() ;

	g_pCamera->OnFrameMove() ;

	// Clear the back-buffer to a RED color
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );

	// Begin the scene
	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		// Set the environment texture
		g_pd3dDevice->SetTexture(0, g_pTexture) ;
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );

		// Draw teapot 
		g_pMesh->DrawSubset(0) ;

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

	g_pCamera->HandleMessages(hWnd, msg, wParam, lParam) ;

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