#include <windows.h>
#include <D2D1.h> // header for Direct2D

#define SAFE_RELEASE(P) if(P){P->Release() ; P = NULL ;}

ID2D1Factory* pD2DFactory = NULL ;				// Direct2D factory
ID2D1HwndRenderTarget* pRenderTarget = NULL;	// Render target
ID2D1SolidColorBrush* pBlackBrush = NULL ;		// Black brush for rectangle outline
ID2D1RadialGradientBrush* pRadialGradientBrush = NULL ; // Radial gradient brush

// Ellipse to draw
D2D1_ELLIPSE g_Ellipse = D2D1::Ellipse(D2D1::Point2F(300, 300), 200, 200);

RECT rc ;		// Render area
HWND g_Hwnd ;	// Window handle

VOID CreateD2DResource(HWND hWnd)
{
	// This function was called in the DrawRectangle function which in turn called to response the
	// WM_PAINT Message, to avoid creating resource every time, we test the pointer to pRenderTarget
	// If the resource already create, skip the function
	if (!pRenderTarget)
	{
		HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory) ;
		if (FAILED(hr))
		{
			MessageBox(hWnd, "Create D2D factory failed!", "Error", 0) ;
			return ;
		}

		// Obtain the size of the drawing area
		GetClientRect(hWnd, &rc) ;

		// Create a Direct2D render target
		hr = pD2DFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(
			hWnd, 
			D2D1::SizeU(rc.right - rc.left,rc.bottom - rc.top)
			), 
			&pRenderTarget
			) ;
		if (FAILED(hr))
		{
			MessageBox(hWnd, "Create render target failed!", "Error", 0) ;
			return ;
		}

		// Create a black brush to draw outline of the rectangle
		hr = pRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::Black),
			&pBlackBrush
			) ;
		if (FAILED(hr))
		{
			MessageBox(hWnd, "Create black brush failed!", "Error", 0) ;
			return ;
		}

		// Define gradient stops
		D2D1_GRADIENT_STOP gradientStops[2] ;
		gradientStops[0].color = D2D1::ColorF(D2D1::ColorF::Yellow) ;
		gradientStops[0].position = 0.f ;
		gradientStops[1].color = D2D1::ColorF(D2D1::ColorF::Blue) ;
		gradientStops[1].position = 1.f ;

		// Create gradient stops collection
		ID2D1GradientStopCollection* pGradientStops = NULL ;
		hr = pRenderTarget->CreateGradientStopCollection(
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

		// Create a linear gradient brush to fill in the ellipse
		hr = pRenderTarget->CreateRadialGradientBrush(
			D2D1::RadialGradientBrushProperties(
			g_Ellipse.point,
			D2D1::Point2F(0, 0),
			g_Ellipse.radiusX,
			g_Ellipse.radiusY),
			pGradientStops,
			&pRadialGradientBrush
			) ;

		if (FAILED(hr))
		{
			MessageBox(hWnd, "Create linear gradient brush failed!", "Error", 0) ;
			return ;
		}
	}
}

VOID DrawRectangle()
{
	CreateD2DResource(g_Hwnd) ;

	pRenderTarget->BeginDraw() ;

	// Clear background color to dark cyan
	pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

	// Draw ellipse
	pRenderTarget->DrawEllipse(
		&g_Ellipse,
		pBlackBrush,
		2.f // stroke width
		) ;

	// Fill ellipse
	pRenderTarget->FillEllipse(
		&g_Ellipse,
		pRadialGradientBrush
	) ;

	HRESULT hr = pRenderTarget->EndDraw() ;
	if (FAILED(hr))
	{
		MessageBox(NULL, "Draw failed!", "Error", 0) ;
	}
}

VOID Cleanup()
{
	SAFE_RELEASE(pRenderTarget) ;
	SAFE_RELEASE(pBlackBrush) ;
	SAFE_RELEASE(pD2DFactory) ;
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
		CW_USEDEFAULT,				// initial x size
		CW_USEDEFAULT,				// initial y size
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