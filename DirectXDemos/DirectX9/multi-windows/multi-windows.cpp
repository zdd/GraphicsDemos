#include <d3d9.h>
#include <d3dx9.h>

HINSTANCE g_3DhInst;
ATOM g_3DWinClass;
HWND g_3DWindow=0;
HWND g_3DFocusWindow=0;
DWORD g_3DWindowStyleWin;
DWORD g_3DWindowStyleFullScreen;
DWORD g_3DWindowStyleExWin;
DWORD g_3DWindowStyleExFullScreen;
D3DPRESENT_PARAMETERS g_d3dpp;
LPDIRECT3D9 g_pD3D=0;
LPDIRECT3DDEVICE9 g_pDev=0;
bool g_bResetDevice = false;
WCHAR g_strWinClass[] = L"QuickDX9";
WCHAR g_strWinTitle[] = L"TestApp";

LRESULT WINAPI TestWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg==WM_CLOSE)
	{
		PostQuitMessage(0);
	}
	else if (msg==WM_SIZE)
	{
		g_bResetDevice = true;
	}
	else if (msg==WM_KEYDOWN && wParam == VK_ESCAPE)
	{
		PostQuitMessage(0);
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

bool RegisterWin()
{
	WNDCLASSEX wndclass;

	memset(&wndclass, 0, sizeof(wndclass));
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_OWNDC;
	wndclass.lpfnWndProc = TestWinProc;
	wndclass.hInstance = g_3DhInst;
	wndclass.lpszClassName = g_strWinClass;
	g_3DWinClass = RegisterClassEx(&wndclass);
	return g_3DWinClass != 0;
}

bool CreateWin()
{
	RECT rc;

	g_3DWindowStyleFullScreen = g_3DWindowStyleWin = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
	g_3DWindowStyleExFullScreen = g_3DWindowStyleExWin = WS_EX_OVERLAPPEDWINDOW;
	rc.left = 0;
	rc.top = 0;
	rc.right = 800;
	rc.bottom = 600;
	AdjustWindowRectEx(&rc, g_3DWindowStyleWin, false, g_3DWindowStyleExWin);
	g_3DFocusWindow = CreateWindowEx(g_3DWindowStyleExWin, g_strWinClass, g_strWinTitle, g_3DWindowStyleWin, 0, 0, rc.right - rc.left, rc.bottom - rc.top, 0, 0, g_3DhInst, 0);
	g_3DWindow = g_3DFocusWindow;
	return g_3DWindow != 0;
}

bool MsgPump()
{
	MSG msg;
	static bool exit = false;

	if (exit)
		return false;

	while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			exit = true;
			return false;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return true;
}

void Cleanup()
{
	if (g_pDev)
		g_pDev->Release();
	if (g_pD3D)
		g_pD3D->Release();
	if (g_3DWindow)
		DestroyWindow(g_3DWindow);
}


bool CreateD3D()
{
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	return g_pD3D != 0;
}

bool CreateDevice()
{
	RECT rc;

	GetClientRect(g_3DWindow, &rc);
	memset(&g_d3dpp, 0, sizeof(g_d3dpp));
	g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	g_d3dpp.EnableAutoDepthStencil = true;
	g_d3dpp.AutoDepthStencilFormat = D3DFMT_D24X8;
	g_d3dpp.hDeviceWindow = g_3DWindow;
	g_d3dpp.BackBufferWidth = rc.right - rc.left;
	g_d3dpp.BackBufferHeight = rc.bottom - rc.top;
	g_d3dpp.BackBufferCount = 1;
	g_d3dpp.Windowed = true;
	g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE ;
	if (FAILED(g_pD3D->CreateDevice(0, D3DDEVTYPE_HAL, g_3DFocusWindow, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pDev)))
		return false;
	return true;
}

void TestPreReset();
bool TestPostReset();

bool ResetDevice()
{
	TestPreReset();
	if (FAILED(g_pDev->Reset(&g_d3dpp)))
		return false;
	return TestPostReset();
}

bool TestDevice()
{
	HRESULT hr = g_pDev->TestCooperativeLevel();

	if (hr == D3DERR_DEVICENOTRESET || (hr == S_OK && g_bResetDevice))
	{
		g_bResetDevice = false;
		if (!ResetDevice())
			return false;
	}
	else if (hr == D3DERR_DEVICELOST)
		return false;

	return true;
}

HWND g_swapwnd = 0;
LPDIRECT3DSWAPCHAIN9 g_pSwap = 0;
D3DPRESENT_PARAMETERS g_scparam;
LPDIRECT3DSURFACE9 g_pSwapDepthSurf = 0;
LPDIRECT3DSURFACE9 g_pSwapRenderSurf = 0;

LPDIRECT3DSURFACE9 g_pDevDepthSurf = 0;
LPDIRECT3DSURFACE9 g_pDevRenderSurf = 0;

bool TestSetup()
{
	RECT rc;
	rc.left = 0;
	rc.top = 0;
	rc.right = 400;
	rc.bottom = 400;
	AdjustWindowRectEx(&rc, g_3DWindowStyleWin, false, g_3DWindowStyleExWin);
	g_swapwnd = CreateWindowEx(g_3DWindowStyleExWin, g_strWinClass, L"ChainWnd", g_3DWindowStyleWin, 800, 0, rc.right - rc.left, rc.bottom - rc.top, 0, 0, g_3DhInst, 0);

	return TestPostReset();
}

void TestCleanup()
{
	TestPreReset();
	if (g_swapwnd)
		DestroyWindow(g_swapwnd);
}

void TestPreReset()
{
	if (g_pSwapDepthSurf)
		g_pSwapDepthSurf->Release();
	g_pSwapDepthSurf = 0;
	if (g_pSwapRenderSurf)
		g_pSwapRenderSurf->Release();
	g_pSwapRenderSurf = 0;
	if (g_pSwap)
		g_pSwap->Release();
	g_pSwap = 0;
	if (g_pDevRenderSurf)
		g_pDevRenderSurf->Release();
	g_pDevRenderSurf = 0;
	if (g_pDevDepthSurf)
		g_pDevDepthSurf->Release();
	g_pDevDepthSurf = 0;
}

bool TestPostReset()
{
	memset(&g_scparam, 0, sizeof(g_scparam));
	g_scparam.BackBufferCount = 1;
	g_scparam.BackBufferFormat = D3DFMT_X8R8G8B8;
	g_scparam.BackBufferHeight = 400;
	g_scparam.BackBufferWidth = 400;
	g_scparam.hDeviceWindow = g_swapwnd;
	g_scparam.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_scparam.Windowed = true;
	g_scparam.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	g_pDev->CreateAdditionalSwapChain(&g_scparam, &g_pSwap);
	g_pSwap->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &g_pSwapRenderSurf);
	g_pDev->CreateDepthStencilSurface(400,400,D3DFMT_D24X8, D3DMULTISAMPLE_NONE, 0, false, &g_pSwapDepthSurf, 0);

	g_pDev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &g_pDevRenderSurf);
	g_pDev->GetDepthStencilSurface(&g_pDevDepthSurf);
	return true;
}

