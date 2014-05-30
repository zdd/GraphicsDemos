#include <wincodec.h>
#include <time.h> // for random number
#include <D2D1.h> 

#define SAFE_RELEASE(P) if(P){P->Release() ; P = NULL ;}

ID2D1Factory*				g_pD2DFactory = NULL ;	// Direct2D factory
ID2D1HwndRenderTarget*		g_pRenderTarget = NULL;	// Render target
ID2D1SolidColorBrush*		g_pBlackBrush = NULL ;	// A black brush, reflect the line color
ID2D1Bitmap*				g_pBitmap = NULL ;		// The Entire bitmap, will be split to small parts
IWICImagingFactory*			g_pWICFactory = NULL ;

HWND g_Hwnd ;	// Window handle
D2D1_RECT_U g_PictureRect ;	// The rectangle to hold the picture
const int g_NumColumns = 4;	// Number of columns
const int g_NumRows = 4;	// Number of rows
const int g_NumCells = g_NumColumns * g_NumRows ;

int srcPieceWidth = -1 ; 
int srcPieceHeight = -1 ;
int destPieceWidth = -1 ;
int destPieceHeight = -1 ;

ID2D1Bitmap* g_pBitmapPiece[g_NumCells] = {NULL} ; 

int g_ClickCount = -1 ;
int g_Points[2] ;

struct Piece
{
	int id ;
	ID2D1Bitmap* bitmap ;
	Piece(int i, ID2D1Bitmap* p):id(i), bitmap(p){}
};

Piece* g_pPieces[g_NumCells] ;

bool g_bShowHint = false ;

// Disorder the image grid by disorder it's id in g_pPieces 
void Disorder()
{
	srand((unsigned int)time(0)) ;

	for (int i = 0; i < g_NumCells; ++i)
	{
		int a = rand() % g_NumCells ;
		int b = rand() % g_NumCells ;
		if (a != b)
		{
			int temp = g_pPieces[a]->id ;
			g_pPieces[a]->id = g_pPieces[b]->id ;
			g_pPieces[b]->id = temp ;
		}
	}
}

// Restore the image 
void Restore()
{
	for (int i = 0; i < g_NumCells; ++i)
		g_pPieces[i]->id = i ;
}

// Determine whether the picture was resolved
bool IsDone()
{
	for (int i = 0; i < g_NumCells; ++i)
	{
		if(g_pPieces[i]->id != i)
			return false ;
	}

	return true ;
}

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

		// Create WIC factory
		hr = CoCreateInstance(
			CLSID_WICImagingFactory1, 
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_IWICImagingFactory,
			reinterpret_cast<void **>(&g_pWICFactory)
			) ;
		if (FAILED(hr))
		{
			MessageBox(hWnd, "Create WIC factory failed!", "Error", 0) ;
			return ;
		}

		// Obtain the size of the drawing area
		RECT rc ;		
		GetClientRect(hWnd, &rc) ;

		g_PictureRect.left = rc.left ;
		g_PictureRect.right = rc.right ;
		g_PictureRect.top = rc.top ;
		g_PictureRect.bottom = rc.bottom ;

		destPieceWidth = (g_PictureRect.right - g_PictureRect.left) / g_NumColumns ;
		destPieceHeight = (g_PictureRect.bottom - g_PictureRect.top) / g_NumRows ;

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
			&g_pBlackBrush
			) ;
		if (FAILED(hr))
		{
			MessageBox(hWnd, "Create brush failed!", "Error", 0) ;
			return ;
		}

		// Load bitmap from file
		hr = LoadBitmapFromFile(
			g_pRenderTarget,
			g_pWICFactory,
			L"picture.jpg",
			0,
			0,
			&g_pBitmap
			) ;

		if (FAILED(hr))
		{
			MessageBox(hWnd, "Create bitmap failed!", "Error", 0) ;
			return ;
		}

		D2D1_SIZE_U bitmapSize = g_pBitmap->GetPixelSize() ;
		srcPieceWidth = bitmapSize.width / g_NumColumns ;
		srcPieceHeight = bitmapSize.height / g_NumRows ;

		const D2D1_POINT_2U topleft = D2D1::Point2U(0, 0) ;

		// Initialize pieces
		for (int i = 0; i < g_NumCells; ++i)
		{
			// Create the bitmap piece
			const D2D1_BITMAP_PROPERTIES bitmapProp = D2D1::BitmapProperties(g_pBitmap->GetPixelFormat()) ;
			HRESULT hr = g_pRenderTarget->CreateBitmap(
				D2D1::SizeU(srcPieceWidth, srcPieceHeight),
				bitmapProp,
				&g_pBitmapPiece[i]
				) ;
			if (FAILED(hr))
			{
				MessageBox(NULL, "Create bitmap failed!", "Error", 0) ;
			}

			g_pPieces[i] = new Piece(i, g_pBitmapPiece[i]) ;

			const D2D1_RECT_U srcRect = D2D1::RectU(
				(i % g_NumColumns) * srcPieceWidth, 
				(i / g_NumColumns) * srcPieceHeight, 
				(i % g_NumColumns + 1) * srcPieceWidth, 
				(i / g_NumColumns + 1) * srcPieceHeight
				) ;

			// Copy the content from the big bitmap
			g_pPieces[i]->id = i ;
			g_pPieces[i]->bitmap->CopyFromBitmap(
				&topleft,
				g_pBitmap,
				&srcRect
				) ;
		}

		// Disorder pieces
		Disorder() ;
	}
}

