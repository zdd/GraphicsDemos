#ifndef FPS_H
#define FPS_H

#include <d3dx9.h>

#define SAFE_RELEASE(P) if(P){ P->Release(); P = NULL;}

class FPS
{
public:
	FPS(void);
	~FPS(void);

	bool Init(LPDIRECT3DDEVICE9 pd3dDevice) ;
	void Show(POINT position, float timeDelta) ;
	void Release() ;

private:
	ID3DXFont* m_Font ;	// font
	WCHAR* m_Buffer ;	// buffer to hold the FPS text
	int m_FrameCount ;	// frame counter
	float m_timeCount ;	// timer for current round
};

#endif

