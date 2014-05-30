/*
Description:	This demo show you how use brush transformation, D2D does not provide bitmap load function, so we use WIC instead.
Date:			2012-09-20
Author:			zdd(vckzdd@gmail.com)
*/

#include <windows.h>
#include <wincodec.h>
#include <D2D1.h> // header for Direct2D

#define SAFE_RELEASE(P) if(P){P->Release() ; P = NULL ;}
#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

ID2D1Factory*			pD2DFactory		= NULL;		// Direct2D factory
ID2D1HwndRenderTarget*	pRenderTarget	= NULL;		// Render target
ID2D1SolidColorBrush*	pBlackBrush		= NULL ;	// A black brush, reflect the line color
ID2D1Bitmap*			pBitmap			= NULL;
IWICImagingFactory*		pWICFactory		= NULL;
ID2D1BitmapBrush*		g_pBitmapBrush  = NULL;		// Bitmap brush

RECT rc ;		// Render area
HWND g_Hwnd ;	// Window handle

HRESULT LoadBitmapFromFile(
	ID2D1RenderTarget *pRenderTarget,
	IWICImagingFactory *pIWICFactory,
	PCWSTR uri,
	UINT destinationWidth,
	UINT destinationHeight,
	ID2D1Bitmap **ppBitmap
) ;

VOID CreateD2DResource(HWND hWnd)
{
	if (!pRenderTarget)
	{
		HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory) ;
		if (FAILED(hr))
		{
			MessageBox(hWnd, "Create D2D factory failed!", "Error", 0) ;
			return ;
		}

		// Create WIC factory
		hr = CoCreateInstance(
			CLSID_WICImagingFactory1,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_IWICImagingFactory,
			reinterpret_cast<void **>(&pWICFactory)
			) ;
		if (FAILED(hr))
		{
			MessageBox(hWnd, "Create WIC factory failed!", "Error", 0) ;
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

		// Create a black brush
		hr = pRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::Black),
			&pBlackBrush
			) ;
		if (FAILED(hr))
		{
			MessageBox(hWnd, "Create black brush failed!", "Error", 0) ;
			return ;
		}

		// Load bitmap from file
		hr = LoadBitmapFromFile(
			pRenderTarget,
			pWICFactory,
			L"sampleImage.jpg",
			0,
			0,
			&pBitmap
			) ;

		if (FAILED(hr))
		{
			MessageBox(hWnd, "Create bitmap failed!", "Error", 0) ;
			return ;
		}

		// Create a bitmap brush
		hr = pRenderTarget->CreateBitmapBrush(pBitmap, &g_pBitmapBrush);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "Create bitmap brush failed!", "Error", 0) ;
			return ;
		}
	}
}
	
