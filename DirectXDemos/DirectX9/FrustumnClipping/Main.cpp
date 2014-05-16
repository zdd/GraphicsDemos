/*
This demo has a problem, when the model was at the left or bottom of
the window, the model was not displayed correctly, it will disappear when
it is partly hide by the window boundary. may be the culling algorithm was
not correct.
*/

#include <d3dx9.h>
#include "Camera.h"
#include "Terrain.h"
#include "Font.h"
#include <limits.h>
#include <d3d9types.h>

#define SAFE_RELEASE(p) { if (p) { (p)->Release(); (p) = NULL; } }

LPDIRECT3D9             g_pD3D				= NULL ; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice		= NULL ; // Our rendering device
ID3DXMesh*				g_pTeapotMesh				= NULL ; // Hold the teapot

Camera*					g_pCamera			= new Camera() ;	// First person view camera
Terrain*				g_pTerrain			= new Terrain() ;	// A flat terrain 

const int				g_numModel = 100; // Number of models
ID3DXMesh*				g_mesh[g_numModel] = {NULL} ;
D3DXMATRIX				g_matWorld[g_numModel] ; // World matrix for each model

Font*					g_Font				= NULL ; // Font

int						g_numVertices   = 0;
int						g_numFaces		= 0;

float GetRandomFloat(float lowBound, float highBound)
{
	if( lowBound >= highBound ) // bad input
		return lowBound;

	// get random float in [0, 1] interval
	float f = (rand() % 10000) * 0.0001f; 

	// return float in [lowBound, highBound] interval. 
	return (f * (highBound - lowBound)) + lowBound; 
}

void GetRandomVector(D3DXVECTOR3* out, D3DXVECTOR3* min, D3DXVECTOR3* max)
{
	out->x = GetRandomFloat(min->x, max->x);
	out->y = GetRandomFloat(min->y, max->y);
	out->z = GetRandomFloat(min->z, max->z);
}

// Randomly generate g_numModel(1000) boxes
void GenerateModel()
{
	// Randomly generate width, height and depth for boxes

	// Create each box
	for (int i = 0; i < g_numModel; ++i)
	{
		float width	 = GetRandomFloat(1.0f, 10.f);
		float height = GetRandomFloat(1.0f, 10.f);
		float depth  = GetRandomFloat(1.0f, 10.f);
		D3DXCreateBox(g_pd3dDevice, width, height, depth, &g_mesh[i], NULL);
		//D3DXCreateTeapot(g_pd3dDevice, &g_mesh[i], NULL);

		// Move each boxes to a random position
		float posX = GetRandomFloat(-100.f, 100.f);
		float posY = GetRandomFloat(-100.f, 100.f);
		float posZ = GetRandomFloat(-100.f, 100.f);

		D3DXMatrixTranslation(&g_matWorld[i], posX, posY, posZ) ;
	}
}

// Compute the AABB manually
// Loop each vertex in the vertex buffer, and calculate the minX, minY, minZ
// maxX, maxY, maxZ, then the box.minPt = (minX, minY, minZ), box.maxPt = (maxX, maxY, maxZ)
void ComputeAABB(ID3DXMesh* mesh, AABB& box)
{
	float minX = (float)INT_MAX; 
	float minY = (float)INT_MAX; 
	float minZ = (float)INT_MAX;

	float maxX = (float)INT_MIN;
	float maxY = (float)INT_MIN;
	float maxZ = (float)INT_MIN;

	IDirect3DVertexBuffer9* vertexBuffer = NULL;
	mesh->GetVertexBuffer(&vertexBuffer);

	// Get vertex buffer description
	D3DVERTEXBUFFER_DESC pDesc;
	vertexBuffer->GetDesc(&pDesc);

	// Get vertex buffer size
	UINT vertexBufferSize = pDesc.Size;

	// Get number of bytes per vertex
	DWORD numBytesPerVertex = mesh->GetNumBytesPerVertex();

	// Get Flexible Vertex Format
	DWORD FVFCode = mesh->GetFVF();

	DWORD numVertex = mesh->GetNumVertices();

	float *v;
	HRESULT result = vertexBuffer->Lock(0, vertexBufferSize, (void**)&v, D3DLOCK_READONLY);
	if (result != D3D_OK)
	{
		MessageBox(NULL, "Lock vertex buffer failed", "Error", 0);
		return;
	}

	for (DWORD i = 0; i < vertexBufferSize; i += 6)
	{
		// Get vertex position
		float x = v[0];
		float y = v[1];
		float z = v[2];

		// Update min and max values
		if (x > maxX){ maxX = x;}
		if (x < minX){ minX = x;}

		if (y > maxY) {maxY = y;}
		if (y < minY) {minY = y;}

		if (z > maxZ) {maxZ = z;}
		if (z < minZ) {minZ = z;}

		// Get vertex normal
		float nx = v[3];
		float ny = v[4];
		float nz = v[5];
	}

	box.minPt = D3DXVECTOR3(minX, minY, minZ);
	box.maxPt = D3DXVECTOR3(maxX, maxY, maxZ);
}

