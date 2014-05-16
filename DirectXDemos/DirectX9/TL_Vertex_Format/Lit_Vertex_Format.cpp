#include <tchar.h>
#include <d3dx9.h>
#include <mmsystem.h>
#pragma warning( disable : 4996 ) // disable deprecated warning 
#include <strsafe.h>
#pragma warning( default : 4996 ) 


//
// 这个例子使用D3DFVF_XYZ|D3DFVF_DIFFUSE顶点格式创建了一个Cube（立方体）
// 该顶点格式无需光照，自带颜色信息，所以填充完顶点数据以后，只需设置一下
// 视图矩阵和投影矩阵就可以渲染了
//
// 该程序响应了键盘消息，使Cube可以按三个坐标轴方向旋转
// 按下X,Y,Z键时，分别按X,Y,Z轴方向旋转
// 按空格键时停止旋转
// 按ESC键退出程序
//


LPDIRECT3D9             g_pD3D       = NULL; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; // Our rendering device
LPDIRECT3DVERTEXBUFFER9 g_pVB        = NULL; // Buffer to hold vertices
int g_rotAxis = 0; 

// A structure for our custom vertex type
struct CUSTOMVERTEX
{
	FLOAT x, y, z;	// vertex position
	DWORD color;	// vertex color
};

// Vertex format
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)

HRESULT InitVB();

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

	InitVB();

	// Turn off culling, so we see the front and back of the triangle
	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );

	// Disable lighting, since the vertex contains colors
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING , FALSE );

	return S_OK;
}

HRESULT InitVB()
{
	// 八个顶点
	CUSTOMVERTEX A = { 0.0f, 0.0f, -1.0f, 0xffff0000 }; // Position and color
	CUSTOMVERTEX B = { 1.0f, 0.0f, -1.0f, 0x0000ffff };
	CUSTOMVERTEX C = { 1.0f, 0.0f,  0.0f, 0x00ffff00 };
	CUSTOMVERTEX D = { 0.0f, 0.0f,  0.0f, 0xff0000ff };
	CUSTOMVERTEX E = { 0.0f, 1.0f, -1.0f, 0x00ffff00 };
	CUSTOMVERTEX F = { 1.0f, 1.0f, -1.0f, 0xff0000ff };
	CUSTOMVERTEX G = { 1.0f, 1.0f,  0.0f, 0xffff0000 };
	CUSTOMVERTEX H = { 0.0f, 1.0f,  0.0f, 0x0000ffff };

	// Triangles
	CUSTOMVERTEX vertices[] =
	{
		A, B, C, A, C, D, // bottom
		E, H, G, E, G, F, // top
		A, E, F, A, F, B, // front
		C, G, H, C, H, D, // back
		D, H, E, D, E, A, // left
		B, F, G, B, G, C, // right
	};

	// Create vertex buffer
	if( FAILED( g_pd3dDevice->CreateVertexBuffer( 36 * sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX,
		D3DPOOL_DEFAULT, &g_pVB, NULL ) ) )
	{
		return E_FAIL;
	}

	// Fill in date to vertex buffer
	VOID* pVertices;
	if( FAILED( g_pVB->Lock( 0, sizeof(vertices), (void**)&pVertices, 0 ) ) )
		return E_FAIL;
	memcpy( pVertices, vertices, sizeof(vertices) );
	g_pVB->Unlock();

	return S_OK;
}

VOID Cleanup()
{
	if( g_pVB != NULL )        
		g_pVB->Release();

	if( g_pd3dDevice != NULL ) 
		g_pd3dDevice->Release();

	if( g_pD3D != NULL )       
		g_pD3D->Release();
}

VOID SetupMatrix()
{
	// Translate so the center of the box is the coordinate system origin.
	D3DXMATRIXA16 matTrans ;
	D3DXMatrixTranslation( &matTrans, -0.5f, -0.5f, 0.5f) ;

	// rotation by time elapsed
	D3DXMATRIXA16 matRol ;
	UINT  iTime  = timeGetTime() % 1000;
	FLOAT fAngle = iTime * (2.0f * D3DX_PI) / 1000.0f;


	if( g_rotAxis == 4 )	// rotate by x-axis.
		D3DXMatrixRotationX( &matRol, fAngle );

	else if( g_rotAxis == 2 ) // rotate by y-axis.
		D3DXMatrixRotationY( &matRol, fAngle );

	else if( g_rotAxis == 1 ) // rotate by z-axis.
		D3DXMatrixRotationZ( &matRol, fAngle );

	else
		D3DXMatrixIdentity( &matRol ) ; // hold on

	D3DXMATRIXA16 matWorld = matTrans * matRol ;
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	// Set up view matrix
	D3DXVECTOR3 vEyePt( 0.0f, 3.0f,-5.0f );		
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

VOID Render()
{
	// Clear the backbuffer to a blue color
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );

	// Begin the scene
	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		SetupMatrix() ;
		// Draw the triangles in the vertex buffer. This is broken into a few
		// steps. We are passing the vertices down a "stream", so first we need
		// to specify the source of that stream, which is our vertex buffer. Then
		// we need to let D3D know what vertex shader to use. Full, custom vertex
		// shaders are an advanced topic, but in most cases the vertex shader is
		// just the FVF, so that D3D knows what type of vertices we are dealing
		// with. Finally, we call DrawPrimitive() which does the actual rendering
		// of our geometry (in this case, just one triangle).
		g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof(CUSTOMVERTEX) );
		g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );

		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 12 );

		// End the scene
		g_pd3dDevice->EndScene();
	}

	// Present the backbuffer contents to the display
	g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}

//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg )
	{
	case WM_KEYDOWN:
		{
			switch( wParam )
			{
			case VK_ESCAPE:
				PostQuitMessage(0);
				break ;

			case 'X':
				g_rotAxis = 4 ;
				break ;

			case 'Y':
				g_rotAxis = 2 ;
				break ;

			case 'Z':
				g_rotAxis = 1 ;
				break ;

			case VK_SPACE:
				g_rotAxis = 0 ;
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

	winClass.lpszClassName = "Lit Cube";
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
		"Lit Cube",					// window caption
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