/*
Description:
This demo show how to create and use D3D Mesh
We Draw a n * n grid, here n = 8, you can change n to 
generate a different size of grid!

Steps to use D3D mesh
1. Create Mesh
2. Lock mesh vertex buffer and index buffer
3. Fill in Data
4. Unlock buffers
5. Create normals if needed
6. Draw mesh subset
*/
#include <d3dx9.h>
#include <MMSystem.h>

LPDIRECT3D9             g_pD3D				= NULL ;	// Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice		= NULL ;	// Our rendering device
ID3DXMesh*				g_pMesh				= NULL;		// D3D mesh

bool					g_bActive			= true ; // Is window active?

#define SAFE_RELEASE(P) if(P){ P->Release(); P = NULL;}

#define VERTEX_FVF D3DFVF_XYZ // vertex format

const int VertexperRow = 8;
const int VertexperCol = 8;
const WORD NumVertex = VertexperRow * VertexperRow;
const WORD NumTriangles = (VertexperRow - 1) * (VertexperRow - 1) * 2;
const WORD NumIndex = NumTriangles * 3;

HRESULT InitMesh()
{
	// Create mesh
	HRESULT hr = D3DXCreateMeshFVF(
		NumTriangles, 
		NumVertex, 
		D3DXMESH_MANAGED | D3DXMESH_32BIT,
		VERTEX_FVF, 
		g_pd3dDevice,
		&g_pMesh);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	float offsetX = VertexperRow * 0.5f;
	float offsetZ = VertexperCol * 0.5f;

	// Create vertex data
	int k = 0;
	D3DXVECTOR3* pvertexArray = new D3DXVECTOR3[NumVertex];
	for (int i = 0; i < VertexperRow; ++i)
	{
		for (int j = 0; j < VertexperCol; ++j)
		{
			pvertexArray[k].x = j - offsetX;
			pvertexArray[k].y = 0.0f;
			pvertexArray[k].z = i - offsetZ;
			++k;
		}
	}

	// Lock mesh vertex buffer and copy data
	void* pVertices;
	hr = g_pMesh->LockVertexBuffer(0, &pVertices);
	if (FAILED(hr))
	{
		return E_FAIL;
	}
	memcpy(pVertices, pvertexArray, NumVertex * sizeof(D3DXVECTOR3));
	g_pMesh->UnlockVertexBuffer();

	// Release vertex buffer
	delete []pvertexArray;
	pvertexArray = NULL;

	// Create index data
	k = 0;
	DWORD* pindexArray = new DWORD[NumIndex];
	for (DWORD i = 0; i < VertexperRow - 1; ++i)
	{
		for (DWORD j = 0; j < VertexperCol - 1; ++j)
		{
			pindexArray[k]     = i * VertexperCol + j;				// 0
			pindexArray[k + 1] = i * VertexperCol + (j + 1);		// 1
			pindexArray[k + 2] = (i + 1) * VertexperCol + j;		// 2

			pindexArray[k + 3] = (i + 1) * VertexperCol + j;		// 3
			pindexArray[k + 4] = i * VertexperCol + (j + 1);		// 4
			pindexArray[k + 5] = (i + 1) * VertexperCol + (j + 1);	// 5

			k += 6;
		}
	}

	// Lock mesh index buffer and copy data
	DWORD* pIndices = NULL;
	hr = g_pMesh->LockIndexBuffer(0, (void**)&pIndices);
	if (FAILED(hr))
	{
		return E_FAIL;
	}
	memcpy(pIndices, pindexArray, NumIndex * sizeof(DWORD));
	g_pMesh->UnlockIndexBuffer();

	// Release index array
	delete []pindexArray;
	pindexArray = NULL;

	return D3D_OK;
}

// Draw mesh using index buffer
void Draw()
{
	g_pMesh->DrawSubset(0);
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
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE) ;
	g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME) ;
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	InitMesh();

	return S_OK;
}

VOID Cleanup()
{
	SAFE_RELEASE(g_pMesh);
	SAFE_RELEASE(g_pd3dDevice) ;
	SAFE_RELEASE(g_pD3D) ;
}

void SetupMatrix()
{
	D3DXVECTOR3 eyePt(0.0f, 10.0f, -10.0f) ;
	D3DXVECTOR3 upVec(0.0f, 1.0f, 0.0f) ;
	D3DXVECTOR3 lookCenter(0.0f, 0.0f, 0.0f) ;

	// Set view matrix
	D3DXMATRIX view ;
	D3DXMatrixLookAtLH(&view, &eyePt, &lookCenter, &upVec) ;
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &view) ;

	// Set projection matrix
	D3DXMATRIX proj ;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI / 4, 1.0f, 1.0f, 1000.0f) ;
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &proj) ;
}

void Render(float timeDelta)
{
	if (!g_bActive) // Yield CPU time to other processes
		Sleep(50) ;

	SetupMatrix() ;

	// Clear the back-buffer to a RED color
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(255,0,0), 1.0f, 0 );

	// Begin the scene
	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		Draw();

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