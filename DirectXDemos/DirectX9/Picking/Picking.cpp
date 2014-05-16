#include <d3dx9.h>   
#pragma warning( disable : 4996 ) // disable deprecated warning    
#pragma warning( default : 4996 )    
  
  
LPDIRECT3D9             g_pD3D       = NULL;    
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; // Our rendering device   
ID3DXMesh* g_mesh = 0; // mesh for sphere

// forward declaration

// Picking ray
struct Ray
{
     D3DXVECTOR3 _origin;
     D3DXVECTOR3 _direction;
};

// calculate the picking ray according to screen coordinates
Ray CalcPickingRay(LPDIRECT3DDEVICE9 Device, int x, int y) ;

// transform the picking ray from view space to world space
void TransformRay(Ray* ray, D3DXMATRIX* T) ;

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
  
    // Turn off culling, so we see the front and back of the triangle   
    g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );   
  
    // Draw a wireframe shpere   
    g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);   
    g_pd3dDevice->SetRenderState( D3DRS_LIGHTING , FALSE );   
  
	// Create a sphere
	D3DXCreateSphere(g_pd3dDevice, 1.0f, 10, 10, &g_mesh, NULL) ;

    return S_OK;   
}   
  
VOID Cleanup()   
{   
    if( g_pd3dDevice != NULL )    
        g_pd3dDevice->Release();   
  
    if( g_pD3D != NULL )          
        g_pD3D->Release();   

	if(g_mesh != NULL)
		g_mesh->Release();
}   
  
VOID SetupMatrix()   
{   
    // Draw the sphere at origin   
    D3DXMATRIXA16 matWorld ;   
    D3DXMatrixTranslation( &matWorld, 0.0f, 0.0f, 0.0f) ;   
    g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );   
  
    // Set view matrix   
    D3DXVECTOR3 vEyePt( 0.0f, 3.0f,-5.0f );   
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );   
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );   
    D3DXMATRIXA16 matView;   
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );   
    g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );   
  
    // Set projection matrix   
    D3DXMATRIXA16 matProj;   
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 100.0f );   
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );   
  
}   
  
VOID Render()   
{   
    // Clear the backbuffer to a black color   
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );   
  
  
    // Begin the scene   
    if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )   
    {   
        SetupMatrix() ;   
  
		// Draw the sphere
		g_mesh->DrawSubset(0) ;

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
            }   
        } 

		// User click on screen
		case WM_LBUTTONDOWN:
		{
			// Retrieve screen point
			int iMouseX = (short)LOWORD(lParam) ;
			int iMouseY = (short)HIWORD(lParam) ;
			
			// Calculate the picking ray
			Ray ray = CalcPickingRay(g_pd3dDevice, iMouseX, iMouseY) ;

			// transform the ray from view space to world space
			// get view matrix
			D3DXMATRIX view;
			g_pd3dDevice->GetTransform(D3DTS_VIEW, &view);

			// inverse it
			D3DXMATRIX viewInverse;
			D3DXMatrixInverse(&viewInverse, 0, &view);

			// apply on the ray
			TransformRay(&ray, &viewInverse) ;

			// collision detection
			D3DXVECTOR3 v(0.0f, 0.0f, 0.0f);
			if(D3DXSphereBoundProbe(&v, 1.0f, &ray._origin, &ray._direction))
			{
				g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
			}
			break ;
		}
		return 0 ;

		case WM_LBUTTONUP:
			{
				g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
				break ;
			}
        return 0 ;   
  
        case WM_DESTROY:   
            Cleanup();   
            PostQuitMessage( 0 );   
            return 0;   
    }   
  
    return DefWindowProc( hWnd, msg, wParam, lParam );   
}   
  
//-----------------------------------------------------------------------------   
// Name: wWinMain()   
// Desc: The application's entry point   
//-----------------------------------------------------------------------------   
INT WINAPI wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, INT )   
{   
    // Register the window class   
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,   
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,   
                      "D3D Tutoria", NULL };   
    RegisterClassEx( &wc );   
  
    // Create the application's window   
    HWND hWnd = CreateWindow( "D3D Tutoria", "Cube",   
                              WS_OVERLAPPEDWINDOW, 10, 10, 600, 600,   
                              NULL, NULL, wc.hInstance, NULL );   
  
    // Initialize Direct3D   
	if( SUCCEEDED( InitD3D( hWnd ) ) )   
	{   

		// Show the window   
		ShowWindow( hWnd, SW_SHOWDEFAULT );   
		UpdateWindow( hWnd );   

		// Enter the message loop   
		MSG msg;   
		ZeroMemory( &msg, sizeof(msg) );   
		while( msg.message!=WM_QUIT )   
		{   
			if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )   
			{   
				TranslateMessage( &msg );   
				DispatchMessage( &msg );   
			}   
			else  
				Render();   
		}   
	}   
  
    UnregisterClass( "D3D Tutoria", wc.hInstance );   
    return 0;   
}   

// 根据用户点击的屏幕上的一点，计算相应的拾取射线
Ray CalcPickingRay(LPDIRECT3DDEVICE9 Device, int screen_x, int screen_y) // x, y是屏幕坐标
{
     float px = 0.0f;
     float py = 0.0f;

	 // Get viewport
     D3DVIEWPORT9 vp;
     Device->GetViewport(&vp);

	 // Get Projection matrix
     D3DXMATRIX proj;
     Device->GetTransform(D3DTS_PROJECTION, &proj);

	 // 根据屏幕坐标计算其对应的投影窗口上的坐标
	 // 这个过程实际上分为两部分
	 // 1. 屏幕坐标到视口坐标  (2.0f*x) / vp.Width)  - 1.0f)
	 // 2. 视口坐标到投影窗口坐标 / proj(0, 0);
	 // 这里假定视口坐标的起点是x=0,y=0
	 // 关于这个公式的具体推导过程，请参看Introduction to 3D Game Programming with DirectX 9.0,第十五章。
     
	 px = ((( 2.0f * screen_x) / vp.Width)  - 1.0f) / proj(0, 0);
     py = (((-2.0f * screen_y) / vp.Height) + 1.0f) / proj(1, 1);

     Ray ray;
     ray._origin    = D3DXVECTOR3(0.0f, 0.0f, 0.0f); // 起点是原点
     ray._direction = D3DXVECTOR3(px, py, 1.0f); // 方向，注意投影平面的Z坐标是1 

     return ray;
}

// transform the ray from view space to world space
void TransformRay(Ray* ray, D3DXMATRIX* invertViewMatrix)
{
	// transform the ray's origin, w = 1.
	D3DXVec3TransformCoord(
		&ray->_origin,
		&ray->_origin,
		invertViewMatrix);

	// transform the ray's direction, w = 0.
	D3DXVec3TransformNormal(
		&ray->_direction,
		&ray->_direction,
		invertViewMatrix);

	// normalize the direction
	D3DXVec3Normalize(&ray->_direction, &ray->_direction);
}