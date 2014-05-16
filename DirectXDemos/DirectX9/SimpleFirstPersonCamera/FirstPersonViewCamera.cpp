#include "FirstPersonViewCamera.h"

FirstPersonViewCamera::FirstPersonViewCamera(void)
{
	// By default, we set the camera position to the origin of word space
	m_vPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f) ;

	// Set right vector parallel to x-axis
	m_vRight = D3DXVECTOR3(1.0f, 0.0f, 0.0f) ;

	// Set up vector parallel to y-axis
	m_vUp = D3DXVECTOR3(0.0, 1.0f, 0.0f) ;

	// Set look vector parallel to z-axis
	m_vLook = D3DXVECTOR3(0.0f, 0.0f, 1.0f) ;
}

FirstPersonViewCamera::~FirstPersonViewCamera(void)
{
	// We don't have any resource to release, just leave the destructor as blank
}

// viewMatrix is the out parameter
void FirstPersonViewCamera::CalcViewMatrix(D3DXMATRIX *v)
{
	// Initially, in the constructor, we made m_vRight, m_vUp and m_vLook orthogonal to each other
	// and we won't change them in the class, so we didn't need the code below, if we build up a aircraft
	// camera, then we need to change m_vRight, m_vUp and m_vLook, which of cause, we need the following code

	/*D3DXVec3Normalize(&m_vLook, &m_vLook);

	D3DXVec3Cross(&m_vUp, &m_vLook, &m_vRight);
	D3DXVec3Normalize(&m_vUp, &m_vUp);

	D3DXVec3Cross(&m_vRight, &m_vUp, &m_vLook);
	D3DXVec3Normalize(&m_vRight, &m_vRight);*/

	float x = -D3DXVec3Dot(&m_vRight, &m_vPos) ;
	float y = -D3DXVec3Dot(&m_vUp, &m_vPos) ;
	float z = -D3DXVec3Dot(&m_vLook, &m_vPos) ;

	v->_11 = m_vRight.x; v->_12 = m_vUp.x; v->_13 = m_vLook.x; v->_14 = 0.0f ;
	v->_21 = m_vRight.y; v->_22 = m_vUp.y; v->_23 = m_vLook.y; v->_24 = 0.0f ;
	v->_31 = m_vRight.z; v->_32 = m_vUp.z; v->_33 = m_vLook.z; v->_34 = 0.0f ;
	v->_41 =         x ; v->_42 =	   y ; v->_43 =		   z ; v->_44 = 1.0f ;
}

void FirstPersonViewCamera::MoveForward(float unit)
{
	m_vPos.z += unit ;
}

void FirstPersonViewCamera::MoveBack(float unit)
{
	m_vPos.z -= unit ;
}

void FirstPersonViewCamera::MoveLeft(float unit)
{
	m_vPos.x -= unit ;
}

void FirstPersonViewCamera::MoveRight(float unit)
{
	m_vPos.x += unit ;
}

void FirstPersonViewCamera::Restore()
{
	m_vPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f) ;
}