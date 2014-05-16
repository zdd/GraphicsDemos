#include <d3dx9.h>

LPDIRECT3D9             g_pD3D			= NULL ;  
LPDIRECT3DDEVICE9       g_pd3dDevice	= NULL ; 
IDirect3DSurface9*		g_pBackBuffer	= NULL ;	// back buffer surface
IDirect3DSurface9*		g_pSourceSurface = NULL ;	// Surface to hold the desktop image

D3DSURFACE_DESC pd3dsdBackBuffer ;
D3DPRESENT_PARAMETERS d3dpp ;
int screenWidth ;
int screenHeight ;
RECT rect ;	// area to copy to back buffer
bool g_bEndRender = FALSE ;

const int g_PoolSize = 10 ;
IDirect3DSurface9* g_SurfPool[g_PoolSize] ;

// Mutex variable
HANDLE g_hMutex;
HANDLE g_hFullSemaphore;
HANDLE g_hEmptySemaphore;

const int g_ProducerCount = 1 ; // Number of producer thread
const int g_ConsumerCount = 1 ; // Number of consumer thread

int in = 0 ;	// input index
int out = 0 ;	// output index

DWORD WINAPI Producer(LPVOID);	// Get desktop image
DWORD WINAPI Consumer(LPVOID);	// Write desktop image to back buffer


#define SAFE_RELEASE(p) if (p){p->Release() ; p = NULL ;}

HRESULT InitD3D( HWND hWnd ) 
{ 
	HRESULT hr ;

	// Create the D3D object, which is needed to create the D3DDevice. 
	if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) ) 
	{ 
		MessageBox(NULL, "Create D3D9 object failed!", "Error", 0) ; 
		return E_FAIL; 
	} 

	D3DDISPLAYMODE	ddm;
	hr = g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&ddm) ;
	if(FAILED(hr))
	{
		MessageBox(NULL, "Unable to Get Adapter Display Mode", "Error", 0) ; 
		return E_FAIL;
	}

	ZeroMemory( &d3dpp, sizeof(d3dpp) ); 

	d3dpp.Windowed = TRUE;  
	d3dpp.BackBufferCount = 1 ;
	d3dpp.BackBufferWidth = ddm.Width ;
	d3dpp.BackBufferHeight = ddm.Height ;
	d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP; 
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8; 

	// Create device 
	if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, 
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, 
		&d3dpp, &g_pd3dDevice ) ) ) 
	{ 
		MessageBoxA(NULL, "Create D3D9 device failed!", "Error", 0) ; 
		return E_FAIL; 
	} 

	// Create Semaphore variable
	g_hMutex = CreateMutex(NULL,FALSE,NULL);
	g_hFullSemaphore = CreateSemaphore(NULL, g_PoolSize - 1, g_PoolSize - 1, NULL);
	g_hEmptySemaphore = CreateSemaphore(NULL, 0, g_PoolSize - 1, NULL);

	// Get back buffer
	g_pd3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &g_pBackBuffer) ;

	// Create surface
	for (int i = 0; i < g_PoolSize; ++i)
	{
		g_SurfPool[i] = NULL ;
		if (FAILED(hr = g_pd3dDevice->CreateOffscreenPlainSurface(d3dpp.BackBufferWidth, 
			d3dpp.BackBufferHeight, d3dpp.BackBufferFormat, D3DPOOL_SYSTEMMEM, &g_SurfPool[i], NULL)))
		{
			return hr;
		}
	}

	return S_OK; 
} 

VOID Cleanup() 
{ 
	SAFE_RELEASE(g_pD3D) ; 
	SAFE_RELEASE(g_pd3dDevice) ; 
	SAFE_RELEASE(g_pBackBuffer) ;

	for (int i = 0; i < g_PoolSize; ++i)
	{
		SAFE_RELEASE(g_SurfPool[i]) ;
	}
} 

VOID Produce()
{
	HRESULT hr ;
	hr = g_pd3dDevice->GetFrontBufferData(0, g_SurfPool[in]) ;
	if (FAILED(hr))
	{
		MessageBox(NULL, "Get desktop image failed!", "Error", 0) ;
	}
	else
	{
		in = (in + 1) % g_PoolSize ;
	}
}

DWORD WINAPI Producer(LPVOID lpParam)
{
	while (!g_bEndRender)
	{
		WaitForSingleObject(g_hFullSemaphore, INFINITE);
		WaitForSingleObject(g_hMutex, INFINITE);
		Produce();
		ReleaseMutex(g_hMutex);
		ReleaseSemaphore(g_hEmptySemaphore, 1, NULL);
	}

	return 0 ;
}

