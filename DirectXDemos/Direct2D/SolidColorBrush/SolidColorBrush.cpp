#include <windows.h>
#include <D2D1.h> // header for Direct2D

#define SAFE_RELEASE(P) if(P){P->Release() ; P = NULL ;}

ID2D1Factory* pD2DFactory = NULL ;				// Direct2D factory
ID2D1HwndRenderTarget* pRenderTarget = NULL;	// Render target
ID2D1SolidColorBrush* pBlackBrush = NULL ;		// A black brush, reflect the line color
ID2D1SolidColorBrush* pGreenBrush = NULL ;

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

		// Create a yellow brush to fill in the rectangle
		hr = pRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF(D2D1::ColorF::Yellow, 1.0f)),  
			&pGreenBrush
			);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "Create green brush failed!", "Error", 0) ;
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

	D2D1_RECT_F rectangle = D2D1::RectF(100.f, 100.f, 500.f, 500.f) ;

	// Draw Rectangle
	pRenderTarget->DrawRectangle(
		&rectangle,
		pBlackBrush
		);

	// Fill rectangle
	pRenderTarget->FillRectangle(
		&rectangle,
		pGreenBrush
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