#include <windows.h>
#include <D2D1.h> // header for Direct2D

#define SAFE_RELEASE(P) if(P){P->Release() ; P = NULL ;}

ID2D1Factory*				g_pD2DFactory = NULL ;		// Direct2D factory
ID2D1HwndRenderTarget*		g_pRenderTarget = NULL;		// Render target
ID2D1SolidColorBrush*		g_pBlackBrush = NULL ;		// A black brush, reflect the line color
ID2D1SolidColorBrush*		g_pRedBrush = NULL ;		// A Red brush for second hand
ID2D1SolidColorBrush*		g_pYellowBrush = NULL ;		// A yellow brush for clock outline
ID2D1RadialGradientBrush*	g_pRadialGradientBrush  = NULL ;	// For clock pan

HWND g_Hwnd ;	// Window handle
RECT g_ClientRect ;	// Client area of window

float top = -1.f ;		// Clock top
float bottom = -1.f ;	// Clock bottom
float left = -1.f ;		// Clock left
float right = -1.f ;	// Clock right
float centerX = -1.f ;	// Clock center x
float centerY = -1.f ;	// Clock center y

// Second hand
D2D1_POINT_2F g_SecondHandP0 = D2D1::Point2F(0, 0) ;
D2D1_POINT_2F g_SecondHandP1 = D2D1::Point2F(0, 0) ;

// Minute hand
D2D1_POINT_2F g_MinuteHandP0 = D2D1::Point2F(0, 0) ;
D2D1_POINT_2F g_MinuteHandP1 = D2D1::Point2F(0, 0) ;

// Hour hand
D2D1_POINT_2F g_HourHandP0 = D2D1::Point2F(0, 0) ;
D2D1_POINT_2F g_HourHandP1 = D2D1::Point2F(0, 0) ;

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

		// Create a black brush
		hr = g_pRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::Black),
			&g_pBlackBrush
			) ;
		if (FAILED(hr))
		{
			MessageBox(hWnd, "Create black brush failed!", "Error", 0) ;
			return ;
		}

		// Create a red brush
		hr = g_pRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::Red),
			&g_pRedBrush
			) ;
		if (FAILED(hr))
		{
			MessageBox(hWnd, "Create red brush failed!", "Error", 0) ;
			return ;
		}

		// Create a yellow brush
		hr = g_pRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::Yellow),
			&g_pYellowBrush
			) ;
		if (FAILED(hr))
		{
			MessageBox(hWnd, "Create yellow brush failed!", "Error", 0) ;
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

		// Create a linear gradient brush to fill in the rectangle
		hr = g_pRenderTarget->CreateRadialGradientBrush(
			D2D1::RadialGradientBrushProperties(
			D2D1::Point2F(g_ClientRect.right / 2.f, g_ClientRect.bottom / 2.f),
			D2D1::Point2F(0, 0),
			g_ClientRect.right / 2.f,
			g_ClientRect.bottom / 2.f),
			pGradientStops,
			&g_pRadialGradientBrush
			) ;

		if (FAILED(hr))
		{
			MessageBox(hWnd, "Create linear gradient brush failed!", "Error", 0) ;
			return ;
		}
	}
}

void InitializeClockHands()
{
	// Initialize the second hand
	g_SecondHandP0 = D2D1::Point2F(centerX, top + 10.f) ;
	g_SecondHandP1 = D2D1::Point2F(centerX, bottom / 2.f + 100.f) ;

	// Initialize the minute hand
	g_MinuteHandP0 = D2D1::Point2F(centerX, top + 80.f) ;
	g_MinuteHandP1 = D2D1::Point2F(centerX, bottom / 2.f + 50.f) ;

	// Initialize the hour hand
	g_HourHandP0 = D2D1::Point2F(centerX, top + 150.f) ;
	g_HourHandP1 = D2D1::Point2F(centerX, bottom / 2.f + 30.f) ;
}

