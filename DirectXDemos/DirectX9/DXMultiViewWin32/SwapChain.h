#ifndef SWAP_CHAIN
#define SWAP_CHAIN

#include <d3dx9.h>
#include <DxErr.h>

class SwapChain
{
public:
	SwapChain(void);
	~SwapChain(void);
	void Create(LPDIRECT3D9 pD3D, 
		LPDIRECT3DDEVICE9 pDevice,  
		HWND  hWnd, 
		D3DCOLOR clearColor, 
		bool enableZbuffer, 
		int backBufferWidth, 
		int backBufferHeight) ;
	void BeginScene() ;
	void EndScene() ;
	void Present() ;
	void SetWorldMatrix() ;
	void SetViewMatrix() ;
	void SetPerpectiveMatrix() ;
	IDirect3DSwapChain9* GetSwapChain() const ;
	void SetSwapChain(IDirect3DSwapChain9* swapChain) ;
	IDirect3DSurface9* GetBackBuffer() const ;
	IDirect3DSurface9* GetZbuffer() const ;
	D3DCOLOR GetClearColor() const ;

private:
	LPDIRECT3D9				m_pD3D ;
	LPDIRECT3DDEVICE9		g_pDevice ;
	IDirect3DSurface9*		m_pBackbuffer ;
	IDirect3DSurface9*		m_pZbuffer ;
	IDirect3DSwapChain9*	m_pSwapChain ;
	D3DCOLOR				m_ClearColor ;
};

#endif // SWAP_CHAIN		