/*

This Demo shows how to Animate a texture by changing texture coordinates
Usage:
R-Rotate
T-Translate
S-Scale

*/

#include <d3dx9.h>
#include <DxErr.h>

LPDIRECT3D9             g_pD3D				= NULL ; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice		= NULL ; // Our rendering device
LPDIRECT3DVERTEXBUFFER9 g_pVB				= NULL ; // Vertex buffer
LPDIRECT3DTEXTURE9		g_pTexture			= NULL ; // Texture

#define SAFE_RELEASE(P) if(P){ P->Release(); P = NULL;}

int g_EffectFlag = 0 ; // flag of effect, translation:1, rotation:2, scaling:3
int g_Factor = 2 ;	// scale factor

D3DXMATRIX matTexture ;	// Texture matrix

float angle = 0.0f ; // rotation angle

struct Vertex
{
	float x, y, z ; // Position
	float u, v ;	// Texture coordinates
};

#define Vertex_FVF D3DFVF_XYZ | D3DFVF_TEX1

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

	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE) ;

	// Create texture
	HRESULT hr = D3DXCreateTextureFromFile(g_pd3dDevice, "../Common/Media/chessboard.jpg", &g_pTexture) ;
	if (FAILED(hr))
	{
		DXTRACE_ERR_MSGBOX(DXGetErrorString(hr), hr) ;
	}

	// Texture state
	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU,  D3DTADDRESS_WRAP );
	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV,  D3DTADDRESS_WRAP );

	D3DXMatrixIdentity(&matTexture) ;

	return S_OK;
}

VOID InitVertexBuffer()
{
	// Define vertex data for quad
	Vertex Quad[] =
	{
		{-4.0f, -4.0f, 0.0f, 0.0f,  1.0f},  // 0
		{-4.0f,  4.0f, 0.0f, 0.0f,  0.0f},  // 1
		{ 4.0f, -4.0f, 0.0f, 1.0f,  1.0f},  // 3
		{ 4.0f,  4.0f, 0.0f, 1.0f,  0.0f},  // 2
	} ;

	// Create vertex buffer
	HRESULT hr ;
	hr = g_pd3dDevice->CreateVertexBuffer(4 * sizeof(Vertex), D3DUSAGE_WRITEONLY, 
		Vertex_FVF, D3DPOOL_MANAGED, &g_pVB, NULL) ;
	if (FAILED(hr))
	{
		DXTRACE_ERR_MSGBOX(DXGetErrorString(hr), hr) ;
	}

	// Fill vertex buffer
	Vertex* v ;
	g_pVB->Lock(0, 0, (void**)&v, 0) ;
	memcpy(v, Quad, 4 * sizeof(Vertex)) ;
	g_pVB->Unlock() ;
}

VOID RenderQuad()
{
	//// Disable lighting
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE) ;

	// Set texture
	g_pd3dDevice->SetTexture(0, g_pTexture) ;
	g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(Vertex)) ;
	g_pd3dDevice->SetFVF(Vertex_FVF) ;

	// Draw quad
	g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID) ;
	g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2) ;
}

VOID Cleanup()
{
	SAFE_RELEASE(g_pVB) ;
	SAFE_RELEASE(g_pTexture) ;
	SAFE_RELEASE(g_pd3dDevice) ;
	SAFE_RELEASE(g_pD3D) ;
}

VOID SetupMatrix()
{
	// Set view matrix
	D3DXVECTOR3 eyePt(0, 1.0f, -15.0f) ;
	D3DXVECTOR3 lookAt(0, 0.0f, 0) ;
	D3DXVECTOR3 upVec(0, 1.0f, 0) ;
	D3DXMATRIX view ;
	D3DXMatrixLookAtLH(&view, &eyePt, &lookAt, &upVec) ;
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &view) ;

	// Set projection matrix
	D3DXMATRIX proj ;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI / 4, 1.0f, 1.0f, 1000.0f) ;
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &proj) ;
}

void UpdateTextureCoordinates(float timeDelta)
{
	g_pd3dDevice->SetTextureStageState(0, 
		D3DTSS_TEXTURETRANSFORMFLAGS, 
		D3DTTFF_COUNT2);

	if (g_EffectFlag == 1) // Translation
	{
		// Translate
		matTexture._31 += timeDelta / 3 ;
	}
	else if (g_EffectFlag == 2) // Rotation
	{
		angle += timeDelta ; // increasing total angle
		matTexture._11 = cosf(angle) ;
		matTexture._12 = sinf(angle) ;
		matTexture._21 = -sinf(angle) ;
		matTexture._22 = cosf(angle) ;
	}
	else if (g_EffectFlag == 3) // Scaling
	{
		// if the picture is too small, increasing
		if (matTexture._11 < 0.6)
			g_Factor = -g_Factor ;

		// if the picture is too large, decreasing
		if (matTexture._11 > 3.0f)
			g_Factor = -g_Factor ;

		matTexture._11 += timeDelta * g_Factor;
		matTexture._22 += timeDelta * g_Factor;
	}
	
	g_pd3dDevice->SetTransform(D3DTS_TEXTURE0, &matTexture) ;
}

VOID Render(float timeDelta)
{
	SetupMatrix() ;

	UpdateTextureCoordinates(timeDelta) ;

	// Clear the back-buffer to a RED color
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff568800, 1.0f, 0 );

	// Begin the scene
	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		RenderQuad() ;

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
			// Since the Rotation matrix and the scaling matrix will effect each other
			// so we Identity the matTexture at each key press to clear the old matrix data
			switch( wParam )
			{
			case 'T':
				D3DXMatrixIdentity(&matTexture) ;
				g_EffectFlag = 1 ;
				break ;
			case 'R':
				D3DXMatrixIdentity(&matTexture) ;
				g_EffectFlag = 2 ;
				break ;
			case 'S':
				D3DXMatrixIdentity(&matTexture) ;
				g_EffectFlag = 3 ;
				break ;
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

	winClass.lpszClassName = "DynamicTexture";
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
		"DynamicTexture",					// window caption
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
		InitVertexBuffer() ;

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
				Render(timeDelta) ;

				// Update last time to current time for next loop
				lastTime = currTime;
			}
		}
	}

	UnregisterClass(winClass.lpszClassName, hInstance) ;
	return 0;
}