#include "Camera.h"

Camera::Camera(void)
{
	m_vPos = D3DXVECTOR3(0.0f, 0.5f, -5.0f) ;
	m_vRight = D3DXVECTOR3(1.0f, 0.0f, 0.0f) ;
	m_vUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f) ; 
	m_vLook = D3DXVECTOR3(0.0f, 0.0f, 1.0f) ;
}

Camera::~Camera(void)
{
}

void Camera::GetViewMatrix(D3DXMATRIX *view)
{
	// Keep camera's axes orthogonal to each other:
	D3DXVec3Normalize(&m_vLook, &m_vLook);

	D3DXVec3Cross(&m_vUp, &m_vLook, &m_vRight);
	D3DXVec3Normalize(&m_vUp, &m_vUp);

	D3DXVec3Cross(&m_vRight, &m_vUp, &m_vLook);
	D3DXVec3Normalize(&m_vRight, &m_vRight);

	// Build the view matrix:
	float x = -D3DXVec3Dot(&m_vRight, &m_vPos);
	float y = -D3DXVec3Dot(&m_vUp, &m_vPos);
	float z = -D3DXVec3Dot(&m_vLook, &m_vPos);

	view->_11 = m_vRight.x ;
	view->_12 = m_vUp.x ;
	view->_13 = m_vLook.x ;
	view->_14 = 0.0f ;

	view->_21 = m_vRight.y ;
	view->_22 = m_vUp.y ;
	view->_23 = m_vLook.y ;
	view->_24 = 0.0f ;

	view->_31 = m_vRight.z ;
	view->_32 = m_vUp.z ;
	view->_33 = m_vLook.z ;
	view->_34 = 0.0f ;

	view->_41 = x ;
	view->_42 = y ;
	view->_43 = z ;
	view->_44 = 1.0f ;
}

void Camera::Walk(float unit)
{
	m_vPos += m_vLook * unit ;
}

void Camera::Strafe(float unit)
{
	m_vPos += m_vRight * unit ; 
}

HRESULT Camera::HandleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_KEYDOWN:
		{
			switch( wParam )
			{
			case 'A':
			case VK_LEFT:
				Strafe(-0.1f) ;
				return TRUE ;

			case 'D':
			case VK_RIGHT:
				Strafe(0.1f) ;
				return TRUE ;

			case 'W':
			case VK_UP:
				Walk(0.1f) ;
				return TRUE ;

			case 'S':
			case VK_DOWN:
				Walk(-0.1f) ;
				return TRUE ;
			}
		}
	}

	return FALSE ;
}