#include <windows.h>
#include <D2D1.h>

#define SAFE_RELEASE(P) if(P){P->Release() ; P = NULL ;}
const int GEOMETRY_COUNT = 4;

ID2D1Factory*			g_pD2DFactory	= NULL;	// Direct2D factory
ID2D1HwndRenderTarget*	g_pRenderTarget	= NULL;	// Render target
ID2D1SolidColorBrush*	g_pBlackBrush	= NULL;	// Outline brush
ID2D1SolidColorBrush*   g_pRedBrush     = NULL; // Fill in brush

// 4 circle to build up a geometry group.
D2D1_ELLIPSE g_Ellipse0 = D2D1::Ellipse(D2D1::Point2F(300, 300), 50, 50);
D2D1_ELLIPSE g_Ellipse1 = D2D1::Ellipse(D2D1::Point2F(300, 300), 100, 100);
D2D1_ELLIPSE g_Ellipse2 = D2D1::Ellipse(D2D1::Point2F(300, 300), 150, 150);
D2D1_ELLIPSE g_Ellipse3 = D2D1::Ellipse(D2D1::Point2F(300, 300), 200, 200);

D2D1_ELLIPSE g_Ellipse[GEOMETRY_COUNT] = 
{
	g_Ellipse0, 
	g_Ellipse1,
	g_Ellipse2,
	g_Ellipse3
};

// Array to store the 4 ellipse geometry pointer.
ID2D1EllipseGeometry* g_pEllipseArray[GEOMETRY_COUNT] = { NULL };

ID2D1GeometryGroup* g_pGeometryGroup = NULL;

VOID CreateD2DResource(HWND hWnd)
{
	if (!g_pRenderTarget)
	{
		HRESULT hr ;

		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_pD2DFactory) ;
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

		// Create the outline brush(black)
		hr = g_pRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::Black),
			&g_pBlackBrush
			) ;
		if (FAILED(hr))
		{
			MessageBox(hWnd, "Create outline brush(black) failed!", "Error", 0) ;
			return ;
		}

		// Create the fill in brush(red)
		hr = g_pRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::Red),
			&g_pRedBrush
			);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "Create fillin brush(red) failed!", "Error", 0) ;
			return ;
		}

		// Create the 4 ellipse.
		for (int i = 0; i < GEOMETRY_COUNT; ++i)
		{
			hr = g_pD2DFactory->CreateEllipseGeometry(g_Ellipse[i], &g_pEllipseArray[i]);
			if (FAILED(hr)) 
			{
				MessageBox(hWnd, "Create Ellipse Geometry failed!", "Error", 0);
				return;
			}
		}

		// Create the geometry group, the 4 circles make up a group.
		hr = g_pD2DFactory->CreateGeometryGroup(
			D2D1_FILL_MODE_ALTERNATE,
			(ID2D1Geometry**)&g_pEllipseArray,
			ARRAYSIZE(g_pEllipseArray),
			&g_pGeometryGroup
		);
	}
}

VOID DrawRectangle(HWND hwnd)
{
	CreateD2DResource(hwnd) ;

	g_pRenderTarget->BeginDraw() ;

	// Clear background color to White
	g_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

	// Draw geometry group
	g_pRenderTarget->DrawGeometry(g_pGeometryGroup, g_pBlackBrush, 5.0f);

	// Fill geometry group
	g_pRenderTarget->FillGeometry(g_pGeometryGroup, g_pRedBrush);

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
	SAFE_RELEASE(g_pRedBrush);
	SAFE_RELEASE(g_pGeometryGroup);

	for (int i = 0; i < GEOMETRY_COUNT; ++i)
	{
		SAFE_RELEASE(g_pEllipseArray[i]);
		g_pEllipseArray[i] = NULL;
	}

	SAFE_RELEASE(g_pD2DFactory) ;
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
		"Geometry Group",			// window caption
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

		MSG msg ;  
		ZeroMemory(&msg, sizeof(msg)) ;

		while (GetMessage (&msg, NULL, 0, 0))  
		{
			TranslateMessage (&msg) ;
			DispatchMessage (&msg) ;
		}

		return msg.wParam ;
}