class TestVert
{
public:
	float x,y,z;
	DWORD color;
};

TestVert g_aoVerts[4];
D3DXMATRIX w,v,p;

void TestUpdate()
{
	g_aoVerts[0].x = -1; 
	g_aoVerts[0].y = -1; 
	g_aoVerts[0].z =  0;
	g_aoVerts[0].color = 0xFFFF0000;

	g_aoVerts[1].x = -1; 
	g_aoVerts[1].y =  1; 
	g_aoVerts[1].z =  0;
	g_aoVerts[1].color = 0xFFFFFF00;

	g_aoVerts[2].x =  1; 
	g_aoVerts[2].y = -1; 
	g_aoVerts[2].z =  0;
	g_aoVerts[2].color = 0xFFFF00FF;

	g_aoVerts[3].x =  1; 
	g_aoVerts[3].y =  1; 
	g_aoVerts[3].z =  0;
	g_aoVerts[3].color = 0xFFFFFFFF;

	POINT pt;
	GetCursorPos(&pt);
	D3DXMatrixRotationYawPitchRoll(&w, (pt.x - 800.0f) / 800 * D3DX_PI / 2, (pt.y - 600.0f) / 600 * D3DX_PI / 2, 0);

	w._43 = 2;
	D3DXMatrixPerspectiveFovLH(&p, D3DX_PI/4, 1.0, 0.1f, 500.0f);

	g_pDev->SetTransform(D3DTS_WORLD, &w);
	g_pDev->SetTransform(D3DTS_PROJECTION, &p);
	D3DXMatrixIdentity(&w);
	g_pDev->SetTransform(D3DTS_VIEW, &w);
}

void TestRender()
{
	g_pDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	g_pDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	g_pDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	g_pDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);

	g_pDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	g_pDev->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	g_pDev->SetRenderState(D3DRS_LIGHTING, false);

	g_pDev->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
	g_pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	g_pDev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, g_aoVerts, sizeof(TestVert));
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR cmdLine, int cmdShow)
{

	g_3DhInst = hInst;
	do
	{
		if (!RegisterWin())
			break;
		if (!CreateWin())
			break;
		if (!CreateD3D())
			break;
		if (!CreateDevice())
			break;
		if (!TestSetup())
			break;
		while (MsgPump())
		{
			if (TestDevice())
			{
				D3DVIEWPORT9 vp;

				vp.X = 0;
				vp.Y = 0;
				vp.Width = 800;
				vp.Height = 600;
				vp.MinZ = 0;
				vp.MaxZ = 1;
				TestUpdate();
				g_pDev->SetRenderTarget(0, g_pDevRenderSurf);
				g_pDev->SetDepthStencilSurface(g_pDevDepthSurf);
				g_pDev->SetViewport(&vp);
				g_pDev->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x456789, 1.0f, 0);
				g_pDev->BeginScene();
				TestRender();
				g_pDev->EndScene();
				g_pDev->Present(0,0,0,0);

				g_pDev->SetRenderTarget(0, g_pSwapRenderSurf);
				g_pDev->SetDepthStencilSurface(g_pSwapDepthSurf);
				vp.Width = 400;
				vp.Height = 400;
				g_pDev->SetViewport(&vp);
				g_pDev->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x654321, 1.0f, 0);
				g_pDev->BeginScene();
				TestRender();
				g_pDev->EndScene();
				g_pSwap->Present(0, 0, 0, 0, 0);
			}
		};
	} while (0);

	TestCleanup();
	Cleanup();

	return 0;
}


class Renderable
{
public:
	virtual void render();
	// anything else
};

// anything help to render the object goes in the subclass here.
class SomethingtoRender : Renderable
{
	void render()
	{
		// implementation details
	}
};

