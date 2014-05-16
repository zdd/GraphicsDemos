#include "Camera.h"

Camera* gCamera = 0;

Camera::Camera()
{
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);

	m_vEyePoint   = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vRight = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	m_vUp    = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_vForward  = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

	// Client should adjust to a value that makes sense for application's
	// unit scale, and the object the camera is attached to--e.g., car, jet,
	// human walking, etc.
	mSpeed  = 1.0f;

	mDXInput = new DXInput() ;
}

const D3DXMATRIX& Camera::GetViewMatrix() const
{
	return m_matView;
}

const D3DXMATRIX& Camera::GetProjMatrix() const
{
	return m_matProj;
}

void Camera::SetViewParams(D3DXVECTOR3& pos, D3DXVECTOR3& target, D3DXVECTOR3& up)
{
	// the three code block below make sure that L, R and U are orthogonal to each other
	D3DXVECTOR3 L = target - pos;
	D3DXVec3Normalize(&L, &L);

	D3DXVECTOR3 R;
	D3DXVec3Cross(&R, &up, &L);
	D3DXVec3Normalize(&R, &R);

	D3DXVECTOR3 U;
	D3DXVec3Cross(&U, &L, &R);
	D3DXVec3Normalize(&U, &U);

	m_vEyePoint   = pos;
	m_vRight = R;
	m_vUp    = U;
	m_vForward  = L;

	BuildViewMatrix();
}

void Camera::SetProjParams(float fov, float aspect, float nearZ, float farZ)
{
	D3DXMatrixPerspectiveFovLH(&m_matProj, fov, aspect, nearZ, farZ);
}

void Camera::Update(float dt, float offsetHeight)
{
	mDXInput->Update() ;

	// Find the net direction the camera is traveling in (since the
	// camera could be running and strafing).
	D3DXVECTOR3 dir(0.0f, 0.0f, 0.0f);

	if( mDXInput->KeyDown(DIK_W) )
		dir += m_vForward;
	if( mDXInput->KeyDown(DIK_S) )
		dir -= m_vForward;
	if( mDXInput->KeyDown(DIK_D) )
		dir += m_vRight;
	if( mDXInput->KeyDown(DIK_A) )
		dir -= m_vRight;

	// Move at mSpeed along net direction.
	D3DXVec3Normalize(&dir, &dir);
	D3DXVECTOR3 newPos = m_vEyePoint + dir * mSpeed * dt;
	
	m_vEyePoint = newPos ;
	if (m_vEyePoint.y < 1.0f)
	{
		m_vEyePoint.y = 1.0f ;
	}
	if (m_vEyePoint.y > 2.0f)
	{
		m_vEyePoint.y = 2.0f ;
	}

	// We rotate at a fixed speed.
	float pitch  = mDXInput->MouseDY() / 360.0f;
	float yAngle = mDXInput->MouseDX() / 360.0f;

	// Rotate camera's look and up vectors around the camera's right vector.
	D3DXMATRIX R;
	D3DXMatrixRotationAxis(&R, &m_vRight, pitch);
	D3DXVec3TransformCoord(&m_vForward, &m_vForward, &R);
	D3DXVec3TransformCoord(&m_vUp, &m_vUp, &R);

	// Rotate camera axes about the world's y-axis.
	D3DXMatrixRotationY(&R, yAngle);
	D3DXVec3TransformCoord(&m_vRight, &m_vRight, &R);
	D3DXVec3TransformCoord(&m_vForward, &m_vForward, &R);

	// Rebuild the view matrix to reflect changes.
	BuildViewMatrix();
}

void Camera::BuildViewMatrix()
{
	// Keep camera's axes orthogonal to each other and of unit length.
	D3DXVec3Normalize(&m_vForward, &m_vForward);

	D3DXVec3Cross(&m_vUp, &m_vForward, &m_vRight);
	D3DXVec3Normalize(&m_vUp, &m_vUp);

	D3DXVec3Cross(&m_vRight, &m_vUp, &m_vForward);
	D3DXVec3Normalize(&m_vRight, &m_vRight);

	// Fill in the view matrix entries.

	float x = -D3DXVec3Dot(&m_vEyePoint, &m_vRight);
	float y = -D3DXVec3Dot(&m_vEyePoint, &m_vUp);
	float z = -D3DXVec3Dot(&m_vEyePoint, &m_vForward);

	m_matView(0,0) = m_vRight.x; 
	m_matView(1,0) = m_vRight.y; 
	m_matView(2,0) = m_vRight.z; 
	m_matView(3,0) = x;   

	m_matView(0,1) = m_vUp.x;
	m_matView(1,1) = m_vUp.y;
	m_matView(2,1) = m_vUp.z;
	m_matView(3,1) = y;  

	m_matView(0,2) = m_vForward.x; 
	m_matView(1,2) = m_vForward.y; 
	m_matView(2,2) = m_vForward.z; 
	m_matView(3,2) = z;   

	m_matView(0,3) = 0.0f;
	m_matView(1,3) = 0.0f;
	m_matView(2,3) = 0.0f;
	m_matView(3,3) = 1.0f;
}

const D3DXVECTOR3 Camera::GetLookAt() const
{
	return m_vForward ;
}

const D3DXVECTOR3 Camera::GetEyePoint() const
{
	return m_vEyePoint ;
}

const D3DXVECTOR3 Camera::GetUpVector() const
{
	return m_vUp ;
}