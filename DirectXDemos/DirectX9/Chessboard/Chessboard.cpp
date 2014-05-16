/*
This is a Demo show you how to obtain special effect by using viewport, in this Demo, we draw a chessboard
1 Create the rectangles you want to clear
2 Use Clear function to clear the rectangles
*/

#include <d3dx9.h>

LPDIRECT3D9             g_pD3D				= NULL ;	// Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice		= NULL ;	// Our rendering device
ID3DXMesh*				g_pTeapotMesh				= NULL ;	// Hold the teapot
const unsigned int		g_numBlackCell		= 2 ;		// Number of black cells
const unsigned int		g_numWhiteCell		= 2 ;		// Number of white cells
D3DRECT					g_pRectBlack[g_numBlackCell] ;	// Black rectangles for chessboard
D3DRECT					g_pRectWhite[g_numWhiteCell] ;	// White rectangles for chessboard
const unsigned int		g_numCellsPerColumn			= 2 ;		// Number of cells per edge for each color

// We prepare for rectangles for Clear function, two in red two in yellow
void PrepareRectangles(HWND hWnd)
{
	// Get window info
	RECT rect ; 
	GetClientRect(hWnd, &rect) ;

	// start position for the first cell
	int startX = rect.left ;
	int startY = rect.top ;

	// width and height for each cell
	int widthX = (rect.right - rect.left) / g_numCellsPerColumn ;
	int widthY = (rect.bottom - rect.top) / g_numCellsPerColumn ;

	g_pRectWhite[0].x1 = 0 ;
	g_pRectWhite[0].y1 = 0 ;
	g_pRectWhite[0].x2 = widthX ;
	g_pRectWhite[0].y2 = widthY ;

	g_pRectWhite[1].x1 = widthX ;
	g_pRectWhite[1].y1 = widthY ;
	g_pRectWhite[1].x2 = 2 * widthX ;
	g_pRectWhite[1].y2 = 2 * widthY ;

	g_pRectBlack[0].x1 = widthX ;
	g_pRectBlack[0].y1 = 0 ;
	g_pRectBlack[0].x2 = 2 * widthX ;
	g_pRectBlack[0].y2 = widthY ;

	g_pRectBlack[1].x1 = 0 ;
	g_pRectBlack[1].y1 = widthY ;
	g_pRectBlack[1].x2 = widthX ;
	g_pRectBlack[1].y2 = 2 * widthY ;
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

	//g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE) ;

	g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME) ;

	// Create teapot
	D3DXCreateTeapot(g_pd3dDevice, &g_pTeapotMesh, NULL) ;

	// Prepare rectangles
	PrepareRectangles(hWnd) ;

	return S_OK;
}

VOID Cleanup()
{
	// Release teapot mesh
	if(g_pTeapotMesh != NULL)
		g_pTeapotMesh->Release() ;

	// Release D3D
	if( g_pD3D != NULL)
		g_pD3D->Release();

	// Release device
	if( g_pd3dDevice != NULL) 
		g_pd3dDevice->Release();
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

	// Clear the viewport
	g_pd3dDevice->Clear(g_numBlackCell, g_pRectBlack, D3DCLEAR_TARGET, 0xff000000, 0, 0) ;
	g_pd3dDevice->Clear(g_numWhiteCell, g_pRectWhite, D3DCLEAR_TARGET, 0xffffffff, 0, 0) ;

	// Begin the scene
	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
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

	winClass.lpszClassName = "Viewport";
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
		"Viewport",					// window caption
		WS_OVERLAPPEDWINDOW, 		// window style
		0,							// initial x position
		0,							// initial y position
		656,						// initial window width
		672,						// initial window height
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