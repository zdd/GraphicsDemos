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

		// Create left mountain geometry
		hr = g_pD2DFactory->CreatePathGeometry(&g_pLeftMountainGeometry) ;
		if (SUCCEEDED(hr))
		{
			ID2D1GeometrySink *pSink = NULL;

			hr = g_pLeftMountainGeometry->Open(&pSink);
			if (SUCCEEDED(hr))
			{
				pSink->SetFillMode(D2D1_FILL_MODE_WINDING);

				pSink->BeginFigure(
					D2D1::Point2F(346,255),
					D2D1_FIGURE_BEGIN_FILLED
					);
				D2D1_POINT_2F points[5] = {
					D2D1::Point2F(267, 177),
					D2D1::Point2F(236, 192),
					D2D1::Point2F(212, 160),
					D2D1::Point2F(156, 255),
					D2D1::Point2F(346, 255), 
				};
				pSink->AddLines(points, ARRAYSIZE(points));
				pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
			}
			pSink->Close() ;

			SAFE_RELEASE(pSink) ;
		}
		
		// Create right mountain geometry
		hr = g_pD2DFactory->CreatePathGeometry(&g_pRightMountainGeometry) ;
		if (SUCCEEDED(hr))
		{
			ID2D1GeometrySink *pSink = NULL;

			hr = g_pRightMountainGeometry->Open(&pSink);
			if (SUCCEEDED(hr))
			{
				pSink->SetFillMode(D2D1_FILL_MODE_WINDING);

				pSink->BeginFigure(
					D2D1::Point2F(575,263),
					D2D1_FIGURE_BEGIN_FILLED
					);
				D2D1_POINT_2F points[] = {
					D2D1::Point2F(481, 146),
					D2D1::Point2F(449, 181),
					D2D1::Point2F(433, 159),
					D2D1::Point2F(401, 214),
					D2D1::Point2F(381, 199), 
					D2D1::Point2F(323, 263), 
					D2D1::Point2F(575, 263)
				};
				pSink->AddLines(points, ARRAYSIZE(points));
				pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
			}
			hr = pSink->Close();

			SAFE_RELEASE(pSink);

		}
		// Create sun geometry
		hr = g_pD2DFactory->CreatePathGeometry(&g_pSunGeometry) ;
		if (SUCCEEDED(hr))
		{
			ID2D1GeometrySink *pSink = NULL;

			hr = g_pSunGeometry->Open(&pSink);
			if (SUCCEEDED(hr))
			{
				pSink->SetFillMode(D2D1_FILL_MODE_WINDING);

				pSink->BeginFigure(
					D2D1::Point2F(270, 255),
					D2D1_FIGURE_BEGIN_FILLED
					);
				pSink->AddArc(
					D2D1::ArcSegment(
					D2D1::Point2F(440, 255), // end point
					D2D1::SizeF(85, 85),
					0.0f, // rotation angle
					D2D1_SWEEP_DIRECTION_CLOCKWISE,
					D2D1_ARC_SIZE_SMALL
					));            
				pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

				pSink->BeginFigure(
					D2D1::Point2F(299, 182),
					D2D1_FIGURE_BEGIN_HOLLOW
					);
				pSink->AddBezier(
					D2D1::BezierSegment(
					D2D1::Point2F(299, 182),
					D2D1::Point2F(294, 176),
					D2D1::Point2F(285, 178)
					));
				pSink->AddBezier(
					D2D1::BezierSegment(
					D2D1::Point2F(276, 179),
					D2D1::Point2F(272, 173),
					D2D1::Point2F(272, 173)
					));
				pSink->EndFigure(D2D1_FIGURE_END_OPEN);

				pSink->BeginFigure(
					D2D1::Point2F(354, 156),
					D2D1_FIGURE_BEGIN_HOLLOW
					);
				pSink->AddBezier(
					D2D1::BezierSegment(
					D2D1::Point2F(354, 156),
					D2D1::Point2F(358, 149),
					D2D1::Point2F(354, 142)
					));
				pSink->AddBezier(
					D2D1::BezierSegment(
					D2D1::Point2F(349, 134),
					D2D1::Point2F(354, 127),
					D2D1::Point2F(354, 127)
					));
				pSink->EndFigure(D2D1_FIGURE_END_OPEN);

				pSink->BeginFigure(
					D2D1::Point2F(322,164),
					D2D1_FIGURE_BEGIN_HOLLOW
					);
				pSink->AddBezier(
					D2D1::BezierSegment(
					D2D1::Point2F(322, 164),
					D2D1::Point2F(322, 156),
					D2D1::Point2F(314, 152)
					));
				pSink->AddBezier(
					D2D1::BezierSegment(
					D2D1::Point2F(306, 149),
					D2D1::Point2F(305, 141),
					D2D1::Point2F(305, 141)
					));              
				pSink->EndFigure(D2D1_FIGURE_END_OPEN);

				pSink->BeginFigure(
					D2D1::Point2F(385, 164),
					D2D1_FIGURE_BEGIN_HOLLOW
					);
				pSink->AddBezier(
					D2D1::BezierSegment(
					D2D1::Point2F(385,164),
					D2D1::Point2F(392,161),
					D2D1::Point2F(394,152)
					));
				pSink->AddBezier(
					D2D1::BezierSegment(
					D2D1::Point2F(395,144),
					D2D1::Point2F(402,141),
					D2D1::Point2F(402,142)
					));                
				pSink->EndFigure(D2D1_FIGURE_END_OPEN);

				pSink->BeginFigure(
					D2D1::Point2F(408,182),
					D2D1_FIGURE_BEGIN_HOLLOW
					);
				pSink->AddBezier(
					D2D1::BezierSegment(
					D2D1::Point2F(408,182),
					D2D1::Point2F(416,184),
					D2D1::Point2F(422,178)
					));
				pSink->AddBezier(
					D2D1::BezierSegment(
					D2D1::Point2F(428,171),
					D2D1::Point2F(435,173),
					D2D1::Point2F(435,173)
					));
				pSink->EndFigure(D2D1_FIGURE_END_OPEN);
			}
			hr = pSink->Close();

			SAFE_RELEASE(pSink);
		}

		// Create river geometry
		hr = g_pD2DFactory->CreatePathGeometry(&g_pRiverGeometry) ;
		if (SUCCEEDED(hr))
		{
			ID2D1GeometrySink *pSink = NULL;

			hr = g_pRiverGeometry->Open(&pSink);
			if (SUCCEEDED(hr))
			{
				pSink->SetFillMode(D2D1_FILL_MODE_WINDING);
				pSink->BeginFigure(
					D2D1::Point2F(183, 392),
					D2D1_FIGURE_BEGIN_FILLED
					);
				pSink->AddBezier(
					D2D1::BezierSegment(
					D2D1::Point2F(238, 284),
					D2D1::Point2F(472, 345),
					D2D1::Point2F(356, 303)
					));
				pSink->AddBezier(
					D2D1::BezierSegment(
					D2D1::Point2F(237, 261),
					D2D1::Point2F(333, 256),
					D2D1::Point2F(333, 256)
					));
				pSink->AddBezier(
					D2D1::BezierSegment(
					D2D1::Point2F(335, 257),
					D2D1::Point2F(241, 261),
					D2D1::Point2F(411, 306)
					));
				pSink->AddBezier(
					D2D1::BezierSegment(
					D2D1::Point2F(574, 350),
					D2D1::Point2F(288, 324),
					D2D1::Point2F(296, 392)
					));
				pSink->EndFigure(D2D1_FIGURE_END_OPEN);
			}
			pSink->Close() ;
			SAFE_RELEASE(pSink) ;
		}
	}
}

