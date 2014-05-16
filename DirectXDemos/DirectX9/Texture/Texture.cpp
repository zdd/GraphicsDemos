/*
This demo show how to use shader to draw texture
*/

/*
This demo show you how to write code with DirectX Effect, we create a effect framework here
So that every effect can reuse this framework, in this framework, we simply render a teapot.
*/

/*
This demo show how to draw a teapot with D3D function D3DXCreateTeapot
*/
#define D3D_DEBUG_INFO
#include <d3dx9.h>
#include <MMSystem.h>

LPDIRECT3D9             g_pD3D				= NULL ; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice		= NULL ; // Our rendering device
ID3DXEffect*			g_pEffect			= NULL ; // Effect interface

LPDIRECT3DVERTEXBUFFER9 g_pVB				= NULL ; // Vertex buffer pointer
LPDIRECT3DINDEXBUFFER9  g_pIB				= NULL ; // Index buffer pointer
LPDIRECT3DTEXTURE9		g_pCubeTexture		= NULL ; // Texture

// Vertex
struct Vertex
{
	float x, y, z ;		// Position
	float u, v ;		// Texture
};

#define VERTEX_FVF (D3DFVF_XYZ | D3DFVF_TEX1)

bool					g_bActive			= true ; // Is window active?

// Handles for effects
D3DXHANDLE g_hWVP;		// world, view and projection matrix handle.
D3DXHANDLE g_hTech;		// Technique handle

#define SAFE_RELEASE(P) if(P){ P->Release(); P = NULL;}

HRESULT BuildEffect()
{
	char* effectFile = "Texture.fx";

	ID3DXBuffer* errors = 0;

	// Create Effect from file
	HRESULT hr = D3DXCreateEffectFromFile(
		g_pd3dDevice,
		effectFile, 
		NULL,
		NULL,
		D3DXSHADER_DEBUG,
		NULL,
		&g_pEffect,
		&errors);
	/*if (FAILED(hr))
	{
		MessageBox(NULL, "Create Effect from file failed!", "Error", 0);
		return E_FAIL;
	}*/

	if( errors )
		MessageBox(0, (char*)errors->GetBufferPointer(), 0, 0);

	g_hWVP  = g_pEffect->GetParameterByName(0, "gWVP");
	g_hTech = g_pEffect->GetTechniqueByName("Tech1");

	return D3D_OK;
}

