#include "FPS.h"

FPS::FPS(void)
{
	m_Font = NULL ;

	m_Buffer = new WCHAR[10] ;
	memset(m_Buffer, 0, sizeof(m_Buffer) * 4) ;

	m_FrameCount = 0 ;
	m_timeCount = 0 ;
}

FPS::~FPS(void)
{
	Release() ;
}

bool FPS::Init( LPDIRECT3DDEVICE9 pd3dDevice)
{
	// Create font
	if( S_OK != D3DXCreateFont(pd3dDevice,
		20,
		10,
		5,
		1,
		FALSE,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, 
		DEFAULT_QUALITY, 
		DEFAULT_PITCH | FF_DONTCARE,
		"Aria", 
		&m_Font ))
		return false ;

	return S_OK ;
}

void FPS::Release()
{
	delete []m_Buffer ;
	m_Buffer = NULL ;

	SAFE_RELEASE(m_Font) ;
}

void FPS::Show(POINT position, float timeDelta)
{
	++m_FrameCount ;
	m_timeCount += timeDelta ;

	if (m_timeCount >= 1.0f)
	{
		errno_t result = _itow_s( m_FrameCount, m_Buffer, sizeof(m_Buffer), 10 ) ; 
		if( result ) // if _itow_s is successful, the return value is zero.
			throw "Convert integer to string failed!" ;

		m_FrameCount = 0 ;
		m_timeCount = 0 ;
	}

	LONG left	= position.x ;
	LONG top	= position.y ;
	LONG right	= position.x + 200 ;
	LONG bottom	= position.y + 50 ;

	RECT rc = { left, top, right, bottom } ;

	m_Font->DrawTextW(NULL, m_Buffer, -1, &rc, DT_TOP|DT_LEFT|DT_SINGLELINE, 0xffff0000) ;
}