//
// Creates a Direct2D bitmap from the specified
// file name.
//
HRESULT LoadBitmapFromFile(
	ID2D1RenderTarget *pRenderTarget,
	IWICImagingFactory *pIWICFactory,
	PCWSTR uri,
	UINT destinationWidth,
	UINT destinationHeight,
	ID2D1Bitmap **ppBitmap
	)
{
	HRESULT hr = S_OK;

	IWICBitmapDecoder *pDecoder = NULL;
	IWICBitmapFrameDecode *pSource = NULL;
	IWICStream *pStream = NULL;
	IWICFormatConverter *pConverter = NULL;
	IWICBitmapScaler *pScaler = NULL;

	hr = pIWICFactory->CreateDecoderFromFilename(
		uri,
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&pDecoder
		);
	if (SUCCEEDED(hr))
	{

		// Create the initial frame.
		hr = pDecoder->GetFrame(0, &pSource);
	}

	if (SUCCEEDED(hr))
	{
		// Convert the image format to 32bppPBGRA
		// (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
		hr = pIWICFactory->CreateFormatConverter(&pConverter);
	}
	if (SUCCEEDED(hr))
	{
		// If a new width or height was specified, create an
		// IWICBitmapScaler and use it to resize the image.
		if (destinationWidth != 0 || destinationHeight != 0)
		{
			UINT originalWidth, originalHeight;
			hr = pSource->GetSize(&originalWidth, &originalHeight);
			if (SUCCEEDED(hr))
			{
				if (destinationWidth == 0)
				{
					FLOAT scalar = static_cast<FLOAT>(destinationHeight) / static_cast<FLOAT>(originalHeight);
					destinationWidth = static_cast<UINT>(scalar * static_cast<FLOAT>(originalWidth));
				}
				else if (destinationHeight == 0)
				{
					FLOAT scalar = static_cast<FLOAT>(destinationWidth) / static_cast<FLOAT>(originalWidth);
					destinationHeight = static_cast<UINT>(scalar * static_cast<FLOAT>(originalHeight));
				}

				hr = pIWICFactory->CreateBitmapScaler(&pScaler);
				if (SUCCEEDED(hr))
				{
					hr = pScaler->Initialize(
						pSource,
						destinationWidth,
						destinationHeight,
						WICBitmapInterpolationModeCubic
						);
				}
				if (SUCCEEDED(hr))
				{
					hr = pConverter->Initialize(
						pScaler,
						GUID_WICPixelFormat32bppPBGRA,
						WICBitmapDitherTypeNone,
						NULL,
						0.f,
						WICBitmapPaletteTypeMedianCut
						);
				}
			}
		}
		else // Don't scale the image.
		{
			hr = pConverter->Initialize(
				pSource,
				GUID_WICPixelFormat32bppPBGRA,
				WICBitmapDitherTypeNone,
				NULL,
				0.f,
				WICBitmapPaletteTypeMedianCut
				);
		}
	}
	if (SUCCEEDED(hr))
	{
		// Create a Direct2D bitmap from the WIC bitmap.
		hr = pRenderTarget->CreateBitmapFromWicBitmap(
			pConverter,
			NULL,
			ppBitmap
			);
	}

	SAFE_RELEASE(pDecoder);
	SAFE_RELEASE(pSource);
	SAFE_RELEASE(pStream);
	SAFE_RELEASE(pConverter);
	SAFE_RELEASE(pScaler);

	return hr;
}

VOID CalculateTranslationMatrix(D2D1_MATRIX_3X2_F* matrix)
{
	static float totalTime = 0.0f;

	// Get start time
	static DWORD startTime = timeGetTime();

	// Get current time
	DWORD currentTime = timeGetTime();

	// Calculate time elapsed
	float timeElapsed = (currentTime - startTime) * 0.001f;

	// Accumulate total time elapsed
	totalTime += timeElapsed;

	// Build up the translation matrix
	matrix->_11 = 1.0f;
	matrix->_12 = 0.0f;
	matrix->_21 = 0.0f;
	matrix->_22 = 1.0f;
	matrix->_31 = totalTime;
	matrix->_32 = totalTime;
}

void DrawBitmap()
{
	CreateD2DResource(g_Hwnd) ;

	pRenderTarget->BeginDraw() ;

	// Clear background color to White
	pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

	D2D1_RECT_F paintRect = D2D1::RectF(100, 100, 500, 500);

	D2D1_MATRIX_3X2_F matrix;
	CalculateTranslationMatrix(&matrix);
	g_pBitmapBrush->SetTransform(&matrix);

	pRenderTarget->FillRectangle(&paintRect, g_pBitmapBrush);
	pRenderTarget->DrawRectangle(paintRect, pBlackBrush, 1, NULL);

		
	HRESULT hr = pRenderTarget->EndDraw() ;
	if (FAILED(hr))
	{
		MessageBox(NULL, "Draw failed!", "Error", 0) ;
		return ;
	}
}

VOID Cleanup()
{
	SAFE_RELEASE(g_pBitmapBrush);
	SAFE_RELEASE(pBlackBrush);
	SAFE_RELEASE(pRenderTarget) ;
	SAFE_RELEASE(pD2DFactory) ;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)   
{
	switch (message)    
	{
	case   WM_PAINT:
		DrawBitmap() ;
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
		"Draw Bitmap",			// window caption
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