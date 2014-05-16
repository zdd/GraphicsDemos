/*
This demo show how to use directional light in D3D

Key steps:
1. Create a teapot
2. Create material and set it
2. Create a directional light and set it
3. Enable lighting before render
4. Render

Note:
1. The D3D teapot mesh has normals itself after create, If your model 
does not contains normal(user defined mesh), you need to compute the
normals by yourself, the light only works when vertex normal is available.

2. If your model contains color info, you don't need to set material, else you 
should set it before using light.

3. The material defines which color reflect off the surface, so, we often set up 
a white color light, thus it can reflect all kinds of colors defined by the material.

4. When you enable lighting in D3D, you must use material, but light is optional.

5. material.Emissive, This component is used to add to the overall color of the surface, 
making it appear brighter like its giving off its own light. so if this field has set,
the model is visible even if lighting was disabled.

*/

#include <d3dx9.h>
#include <MMSystem.h>

LPDIRECT3D9             g_pD3D				= NULL ; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice		= NULL ; // Our rendering device
ID3DXMesh*				g_pSphereMesh		= NULL ; // Hold the teapot

bool					g_bActive			= true ; // Is window active?

#define SAFE_RELEASE(P) if(P){ P->Release(); P = NULL;}

HRESULT SetupDirectionalLight(D3DXVECTOR3& direction, D3DXCOLOR color)
{
	D3DLIGHT9 dirLight;
	ZeroMemory(&dirLight, sizeof(dirLight));

	dirLight.Type = D3DLIGHT_DIRECTIONAL;
	dirLight.Range = 0.1f;
	dirLight.Direction = direction;

	dirLight.Ambient   = color * 0.6f;
	dirLight.Diffuse   = color;
	dirLight.Specular  = color * 0.6f;

	// Enable lighting
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// Set light
	g_pd3dDevice->SetLight(0, &dirLight);

	// Enable light
	g_pd3dDevice->LightEnable(0, TRUE);

	return D3D_OK;
}

HRESULT SetupMaterial(D3DXCOLOR color)
{
	D3DMATERIAL9 material;
	ZeroMemory(&material, sizeof(material));

	material.Ambient = color;
	material.Diffuse = color;
	material.Specular = color;
	/*
	This component is used to add to the overall color of the surface, 
	making it appear brighter like its giving off its own light.
	*/
	material.Emissive = D3DXCOLOR(0, 0, 0, 0);
	material.Power = 2.0f; // only affect specular color

	g_pd3dDevice->SetMaterial(&material);

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

	// Create sphere
	D3DXCreateSphere(g_pd3dDevice, 1.0f, 20, 20, &g_pSphereMesh, 0);

	// Setup material
	D3DCOLOR red = D3DCOLOR_XRGB(255, 0, 0);
	SetupMaterial(red);

	// Setup light
	D3DCOLOR white = D3DCOLOR_XRGB(255, 255, 255);
	D3DXVECTOR3 direction(0, 0, -10);
	SetupDirectionalLight(direction, white);

	return S_OK;
}

VOID Cleanup()
{
	SAFE_RELEASE(g_pSphereMesh) ;
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
		// Draw teapot 
		g_pSphereMesh->DrawSubset(0) ;

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

	winClass.lpszClassName = "DirectionalLight";
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
		"DirectionalLight",			// window caption
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