VOID DrawRectangle()
{
	CreateD2DResource(g_Hwnd) ;

	g_pRenderTarget->BeginDraw() ;

	g_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

	g_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

	g_pSceneBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Red, 1.f)) ;
	g_pRenderTarget->FillGeometry(g_pSunGeometry, g_pSceneBrush);

	g_pSceneBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black, 1.f));
	g_pRenderTarget->DrawGeometry(g_pSunGeometry, g_pSceneBrush, 1.f);

	g_pSceneBrush->SetColor(D2D1::ColorF(D2D1::ColorF::OliveDrab, 1.f));
	g_pRenderTarget->FillGeometry(g_pLeftMountainGeometry, g_pSceneBrush);

	g_pSceneBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black, 1.f));
	g_pRenderTarget->DrawGeometry(g_pLeftMountainGeometry, g_pSceneBrush, 1.f);

	g_pSceneBrush->SetColor(D2D1::ColorF(D2D1::ColorF::LightSkyBlue, 1.f));
	g_pRenderTarget->FillGeometry(g_pRiverGeometry, g_pSceneBrush);

	g_pSceneBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black, 1.f));
	g_pRenderTarget->DrawGeometry(g_pRiverGeometry, g_pSceneBrush, 1.f);

	g_pSceneBrush->SetColor(D2D1::ColorF(D2D1::ColorF::YellowGreen, 1.f));
	g_pRenderTarget->FillGeometry(g_pRightMountainGeometry, g_pSceneBrush);

	g_pSceneBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black, 1.f));
	g_pRenderTarget->DrawGeometry(g_pRightMountainGeometry, g_pSceneBrush, 1.f);


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