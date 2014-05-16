/*
This demo show you how to use material alpha
In this demo, we use material alpha to mix with the vertex color to 
generate the transparent effect.

1. Enable lighting
2. set material and make sure it contains diffuse color
3. Set material as diffuse color source
   g_pd3dDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL );

4. set material color as blend color and material alpha as blend alpha
5. blend.

Question:
it seems that if we set the material as diffuse source, we can only blend material color with material alpha
we can not use material alpha to blend colors from other source, is that right?
*/


#include <d3dx9.h>
#include <mmsystem.h>

LPDIRECT3D9             g_pD3D       = NULL; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; // Our rendering device
LPDIRECT3DTEXTURE9		g_pCubeTexture = NULL ; // Texture
ID3DXMesh*				g_pTeapotMesh = NULL;

int g_rotAxis = 0; 

// Vertex format
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE)

HRESULT InitVB();

HRESULT SetupMaterial(D3DXCOLOR color)
{
	D3DMATERIAL9 material;
	ZeroMemory(&material, sizeof(material));

	material.Ambient = color;
	material.Diffuse = color;
	material.Specular = color;
	material.Emissive = color;

	g_pd3dDevice->SetMaterial(&material);

	return D3D_OK;
}

HRESULT InitD3D( HWND hWnd )
{
	// Create the D3D object.
	if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		return E_FAIL;

	// Set up the structure used to create the D3DDevice
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	// Create the D3DDevice
	if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice ) ) )
	{
		return E_FAIL;
	}

	// Create teapot
	D3DXCreateTeapot(g_pd3dDevice, &g_pTeapotMesh, NULL);

	// Disable lighting, since the vertex contains colors
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING , true );

	// Setup material
	DWORD color = 0x80ff0000;
	SetupMaterial(color);

	return S_OK;
}

VOID Cleanup()
{
	if (g_pTeapotMesh != NULL)
		g_pTeapotMesh->Release();

	if(g_pCubeTexture != NULL)
		g_pCubeTexture->Release();

	if( g_pd3dDevice != NULL ) 
		g_pd3dDevice->Release();

	if( g_pD3D != NULL )       
		g_pD3D->Release();
}

VOID SetupMatrix()
{
	// Translate so the center of the box is the coordinate system origin.
	D3DXMATRIXA16 matWorld ;
	D3DXMatrixTranslation( &matWorld, -0.5f, -0.5f, 0.5f) ;
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	// Set up view matrix
	D3DXVECTOR3 vEyePt( 0.0f, 3.0f, -5.0f );		
	D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );	
	D3DXVECTOR3 vUpVec( 0.0f, 3.0f, 0.0f );	
	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

	// Set up perspective matrix
	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 1000.0f );
	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
}


void DrawTeapot()
{
	// Enable alpha blending
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);

	// Set diffuse source to vertex color, this is the default settings.
	g_pd3dDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL );

	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);   //Ignored
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1);

	//Set the alpha to come completely from the diffuse
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);   //Ignored
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG1);

	// How does D3D know which is source, which is dest?
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,	D3DBLEND_SRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,	D3DBLEND_INVSRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_BLENDOP,	D3DBLENDOP_ADD);

	g_pTeapotMesh->DrawSubset(0);
}

VOID Render()
{
	// Clear the back-buffer to a blue color
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );

	// Begin the scene
	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		SetupMatrix() ;

		//DrawCube();
		DrawTeapot();

		// End the scene
		g_pd3dDevice->EndScene();
	}

	// Present the backbuffer contents to the display
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
			}
		}
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

	winClass.lpszClassName = "Material Alpha";
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
		"Material Alpha",					// window caption
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
				Render() ;

				// Update last time to current time for next loop
				lastTime = currTime;
			}
		}
	}

	UnregisterClass(winClass.lpszClassName, hInstance) ;
	return 0;
}