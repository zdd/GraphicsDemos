#ifndef ARCBALL_H
#define ARCBALL_H

#include <Windows.h>
#include <d3dx9.h>

// This class will work with Camera class to implemented the rotation, zoom functionality for a model
class ArcBall
{
public:
	ArcBall(void);
	~ArcBall(void);

public:
	void Reset() ;
	void OnBegin(int nX, int nY) ;	// Begin drag arcball
	void OnMove(int nX, int nY) ;	// Dragging arcball
	void OnEnd() ;					// End drag
	void SetWindow( int nWidth, int nHeight, float fRadius = 0.9f) ;
	D3DXQUATERNION QuatFromBallPoints( const D3DXVECTOR3& vFrom, const D3DXVECTOR3& vTo );
	const D3DXMATRIX* GetRotationMatrix() ;

private:
	POINT	m_Offset ;	// window offset
	int		m_nWidth ;	// window width
	int		m_nHeight ; // window height
	float	m_fRadius ;	// Arcball radius in screen coordinates
	bool	m_bDrag ;	// User drag the arcball?

	D3DXVECTOR2		m_vCenter ;		// Center of arcball
	D3DXVECTOR3		m_vDownPt ;		// Starting point of arc ball rotation
	D3DXVECTOR3		m_vCurrentPt ;	// Current point of arc ball rotation
	D3DXQUATERNION	m_qDown ;		// Quaternion before mouse down
	D3DXQUATERNION	m_qNow ;		// Current quaternion
	D3DXMATRIX		m_mRotation ;	// Rotation matrix when user drag the arcball
	D3DXVECTOR3		ScreenToVector(float fScreenPtX, float fScreenPtY) ;

};

#endif // end ARCBALL_H