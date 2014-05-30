/*
This demo show you how Direct2D intoperate with D3D, this is also a demostration of how to make Direct2D full screen.
*/

#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <d2d1.h>

// D3D stuffs
ID3D11Device*			g_pd3dDevice		= NULL;
ID3D11DeviceContext*	g_pImmediateContext = NULL;
IDXGISwapChain*			g_pSwapChain		= NULL;
ID3D11RenderTargetView* g_pRenderTargetView = NULL;
ID3D11InputLayout*		g_pVertexLayout		= NULL;
ID3D11Buffer*			g_pVertexBuffer     = NULL;
ID3D11VertexShader*		g_pVertexShader		= NULL;
ID3D11PixelShader*		g_pPixelShader		= NULL;

// D2D stuffs
ID2D1Factory*			g_pD2DFactory	= NULL;	// Direct2D factory
ID2D1RenderTarget*		g_pRenderTarget	= NULL;	// Render target
ID2D1SolidColorBrush*	g_pBlackBrush	= NULL;	// A black brush, reflect the line color
IDXGISurface*			g_pBackBuffer   = NULL; // 

bool					g_bActive			= true ; // Is window active?
#define SAFE_RELEASE(P) if(P){ P->Release(); P = NULL;}

VOID CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
VOID InitVertexBuffer();
VOID InitVertexShader();
VOID InitPixelShader();

VOID CreateD2DResource()
{
	if (!g_pRenderTarget)
	{
		HRESULT hr ;

		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_pD2DFactory) ;
		if (FAILED(hr))
		{
			MessageBox(NULL, "Create D2D factory failed!", "Error", 0) ;
			return ;
		}

		// Get a surface from the swap chain
		hr = g_pSwapChain->GetBuffer(
			0, 
			IID_PPV_ARGS(&g_pBackBuffer)
			);
		if(FAILED(hr))
		{
			MessageBox(NULL, "Get back buffer form swap chain failed!", "Error", 0);
		}

		// Create the DXGI surface render target
		FLOAT dpiX;
		FLOAT dpiY;
		g_pD2DFactory->GetDesktopDpi(&dpiX, &dpiY);

		D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
			dpiX,
			dpiY
			);

		// Create teh Direct2D render target which can draw on the swap chain surface
		hr = g_pD2DFactory->CreateDxgiSurfaceRenderTarget(
			g_pBackBuffer,
			&props,
			&g_pRenderTarget
			);
		if(FAILED(hr))
		{
			MessageBox(NULL, "Create Direct2D render target failed", "Error", 0);
		}

		// Create a brush
		hr = g_pRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::Black),
			&g_pBlackBrush
			) ;
		if (FAILED(hr))
		{
			MessageBox(NULL, "Create brush failed!", "Error", 0) ;
			return ;
		}
	}
}

HRESULT InitD3D( HWND hWnd )
{
	RECT rect;
	HWND hwnd = GetDesktopWindow();
	GetWindowRect(hwnd, &rect);

	int desktopWidth  = rect.right - rect.left;
	int desktopHeight = rect.bottom - rect.top;

	// Setup a DXGI swap chain descriptor
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof( sd ) );

	sd.BufferCount = 1;									// number of buffer
	sd.BufferDesc.Width = desktopWidth;					// buffer width, can we set it to the screen width?
	sd.BufferDesc.Height = desktopHeight;				// buffer height, can we set it to the screen height?
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// buffer format, 32 bit color with alpha(RGBA)
	sd.BufferDesc.RefreshRate.Numerator = 60;			// refresh rate?
	sd.BufferDesc.RefreshRate.Denominator = 1;			// WHAT'S THIS?
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// use buffer as render target
	sd.OutputWindow = hWnd;								// output window handle
	sd.SampleDesc.Count = 1;							// WHAT'S THIS?
	sd.SampleDesc.Quality = 0;							// WHAT'S THIS?
	sd.Windowed = FALSE;								// window mode

	// Create device and swap chain
	D3D_FEATURE_LEVEL FeatureLevelsRequested = D3D_FEATURE_LEVEL_11_0;	// Use d3d11
	UINT              numLevelsRequested = 1;							// Number of levels 
	D3D_FEATURE_LEVEL FeatureLevelsSupported;

	HRESULT hr;
	if (FAILED (hr = D3D11CreateDeviceAndSwapChain( NULL, 
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_BGRA_SUPPORT,
		&FeatureLevelsRequested,
		numLevelsRequested,
		D3D11_SDK_VERSION,
		&sd, 
		&g_pSwapChain,
		&g_pd3dDevice,
		&FeatureLevelsSupported,
		&g_pImmediateContext )))
	{
		return hr;
	}

	// Create render target and bind the back-buffer
	ID3D11Texture2D* pBackBuffer;

	// Get a pointer to the back buffer
	hr = g_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );

	// Create a render-target view
	g_pd3dDevice->CreateRenderTargetView( pBackBuffer, NULL, &g_pRenderTargetView);

	// Bind the view
	g_pImmediateContext->OMSetRenderTargets( 1, &g_pRenderTargetView, NULL ); // WHAT'S OM here mean?

	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (float)desktopWidth;	// this should be similar with the back-buffer width, global it!
	vp.Height = (float)desktopHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_pImmediateContext->RSSetViewports( 1, &vp );

	InitVertexShader();
	InitPixelShader();
	InitVertexBuffer();
		                       
	return S_OK;
}

