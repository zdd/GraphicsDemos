/*
Description:  This demo show how to use DirectWrite to render font
Date:	2012-09-14
*/

#include <d2d1.h>
#include <dwrite.h>

#define SAFE_RELEASE(P) if(P){P->Release() ; P = NULL ;}

// Globals 
ID2D1Factory*			g_pD2DFactory		= NULL;
ID2D1HwndRenderTarget*	g_pRenderTarget		= NULL;
ID2D1SolidColorBrush*	g_pBlackBrush		= NULL;
IDWriteFactory*			g_pDWriteFactory	= NULL;
IDWriteTextFormat*		g_pTextFormat		= NULL;

/*
Create device independent resources, all DriectWrite resources are device independent
*/
VOID CreateDeviceIndependentResources()
{
	// Create Direct2D Factory
	HRESULT hr = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		&g_pD2DFactory
		);
	if(FAILED(hr))
	{
		MessageBox(NULL, L"Create Direct2D factory failed!", L"Error", 0);
		return;
	}

	// Create DirectWrite Factory
	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED, 
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&g_pDWriteFactory)
		);
	if(FAILED(hr))
	{
		MessageBox(NULL, L"Create DirectWrite factory failed!", L"Error", 0);
		return;
	}

	// Create IDWriteTextFormat
	hr = g_pDWriteFactory->CreateTextFormat(
		L"Gabriola",				// Font family name
		NULL,						// Font collection(NULL sets it to the system font collection)
		DWRITE_FONT_WEIGHT_REGULAR,	// Weight
		DWRITE_FONT_STYLE_NORMAL,	// Style
		DWRITE_FONT_STRETCH_NORMAL,	// Stretch
		50.0f,						// Size	
		L"en-us",					// Local
		&g_pTextFormat				// Pointer to recieve the created object
		);
	if(FAILED(hr))
	{
		MessageBox(NULL, L"Create IDWriteTextFormat failed!", L"Error", 0);
		return;
	}
}

/*
Create device dependent resources, render target and brush are all device dependent resources
*/
VOID CreateDeviceResources(HWND Hwnd)
{
	// Get the client area size
	RECT rc;
	GetClientRect(Hwnd, &rc);

	D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

	// Only creaste once
	if(!g_pRenderTarget)
	{
		// Create render target
		HRESULT hr = g_pD2DFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(Hwnd, size),
			&g_pRenderTarget
		);
		if(FAILED(hr))
		{
			MessageBox(Hwnd, L"Create render target failed!", L"Error", 0);
			return;
		}

		// Create a black brush
		hr = g_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &g_pBlackBrush);
		if(FAILED(hr))
		{
			MessageBox(Hwnd, L"Create solid brush failed!", L"Error", 0);
			return;
		}
	}
}

// Cleanup the device dependent resources
VOID DiscardDeviceResources()
{
	SAFE_RELEASE(g_pRenderTarget);
	SAFE_RELEASE(g_pBlackBrush);
}

VOID DrawText(HWND hwnd)
{
	const wchar_t* wszText = L"Hello, Direct2D!\n";		// String to render
	UINT32 cTextLength = (UINT32)wcslen(wszText);	// Get text length

	g_pTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);
	// Center the text
	HRESULT hr = g_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	if(FAILED(hr))
	{
		MessageBox(NULL, L"Center text failed!", L"Error", 0);
		return;
	}

	// Center the paragraph
	hr = g_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	if(FAILED(hr))
	{
		MessageBox(NULL, L"Center paragraph failed!", L"Error", 0);
		return;
	}

	// Set line spacing
	hr = g_pTextFormat->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_DEFAULT, 50, 80);
	if(FAILED(hr))
	{
		MessageBox(NULL, L"Set line spacing failed!", L"Error", 0);
		return;
	}
	
	// Create text layout rect
	RECT rc;
	GetClientRect(hwnd, &rc);
	D2D1_RECT_F textLayoutRect = D2D1::RectF(
		static_cast<FLOAT>(rc.left),
		static_cast<FLOAT>(rc.top),
		static_cast<FLOAT>(rc.right - rc.left),
		static_cast<FLOAT>(rc.bottom - rc.top)
		);

	// Draw text
	g_pRenderTarget->DrawText(
		wszText,		// Text to render
		cTextLength,	// Text length
		g_pTextFormat,	// Text format
		textLayoutRect,	// The region of the window where the text will be rendered
		g_pBlackBrush	// The brush used to draw the text
		);
}

VOID Render(HWND hwnd)
{
	// Create device dependent resources
	CreateDeviceResources(hwnd);

	g_pRenderTarget->BeginDraw();
	g_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

	DrawText(hwnd);

	g_pRenderTarget->EndDraw();
}

// When window size changed, we need to resize the render target as well
VOID OnResize(UINT width, UINT height)
{
	if(g_pRenderTarget)
	{
		D2D1_SIZE_U size = D2D1::SizeU(width, height);
		g_pRenderTarget->Resize(size);
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)   
{
	switch (message)    
	{
	case   WM_PAINT:
		Render(hwnd);
		ValidateRect(hwnd, NULL) ;
		return 0 ;

	case WM_SIZE:
		{
			UINT width = LOWORD(lParam);
			UINT height = HIWORD(lParam);
			OnResize(width, height);
			break;
		}

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
		DiscardDeviceResources(); 
		PostQuitMessage( 0 ); 
		return 0; 
	}

	return DefWindowProc (hwnd, message, wParam, lParam) ;
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow )
{
	WNDCLASSEX winClass ;

	winClass.lpszClassName = L"Direct2D Render text";
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
		MessageBox ( NULL, TEXT( "This program requires Windows NT!" ), L"error", MB_ICONERROR) ;
		return 0 ;  
	}   

	HWND hwnd = CreateWindowEx(NULL,  
		L"Direct2D Render text",	    // window class name
		L"Direct2D Render text",		// window caption
		WS_OVERLAPPEDWINDOW, 			// window style
		CW_USEDEFAULT,					// initial x position
		CW_USEDEFAULT,					// initial y position
		500,							// initial x size
		500,							// initial y size
		NULL,							// parent window handle
		NULL,							// window menu handle
		hInstance,						// program instance handle
		NULL) ;							// creation parameters

	CreateDeviceIndependentResources();

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
