#include "Camera.h"

Camera::Camera(void)
{
	D3DXMatrixIdentity( &m_matWorld );
	D3DXMatrixIdentity( &m_matModelRotate );
	D3DXMatrixIdentity( &m_matModelRotLast );
	D3DXMatrixIdentity( &m_matCameraRotLast );
	m_vModelCenter = D3DXVECTOR3( 0, 0, 0 );
	m_fRadius = 100.0f;			
	m_fMaxRadius = 150.0f;
	m_fMinRadius = 50.0f ;
	m_nMouseWheelDelta = 0 ;
	m_bDragSinceLastUpdate = false ;
}

Camera::~Camera(void)
{
}

void Camera::Reset()
{
	D3DXMatrixIdentity(&m_matWorld) ;
	D3DXMatrixIdentity(&m_matModelRotate) ;
	D3DXMatrixIdentity(&m_matModelRotLast) ;
	D3DXMatrixIdentity( &m_matCameraRotLast );

	m_bDragSinceLastUpdate = false ;
	m_WorldArcBall.Reset();
	m_ViewArcBall.Reset();
}

// Update the scene for every frame
void Camera::OnFrameMove()
{
	// No need to handle if no drag since last frame move
	if(!m_bDragSinceLastUpdate)
		return ;

	m_bDragSinceLastUpdate = false ;

	if(m_nMouseWheelDelta)
	{
		m_fRadius -= m_nMouseWheelDelta * m_fRadius * 0.1f / 120.0f;

		// Make the radius in range of [m_fMinRadius, m_fMaxRadius]
		// This can Prevent the cube became too big or too small
		m_fRadius = max(m_fRadius, m_fMinRadius) ;
		m_fRadius = min(m_fRadius, m_fMaxRadius) ;
	}

	// The mouse delta is retrieved IN every WM_MOUSE message and do not accumulate, so clear it after one frame
	m_nMouseWheelDelta = 0 ;

	// Get the inverse of the view Arcball's rotation matrix
	D3DXMATRIX mCameraRot ;
	D3DXMatrixInverse( &mCameraRot, NULL, m_ViewArcBall.GetRotationMatrix() );

	// Transform vectors based on camera's rotation matrix
	D3DXVECTOR3 vWorldUp;
	D3DXVECTOR3 vLocalUp = D3DXVECTOR3( 0, 1, 0 );
	D3DXVec3TransformCoord( &vWorldUp, &vLocalUp, &mCameraRot );

	D3DXVECTOR3 vWorldAhead;
	D3DXVECTOR3 vLocalAhead = D3DXVECTOR3( 0, 0, 1 );
	D3DXVec3TransformCoord( &vWorldAhead, &vLocalAhead, &mCameraRot );

	// Update the eye point based on a radius away from the lookAt position
	m_vEyePt = m_vLookatPt - vWorldAhead * m_fRadius;

	// Update the view matrix
	D3DXMatrixLookAtLH( &m_matView, &m_vEyePt, &m_vLookatPt, &vWorldUp );
}

// This function is used to handling the mouse message for the view arc ball
// include the right button message, the left button message is handled in the game window message process function
LRESULT Camera::HandleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// update view arc ball
	if(uMsg == WM_RBUTTONDOWN)
	{
		SetCapture(hWnd) ;

		m_bDragSinceLastUpdate = true ;
		int iMouseX = (short)LOWORD(lParam) ;
		int iMouseY = (short)HIWORD(lParam) ;
		m_ViewArcBall.OnBegin(iMouseX, iMouseY) ;
	}

	// mouse move
	if(uMsg == WM_MOUSEMOVE)
	{
		m_bDragSinceLastUpdate = true ;
		int iMouseX = ( short )LOWORD( lParam );
		int iMouseY = ( short )HIWORD( lParam );
		//m_WorldArcBall.OnMove(iMouseX, iMouseY) ;
		m_ViewArcBall.OnMove(iMouseX, iMouseY) ;
	}

	// right button up, terminate view arc ball rotation
	if(uMsg == WM_RBUTTONUP)
	{
		m_bDragSinceLastUpdate = true ;
		m_ViewArcBall.OnEnd();
		ReleaseCapture() ;
	}

	// Mouse wheel, zoom in/out
	if(uMsg == WM_MOUSEWHEEL) 
	{
		m_bDragSinceLastUpdate = true ;
		m_nMouseWheelDelta += ( short )HIWORD( wParam );
	}

	return TRUE ;
}

void Camera::SetViewParams( D3DXVECTOR3* pvEyePt, D3DXVECTOR3* pvLookatPt, D3DXVECTOR3* pvUp)
{
	m_vEyePt	= *pvEyePt ;
	m_vLookatPt = *pvLookatPt ;
	m_vUp		= *pvUp ;

	D3DXMatrixLookAtLH(&m_matView, pvEyePt, pvLookatPt, pvUp) ;
	m_bDragSinceLastUpdate = true ;
}

void Camera::SetProjParams(float fFOV, float fAspect, float fNearPlane, float fFarPlane)
{
	m_fFOV			= fFOV ;
	m_fAspect		= fAspect ;
	m_fNearPlane	= fNearPlane ;
	m_fFarPlane		= fFarPlane ;

	D3DXMatrixPerspectiveFovLH(&m_matProj, fFOV, fAspect, fNearPlane, fFarPlane) ;
	m_bDragSinceLastUpdate = true ;
}

void Camera::SetWindow(int nWidth, int nHeight, float fArcballRadius)
{
	m_WorldArcBall.SetWindow(nWidth, nHeight, fArcballRadius) ;
	m_ViewArcBall.SetWindow(nWidth, nHeight, fArcballRadius) ;
}

const D3DXMATRIX* Camera::GetWorldMatrix() const
{
	return &m_matWorld ;
}

const D3DXMATRIX* Camera::GetViewMatrix() const
{
	return &m_matView ;
}

const D3DXMATRIX* Camera::GetProjMatrix() const
{
	return &m_matProj ;
}

const D3DXVECTOR3* Camera::GetEyePoint() const
{
	return &m_vEyePt ;
}

void Camera::SetWorldMatrix(D3DXMATRIX *matModelWorld)
{
	m_matWorld = *matModelWorld ;
	m_bDragSinceLastUpdate = true ;
}

D3DXQUATERNION Camera::GetRotationQuat()
{
	return m_WorldArcBall.GetRotationQuat() ;
}

const D3DXMATRIX* Camera::GetModelRotMatrix() const
{
	return &m_matModelRotate ;
}