VOID InitVertexBuffer()
{
	// The vertex format
	struct SimpleVertex
	{
		DirectX::XMFLOAT3 Pos;	// Position
	};

	// Create the vertex buffer
	SimpleVertex vertices[] = 
	{
		DirectX::XMFLOAT3( 0.0f, 0.5f, 0.5f ),
        DirectX::XMFLOAT3( 0.5f, -0.5f, 0.5f ),
        DirectX::XMFLOAT3( -0.5f, -0.5f, 0.5f ),
	};

	// Vertex Buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;		// Buffer usage
	bd.ByteWidth = sizeof(SimpleVertex) * 3;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	// copy vertex buffer data
	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = vertices;
	HRESULT	hr = g_pd3dDevice->CreateBuffer(&bd, &initData, &g_pVertexBuffer);
	if(FAILED(hr))
	{
		MessageBox(NULL, "Create vertex buffer failed", "Error", 0);
	}

	// Set vertex buffer
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// Set geometry type
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

VOID InitVertexShader()
{
	// Compile the vertex shader from file
	ID3DBlob*	pVSBlob = NULL;
	CompileShaderFromFile((WCHAR*)"triangle_shader.fx", "VS", "vs_4_0", &pVSBlob);

	// Create vertex shader
	HRESULT hr = g_pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &g_pVertexShader);
	if(FAILED(hr))
	{
		pVSBlob->Release();
		MessageBox(NULL, "Create vertex shader failed", "Error", 0);
	}

	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	UINT numElements = ARRAYSIZE(layout);

	// Create the input layout
	hr = g_pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), 
													pVSBlob->GetBufferSize(), &g_pVertexLayout);
	pVSBlob->Release();

	// Set the input layout
	g_pImmediateContext->IASetInputLayout(g_pVertexLayout);
}

VOID InitPixelShader()
{
	// Compile the pixel shader
	ID3DBlob*	pPSBlob = NULL;
	CompileShaderFromFile(L"triangle_shader.fx", "PS", "ps_4_0", &pPSBlob);

	// Create the pixel shader
	HRESULT hr = g_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &g_pPixelShader);
	pPSBlob->Release();
	if(FAILED(hr))
	{
		MessageBox(NULL, "Create pixel shader failed", "Error", 0);
	}
}

VOID CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
	ID3DBlob*	pErrorBlob;

	hr = D3DCompileFromFile(szFileName, NULL, NULL, szEntryPoint, szShaderModel, dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
	if(FAILED(hr))
	{
		if(pErrorBlob != NULL)
		{
			OutputDebugString((CHAR*)pErrorBlob->GetBufferPointer());
			pErrorBlob->Release();
		}
	}

	if(pErrorBlob)
		pErrorBlob->Release();
}

VOID Cleanup()
{
	if(g_pImmediateContext) 
		g_pImmediateContext->ClearState();

	// Release D3D resources
	SAFE_RELEASE(g_pVertexBuffer);
	SAFE_RELEASE(g_pVertexLayout);
	SAFE_RELEASE(g_pVertexShader);
	SAFE_RELEASE(g_pPixelShader);
	SAFE_RELEASE(g_pRenderTargetView);
	SAFE_RELEASE(g_pSwapChain);
	SAFE_RELEASE(g_pImmediateContext);
	SAFE_RELEASE(g_pd3dDevice);

	// Release D2D resources
	SAFE_RELEASE(g_pD2DFactory);	
	SAFE_RELEASE(g_pRenderTarget);
	SAFE_RELEASE(g_pBlackBrush);
	SAFE_RELEASE(g_pBackBuffer); 
}

