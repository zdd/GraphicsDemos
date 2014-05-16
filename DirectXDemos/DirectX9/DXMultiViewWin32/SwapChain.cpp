#include "StdAfx.h"
#include "assert.h"
#include "SwapChain.h"

SwapChain::SwapChain(void): m_pD3D(NULL), g_pDevice(NULL), m_pBackbuffer(NULL), 
m_pZbuffer(NULL), m_pSwapChain(0), m_ClearColor(0)
{
}

SwapChain::~SwapChain(void)
{
}

IDirect3DSurface9* SwapChain::GetBackBuffer() const
{
	return m_pBackbuffer ;
}

IDirect3DSurface9* SwapChain::GetZbuffer() const
{
	return m_pZbuffer ;
}

void SwapChain::Create(LPDIRECT3D9 pD3D, 
					   LPDIRECT3DDEVICE9 pDevice, 
					   HWND hWnd, 
					   D3DCOLOR clearColor, 
					   bool enableZbuffer, 
					   int backBufferWidth, 
					   int backBufferHeight )
{
	assert(pD3D != NULL || pDevice != NULL) ;

	m_pD3D = pD3D ;
	g_pDevice = pDevice ;
	m_ClearColor = clearColor ;

	HRESULT hr ;

	// Get display mode
	D3DDISPLAYMODE d3ddm ;
	pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm) ;

	// Initialize D3D presentation parameters
	D3DPRESENT_PARAMETERS d3dpp ;
	ZeroMemory(&d3dpp, sizeof(d3dpp)) ;
	d3dpp.BackBufferCount = 1 ;
	d3dpp.BackBufferWidth = backBufferWidth ;
	d3dpp.BackBufferHeight = backBufferHeight ;
	d3dpp.hDeviceWindow = hWnd ;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE ;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD ; // This parameter must be set!!!
	d3dpp.EnableAutoDepthStencil = FALSE ; // Here must be false!!!, or CreateAdditionalSwapChain will be failed!
	d3dpp.BackBufferFormat = d3ddm.Format ;
	d3dpp.Windowed = TRUE ;
	d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

	// Create swap chain
	hr = pDevice->CreateAdditionalSwapChain(&d3dpp, &m_pSwapChain) ;
	if (FAILED(hr))
	{
		DXTRACE_ERR_MSGBOX("Create additional swap chain failed!", hr) ;
		return ;
	}

	// Initialize back-buffer
	hr = m_pSwapChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &m_pBackbuffer) ;
	if (FAILED(hr))
	{
		DXTRACE_ERR_MSGBOX("Get back buffer failed!", hr) ;
		return ;
	}

	// Create z-buffer
	hr = pDevice->CreateDepthStencilSurface(backBufferWidth, 
		backBufferHeight, 
		D3DFMT_D24S8, 
		D3DMULTISAMPLE_NONE, 
		0, 
		FALSE, 
		&m_pZbuffer, 
		NULL) ;
	if (FAILED(hr))
	{
		DXTRACE_ERR_MSGBOX("Create z-buffer failed!", hr) ;
		return ;
	}

	hr = g_pDevice->SetDepthStencilSurface(m_pZbuffer) ;
	if (FAILED(hr))
	{
		DXTRACE_ERR_MSGBOX("Set depth stecil buffer failed!", hr) ;
		return ;
	}
}

IDirect3DSwapChain9* SwapChain::GetSwapChain() const
{
	return m_pSwapChain ;
}

void SwapChain::SetSwapChain(IDirect3DSwapChain9* swapChain)
{
	m_pSwapChain = swapChain ;
}

D3DCOLOR SwapChain::GetClearColor() const
{
	return m_ClearColor ;
}

void SwapChain::BeginScene()
{
	// Set render target
	HRESULT hr = g_pDevice->SetRenderTarget(0, m_pBackbuffer) ;

	if (FAILED(hr))
	{
		DXTRACE_ERR_MSGBOX("Set render target failed!", hr) ;
		return ;
	}

	hr = g_pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, m_ClearColor, 1.0f, 0) ;
	if (FAILED(hr))
	{
		DXTRACE_ERR_MSGBOX("Clear target failed!", hr) ;
		return ;
	}

	hr = g_pDevice->BeginScene() ;
	if (FAILED(hr))
	{
		DXTRACE_ERR_MSGBOX("Begin Scene failed!", hr) ;
		return ;
	}
}

void SwapChain::EndScene()
{
	HRESULT hr ;
	hr = g_pDevice->EndScene() ;
	if (FAILED(hr))
	{
		DXTRACE_ERR_MSGBOX("End Scene failed!", hr) ;
		return ;
	}
}

void SwapChain::Present()
{
	HRESULT hr ;

	// Wire frame mode
	g_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME) ;

	hr = m_pSwapChain->Present(NULL, NULL, NULL, NULL, NULL) ;
	if (FAILED(hr))
	{
		DXTRACE_ERR_MSGBOX("Present failed!", hr) ;
		return ;
	}
}

void SwapChain::SetPerpectiveMatrix()
{
	D3DSURFACE_DESC d3ddesc ;
	ZeroMemory(&d3ddesc, sizeof(d3ddesc)) ;
	m_pBackbuffer->GetDesc(&d3ddesc) ;
	float fAspect = (float)d3ddesc.Width / d3ddesc.Height ;

	D3DXMATRIX proj ;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI / 4, fAspect, 1.0f, 1000.0f) ;
	g_pDevice->SetTransform(D3DTS_PROJECTION, &proj) ;
}

void SwapChain::SetViewMatrix()
{
	D3DXVECTOR3 eyePt(0, 0, -5.0f ) ;
	D3DXVECTOR3 lookAt(0, 0, 0) ;
	D3DXVECTOR3 upVector(0, 1.0f, 0) ;

	D3DXMATRIX view ;
	D3DXMatrixLookAtLH(&view, &eyePt, &lookAt, &upVector) ;
	g_pDevice->SetTransform(D3DTS_VIEW, &view) ;
}

void SwapChain::SetWorldMatrix()
{
	D3DXMATRIX world ;
	D3DXMatrixTranslation(&world, 0, 0, 0) ;
	g_pDevice->SetTransform(D3DTS_WORLD, &world) ;
}