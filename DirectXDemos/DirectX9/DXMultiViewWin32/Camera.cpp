#include "stdafx.h"
#include "Camera.h"

Camera::Camera(void)
{
	D3DXMatrixIdentity( &m_matWorld );
	D3DXMatrixIdentity( &m_matModelRotate );
	D3DXMatrixIdentity( &m_matModelRotLast );
	D3DXMatrixIdentity( &m_matCameraRotLast );
	m_vModelCenter = D3DXVECTOR3( 0, 0, 0 );
	m_fRadius = 5.0f;			// Distance from the camera to model
	m_fDefaultRadius = 5.0f;	// Default distance from camera to model
	m_nMouseWheelDelta = 0 ;
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

	m_fRadius = m_fDefaultRadius;
	m_WorldArcBall.Reset();
	m_ViewArcBall.Reset();
}

// Update the scene for every frame
void Camera::OnFrameMove()
{
	// Change the radius from the camera to the model based on wheel scrolling
	if(m_nMouseWheelDelta)
		m_fRadius -= m_nMouseWheelDelta * m_fRadius * 0.1f / 120.0f;
	// 这里需要确保m_fRadius在一个范围内，使得Model不会太大或太小
	m_nMouseWheelDelta = 0 ;

	// Get the inverse of the view arcball's rotation matrix
	D3DXMATRIX mCameraRot ;
	D3DXMatrixInverse( &mCameraRot, NULL, m_ViewArcBall.GetRotationMatrix() );
	//D3DXMatrixInverse( &mCameraRot, NULL, m_WorldArcBall.GetRotationMatrix() );


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

	D3DXMATRIX mInvView;
	D3DXMatrixInverse( &mInvView, NULL, &m_matView );
	mInvView._41 = mInvView._42 = mInvView._43 = 0;

	D3DXMATRIX mModelLastRotInv;
	D3DXMatrixInverse( &mModelLastRotInv, NULL, &m_matModelRotLast );

	// Accumulate the delta of the arc ball's rotation in view space.
	// Note that per-frame delta rotations could be problematic over long periods of time.
	D3DXMATRIX mModelRot;
	mModelRot = *m_WorldArcBall.GetRotationMatrix();
	m_matModelRotate *= m_matView * mModelLastRotInv * mModelRot * mInvView;

	m_matCameraRotLast = mCameraRot;
	m_matModelRotLast = mModelRot;

	// Since we're accumulating delta rotations, we need to orthonormalize 
	// the matrix to prevent eventual matrix skew
	D3DXVECTOR3* pXBasis = ( D3DXVECTOR3* )&m_matModelRotate._11;
	D3DXVECTOR3* pYBasis = ( D3DXVECTOR3* )&m_matModelRotate._21;
	D3DXVECTOR3* pZBasis = ( D3DXVECTOR3* )&m_matModelRotate._31;
	D3DXVec3Normalize( pXBasis, pXBasis );
	D3DXVec3Cross( pYBasis, pZBasis, pXBasis );
	D3DXVec3Normalize( pYBasis, pYBasis );
	D3DXVec3Cross( pZBasis, pXBasis, pYBasis );

	// Translate the rotation matrix to the same position as the lookAt position
	m_matModelRotate._41 = m_vLookatPt.x;
	m_matModelRotate._42 = m_vLookatPt.y;
	m_matModelRotate._43 = m_vLookatPt.z;

	// Translate world matrix so its at the center of the model
	D3DXMATRIX mTrans;
	D3DXMatrixTranslation( &mTrans, -m_vModelCenter.x, -m_vModelCenter.y, -m_vModelCenter.z );
	m_matWorld = mTrans * m_matModelRotate;
}

LRESULT Camera::HandleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// Update model world arc ball
	if(uMsg == WM_LBUTTONDOWN)
	{
		int iMouseX = (short)LOWORD(lParam) ;
		int iMouseY = (short)HIWORD(lParam) ;
		m_WorldArcBall.OnBegin(iMouseX, iMouseY) ;
	}

	// update view arc ball
	if(uMsg == WM_RBUTTONDOWN)
	{
		int iMouseX = (short)LOWORD(lParam) ;
		int iMouseY = (short)HIWORD(lParam) ;
		m_ViewArcBall.OnBegin(iMouseX, iMouseY) ;
	}

	// mouse move
	if(uMsg == WM_MOUSEMOVE)
	{
		int iMouseX = ( short )LOWORD( lParam );
		int iMouseY = ( short )HIWORD( lParam );
		m_WorldArcBall.OnMove(iMouseX, iMouseY) ;
		m_ViewArcBall.OnMove(iMouseX, iMouseY) ;
	}

	// left button up, terminate world ball rotation
	if(uMsg == WM_LBUTTONUP)
	{
		m_WorldArcBall.OnEnd();
	}

	// right button up, terminate view arc ball rotation
	if(uMsg == WM_RBUTTONUP)
	{
		m_ViewArcBall.OnEnd();
	}

	// Mouse wheel, zoom in/out
	if(uMsg == WM_MOUSEWHEEL) 
	{
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
}

void Camera::SetProjParams(float fFOV, float fAspect, float fNearPlane, float fFarPlane)
{
	m_fFOV			= fFOV ;
	m_fAspect		= fAspect ;
	m_fNearPlane	= fNearPlane ;
	m_fFarPlane		= fFarPlane ;

	D3DXMatrixPerspectiveFovLH(&m_matProj, fFOV, fAspect, fNearPlane, fFarPlane) ;
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

void Camera::SetWorldMatrix(const D3DXMATRIX *matWorld)
{
	m_matWorld = *matWorld ;
}

void Camera::SetViewMatrix(const D3DXMATRIX* matView)
{
	m_matView = *matView ;
}

void Camera::SetProjectionMatrix(const D3DXMATRIX* matProj)
{
	m_matWorld = *matProj ;
}