VOID RenderD2DRectangle()
{
	if(g_pRenderTarget)
	{
		g_pRenderTarget->BeginDraw();

		// Draw rectangle
		D2D1_RECT_F rect = D2D1::RectF(100, 100, 500, 500);	
		g_pRenderTarget->DrawRectangle(&rect, g_pBlackBrush);

		g_pRenderTarget->EndDraw();
	}
}

VOID Render(float timeDelta)
{
	CreateD2DResource();

	if (!g_bActive)
		Sleep(50) ;

	// Clear the back-buffer to a BLUE color
	 float color[4] = {1.0f, 0, 0, 0.0f };
	g_pImmediateContext->ClearRenderTargetView( g_pRenderTargetView, color);

	// Render the triangle
	g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
	g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
	g_pImmediateContext->Draw(3, 0);

	// Draw D2D content
	RenderD2DRectangle();

	// Present the sence from back buffer to front buffer
	g_pSwapChain->Present(0, 0);
}

LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg )
	{
	case WM_KEYDOWN:
		{
			switch( wParam )
			{
			case VK_ESCAPE:
				SendMessage( hWnd, WM_CLOSE, 0, 0 );
				break ;
			default:
				break ;
			}
		}
		return 0 ;

	case WM_SIZE:
		if(wParam == SIZE_MAXHIDE || wParam == SIZE_MINIMIZED)
			g_bActive = false;
		else
			g_bActive = true;
		return 0;

	case WM_ACTIVATEAPP:
		if(wParam == TRUE)
			g_bActive = true ;
		else
			g_bActive = false ;
		return 0 ;

	case WM_DESTROY:
		Cleanup();
		PostQuitMessage( 0 );
		return 0;
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}

INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR szCmdLine, int iCmdShow)
{
	WNDCLASSEX winClass ;

	winClass.lpszClassName = "Triangle";
	winClass.cbSize        = sizeof(WNDCLASSEX);
	winClass.style         = CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc   = MsgProc;
	winClass.hInstance     = hInstance;
	winClass.hIcon	       = NULL ;
	winClass.hIconSm	   = NULL ;
	winClass.hCursor       = LoadCursor(NULL, IDC_ARROW) ;
	winClass.hbrBackground = NULL ;
	winClass.lpszMenuName  = NULL ;
	winClass.cbClsExtra    = 0;
	winClass.cbWndExtra    = 0;

	RegisterClassEx (&winClass) ;  

	HWND hWnd = CreateWindowEx(NULL,  
		winClass.lpszClassName,		// window class name
		"Triangle",				// window caption
		WS_OVERLAPPEDWINDOW, 		// window style
		0,							// initial x position
		0,							// initial y position
		1000,						// initial window width
		1000,						// initial window height
		NULL,						// parent window handle
		NULL,						// window menu handle
		hInstance,					// program instance handle
		NULL) ;						// creation parameters

	// Create window failed
	if(hWnd == NULL)
	{
		MessageBoxA(hWnd, "Create Window failed!", "Error", 0) ;
		return -1 ;
	}

	// Initialize Direct3D
	if( SUCCEEDED(InitD3D(hWnd)))
	{ 
		// Show the window
		ShowWindow( hWnd, SW_SHOWDEFAULT );
		UpdateWindow( hWnd );

		MSG msg ; 
		ZeroMemory( &msg, sizeof(msg) );
		PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );

		// Get last time
		static DWORD lastTime = timeGetTime();

		while (msg.message != WM_QUIT)  
		{
			if(PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) != 0)
			{
				TranslateMessage (&msg) ;
				DispatchMessage (&msg) ;
			}
			else // Render the game if there is no message to process
			{
				// Get current time
				DWORD currTime  = timeGetTime();

				// Calculate time elapsed
				float timeDelta = (currTime - lastTime) * 0.001f;

				// Render
				Render(timeDelta) ;

				// Update last time to current time for next loop
				lastTime = currTime;
			}
		}
	}

	UnregisterClass(winClass.lpszClassName, hInstance) ;
	return 0;
}
