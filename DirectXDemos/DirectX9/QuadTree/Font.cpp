#include "Font.h"

Font::Font(LPDIRECT3DDEVICE9 pd3dDevice)
{
	m_pd3dDevice = pd3dDevice ;
	m_pFont = NULL ;

	if (FAILED(SetupFont(20, 10, "Aria")))
	{
		MessageBox(NULL, "Setup Font failed!", "Error", 0) ;
		return ;
	}
}

Font::~Font(void)
{
	Release() ;
}

void Font::Draw( POINT position, char* text, D3DXCOLOR color)
{
	LONG left	= position.x ;
	LONG top	= position.y ;
	LONG right	= position.x + 200 ;
	LONG bottom	= position.y + 50 ;

	RECT rc = { left, top, right, bottom } ;

	m_pFont->DrawTextA(NULL, text, -1, &rc, DT_TOP|DT_LEFT|DT_SINGLELINE, color) ;
}

bool Font::SetupFont(int fontHeight, int fontWidth, LPCTSTR fontType)
{
	if( S_OK != D3DXCreateFont(m_pd3dDevice,
		fontHeight,
		fontHeight,
		5,
		1,
		FALSE,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, 
		DEFAULT_QUALITY, 
		DEFAULT_PITCH | FF_DONTCARE,
		fontType, 
		&m_pFont ))
		return false ;

	return S_OK ;
}

void Font::Release()
{
	m_pd3dDevice = NULL ;

	if (m_pFont != NULL)
	{
		m_pFont->Release() ;
		m_pFont = NULL ;
	}
}