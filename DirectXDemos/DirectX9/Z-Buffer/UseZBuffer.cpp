/*
This Demo show you how to use z-buffer

1 Set the z-buffer in the D3DPRESENT_PARAMETERS structure
	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	d3dpp.EnableAutoDepthStencil = TRUE ;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16 ;

2 Enable Z-buffer
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE) ;

3 Clear Z-buffer
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );

*/

#include <d3dx9.h>
#include <MMSystem.h>
#include "Camera.h"

LPDIRECT3D9             g_pD3D				= NULL ; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice		= NULL ; // Our rendering device
ID3DXMesh*				g_pTeapotMesh		= NULL ; // Hold the teapot
ID3DXMesh*				g_pMeshSphere		= NULL ; // Hold the sphere

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
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE) ;

	// Enable lighting
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// Create teapot
	D3DXCreateTeapot(g_pd3dDevice, &g_pTeapotMesh, NULL) ;
	D3DXCreateSphere(g_pd3dDevice, 1.0f, 20, 20, &g_pMeshSphere, NULL) ;

	return S_OK;
}

VOID Cleanup()
{
	// Release teapot mesh
	if(g_pTeapotMesh != NULL)
		g_pTeapotMesh->Release() ;

	// Release sphere
	if (g_pMeshSphere != NULL)
		g_pMeshSphere->Release() ;

	// Release D3D
	if( g_pD3D != NULL)
		g_pD3D->Release();

	// Release device
	if( g_pd3dDevice != NULL) 
		g_pd3dDevice->Release();
}

void SetupLight(D3DCOLORVALUE color)
{
	// Material
	D3DMATERIAL9 material ;
	material.Ambient = color ;
	material.Diffuse = color ;
	material.Emissive = color ;
	g_pd3dDevice->SetMaterial(&material) ;

	// Light
	D3DLIGHT9 light ;
	light.Type = D3DLIGHT_DIRECTIONAL ;
	light.Direction = D3DXVECTOR3(1.0f, 0.0f, 0.0f) ;
	light.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f) ; // white
	light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f) ; // white
	light.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f) ;// white
	light.Range = 1000.0f ;
	g_pd3dDevice->SetLight(0, &light) ;
	g_pd3dDevice->LightEnable(0, TRUE) ;
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

VOID Render()
{
	SetupMatrix() ;

	// Clear the back-buffer to a RED color
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );

	// Begin the scene
	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		// Get viewport
		D3DVIEWPORT9 viewport ;
		g_pd3dDevice->GetViewport(&viewport) ;

		// Draw a red, solid teapot at the front 
		viewport.MinZ = 0.0f ;
		viewport.MaxZ = 0.0f ;
		g_pd3dDevice->SetViewport(&viewport) ;
		SetupLight(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f)) ;
		g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID) ;
		g_pTeapotMesh->DrawSubset(0) ;

		// Draw a yellow, wired sphere behind the teapot 
		viewport.MinZ = 0.5f ;
		viewport.MaxZ = 0.5f ;
		g_pd3dDevice->SetViewport(&viewport) ;
		SetupLight(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f)) ;
		g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME) ;
		g_pMeshSphere->DrawSubset(0) ;

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

	case WM_SYSCOMMAND:							
		{
			switch (wParam)						
			{
			case SC_SCREENSAVE:					
			case SC_MONITORPOWER:				
				return 0;						
			}
			break;								
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
		0,							// initial x position
		0,							// initial y position
		608,						// initial window width
		616,						// initial window height
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