// Create a cube, assume the cube is centered at the word space origin, then the side direction is as followed
// Front - negative Z-axis
// Back - positive Z-axis
// Left - negative X-axis
// Right - positive X-axis
// Top - positive Y-axis
// Bottom - negative Y-axis
HRESULT CreateCube()
{
	// The vertex order in each face is in the following order when you 
	// look at the face perpendicular against it's normal
	// bottom left -> bottom right -> top right -> top left
	// So you should set the cull mode as D3DCULL_CW, since the default mode is D3DCULL_CCW
	Vertex vertices[] =
	{
		// Front face
		{ -1.0f, -1.0f, -1.0f, 0.0f, 1.0f }, // 0
		{ -1.0f,  1.0f, -1.0f, 0.0f, 0.0f }, // 1
		{  1.0f,  1.0f, -1.0f, 1.0f, 0.0f }, // 2
		{  1.0f, -1.0f, -1.0f, 1.0f, 1.0f }, // 3

		// Back face
		{  1.0f, -1.0f, 1.0f, 0.0f, 1.0f }, // 4
		{  1.0f,  1.0f, 1.0f, 0.0f, 0.0f }, // 5
		{ -1.0f,  1.0f, 1.0f, 1.0f, 0.0f }, // 6
		{ -1.0f, -1.0f, 1.0f, 1.0f, 1.0f }, // 7

		// Top face
		{ -1.0f, 1.0f, -1.0f, 0.0f, 1.0f }, // 8
		{ -1.0f, 1.0f,  1.0f, 0.0f, 0.0f }, // 9
		{  1.0f, 1.0f,  1.0f, 1.0f, 0.0f }, // 10
		{  1.0f, 1.0f, -1.0f, 1.0f, 1.0f }, // 11

		// Bottom face
		{  1.0f, -1.0f, -1.0f, 0.0f, 1.0f }, // 12
		{  1.0f, -1.0f,  1.0f, 0.0f, 0.0f }, // 13
		{ -1.0f, -1.0f,  1.0f, 1.0f, 0.0f }, // 14
		{ -1.0f, -1.0f, -1.0f, 1.0f, 1.0f }, // 15

		// Left face
		{ -1.0f, -1.0f,  1.0f, 0.0f, 1.0f }, // 16
		{ -1.0f,  1.0f,  1.0f, 0.0f, 0.0f }, // 17
		{ -1.0f,  1.0f, -1.0f, 1.0f, 0.0f }, // 18
		{ -1.0f, -1.0f, -1.0f, 1.0f, 1.0f }, // 19

		// Right face
		{ 1.0f, -1.0f, -1.0f, 0.0f, 1.0f }, // 20
		{ 1.0f,  1.0f, -1.0f, 0.0f, 0.0f }, // 21
		{ 1.0f,  1.0f,  1.0f, 1.0f, 0.0f }, // 22
		{ 1.0f, -1.0f,  1.0f, 1.0f, 1.0f }, // 23

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

	return S_OK ;
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

	// Create Cube
	CreateCube();

	// Build effect
	BuildEffect();

	return S_OK;
}

void DrawCube()
{
	// translate model to origin
	D3DXMATRIX world ;
	D3DXMatrixTranslation(&world, 0.0f, 0.0f, 0.0f) ;
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &world) ;

	// set view
	D3DXVECTOR3 eyePt(5.0f, 5.0f, -5.0f) ;
	D3DXVECTOR3 upVec(0.0f, 1.0f, 0.0f) ;
	D3DXVECTOR3 lookCenter(0.0f, 0.0f, 0.0f) ;

	D3DXMATRIX view ;
	D3DXMatrixLookAtLH(&view, &eyePt, &lookCenter, &upVec) ;
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &view) ;

	// set projection
	D3DXMATRIX proj ;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI / 4, 1.0f, 1.0f, 1000.0f) ;
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &proj) ;

	D3DXMATRIX worldviewproj = world * view * proj;

	// Set matrix
	g_pEffect->SetMatrix(g_hWVP, &worldviewproj);

	// Set technique
	g_pEffect->SetTechnique(g_hTech);

	// Render pass
	UINT numPass = 0;
	g_pEffect->Begin(&numPass, 0);
	g_pEffect->BeginPass(0);


	// Set texture
	D3DXCreateTextureFromFile(g_pd3dDevice, "../Common/Media/crate.jpg", &g_pCubeTexture) ;
	g_pEffect->SetTexture("g_pCubeTexture", g_pCubeTexture);


	/*g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);*/

	// Set stream source, and index buffer
	g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof(Vertex) );
	g_pd3dDevice->SetIndices(g_pIB) ;
	g_pd3dDevice->SetFVF(VERTEX_FVF) ;

	// Totally 24 points and 12 triangles
	g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12) ;

	g_pEffect->EndPass();
	g_pEffect->End();
}

VOID Cleanup()
{
	SAFE_RELEASE(g_pVB);
	SAFE_RELEASE(g_pIB);
	SAFE_RELEASE(g_pEffect);
	SAFE_RELEASE(g_pCubeTexture);
	SAFE_RELEASE(g_pd3dDevice) ;
	SAFE_RELEASE(g_pD3D) ;
}

void Render(float timeDelta)
{
	if (!g_bActive)
	{
		Sleep(50) ;
	}

	//SetupMatrix() ;

	// Clear the back-buffer to a RED color
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );

	// Begin the scene
	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		// Draw teapot 
		DrawCube();

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