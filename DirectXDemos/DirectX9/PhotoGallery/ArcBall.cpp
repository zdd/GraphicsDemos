#include "ArcBall.h"

ArcBall::ArcBall(void)
{
	Reset() ;
	m_vDownPt = D3DXVECTOR3( 0, 0, 0 ) ;
	m_vCurrentPt = D3DXVECTOR3( 0, 0, 0 ) ;
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

// Begin drag
// When user press the mouse button in window's client are, the drag is begin
void ArcBall::OnBegin(int nX, int nY)
{
	// enter drag state only if user click the window's client area
	if( nX >= m_Offset.x && 
		nX <= m_Offset.x + m_nWidth &&
		nY >= m_Offset.y &&
		nY < m_Offset.y + m_nHeight )
	{
		// begin drag state
		m_bDrag = true ; 

		// Update the previous quaternion to current quaternion
		m_qDown = m_qNow ;

		// Calculate the arcball vector based on cursor position
		m_vDownPt = ScreenToVector((float)nX, (float)nY) ;
	}
}

void ArcBall::OnMove(int nX, int nY)
{
	// Only tack action when mouse button is down
	if(m_bDrag)
	{
		// Calculate the arcball vector for current cursor position
		m_vCurrentPt = ScreenToVector((float)nX, (float)nY) ;

		// Calculate the rotation quaternion
		// This quaternion represent the total rotation from mouse button down till now
		m_qNow = m_qDown * QuatFromBallPoints( m_vDownPt, m_vCurrentPt ) ;
	}
}

// End drag
// When user release the mouse button, the drag is end
void ArcBall::OnEnd()
{
	m_bDrag = false ;
}

// Attach the arcball to a window, we need window size to calculate the arcball vector
void ArcBall::SetWindow( int nWidth, int nHeight, float fRadius)
{
	 m_nWidth = nWidth; 
	 m_nHeight = nHeight; 
	 m_fRadius = fRadius; 
	 m_vCenter = D3DXVECTOR2(m_nWidth / 2.0f, m_nHeight / 2.0f);
}

// Calculate the rotation matrix
const D3DXMATRIX* ArcBall::GetRotationMatrix()
{
	return D3DXMatrixRotationQuaternion(&m_mRotation, &m_qNow) ;
}

// Calculate the rotation quaternion 
// Suppose a quaternion q(x, y, z, w) and a axis-angle(x, y, z, a), then the mapping betweem them is
// q.x = axis.x * sin(a/2) ;
// q.y = axis.y * sin(a/2) ;
// q.z = axis.z * sin(a/2) ;
// q.w = cos(a/2)
D3DXQUATERNION ArcBall::QuatFromBallPoints(const D3DXVECTOR3 &vFrom, const D3DXVECTOR3 &vTo)
{
	D3DXVECTOR3 vPart;

	// Note that, both vFrom and vTo are unit vectors
	float fDot = D3DXVec3Dot( &vFrom, &vTo );	// angle part
	D3DXVec3Cross( &vPart, &vFrom, &vTo );		// axis part

	// Based on the formula above, the quaternion represent a rotation that
	// the axis is: x = vPart.x / sinarccos(fDot), y = vPart.y / sinarccos(fDot), z = vPart.z / sinarccos(fDot)
	// the angle is: 2arccos(fDot) ;
	return D3DXQUATERNION( vPart.x, vPart.y, vPart.z, fDot );
}

// Convert the screen point to vector on arcball
D3DXVECTOR3 ArcBall::ScreenToVector(float fScreenPtX, float fScreenPtY)
{
	// 
	FLOAT x = -( fScreenPtX - m_nWidth / 2 ) / ( m_fRadius * m_nWidth / 2 );
	FLOAT y = ( fScreenPtY - m_nHeight / 2 ) / ( m_fRadius * m_nHeight / 2 );

	FLOAT z = 0.0f;
	FLOAT mag = x * x + y * y;

	// Calculate z, if x^2 + y^2 >= 1, then z = 0
	if( mag > 1.0f )
	{
		FLOAT scale = 1.0f / sqrtf( mag );
		x *= scale;
		y *= scale;
	}
	else
		z = sqrtf( 1.0f - mag );

	// After above computation code, x, y, z became a vector on the surface of the arcball
	return D3DXVECTOR3( x, y, z );
}