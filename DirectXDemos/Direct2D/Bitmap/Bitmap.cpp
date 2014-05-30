/*
This Demo show you how to load a bitmap from application's resource or from a file
D2D does not provide bitmap load function, so we use WIC to work with it
*/

#include <windows.h>
#include <wincodec.h>
#include <D2D1.h> // header for Direct2D

#define SAFE_RELEASE(P) if(P){P->Release() ; P = NULL ;}
#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

ID2D1Factory*			pD2DFactory		= NULL ;	// Direct2D factory
ID2D1HwndRenderTarget*	pRenderTarget	= NULL;		// Render target
ID2D1SolidColorBrush*	pBlackBrush		= NULL ;	// A black brush, reflect the line color
ID2D1SolidColorBrush*	pRedBrush		= NULL ;
ID2D1Bitmap*			pBitmap			= NULL ;
IWICImagingFactory*		pWICFactory		= NULL ;

RECT rc ;		// Render area
HWND g_Hwnd ;	// Window handle

HRESULT LoadResourceBitmap(
	ID2D1RenderTarget* pRendertarget,
	IWICImagingFactory* pIWICFactory,
	PCSTR resourceName,
	PCSTR resourceType,
	UINT destinationWidth,
	UINT destinationHeight,
	ID2D1Bitmap** ppBitmap
) ;

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

		// Create WIC factory
		hr = CoCreateInstance(
			CLSID_WICImagingFactory1,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_IWICImagingFactory,
			reinterpret_cast<void **>(&pWICFactory)
			) ;

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

		// Create a red brush
		hr = pRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::Red),
			&pRedBrush
			) ;
		if (FAILED(hr))
		{
			MessageBox(hWnd, "Create red brush failed!", "Error", 0) ;
			return ;
		}

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
	}
}

