#ifndef __ARCBALL_H__
#define __ARCBALL_H__
#include <Windows.h>
#include "d3dx9.h"
class ArcBall
{
public:
	ArcBall(void);
	~ArcBall(void);

public:
	void	Reset() ; // Reset arc ball
	void	OnBegin(int nX, int nY) ;
	void	OnMove(int nX, int nY) ;
	void	OnEnd() ;

	// why here need the call type WINAPI ?
	static D3DXQUATERNION WINAPI QuatFromBallPoints( const D3DXVECTOR3& vFrom, const D3DXVECTOR3& vTo );
	const D3DXMATRIX* GetRotationMatrix() ;
	void SetOffset( INT nX, INT nY ) ;
	void SetWindow( int nWidth, int nHeight, float fRadius = 0.9f) ;
	LRESULT HandleMessages( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) ;

private:
	POINT	m_Offset ;	// window offset
	int		m_nWidth ;	// arc ball's window width
	int		m_nHeight ; // arc ball's window height
	float	m_fRadius ;	// arc ball's radius in screen coordinates
	bool	m_bDrag ;	// whether the arc ball is dragged

	D3DXVECTOR2		m_vCenter ;	// center of arc ball
	D3DXQUATERNION	m_qDown ;	// quaternion before mouse down
	D3DXQUATERNION	m_qNow ;		// current quaternion
	D3DXVECTOR3		m_vDownPt ;		// starting point of arc ball rotate
	D3DXVECTOR3		m_vCurrentPt ;	// current point of arc ball rotate
	D3DXVECTOR3		ScreenToVector(float fScreenPtX, float fScreenPtY) ;

	D3DXMATRIXA16 m_mRotation;
};

#endif // end __ARCBALL_H__