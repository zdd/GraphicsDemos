/*
This demo show how to draw a teapot with D3D function D3DXCreateTeapot
*/

#include <d3dx9.h>
#include <MMSystem.h>

LPDIRECT3D9             g_pD3D				= NULL ; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice		= NULL ; // Our rendering device
LPDIRECT3DVERTEXSHADER9 g_pVertexShader		= NULL;  // Vertex Shader
LPDIRECT3DPIXELSHADER9  g_pPixelShader      = NULL;  // Pixel Shader
ID3DXMesh*				g_pTeapotMesh		= NULL ; // Hold the teapot

bool					g_bActive			= true ; // Is window active?

#define SAFE_RELEASE(P) if(P){ P->Release(); P = NULL;}

void CompileShader();

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

	CompileShader();

	// Create teapot
	D3DXCreateTeapot(g_pd3dDevice, &g_pTeapotMesh, NULL) ;

	return S_OK;
}

void CompileShader()
{
	DWORD dwShaderFlags = 0;

#if defined( DEBUG ) || defined( _DEBUG )
    // Set the D3DXSHADER_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DXSHADER_DEBUG;
    #endif

#ifdef DEBUG_VS
        dwShaderFlags |= D3DXSHADER_SKIPOPTIMIZATION|D3DXSHADER_DEBUG;
    #endif
#ifdef DEBUG_PS
        dwShaderFlags |= D3DXSHADER_SKIPOPTIMIZATION|D3DXSHADER_DEBUG;
    #endif
	
	CHAR* shader_file = "torus_shader.hls";

	// Compile Vertex Shader
	LPD3DXBUFFER pVertexShaderCode;
	HRESULT hr = D3DXCompileShaderFromFile(shader_file, NULL, NULL, "VS", "vs_3_0", dwShaderFlags, &pVertexShaderCode, NULL, NULL);
	if(FAILED(hr))
	{
		MessageBox(NULL, "Error", "Compile Vertex Shader Failed!", 0);
	}

	// Create Vertex Shader
	hr = g_pd3dDevice->CreateVertexShader((DWORD*)pVertexShaderCode->GetBufferPointer(), &g_pVertexShader);
	if(FAILED(hr))
	{
		MessageBox(NULL, "Error", "Create Vertex Shader Failed!", 0);
	}

	// Compile Pixel Shader
	LPD3DXBUFFER pPixelShaderCode;
	hr = D3DXCompileShaderFromFile(shader_file, NULL, NULL, "PS", "ps_3_0", dwShaderFlags, &pPixelShaderCode, NULL, NULL);
	if(FAILED(hr))
	{
		MessageBox(NULL, "Error", "Compile Pixel Shader Failed!", 0);
	}

	// Create Pixel Shader
	hr = g_pd3dDevice->CreatePixelShader((DWORD*)pPixelShaderCode->GetBufferPointer(), &g_pPixelShader);
	if(FAILED(hr))
	{
		MessageBox(NULL, "Error", "Create Pixel Shader Failed!", 0);
	}

	// cleanup
	pVertexShaderCode->Release();
	pPixelShaderCode->Release();
}

VOID Cleanup()
{
	SAFE_RELEASE(g_pTeapotMesh) ;
	SAFE_RELEASE(g_pd3dDevice) ;
	SAFE_RELEASE(g_pD3D) ;
}

void SetupMatrix(float timeDelta)
{
	static float totalTime = 0;
	totalTime += timeDelta;

	// translate model to origin
	D3DXMATRIX matWorld ;
	D3DXMatrixTranslation(&matWorld, 0.0f, 0.0f, 0.0f);

	D3DXMATRIX rotMatrix;
	D3DXMatrixRotationY(&rotMatrix, totalTime);

	matWorld *= rotMatrix;

	// Matrix in shader was stored in column-major order, we need transpose it first.
	// Effect->SetMatrix will transpose it automatically, but SetVertexShader not, this function set the raw data.
	D3DXMatrixTranspose(&matWorld, &matWorld);

	// Set vertex shader variable
	g_pd3dDevice->SetVertexShaderConstantF(0, matWorld, 4);

	// set view
	// Make sure eye point and light postion at the same side beyond the teapot.
	D3DXVECTOR3 eyePt(0.0f, 0.0f, -10.0f) ;
	D3DXVECTOR3 upVec(0.0f, 1.0f, 0.0f) ;
	D3DXVECTOR3 lookCenter(0.0f, 0.0f, 0.0f) ;

	D3DXMATRIX view ;
	D3DXMatrixLookAtLH(&view, &eyePt, &lookCenter, &upVec) ;

	// set projection
	D3DXMATRIX proj ;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI / 4, 1.0f, 1.0f, 1000.0f) ;

	D3DXMATRIX matViewProj = view * proj;
	D3DXMatrixTranspose(&matViewProj, &matViewProj);

	// Set vertex shader variable
	HRESULT hr = g_pd3dDevice->SetVertexShaderConstantF(4, matViewProj, 4);
	if (FAILED(hr))
	{
		MessageBox(NULL, "Error", "Set vertex shader variable failed", 0);
	}
}

void Render(float timeDelta)
{
	if (!g_bActive)
	{
		Sleep(50) ;
	}

	SetupMatrix(timeDelta) ;

	// Clear the back-buffer to a RED color
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(10,66,55), 1.0f, 0 );

	// Begin the scene
	if(SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		g_pd3dDevice->SetVertexShader(g_pVertexShader);
		g_pd3dDevice->SetPixelShader(g_pPixelShader);

		// Draw teapot 
		g_pTeapotMesh->DrawSubset(0) ;

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

	winClass.lpszClassName = "Torus";
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
		"Torus",					// window caption
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