void SetTime()
{
	// Move second hand, minute hand and hour hand to it's original position, at 12 clocks
	InitializeClockHands() ;

	// Get current time
	SYSTEMTIME st ;
	GetLocalTime(&st) ;

	// Get the seconds, minutes and hours part from SYSTEMTIME structure
	WORD seconds = st.wSecond ;
	WORD minutes = st.wMinute ;
	WORD hours = st.wHour < 12 ? st.wHour : st.wHour - 12 ;

	// Calculate the clock hands angle based on the time
	float secondHandAngle = seconds * 6.f ;
	float minuteHandAngle = (minutes + seconds / 60.f) * 6.f ;
	float hourHandAngle = (hours + minutes / 60.f + seconds / 3600.f) * 30.f ;

	// Build up rotation matrix for second hand
	D2D1::Matrix3x2F rotateMatrix = D2D1::Matrix3x2F::Rotation(
		secondHandAngle,
		D2D1::Point2F(centerX, centerY)
		) ;

	// Rotate second hand
	g_SecondHandP0 = rotateMatrix.TransformPoint(g_SecondHandP0) ;
	g_SecondHandP1 = rotateMatrix.TransformPoint(g_SecondHandP1) ;

	// Build up rotation matrix for minute hand
	rotateMatrix = D2D1::Matrix3x2F::Rotation(
		minuteHandAngle,
		D2D1::Point2F(centerX, centerY)
		) ;

	// Rotate minute hand
	g_MinuteHandP0 = rotateMatrix.TransformPoint(g_MinuteHandP0) ;
	g_MinuteHandP1 = rotateMatrix.TransformPoint(g_MinuteHandP1) ;

	// Build up rotation matrix for hour hand
	rotateMatrix = D2D1::Matrix3x2F::Rotation(
		hourHandAngle,
		D2D1::Point2F(centerX, centerY)
		) ;

	// Rotate hour hand
	g_HourHandP0 = rotateMatrix.TransformPoint(g_HourHandP0) ;
	g_HourHandP1 = rotateMatrix.TransformPoint(g_HourHandP1) ;
}

void Initialize(HWND hWnd)
{
	// Get the client area of main window
	GetClientRect(g_Hwnd, &g_ClientRect) ;

	// Initialize clock outline
	top = g_ClientRect.top + 10.f ;
	bottom = g_ClientRect.bottom - 10.f ;
	left = g_ClientRect.left + 10.f ;
	right = g_ClientRect.right - 10.f ;

	// Initialize clock center
	centerX = (right - left) / 2 + left ;
	centerY = (bottom - top) / 2 + top ;

	//InitializeClockHands() ;

	SetTime() ;
}

void DrawClockOutline()
{
	// Draw outline of the clock, a circle
	D2D1_POINT_2F center = D2D1::Point2F(centerX, centerY) ;
	float radius = right > bottom ? (bottom / 2 - 10) : (right / 2 - 10) ;

	D2D1_ELLIPSE ellipse = D2D1::Ellipse(center, radius, radius) ;
	g_pRenderTarget->DrawEllipse(ellipse, g_pBlackBrush, 20.f) ;

	// Fill the circle
	g_pRenderTarget->FillEllipse(
		&ellipse,
		g_pRadialGradientBrush
		) ;

	// Draw another outline
	D2D1_ELLIPSE innerEllipse = D2D1::Ellipse(center, radius - 2, radius - 2) ;
	g_pRenderTarget->DrawEllipse(
		&innerEllipse,
		g_pYellowBrush,
		5.f
		) ;
}

void DrawClockScales()
{
	// Top scale
	D2D1_POINT_2F p0 = D2D1::Point2F(centerX, top + 10.f) ;
	D2D1_POINT_2F p1 = D2D1::Point2F(centerX, top + 50.f) ;

	D2D1::Matrix3x2F rotateMatrix = D2D1::Matrix3x2F::Rotation(
		30.f,
		D2D1::Point2F(centerX, centerY)
		) ;

	// Draw all scales
	int count = 0 ;
	do 
	{
		g_pRenderTarget->DrawLine(p0, p1, g_pBlackBrush, 10.f) ;

		p0 = rotateMatrix.TransformPoint(p0) ;
		p1 = rotateMatrix.TransformPoint(p1) ;

		++count ;

	} while (count < 12);
}

void DrawClockDots()
{
	// Draw dot between scales
	D2D1_POINT_2F dotCenter = D2D1::Point2F(centerX, top + 20.f) ;
	float dotRadius = 3.f ;

	D2D1_ELLIPSE dot = D2D1::Ellipse(
		dotCenter,
		dotRadius,
		dotRadius
		) ;

	D2D1::Matrix3x2F dotRatationMatrix = D2D1::Matrix3x2F::Rotation(
		6.f,
		D2D1::Point2F(centerX, centerY)
		) ;

	int dotCount = 0 ;
	do 
	{
		g_pRenderTarget->DrawEllipse(
			&dot,
			g_pBlackBrush
			) ;
		g_pRenderTarget->FillEllipse(
			&dot,
			g_pBlackBrush
			) ;

		dot.point = dotRatationMatrix.TransformPoint(dot.point) ;

		++dotCount ;

	} while (dotCount < 60);
}

