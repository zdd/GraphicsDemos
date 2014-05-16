/*
This Demo show how to Compute normals by using function D3DXComputeNormals

Steps:
1. Create Mesh
2. Lock vertex buffers and Fill in Mesh data
3. Create normals
4. Draw with light enable(draw with light to check the normals are correct)

Notes:
the Vertex FVF must contains D3DFVF_NORMAL
*/

#include <d3dx9.h>
#include <MMSystem.h>

LPDIRECT3D9             g_pD3D				= NULL ; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice		= NULL ; // Our rendering device
LPDIRECT3DVERTEXBUFFER9 g_pVB				= NULL ; // Vertex buffer
LPDIRECT3DINDEXBUFFER9  g_pIB				= NULL ; // Index buffer
LPD3DXMESH				g_pMesh				= NULL;	 // Mesh

bool					g_bActive			= true ; // Is window active?

#define SAFE_RELEASE(P) if(P){ P->Release(); P = NULL;}

#define Vertex_FVF D3DFVF_XYZ | D3DFVF_NORMAL

struct Vertex
{
	float  x,  y,  z;	// position
	float nx, ny, nz;	// normal
};

const int NumVertex = 8;
const int NumTriangles = 12;
const int NumIndex  = Vertex_FVF * 3;

HRESULT InitMesh()
{
	// Create mesh
	HRESULT hr = D3DXCreateMeshFVF(
		NumTriangles, 
		NumVertex, 
		D3DXMESH_MANAGED | D3DXMESH_32BIT,
		Vertex_FVF, 
		g_pd3dDevice,
		&g_pMesh);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// Define the 8 vertex of the cube
	D3DXVECTOR3 vertex[] =
	{
		D3DXVECTOR3(-0.5f, -0.5f, -0.5f),
		D3DXVECTOR3( 0.5f, -0.5f, -0.5f),
		D3DXVECTOR3( 0.5f,  0.5f, -0.5f),
		D3DXVECTOR3(-0.5f,  0.5f, -0.5f),

		D3DXVECTOR3(-0.5f, -0.5f,  0.5f),
		D3DXVECTOR3( 0.5f, -0.5f,  0.5f),
		D3DXVECTOR3( 0.5f,  0.5f,  0.5f),
		D3DXVECTOR3(-0.5f,  0.5f,  0.5f),
	};

	// Lock vertex buffer and copy data
	void* pVertices = NULL;
	g_pMesh->LockVertexBuffer(0, &pVertices);
	memcpy(pVertices, vertex, sizeof(vertex));
	g_pMesh->UnlockVertexBuffer();

	// Define 36 index of the cube
	DWORD index[] = 
	{
		// Front face
		0, 3, 1,
		3, 2, 1,

		// Back face
		5, 6, 2,
		6, 7, 2,

		// Left face
		4, 7, 0,
		7, 3, 0,

		// Right face
		1, 2, 5,
		2, 6, 5,

		// Top face
		3, 7, 2,
		7, 6, 2,

		// Bottom face
		4, 0, 5,
		0, 1, 5,
	};
	
	// Lock index buffer and copy data
	DWORD* pIndices = NULL;
	g_pMesh->LockIndexBuffer(0, (void**)&pIndices);
	memcpy(pIndices, index, sizeof(index));
	g_pMesh->UnlockIndexBuffer();

	// Compute normals
	D3DXComputeNormals(g_pMesh, 0);

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

	// Disable lighting, since we didn't specify color for vertex
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING , FALSE );   

	InitMesh();

	return S_OK;
}

void SetupLight()
{
	D3DLIGHT9 light;
	light.Type = D3DLIGHT_POINT;
	D3DXCOLOR color(255, 80, 90, 0);

	light.Ambient		= color * 0.6f;
	light.Diffuse		= color;
	light.Specular		= color * 0.6f;
	light.Position		= D3DXVECTOR3(-2.0f, 2.0f, -2.0f);
	light.Range			= 320.0f;
	light.Falloff			= 1.0f;
	light.Attenuation0	= 1.0f;
	light.Attenuation1	= 0.0f;
	light.Attenuation2	= 0.0f;

	g_pd3dDevice->SetLight(0, &light) ;			// Set light ;
	g_pd3dDevice->LightEnable(0, true) ;		// Enable light
	g_pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	g_pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE, true);
}

void DrawCube()
{
	// Setup a point light
	SetupLight();
	g_pMesh->DrawSubset(0);
}

VOID Cleanup()
{
	SAFE_RELEASE(g_pVB);
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
	D3DXVECTOR3 eyePt(2.0f, 2.0f, -2.0f) ;
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
		// Draw cube
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