VOID Consume()
{
	// Update back buffer
	HRESULT hr;
	hr = g_pd3dDevice->UpdateSurface(g_SurfPool[out], &rect, g_pBackBuffer, NULL) ;
	if (FAILED(hr))
	{
		MessageBox(NULL, "Update back buffer failed!", "Error", 0) ;
	}
	else
	{
		out = (out + 1) % g_PoolSize ;
	}
}

DWORD WINAPI Consumer(LPVOID lpParam)
{
	while (!g_bEndRender)
	{
		WaitForSingleObject(g_hEmptySemaphore, INFINITE);
		WaitForSingleObject(g_hMutex, INFINITE);
		Consume();
		ReleaseMutex(g_hMutex);
		ReleaseSemaphore(g_hFullSemaphore, 1, NULL);
	}

	return 0 ;
}

VOID CreateProducerThread()
{
	for (int i = 0; i < g_ProducerCount; ++i)
	{
		DWORD dwThreadID = 0 ;
		HANDLE hBackgroundThread = NULL ;
		hBackgroundThread = CreateThread(	NULL,
			0,
			Producer,
			NULL,
			CREATE_SUSPENDED,
			&dwThreadID ) ;

		if (hBackgroundThread)
		{
			ResumeThread(hBackgroundThread) ;
		}
	}
}

VOID CreateConsumerThread()
{
	for (int i = 0; i < g_ConsumerCount; ++i)
	{
		DWORD dwThreadID = 0 ;
		HANDLE hBackgroundThread = NULL ;
		hBackgroundThread = CreateThread(	NULL,
			0,
			Consumer,
			NULL,
			CREATE_SUSPENDED,
			&dwThreadID ) ;

		if (hBackgroundThread)
		{
			ResumeThread(hBackgroundThread) ;
		}
	}
}

VOID Render() 
{ 
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0); 

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
		g_bEndRender = TRUE ;
		Cleanup(); 
		PostQuitMessage( 0 ); 
		return 0; 
	} 

	return DefWindowProc( hWnd, msg, wParam, lParam ); 
} 

INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR szCmdLine, int iCmdShow) 
{ 
	WNDCLASSEX winClass ; 

	winClass.lpszClassName = "ScreenSynchronizer"; 
	winClass.cbSize        = sizeof(WNDCLASSEX); 
	winClass.style         = CS_HREDRAW | CS_VREDRAW; 
	winClass.lpfnWndProc   = MsgProc; 
	winClass.hInstance     = hInstance; 
	winClass.hIcon         = NULL ; 
	winClass.hIconSm       = NULL ; 
	winClass.hCursor       = LoadCursor(NULL, IDC_ARROW) ; 
	winClass.hbrBackground = NULL ; 
	winClass.lpszMenuName  = NULL ; 
	winClass.cbClsExtra    = 0; 
	winClass.cbWndExtra    = 0; 

	RegisterClassEx (&winClass) ;   

	screenWidth = GetSystemMetrics(SM_CXSCREEN) ;
	screenHeight = GetSystemMetrics(SM_CYSCREEN) ;

	rect.left = 0 ;
	rect.right = screenWidth / 2 ;
	rect.top = 0 ;
	rect.bottom = screenHeight ;

	HWND hWnd = CreateWindowEx(NULL,   
		winClass.lpszClassName,	// window class name 
		"ScreenSynchronizer",	// window caption 
		//WS_OVERLAPPEDWINDOW,	// window style 
		WS_POPUP,
		screenWidth / 2,		// initial x position 
		0,						// initial y position 
		screenWidth,			// initial window width 
		screenHeight,			// initial window height 
		NULL,					// parent window handle 
		NULL,					// window menu handle 
		hInstance,				// program instance handle 
		NULL) ;					// creation parameters 

	if(hWnd == NULL) 
	{ 
		MessageBox(hWnd, "Create Window failed!", "Error", 0) ; 
		return -1 ; 
	} 

	// Initialize Direct3D 
	if( SUCCEEDED(InitD3D(hWnd))) 
	{  
		// Create producer and consumer thread
		CreateProducerThread() ;
		CreateConsumerThread() ;

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
			else // Render the game if there is no message to process 
			{ 
				Render() ; 
			} 
		} 
	} 

	UnregisterClass(winClass.lpszClassName, hInstance) ; 
	return 0; 
} 

