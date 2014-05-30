#include <windows.h>
#include <D2D1.h> 

#define SAFE_RELEASE(P) if(P){P->Release() ; P = NULL ;}

ID2D1Factory*				g_pD2DFactory = NULL ;		// Direct2D factory
ID2D1HwndRenderTarget*		g_pRenderTarget = NULL;		// Render target
ID2D1SolidColorBrush*		g_pSceneBrush = NULL ;		// A black brush, reflect the line color
ID2D1PathGeometry*			g_pLeftMountainGeometry = NULL ;
ID2D1PathGeometry*			g_pRightMountainGeometry = NULL ;
ID2D1PathGeometry*			g_pSunGeometry = NULL ;
ID2D1PathGeometry*			g_pRiverGeometry = NULL ;

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
			MessageBox(hWnd, "Create D2D factory failed!", "Error", 0) ;
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
			&g_pSceneBrush
			) ;
		if (FAILED(hr))
		{
			MessageBox(hWnd, "Create brush failed!", "Error", 0) ;
			return ;
		}

		// create circle with two arcs
		hr = g_pD2DFactory->CreatePathGeometry(&g_pSunGeometry) ;
		if (SUCCEEDED(hr))
		{
			ID2D1GeometrySink *pSink = NULL;

			hr = g_pSunGeometry->Open(&pSink);
			if (SUCCEEDED(hr))
			{
				pSink->SetFillMode(D2D1_FILL_MODE_WINDING);

				pSink->BeginFigure(
					D2D1::Point2F(100, 300),
					D2D1_FIGURE_BEGIN_FILLED
					);

				// Add the top half circle
				pSink->AddArc(
					D2D1::ArcSegment(
					D2D1::Point2F(400, 300), // end point
					D2D1::SizeF(150, 150),
					0.0f, // rotation angle
					D2D1_SWEEP_DIRECTION_CLOCKWISE,
					D2D1_ARC_SIZE_SMALL
					));            

				// Add the bottom half circle
				pSink->AddArc(
					D2D1::ArcSegment(
					D2D1::Point2F(100, 300), // end point
					D2D1::SizeF(150, 150),
					0.0f, // rotation angle
					D2D1_SWEEP_DIRECTION_CLOCKWISE,
					D2D1_ARC_SIZE_SMALL
					));       

				pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
			}
			hr = pSink->Close();

			SAFE_RELEASE(pSink);
		}
	}
}

VOID DrawRectangle()
{
	CreateD2DResource(g_Hwnd) ;

	g_pRenderTarget->BeginDraw() ;

	g_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

	g_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

	g_pSceneBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black, 1.f));
	g_pRenderTarget->DrawGeometry(g_pSunGeometry, g_pSceneBrush, 1.f);

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
	SAFE_RELEASE(g_pSceneBrush) ;
	SAFE_RELEASE(g_pD2DFactory) ;
	SAFE_RELEASE(g_pLeftMountainGeometry) ;
	SAFE_RELEASE(g_pRightMountainGeometry) ;
	SAFE_RELEASE(g_pRiverGeometry) ;
	SAFE_RELEASE(g_pRiverGeometry) ;
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