void DrawClockHands()
{
	// Draw second hand
	g_pRenderTarget->DrawLine(
		g_SecondHandP0,
		g_SecondHandP1,
		g_pRedBrush,
		5.f
		) ;

	// Draw minute hand
	g_pRenderTarget->DrawLine(
		g_MinuteHandP0,
		g_MinuteHandP1,
		g_pBlackBrush,
		10.f
		) ;

	// Draw hour hand
	g_pRenderTarget->DrawLine(
		g_HourHandP0,
		g_HourHandP1,
		g_pBlackBrush,
		15.f
		) ;
}

void DrawClock()
{
	DrawClockOutline() ;
	
	DrawClockScales() ;
	
	DrawClockDots() ;
	
	DrawClockHands() ;
	
	InvalidateRect(g_Hwnd, NULL, FALSE) ;
}

void UpdateClockHand()
{
	// Get rotation center
	D2D1_POINT_2F center = D2D1::Point2F(
		centerX,
		centerY
		) ;

	// Build up rotation matrix
	D2D1::Matrix3x2F rotationMatrix = D2D1::Matrix3x2F::Rotation(
		6.f,
		center
		) ;

	// Rotate the two points of the second hand
	g_SecondHandP0 = rotationMatrix.TransformPoint(g_SecondHandP0) ;
	g_SecondHandP1 = rotationMatrix.TransformPoint(g_SecondHandP1) ;

	rotationMatrix = D2D1::Matrix3x2F::Rotation(
		0.1f,
		center
		) ;

	// Rotate the two points of the minute hand
	g_MinuteHandP0 = rotationMatrix.TransformPoint(g_MinuteHandP0) ;
	g_MinuteHandP1 = rotationMatrix.TransformPoint(g_MinuteHandP1) ;
	
	// Rotate the two points of the hour hand
	rotationMatrix = D2D1::Matrix3x2F::Rotation(
		3 / 3600.f,
		center
		) ;
}

VOID Draw()
{
	CreateD2DResource(g_Hwnd) ;

	g_pRenderTarget->BeginDraw() ;

	// Clear background color to dark cyan
	g_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

	DrawClock() ;

	HRESULT hr = g_pRenderTarget->EndDraw() ;
	if (FAILED(hr))
	{
		MessageBox(NULL, "Draw failed!", "Error", 0) ;
	}

	InvalidateRect(g_Hwnd, NULL, FALSE) ;
}

VOID Cleanup()
{
	SAFE_RELEASE(g_pRenderTarget) ;
	SAFE_RELEASE(g_pBlackBrush) ;
	SAFE_RELEASE(g_pRedBrush) ;
	SAFE_RELEASE(g_pYellowBrush) ;
	SAFE_RELEASE(g_pRadialGradientBrush) ;
	SAFE_RELEASE(g_pD2DFactory) ;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)   
{
	switch (message)    
	{
	case   WM_PAINT:
		Draw() ;
		ValidateRect(g_Hwnd, NULL) ;
		return 0 ;

	case WM_TIMER:
		if (wParam == 1) // rotate clock hands
		{
			UpdateClockHand() ;
			InvalidateRect(g_Hwnd, NULL, FALSE) ;
			UpdateWindow(g_Hwnd) ;
			break ;
		}
		if (wParam == 2) // synchronization
		{
			SetTime() ;
			break ;
		}
		
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
		KillTimer(g_Hwnd, 1) ;
		KillTimer(g_Hwnd, 2) ;
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
		"Clock",			// window caption
		WS_OVERLAPPEDWINDOW, 		// window style
		CW_USEDEFAULT,				// initial x position
		CW_USEDEFAULT,				// initial y position
		600,						// initial x size
		624,						// initial y size
		NULL,						// parent window handle
		NULL,						// window menu handle
		hInstance,					// program instance handle
		NULL) ;						// creation parameters

	Initialize(g_Hwnd) ;

	// Set timer
	SetTimer(g_Hwnd, 1, 1000, (TIMERPROC)NULL) ;

	// This timer is for time synchronization
	SetTimer(g_Hwnd, 2, 60000, (TIMERPROC)NULL) ;

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