// Load bitmap from app's resource
HRESULT LoadResourceBitmap(
						   ID2D1RenderTarget* pRendertarget,
						   IWICImagingFactory* pIWICFactory,
						   PCSTR resourceName,
						   PCSTR resourceType,
						   UINT destinationWidth,
						   UINT destinationHeight,
						   ID2D1Bitmap** ppBitmap
						   )
{
	HRESULT hr = S_OK ;

	IWICBitmapDecoder* pDecoder = NULL ;
	IWICBitmapFrameDecode* pSource = NULL ;
	IWICStream* pStream = NULL ;
	IWICFormatConverter* pConverter = NULL ;
	IWICBitmapScaler* pScaler = NULL ;

	HRSRC imageResHandle = NULL ;
	HGLOBAL imageResDataHandle = NULL ;
	void* pImageFile = NULL ;
	DWORD imageFileSize = 0 ;

	// Find the resource then load it
	imageResHandle = FindResource(HINST_THISCOMPONENT, resourceName, resourceType) ;
	hr = imageResHandle ? S_OK : E_FAIL ;
	if (SUCCEEDED(hr))
	{
		imageResDataHandle = LoadResource(HINST_THISCOMPONENT, imageResHandle) ;

		hr = imageResDataHandle ? S_OK : E_FAIL ;
	}

	// Lock the resource and calculate the image's size
	if (SUCCEEDED(hr))
	{
		// Lock it to get the system memory pointer
		pImageFile = LockResource(imageResDataHandle) ;

		hr = pImageFile ? S_OK : E_FAIL ;
	}
	if (SUCCEEDED(hr))
	{
		// Calculate the size
		imageFileSize = SizeofResource(HINST_THISCOMPONENT, imageResHandle) ;

		hr = imageFileSize ? S_OK : E_FAIL ;
	}

	// Create an IWICStream object
	if (SUCCEEDED(hr))
	{
		// Create a WIC stream to map onto the memory
		hr = pIWICFactory->CreateStream(&pStream) ;
	}

	if (SUCCEEDED(hr))
	{
		// Initialize the stream with the memory pointer and size
		hr = pStream->InitializeFromMemory(
			reinterpret_cast<BYTE*>(pImageFile),
			imageFileSize
			) ;
	}

	// Create IWICBitmapDecoder
	if (SUCCEEDED(hr))
	{
		// Create a decoder for the stream
		hr = pIWICFactory->CreateDecoderFromStream(
			pStream,
			NULL,
			WICDecodeMetadataCacheOnLoad,
			&pDecoder
			) ;
	}

	// Retrieve a frame from the image and store it in an IWICBitmapFrameDecode object
	if (SUCCEEDED(hr))
	{
		// Create the initial frame
		hr = pDecoder->GetFrame(0, &pSource) ;
	}

	// Before Direct2D can use the image, it must be converted to the 32bppPBGRA pixel format.
	// To convert the image format, use the IWICImagingFactory::CreateFormatConverter method to create an IWICFormatConverter object, then use the IWICFormatConverter object's Initialize method to perform the conversion.
	if (SUCCEEDED(hr))
	{
		// Convert the image format to 32bppPBGRA
		// (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
		hr = pIWICFactory->CreateFormatConverter(&pConverter) ;
	}

	if (SUCCEEDED(hr))
	{
		// If a new width or height was specified, create and
		// IWICBitmapScaler and use it to resize the image.
		if (destinationWidth != 0 || destinationHeight != 0)
		{
			UINT originalWidth ;
			UINT originalHeight ;
			hr = pSource->GetSize(&originalWidth, &originalHeight) ;
			if (SUCCEEDED(hr))
			{
				if (destinationWidth == 0)
				{
					FLOAT scalar = static_cast<FLOAT>(destinationHeight) / static_cast<FLOAT>(originalHeight);
					destinationWidth = static_cast<UINT>(scalar * static_cast<FLOAT>(originalWidth)) ;
				}
				else if (destinationHeight == 0)
				{
					FLOAT scalar = static_cast<FLOAT>(destinationWidth) / static_cast<FLOAT>(originalWidth);
					destinationHeight = static_cast<UINT>(scalar * static_cast<FLOAT>(originalHeight));
				}

				hr = pIWICFactory->CreateBitmapScaler(&pScaler) ;
				if (SUCCEEDED(hr))
				{
					hr = pScaler->Initialize(
						pSource,
						destinationWidth,
						destinationHeight,
						WICBitmapInterpolationModeCubic
						) ;
					if (SUCCEEDED(hr))
					{
						hr = pConverter->Initialize(
							pScaler,
							GUID_WICPixelFormat32bppPBGRA,
							WICBitmapDitherTypeNone,
							NULL,
							0.f,
							WICBitmapPaletteTypeMedianCut
							) ;
					}
				}
			}
		}

		else // use default width and height
		{
			hr = pConverter->Initialize(
				pSource,
				GUID_WICPixelFormat32bppPBGRA,
				WICBitmapDitherTypeNone,
				NULL,
				0.f,
				WICBitmapPaletteTypeMedianCut
				) ;
		}
	}

	// Finally, Create an ID2D1Bitmap object, that can be drawn by a render target and used with other Direct2D objects
	if (SUCCEEDED(hr))
	{
		// Create a Direct2D bitmap from the WIC bitmap
		hr = pRendertarget->CreateBitmapFromWicBitmap(
			pConverter,
			NULL,
			ppBitmap
			) ;
	}

	SAFE_RELEASE(pDecoder) ;
	SAFE_RELEASE(pSource) ;
	SAFE_RELEASE(pStream) ;
	SAFE_RELEASE(pConverter) ;
	SAFE_RELEASE(pScaler) ;

	return hr ;
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

VOID DrawRectangle()
{
	CreateD2DResource(g_Hwnd) ;

	pRenderTarget->BeginDraw() ;

	// Clear background color to dark cyan
	pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::DarkCyan));

	// Draw Rectangle
	D2D1_RECT_F rectangle = D2D1::RectF(200.f, 200.f, 400.f, 400.f) ;
	pRenderTarget->DrawRectangle(
		rectangle,
		pBlackBrush
		);

	// Skew
	pRenderTarget->SetTransform(
		D2D1::Matrix3x2F::Skew(30.f, 0.f, D2D1::Point2F(200.f, 200.f))
		) ;

	// Fill the rectangle
	pRenderTarget->FillRectangle(rectangle, pRedBrush) ;

	HRESULT hr = pRenderTarget->EndDraw() ;
	if (FAILED(hr))
	{
		MessageBox(NULL, "Draw failed!", "Error", 0) ;

		return ;
	}
}

void DrawBitmap()
{
	CreateD2DResource(g_Hwnd) ;

	pRenderTarget->BeginDraw() ;

	// Clear background color to dark cyan
	pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

	D2D1_SIZE_F size = pBitmap->GetSize() ;
	D2D1_POINT_2F upperLeftCorner = D2D1::Point2F(0.f, 0.f) ;

	// Draw bitmap
	pRenderTarget->DrawBitmap(
		pBitmap,
		D2D1::RectF(
		upperLeftCorner.x,
		upperLeftCorner.y,
		upperLeftCorner.x + size.width,
		upperLeftCorner.y + size.height)
		) ;

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
	SAFE_RELEASE(pRedBrush) ;
	SAFE_RELEASE(pD2DFactory) ;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)   
{
	switch (message)    
	{
	case   WM_PAINT:
		DrawBitmap() ;
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