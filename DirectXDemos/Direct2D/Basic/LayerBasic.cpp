#include <windows.h>
#include <D2D1.h> 

#define SAFE_RELEASE(P) if(P){P->Release() ; P = NULL ;}

ID2D1Factory*				g_pD2DFactory = NULL ;			// Direct2D factory
ID2D1HwndRenderTarget*		g_pRenderTarget = NULL;			// Render target
ID2D1SolidColorBrush*		g_pBlackBrush = NULL ;			// A black brush, reflect the line color
ID2D1RadialGradientBrush*	g_pRadialGradientBrush = NULL ; // Radial gradient brush
ID2D1Layer*					g_pLayer = NULL ;

HWND g_Hwnd ;	// Window handle

VOID CreateD2DResource(HWND hWnd)
{
	// This function was called in the DrawRectangle function which in turn called to response the
	// WM_PAINT Message, to avoid creating resource every time, we test the pointer to g_pRenderTarget
	// If the resource already create, skip the function
	if (!g_pRenderTarget)
	{
		HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_pD2DFactory) ;
		if (FAILED(hr))
		{
			MessageBox(hWnd, "Create D2D factory failed!", "Errro", 0) ;
			return ;
		}

		// Obtain the size of the drawing area
		RECT rc ;		
		GetClientRect(hWnd, &rc) ;

		// Create a Direct2D render target
		hr = g_pD2DFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(
			hWnd, 
			D2D1::SizeU(rc.right - rc.left,rc.bottom - rc.top)
			), 
			&g_pRenderTarget
			) ;
		if (FAILED(hr))
		{
			MessageBox(hWnd, "Create render target failed!", "Error", 0) ;
			return ;
		}

		// Create a brush
		hr = g_pRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::Black),
			&g_pBlackBrush
			) ;
		if (FAILED(hr))
		{
			MessageBox(hWnd, "Create brush failed!", "Error", 0) ;
			return ;
		}

		// Define gradient stops
		D2D1_GRADIENT_STOP gradientStops[2] ;
		gradientStops[0].color = D2D1::ColorF(D2D1::ColorF::Yellow, 1) ;
		gradientStops[0].position = 0.f ;
		gradientStops[1].color = D2D1::ColorF(D2D1::ColorF::ForestGreen, 1) ;
		gradientStops[1].position = 1.f ;

		// Create gradient stops collection
		ID2D1GradientStopCollection* pGradientStops = NULL ;
		hr = g_pRenderTarget->CreateGradientStopCollection(
			gradientStops,
			2, 
			D2D1_GAMMA_2_2,
			D2D1_EXTEND_MODE_CLAMP,
			&pGradientStops
			) ;
		if (FAILED(hr))
		{
			MessageBox(NULL, "Create gradient stops collection failed!", "Error", 0);
		}

		RECT rect ;
		GetClientRect(hWnd, &rect) ;
		D2D1_POINT_2F center = D2D1::Point2F((rect.right -rect.left) / 2, (rect.bottom - rect.top) / 2) ;


		// Create a linear gradient brush to fill in the rectangle
		hr = g_pRenderTarget->CreateRadialGradientBrush(
			D2D1::RadialGradientBrushProperties(
			center,
			D2D1::Point2F(0, 0),
			50,
			50),
			pGradientStops,
			&g_pRadialGradientBrush
			) ;

		if (FAILED(hr))
		{
			MessageBox(hWnd, "Create linear gradient brush failed!", "Error", 0) ;
			return ;
		}

		// Create layer
		hr = g_pRenderTarget->CreateLayer(NULL, &g_pLayer) ;
		if (FAILED(hr))
		{
			MessageBox(hWnd, "Create layer failed!", "Error", 0) ;
			return ;
		}
	}
}

VOID DrawRectangle()
{
	CreateD2DResource(g_Hwnd) ;

	g_pRenderTarget->BeginDraw() ;

	// Push layer, after BeginDraw
	g_pRenderTarget->PushLayer(
		D2D1::LayerParameters(
		D2D1::InfiniteRect(),
		NULL,
		D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
		D2D1::IdentityMatrix(),
		1.0,
		g_pRadialGradientBrush,
		D2D1_LAYER_OPTIONS_NONE),
		g_pLayer
		);

	// Clear background color to dark cyan
	g_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

	// Draw Rectangle
	g_pRenderTarget->DrawRectangle(
		D2D1::RectF(100.f, 100.f, 500.f, 500.f),
		g_pBlackBrush
		);
	
	// Pop layer before EndDraw
	g_pRenderTarget->PopLayer() ;

	HRESULT hr = g_pRenderTarget->EndDraw() ;
	if (FAILED(hr))
	{
		MessageBox(NULL, "Draw failed!", "Error", 0) ;
		return ;
	}
}

VOID Cleanup()
{
	SAFE_RELEASE(g_pRenderTarget) ;
	SAFE_RELEASE(g_pBlackBrush) ;
	SAFE_RELEASE(g_pRadialGradientBrush) ;
	SAFE_RELEASE(g_pLayer) ;
	SAFE_RELEASE(g_pD2DFactory) ;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)   
{
	switch (message)    
	{
	case   WM_PAINT:
		DrawRectangle() ;
		ValidateRect(g_Hwnd, NULL) ;
		return 0 ;

	case WM_KEYDOWN: 
		{ 
			switch( wParam ) 
			{ 
			case VK_ESCAPE: 
				SendMessage( hwnd, WM_CLOSE, 0, 0 ); 
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

	return DefWindowProc (hwnd, message, wParam, lParam) ;
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow )
{

	WNDCLASSEX winClass ;

	winClass.lpszClassName = "Direct2D";
	winClass.cbSize        = sizeof(WNDCLASSEX);
	winClass.style         = CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc   = WndProc;
	winClass.hInstance     = hInstance;
	winClass.hIcon	       = NULL ;
	winClass.hIconSm	   = NULL ;
	winClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
	winClass.hbrBackground = NULL ;
	winClass.lpszMenuName  = NULL;
	winClass.cbClsExtra    = 0;
	winClass.cbWndExtra    = 0;

	if (!RegisterClassEx (&winClass))   
	{
		MessageBox ( NULL, TEXT( "This program requires Windows NT!" ), "error", MB_ICONERROR) ;
		return 0 ;  
	}   

	g_Hwnd = CreateWindowEx(NULL,  
		"Direct2D",					// window class name
		"Draw Rectangle",			// window caption
		WS_OVERLAPPEDWINDOW, 		// window style
		CW_USEDEFAULT,				// initial x position
		CW_USEDEFAULT,				// initial y position
		600,						// initial x size
		600,						// initial y size
		NULL,						// parent window handle
		NULL,						// window menu handle
		hInstance,					// program instance handle
		NULL) ;						// creation parameters

	ShowWindow (g_Hwnd, iCmdShow) ;
	UpdateWindow (g_Hwnd) ;

	MSG    msg ;  
	ZeroMemory(&msg, sizeof(msg)) ;

	while (GetMessage (&msg, NULL, 0, 0))  
	{
		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}

	return msg.wParam ;
}