VOID DrawBitmap()
{
	CreateD2DResource(g_Hwnd) ;

	g_pRenderTarget->BeginDraw() ;

	// Clear background color to dark cyan
	g_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

	if (g_bShowHint)
	{
		D2D1_RECT_F rect = D2D1::RectF(
			g_PictureRect.left, 
			g_PictureRect.top,
			g_PictureRect.right,
			g_PictureRect.bottom
			) ;

		g_pRenderTarget->DrawBitmap(
			g_pBitmap, 
			&rect
			) ;
	}
	else
	{
		// Draw pieces
		for (int i = 0; i < g_NumCells; ++i)
		{
			int id = g_pPieces[i]->id ;
			D2D1_RECT_F destRect = D2D1::RectF(
				(id % g_NumColumns) * destPieceWidth, 
				(id / g_NumColumns) * destPieceHeight,
				(id % g_NumColumns + 1) * destPieceWidth, 
				(id / g_NumColumns + 1) * destPieceHeight
				) ;

			g_pRenderTarget->DrawBitmap(
				g_pPieces[i]->bitmap,
				&destRect
				) ;
		}
	}

	HRESULT hr = g_pRenderTarget->EndDraw() ;
	if (FAILED(hr))
	{
		MessageBox(NULL, "Draw failed!", "Error", 0) ;
		return ;
	}
}

VOID Cleanup()
{
	for (int i = 0; i < g_NumCells; ++i)
	{
		SAFE_RELEASE(g_pPieces[i]->bitmap) ;
	}

	for (int i = 0; i < g_NumCells; ++i)
	{
		delete g_pPieces[i] ;
		g_pPieces[i] = NULL ;
	}

	SAFE_RELEASE(g_pRenderTarget) ;
	SAFE_RELEASE(g_pBlackBrush) ;
	SAFE_RELEASE(g_pBitmap) ;
	SAFE_RELEASE(g_pWICFactory) ;
	SAFE_RELEASE(g_pD2DFactory) ;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)   
{
	int curId = -1 ;
	WORD mousePosX = -1 ;
	WORD mousePosY = -1 ;
	D2D1_SIZE_U newRenderTargetSize ;

	switch (message)    
	{
	case WM_LBUTTONUP:
		if(IsDone())
		{
			MessageBox(NULL, "Picture was resolved\nPress D to disorder the picture and play again", "Info", 0) ;
			break ;
		}
		// Compute the grid id via mouse position
		mousePosX = LOWORD(lParam) ;
		mousePosY = HIWORD(lParam) ;
		curId = mousePosY / destPieceHeight * g_NumColumns + mousePosX / destPieceWidth ;

		++g_ClickCount ;
		g_Points[g_ClickCount] = curId ;

		// If it is the second click, swap the two grid
		if (g_ClickCount == 1)
		{
			// Swap only when the mouse click two different grids
			if (g_Points[0] != g_Points[1])
			{
				int m = -1 ;
				int n = -1 ;

				for (int i = 0; i < g_NumCells; ++i)
				{
					if (g_pPieces[i]->id == g_Points[0])
					{
						m = i ;
					}
					if (g_pPieces[i]->id == g_Points[1])
					{
						n = i ;
					}
				}

				g_pPieces[m]->id = g_Points[1] ;
				g_pPieces[n]->id = g_Points[0] ;

				InvalidateRect(hwnd, NULL, FALSE) ;
				UpdateWindow(hwnd) ;

				if (IsDone())
				{
					MessageBox(NULL, "Well done!", "Info", 0) ;
				}
			}
			g_ClickCount = -1 ;	
		}
		break ;

	case WM_SIZE: // Is this message before WM_CREATE?
		if (g_pRenderTarget)
		{
			RECT rc ;		
			GetClientRect(hwnd, &rc) ;

			// Update picture size, otherwise the hint will draw the picture with the original size
			g_PictureRect.left = rc.left ;
			g_PictureRect.right = rc.right ;
			g_PictureRect.top = rc.top ;
			g_PictureRect.bottom = rc.bottom ;

			// Resize the render target
			newRenderTargetSize = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top) ;
			g_pRenderTarget->Resize(newRenderTargetSize) ;
			destPieceWidth = (rc.right - rc.left) / g_NumColumns ;
			destPieceHeight = (rc.bottom - rc.top) / g_NumRows ;
			InvalidateRect(hwnd, NULL, FALSE) ;
			UpdateWindow(hwnd) ;
		}
		
		break ;

	case WM_PAINT:
		DrawBitmap() ;
		ValidateRect(g_Hwnd, NULL) ;
		return 0 ;

	case WM_KEYDOWN: 
		{ 
			switch( wParam ) 
			{ 
			case 'D': // disorder the image
				Disorder() ;
				InvalidateRect(hwnd, NULL, FALSE) ;
				UpdateWindow(hwnd) ;
				break ;

			case 'R':
				Restore() ;
				InvalidateRect(hwnd, NULL, FALSE) ;
				UpdateWindow(hwnd) ;
				break ;

			case VK_CONTROL:
				g_bShowHint = true ;
				InvalidateRect(hwnd, NULL, FALSE) ;
				UpdateWindow(hwnd) ;
				break ;

			case VK_ESCAPE: 
				SendMessage( hwnd, WM_CLOSE, 0, 0 ); 
				break ; 

			default: 
				break ; 
			} 
		} 
		break ; 

	case WM_KEYUP:
		{
			switch(wParam)
			{
			case VK_CONTROL:
				g_bShowHint = false ;
				InvalidateRect(hwnd, NULL, FALSE) ;
				UpdateWindow(hwnd) ;
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