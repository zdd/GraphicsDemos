#include <windows.h>
#include <D2D1.h>

#define SAFE_RELEASE(P) if(P){P->Release() ; P = NULL ;}

ID2D1Factory*			pD2DFactory = NULL ;	// Direct2D factory
ID2D1HwndRenderTarget*	pRenderTarget = NULL;	// Render target
ID2D1SolidColorBrush*	pBlackBrush = NULL ;	// A black brush, reflect the line color

VOID CreateD2DResource(HWND hWnd)
{
	if (!pRenderTarget)
	{
		HRESULT hr ;

		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory) ;
		if (FAILED(hr))
		{
			MessageBox(hWnd, "Create D2D factory failed!", "Error", 0) ;
			return ;
		}

		// Obtain the size of the drawing area
		RECT rc ;
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

		// Create a brush
		hr = pRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::Black),
			&pBlackBrush
			) ;
		if (FAILED(hr))
		{
			MessageBox(hWnd, "Create brush failed!", "Error", 0) ;
			return ;
		}
	}
}

// Draw a rectangle
void DrawTriangle(D2D1_POINT_2F p0, D2D1_POINT_2F p1, D2D1_POINT_2F p2, 
				  ID2D1HwndRenderTarget* pRenderTarget, 
				  ID2D1SolidColorBrush* pBlackBrush,
				  FLOAT strokeWidth = 1.0f,
				  ID2D1StrokeStyle *strokeStyle = NULL)
{
	pRenderTarget->DrawLine(p0, p1, pBlackBrush);
	pRenderTarget->DrawLine(p1, p2, pBlackBrush) ;
	pRenderTarget->DrawLine(p2, p0, pBlackBrush) ;
}

// Draw a circle
void DrawCircle(D2D1_POINT_2F center, FLOAT radius,
				ID2D1HwndRenderTarget* pRenderTarget, 
				ID2D1SolidColorBrush* pBlackBrush,
				FLOAT strokeWidth = 1.0f,
				ID2D1StrokeStyle *strokeStyle = NULL)
{
	D2D1_ELLIPSE ellipse = D2D1::Ellipse(center, radius, radius) ;
	pRenderTarget->DrawEllipse(ellipse, pBlackBrush) ;
}

VOID DrawRectangle(HWND hwnd)
{
	CreateD2DResource(hwnd) ;

	pRenderTarget->BeginDraw() ;

	// Clear background color to dark cyan
	pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

	// Draw Rectangle
	pRenderTarget->DrawRectangle(
		D2D1::RectF(100.f, 100.f, 500.f, 500.f),
		pBlackBrush
		);

	//// Draw Rounded Rectangle
	//D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(
	//	D2D1::RectF(100.f, 100.f, 500.f, 500.f),
	//	50.0f,
	//	50.0f) ;
	//pRenderTarget->DrawRoundedRectangle(roundedRect, pBlackBrush, 1.0f) ;

	//// Draw Ellipse
	//D2D1_ELLIPSE ellipse = D2D1::Ellipse(D2D1::Point2F(100.0f, 100.0f), 100.0f, 50.0f) ;
	//pRenderTarget->DrawEllipse(ellipse, pBlackBrush) ;

	//// Draw line
	//pRenderTarget->DrawLine(
	//	D2D1::Point2F(100.f, 100.f), 
	//	D2D1::Point2F(500.f, 500.f), 
	//	pBlackBrush);

	//// Draw triangle
	//DrawTriangle(D2D1::Point2F(300.f, 10.f), D2D1::Point2F(100.f, 210.f), D2D1::Point2F(500.f, 210.f),
	//	pRenderTarget,
	//	pBlackBrush
	//	);

	//// Draw circle
	//DrawCircle(D2D1::Point2F(300.f, 300.f), 100.0f,
	//	pRenderTarget,
	//	pBlackBrush) ;

	HRESULT hr = pRenderTarget->EndDraw() ;
	if (FAILED(hr))
	{
		MessageBox(NULL, "Draw failed!", "Error", 0) ;

		return ;
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
		DrawRectangle(hwnd) ;
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
    
	HWND hwnd = CreateWindowEx(NULL,  
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

        ShowWindow (hwnd, iCmdShow) ;
		UpdateWindow (hwnd) ;

		MSG    msg ;  
		ZeroMemory(&msg, sizeof(msg)) ;

		while (GetMessage (&msg, NULL, 0, 0))  
		{
			TranslateMessage (&msg) ;
			DispatchMessage (&msg) ;
		}

		return msg.wParam ;
}