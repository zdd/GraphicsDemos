#pragma once
#include <d3dx9.h>

class Font
{
public:
	Font(LPDIRECT3DDEVICE9 pd3dDevice);
	void Draw(POINT position, char* text, D3DXCOLOR color);
	void Release() ;
	~Font(void);

private:
	bool SetupFont(int fontHeight, int fontWidth, LPCTSTR fontType) ;
	LPDIRECT3DDEVICE9		m_pd3dDevice ;
	ID3DXFont*				m_pFont ;
};