void DrawModel()
{
	g_numVertices = 0;
	for (int i = 0; i < g_numModel; ++i)
	{
		// Compute AABB without D3D function
		AABB box;
		ComputeAABB(g_mesh[i], box);
		
		// Transform AABB box to World space
		box.transform(g_matWorld[i], box);

		// Draw visible models
		if (g_pCamera->isVisible(box))
		{
			g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_matWorld[i]);
			g_mesh[i]->DrawSubset(0);

			// Compute number of vertex
			g_numVertices += g_mesh[i]->GetNumVertices() ;

			// Compute number of faces
			g_numFaces += g_mesh[i]->GetNumFaces() ;
		}
	}
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

	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE) ;

	g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME) ;

	// Create teapot
	D3DXCreateTeapot(g_pd3dDevice, &g_pTeapotMesh, NULL) ;

	// Set view matrix
	D3DXVECTOR3 eyePt(0, 0, -10.0f) ;
	D3DXVECTOR3 lookAt(0, 0, 0) ;
	D3DXVECTOR3 upVec(0, 1.0f, 0) ;
	g_pCamera->SetViewParams(eyePt, lookAt, upVec) ;

	// Set projection matrix
	g_pCamera->SetProjParams(D3DX_PI / 4, 1.0f /*d3dpp.BackBufferWidth / d3dpp.BackBufferHeight*/, 1.0f, 1000.0f) ;

	// Initialize terrain
	g_pTerrain->BuildGridsBuffer(g_pd3dDevice) ;

	// Create models
	GenerateModel();

	// Create new font
	g_Font = new Font(g_pd3dDevice) ;

	return S_OK;
}

VOID Cleanup()
{
	SAFE_RELEASE(g_pTeapotMesh) ;
	SAFE_RELEASE(g_pd3dDevice) ;
	SAFE_RELEASE(g_pD3D) ;
	g_pTerrain->Release() ;

	// Release all model mesh
	for (int i = 0; i < g_numModel; ++i)
	{
		if (g_mesh[i] != NULL)
		{
			g_mesh[i]->Release() ;
			g_mesh[i] = NULL ;
		}
	}

	// Release font
	if (g_Font != NULL)
	{
		g_Font->Release() ;
		g_Font = NULL ;
	}
}

void SetupMatrix()
{
	D3DXMATRIX view = g_pCamera->view() ;
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &view) ;

	D3DXMATRIX proj = g_pCamera->proj() ;
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &proj) ;
}

VOID Render()
{
	SetupMatrix() ;

	g_pCamera->update(0.1f, 1.0f) ;

	// Clear the back-buffer to a RED color
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );

	// Begin the scene
	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		// Draw teapot 
		// g_pTeapotMesh->DrawSubset(0) ;

		g_pTerrain->Render(g_pd3dDevice) ;

		// Draw all models
		DrawModel() ;

		char buffer[10] ;
		 _itoa_s( g_numVertices, buffer, sizeof(buffer), 10);
		POINT point = { 10, 10 } ;
		g_Font->Draw(point, buffer, 0xffff0000) ;

		/*delete []wc_numvertex ;
		wc_numvertex = NULL ;*/

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

INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR szCmdLine, int iCmdShow)
{
	WNDCLASSEX winClass ;

	winClass.lpszClassName = "Frustum Culling";
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
		"Frustum Culling",					// window caption
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