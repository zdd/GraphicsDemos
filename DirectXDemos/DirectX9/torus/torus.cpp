/*
This is a simple pixel shader program which illustrate how to use pixel shader instead of fixed pipeline
to set color for a model, We draw a red teapot in this example
*/

#include <d3dx9.h> 

LPDIRECT3D9             g_pD3D                  = NULL ; // Used to create the D3DDevice 
LPDIRECT3DDEVICE9       g_pd3dDevice            = NULL ; // Our rendering device 
ID3DXMesh*              g_pTorusMesh           = NULL ; // Hold the sphere 
IDirect3DPixelShader9*	g_pPixelShader			= NULL ; // Pixel shader

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

	// Turn off culling, so we see the front and back of the triangle    
	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );   

	// Wire frame mode
	g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	//g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);    
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING , FALSE );    

	// Create a torus
	D3DXCreateTorus(g_pd3dDevice, 1.0f, 2.0f, 20, 20, &g_pTorusMesh, NULL);

	return S_OK; 
} 

VOID Cleanup() 
{ 
	if( g_pd3dDevice != NULL)  
		g_pd3dDevice->Release(); 

	if( g_pD3D != NULL) 
		g_pD3D->Release(); 

	if(g_pTorusMesh != NULL) 
		g_pTorusMesh->Release() ; 
} 

bool PrepareShader() 
{ 
	ID3DXBuffer *shader ; 
	ID3DXBuffer *errorBuffer ; 

	// Compile shader from file, Shader.txt must exist in current directory, and it must contain
	// the "Main" function
	HRESULT hr = D3DXCompileShaderFromFileA("torus.fx", 0, 0, "Main", "ps_2_0",  
		D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY, &shader, &errorBuffer, NULL) ; 

	// output any error messages 
	if( errorBuffer ) 
	{ 
		MessageBoxA(0, (char*)errorBuffer->GetBufferPointer(), 0, 0); 
		errorBuffer->Release() ; 
	} 

	if(FAILED(hr)) 
	{ 
		MessageBox(0, "D3DXCompileShaderFromFile() - FAILED", 0, 0); 
		return false; 
	} 

	// Create pixel shader 
	hr = g_pd3dDevice->CreatePixelShader((DWORD*)shader->GetBufferPointer(), &g_pPixelShader) ; 

	// handling error 
	if(FAILED(hr)) 
	{ 
		MessageBox(0, "CreatePixelShader - FAILED", 0, 0); 
		return false; 
	} 

	// Release DX buffer
	shader->Release() ; 

	return true ;
} 

void SetupMatrix(float timeDelta) 
{ 
	static float total = 0.0f;
	total += timeDelta;

	// translate model to origin 
	D3DXMATRIX world ; 
	D3DXMatrixTranslation(&world, 0.0f, 0.0f, 0.0f) ; 

	D3DXMATRIX rotMatrix;
	D3DXMatrixRotationY(&rotMatrix, total);
	world *= rotMatrix;
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

VOID Render(float timeDelta) 
{ 
	if(!PrepareShader()) 
		return ; 

	SetupMatrix(timeDelta) ; 

	// Clear the back-buffer to a RED color 
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 ); 

	// Begin the scene 
	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) ) 
	{ 
		// Rendering of scene objects can happen here 
		g_pd3dDevice->SetPixelShader(g_pPixelShader) ; 

		g_pTorusMesh->DrawSubset(0) ; 

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
		GetModuleHandle(NULL), NULL, LoadCursor(NULL, IDC_ARROW), NULL, NULL, 
		"D3D Tutoria", NULL }; 
	RegisterClassEx( &wc ); 

	// Create the application's window 
	HWND hWnd = CreateWindow( "D3D Tutoria", "My first shader",  
		WS_OVERLAPPEDWINDOW , 0, 0, 600, 600, 
		NULL, NULL, wc.hInstance, NULL ); 

	// Get last time
	static DWORD lastTime = timeGetTime();

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

		while (msg.message != WM_QUIT)   
		{ 
			if( PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) != 0) 
			{ 
				TranslateMessage (&msg) ; 
				DispatchMessage (&msg) ; 
			} 
			else // Render the game if no message to process 
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

	UnregisterClass( "D3D Tutoria", wc.hInstance ); 
	return 0; 
} 
