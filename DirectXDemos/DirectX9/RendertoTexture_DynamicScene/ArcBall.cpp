#include "ArcBall.h"

ArcBall::ArcBall(void)
{
	Reset() ;
	m_vDownPt = D3DXVECTOR3( 0, 0, 0 ) ;
	m_vCurrentPt = D3DXVECTOR3( 0, 0, 0 ) ;
	m_Offset.x = 0 ;
	m_Offset.y = 0 ;
}

ArcBall::~ArcBall(void)
{
}

void ArcBall::Reset()
{
	D3DXQuaternionIdentity( &m_qDown );
	D3DXQuaternionIdentity( &m_qNow );
	D3DXMatrixIdentity( &m_mRotation );
	m_bDrag = FALSE;
	m_fRadius = 1.0f;

	RECT rc ;
	GetClientRect(GetForegroundWindow(), &rc) ;
	SetWindow(rc.right, rc.bottom) ;
}

void ArcBall::OnBegin(int nX, int nY)
{
	// enter drag state only if user click the window's client area
	if( nX >= m_Offset.x && 
		nX <= m_Offset.x + m_nWidth &&
		nY >= m_Offset.y &&
		nY < m_Offset.y + m_nHeight )
	{
		m_bDrag = true ; // begin drag state
		m_qDown = m_qNow ;
		m_vDownPt = ScreenToVector((float)nX, (float)nY) ;
	}
}

void ArcBall::OnMove(int nX, int nY)
{
	if(m_bDrag)
	{
		m_vCurrentPt = ScreenToVector((float)nX, (float)nY) ;
		m_qNow = m_qDown * QuatFromBallPoints( m_vDownPt, m_vCurrentPt ) ;
	}
}

void ArcBall::OnEnd()
{
	m_bDrag = false ;
}

void ArcBall::SetOffset( INT nX, INT nY )
{
	m_Offset.x = nX ; 
	m_Offset.y = nY ;
}

void ArcBall::SetWindow( int nWidth, int nHeight, float fRadius)
{
	 m_nWidth = nWidth; 
	 m_nHeight = nHeight; 
	 m_fRadius = fRadius; 
	 m_vCenter = D3DXVECTOR2(m_nWidth / 2.0f, m_nHeight / 2.0f);
}

const D3DXMATRIX* ArcBall::GetRotationMatrix()
{
	return D3DXMatrixRotationQuaternion(&m_mRotation, &m_qNow) ;
}

D3DXQUATERNION ArcBall::QuatFromBallPoints(const D3DXVECTOR3 &vFrom, const D3DXVECTOR3 &vTo)
{
	D3DXVECTOR3 vPart;
	float fDot = D3DXVec3Dot( &vFrom, &vTo );	// rotation angle
	D3DXVec3Cross( &vPart, &vFrom, &vTo );		// rotation axis

	return D3DXQUATERNION( vPart.x, vPart.y, vPart.z, fDot );
}

D3DXVECTOR3 ArcBall::ScreenToVector(float fScreenPtX, float fScreenPtY)
{
	// Scale to screen
	// x值为负，是因为DirectX使用左手系
	FLOAT x = -( fScreenPtX - m_Offset.x - m_nWidth / 2 ) / ( m_fRadius * m_nWidth / 2 );
	FLOAT y = ( fScreenPtY - m_Offset.y - m_nHeight / 2 ) / ( m_fRadius * m_nHeight / 2 );

	FLOAT z = 0.0f;
	FLOAT mag = x * x + y * y;

	if( mag > 1.0f )
	{
		FLOAT scale = 1.0f / sqrtf( mag );
		x *= scale;
		y *= scale;
	}
	else
		z = sqrtf( 1.0f - mag );

	// Return vector
	return D3DXVECTOR3( x, y, z );
}