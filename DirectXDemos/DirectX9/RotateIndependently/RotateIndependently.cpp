/*
The steps to rotate object by its own origin
1. Move object to origin in world coordinates
2. rotate
3. Move object back to its original position

Suppose a object at position(1.0f, 1.0f, 1.0f)
1. Move it to (0.0f, 0.0f, 0.0f)
2. Rotate
3. Move it back to (1.0f, 1.0f, 1.0f)
*/

#include <d3dx9.h>

LPDIRECT3D9             g_pD3D				= NULL ; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice		= NULL ; // Our rendering device

ID3DXMesh*				g_pMeshes[4] = {NULL} ;
D3DXMATRIX				matWorld[4] ;


// variable handle in shader file
D3DXHANDLE ViewProjMatrixHanle = 0 ;

HRESULT InitD3D( HWND hWnd )
{
	// Create the D3D object, which is needed to create the D3DDevice.
	if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		return E_FAIL;

	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	// Create device
	if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice ) ) )
	{
		return E_FAIL;
	}

	// Wire frame mode.
	g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME) ;

	// Disable lighting.
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING , FALSE );   

	// Create geometries.
	D3DXCreateTeapot(g_pd3dDevice, &g_pMeshes[0], NULL) ; // Teapot
	D3DXCreateSphere(g_pd3dDevice, 1.2f, 20, 20, &g_pMeshes[1], NULL) ; // Sphere
	D3DXCreateCylinder(g_pd3dDevice, 1.0f, 1.0f, 2.0f, 20, 5, &g_pMeshes[2], NULL) ; // Cylinder
	D3DXCreateTorus(g_pd3dDevice, 0.5f, 1.0f, 15, 15, &g_pMeshes[3], NULL) ; // Torus

	return S_OK;
}

VOID Cleanup()
{
	if( g_pd3dDevice != NULL) 
		g_pd3dDevice->Release();

	if( g_pD3D != NULL)
		g_pD3D->Release();

	// Release meshes
	for (int i = 0; i < 4; i++)
	{
		if(g_pMeshes[0] != NULL)
			g_pMeshes[i]->Release() ;
	}
}

void SetupMatrix()
{
	// Set object position
	D3DXMatrixTranslation(&matWorld[0], -2.0f, 0.0f, 0.0f) ;
	D3DXMatrixTranslation(&matWorld[1], 2.0f, 0.0f, 0.0f) ;
	D3DXMatrixTranslation(&matWorld[2], 0.0f, 2.0f, 0.0f) ;
	D3DXMatrixTranslation(&matWorld[3], 0.0f, -2.0f, 0.0f) ;

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


VOID Render(float timeDelta)
{
	SetupMatrix() ;

	// Clear the back-buffer to a RED color
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );

	static float totalTime = 0.0f ;


	// Begin the scene
	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		// build rotation matrix
		for (int i = 0; i < 4; i++)
		{
			totalTime += timeDelta ; // angle
			D3DXMATRIX matRotation ;

			if(i == 0) // rotate teapot by Z
				D3DXMatrixRotationZ(&matRotation, totalTime) ;
			else if (i == 1) // rotate sphere by Y
			{
				D3DXMatrixRotationY(&matRotation, totalTime) ;

			}
			else if(i == 2) // rotate Cylinder by X
			{
				D3DXMatrixRotationX(&matRotation, totalTime) ;
			}
			else // rotate torus by axis = (1.0f, 1.0f, 1.0f)
				D3DXMatrixRotationAxis(&matRotation, &D3DXVECTOR3(1.0f, 1.0f, 1.0f), totalTime) ;

			// move object to origin
			D3DXMatrixTranslation(&matWorld[i], 0.0f, 0.0f, 0.0f) ;

			// rotate
			D3DXMatrixMultiply(&matWorld[i], &matWorld[i], &matRotation) ;
		}

		// Move object back to its original position
		D3DXMATRIX world[4];
		D3DXMatrixTranslation(&world[0], -2.0f, 0.0f, 0.0f) ;
		D3DXMatrixTranslation(&world[1], 2.0f, 0.0f, 0.0f) ;
		D3DXMatrixTranslation(&world[2], 0.0f, 2.0f, 0.0f) ;
		D3DXMatrixTranslation(&world[3], 0.0f, -2.0f, 0.0f) ;

		// multiply with world matrix to get the final transform matrix and apply it
		for (int i = 0; i < 4; i++)
		{
			matWorld[i] *= world[i] ;

			g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld[i]) ;
			g_pMeshes[i]->DrawSubset(0) ;

		}

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

INT WINAPI wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, INT )
{
	// Register the window class
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L, 
		GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
		"D3D Tutoria", NULL };
	RegisterClassEx( &wc );

	// Create the application's window
	HWND hWnd = CreateWindow( "D3D Tutoria", "My first shader", 
		WS_OVERLAPPEDWINDOW , 0, 0, 600, 600,
		NULL, NULL, wc.hInstance, NULL );

	// Initialize Direct3D
	if( SUCCEEDED( InitD3D( hWnd ) ) )
	{ 
		// Show the window
		ShowWindow( hWnd, SW_SHOWDEFAULT );
		UpdateWindow( hWnd );

		// Enter the message loop
		MSG    msg ; 
		ZeroMemory( &msg, sizeof(msg) );
		PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );


		static DWORD lastTime = timeGetTime() ;
		while (msg.message != WM_QUIT)  
		{
			if( PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) != 0)
			{
				TranslateMessage (&msg) ;
				DispatchMessage (&msg) ;
			}
			else // Render the game if no message to process
			{
				DWORD currentTime = timeGetTime() ;
				float timeDelta = (float)(currentTime - lastTime) * 0.001f ;
				lastTime = currentTime ;

				Render(timeDelta) ;
			}
		}
	}

	UnregisterClass( "D3D Tutoria", wc.hInstance );
	return 0;
}

