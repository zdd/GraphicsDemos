#include <windows.h>

int ScreenWidth = -1 ;
int ScreenHeight = -1;

void CopyScreen(HWND hWnd)
{
	// Get program window DC, this DC is the drawing destination
	HDC hDC = GetDC(hWnd) ;
	
	// Get the desktop DC, this DC is the drawing content
	HWND hDesktopHwnd = GetDesktopWindow() ;
	HDC hDesktopDC = GetDC(hDesktopHwnd);

	// Copy desktop content to window
	BitBlt(hDC, 0, 0, ScreenWidth / 2, ScreenHeight, hDesktopDC, 0, 0, SRCCOPY); 

	ReleaseDC(hDesktopHwnd,hDesktopDC);
	DeleteDC(hDC);
}

// This was not finished yet!
void StretchScreentoWindow(HWND hWnd)
{
	// Get program window DC, this DC is the drawing destination
	HDC hDC = GetDC(hWnd) ;

	// Get the desktop DC, this DC is the drawing content
	HWND hDesktopHwnd = GetDesktopWindow() ;
	HDC hDesktopDC = GetDC(hDesktopHwnd);
	
	// Create compatible DC for desktop DC
	HDC hDesktoMemDC = CreateCompatibleDC(hDesktopDC) ;

	// Create compatible bitmap for desktop DC
	HBITMAP hBitmap = CreateCompatibleBitmap(hDesktopDC, ScreenWidth, ScreenHeight) ;

	// Select into memory DC
	SelectObject(hDesktoMemDC, hBitmap) ;

	// Stretch desktop content to window
	BOOL res = StretchBlt(hDC, 0, 0, 400, 400, hDesktoMemDC, 0, 0, ScreenWidth, ScreenHeight, SRCCOPY) ;
	if (!res)
	{
		MessageBox(NULL, "Stretch failed!", "Error!", 0) ;
	}
	//BitBlt(hDC,0,0,ScreenWidth / 2,ScreenHeight,hDesktopDC,0,0,SRCCOPY); 

	ReleaseDC(hDesktopHwnd,hDesktopDC);
	DeleteDC(hDC);
	DeleteDC(hDesktoMemDC) ;
	DeleteObject(hBitmap) ;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)   
{
	switch (message)    
	{
	case   WM_PAINT:
		//hdc = BeginPaint (hwnd, &ps) ;
		CopyScreen(hwnd) ;
		//EndPaint (hwnd, &ps) ;
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

	case   WM_DESTROY:
		PostQuitMessage (0) ;
		return 0 ;    
	}

	return DefWindowProc (hwnd, message, wParam, lParam) ;
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow )
{

	WNDCLASSEX winClass ;

	winClass.lpszClassName = "MY_WINDOWS_CLASS";
	winClass.cbSize        = sizeof(WNDCLASSEX);
	winClass.style         = CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc   = WndProc;
	winClass.hInstance     = hInstance;
	winClass.hIcon	       = NULL ;
	winClass.hIconSm	   = NULL ;
	winClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
	winClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	winClass.lpszMenuName  = NULL;
	winClass.cbClsExtra    = 0;
	winClass.cbWndExtra    = 0;

	if (!RegisterClassEx (&winClass))   
	{
		MessageBox ( NULL, TEXT( "This program requires Windows NT!" ), "error", MB_ICONERROR) ;
		return 0 ;  
	}   

	// Get screen resolution
	ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	ScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	HWND   hwnd ; 
	hwnd = CreateWindowEx(NULL,  
		"MY_WINDOWS_CLASS",        // window class name
		"The Hello World Program",	// window caption
		WS_OVERLAPPEDWINDOW, 		// window style
		ScreenWidth / 2,			// initial x position
		0,							// initial y position
		400,				// initial x size
		400,				// initial y size
		NULL,						// parent window handle
		NULL,						// window menu handle
		hInstance,					// program instance handle
		NULL) ;						// creation parameters

	ShowWindow (hwnd, iCmdShow) ;
	UpdateWindow (hwnd) ;

	MSG    msg ;  
	ZeroMemory( &msg, sizeof(msg) ); 
	PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE ); 

	while (msg.message != WM_QUIT)   
	{ 
		if( PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) != 0) 
		{ 
			TranslateMessage (&msg) ; 
			DispatchMessage (&msg) ; 
		} 
		/*else
		{
			CopyScreen(hwnd) ;
		}*/
	}

	return msg.wParam ;
}

