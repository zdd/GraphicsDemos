#include "BaseCamera.h"

BaseCamera::BaseCamera(void)
{
	D3DXMatrixIdentity( &m_matWorld );
}

BaseCamera::~BaseCamera(void)
{
}

void BaseCamera::SetWorldMatrix(D3DXMATRIX* matWorld)
{
	m_matWorld = *matWorld ;
}

void BaseCamera::SetViewParams( D3DXVECTOR3* pvEyePt, D3DXVECTOR3* pvLookatPt, D3DXVECTOR3* pvUp)
{
	m_vEyePoint	= *pvEyePt ;
	m_vLookAt = *pvLookatPt ;
	m_vUpVector		= *pvUp ;

	D3DXMatrixLookAtLH(&m_matView, &m_vEyePoint, &m_vLookAt, &m_vUpVector) ;
}

void BaseCamera::SetProjParams(float fFOV, float fAspect, float fNearPlane, float fFarPlane)
{
	m_fFOV			= fFOV ;
	m_fAspect		= fAspect ;
	m_fNearPlane	= fNearPlane ;
	m_fFarPlane		= fFarPlane ;

	D3DXMatrixPerspectiveFovLH(&m_matProj, fFOV, fAspect, fNearPlane, fFarPlane) ;
}

const D3DXMATRIX* BaseCamera::GetWorldMatrix() const
{
	return &m_matWorld ;
}

const D3DXMATRIX* BaseCamera::GetViewMatrix() const
{
	return &m_matView ;
}

const D3DXMATRIX* BaseCamera::GetProjMatrix() const
{
	return &m_matProj ;
}

// is camera position the equivalent to eye point?
void BaseCamera::SetPosition(D3DXVECTOR3 *vPos)
{
	m_vEyePoint = *vPos ;
}

// Get camera position
const D3DXVECTOR3* BaseCamera::GetPosition() const
{
	return &m_vEyePoint ;
}

// Get look at position
const D3DXVECTOR3* BaseCamera::GetLookAt() const
{
	return &m_vLookAt ;
}

// Map the virtual key code to camera key
const CAMERA_KEYS BaseCamera::MapKey(UINT key) const
{
	switch(key)
	{
	case 'A':
	case VK_LEFT:
		return MOVE_LEFT ;

	case 'D':
	case VK_RIGHT:
		return MOVE_RIGHT ;

	case 'W':
	case VK_UP:
		return MOVE_FORWARD ;

	case 'S':
	case VK_DOWN:
		return MOVE_BACKFORWARD ;

	case VK_SPACE:
		return MOVE_UP ;

	default:
		return UNKOWN ;
	}
}