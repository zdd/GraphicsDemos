#include "FirstPersonCamera.h"

FirstPersonCamera::FirstPersonCamera(void)
{
	m_fMoveScaler = 20.0f ; // Moving speed
	m_fRotationScaler = 0.001f ;
	m_fCameraYawAngle = 0.0f ;
	m_fCameraPitchAngle = 0.0f ;
	m_fFramesToSmoothMouseData = 2.0f ;

	m_vRotVelocity = D3DXVECTOR2(0.0f, 0.0f) ;
	m_vMouseDelta = D3DXVECTOR2(0.0f, 0.0f) ;
	GetCursorPos( &m_ptLastMousePosition ) ;
	m_vVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f) ;
	m_fInitVelocityUp = 0.0f ;
	m_fVelocityUp = 0.0f ;
	m_fGravityAcceleration = 9.8f ;
	m_fDisplacementUp = 0.0f ;
	m_dwStartTime = 0 ;
	m_bOnLand = true ;
}

FirstPersonCamera::~FirstPersonCamera(void)
{

}

// Handle keyboards/mouse input
HRESULT FirstPersonCamera::HandleInput(DXInput* dInput)
{
	if (dInput->KeyDown(DIK_W) || dInput->KeyDown(DIK_UP))
	{
		m_vKeyboardDirection.z += 1.0f ;
	}

	if (dInput->KeyDown(DIK_S) || dInput->KeyDown(DIK_DOWN))
	{
		m_vKeyboardDirection.z -= 1.0f ;
	}

	if (dInput->KeyDown(DIK_A) || dInput->KeyDown(DIK_LEFT))
	{
		m_vKeyboardDirection.x -= 1.0f ;
	}

	if (dInput->KeyDown(DIK_D) || dInput->KeyDown(DIK_RIGHT))
	{
		m_vKeyboardDirection.x += 1.0f ;
	}

	if (dInput->KeyDown(DIK_SPACE))
	{
		if(m_bOnLand) // only jump when character is on land
		{
			m_bOnLand = false ;
			m_fInitVelocityUp = 2.0f ;
			m_dwStartTime = timeGetTime() ; // store the start time of jump
		}
	}

	return TRUE ;
}

void FirstPersonCamera::UpdateMouseDelta()
{
	POINT ptCurMouseDelta;
	POINT ptCurMousePos;

	// Get current position of mouse
	GetCursorPos( &ptCurMousePos );

	// Calculate how far it's moved since last frame
	ptCurMouseDelta.x = ptCurMousePos.x - m_ptLastMousePosition.x;
	ptCurMouseDelta.y = ptCurMousePos.y - m_ptLastMousePosition.y;

	// Record current position for next time
	m_ptLastMousePosition = ptCurMousePos;

	// Set position of camera to center of desktop, 
	// so it always has room to move.  This is very useful
	// if the cursor is hidden.  If this isn't done and cursor is hidden, 
	// then invisible cursor will hit the edge of the screen 
	// and the user can't tell what happened
	POINT ptCenter;

	// Get the center of the current monitor
	MONITORINFO mi;
	mi.cbSize = sizeof( MONITORINFO );
	RECT rc ;
	GetClientRect(GetForegroundWindow(), &rc) ;
	ptCenter.x = ( rc.left + rc.right ) / 2;
	ptCenter.y = ( rc.top + rc.bottom ) / 2;
	SetCursorPos( ptCenter.x, ptCenter.y );
	m_ptLastMousePosition = ptCenter;

	// Smooth the relative mouse data over a few frames so it isn't 
	// jerky when moving slowly at low frame rates.
	float fPercentOfNew = 1.0f / m_fFramesToSmoothMouseData;
	float fPercentOfOld = 1.0f - fPercentOfNew;
	m_vMouseDelta.x = m_vMouseDelta.x * fPercentOfOld + ptCurMouseDelta.x * fPercentOfNew;
	m_vMouseDelta.y = m_vMouseDelta.y * fPercentOfOld + ptCurMouseDelta.y * fPercentOfNew;

	m_vRotVelocity = m_vMouseDelta * m_fRotationScaler;
}

void FirstPersonCamera::UpdateVelocity()
{
	// Update vertical velocity, for jump
	if(!m_bOnLand)
	{
		// Calculate the time to reach the hight point
		static float timeReachHight = m_fInitVelocityUp / m_fGravityAcceleration ;
		
		// Calculate the hight that the jump can reach
		static float height = m_fInitVelocityUp * m_fInitVelocityUp / (2 * m_fGravityAcceleration) ;

		// Get current time
		DWORD currentTime = timeGetTime() ;

		// Calculate time elapsed
		float timeElapse = (float)(currentTime - m_dwStartTime) * 0.001f ;

		// jump complete? this code needs to be check
		// the correct formula should be
		// if(timeElapse >= 2.0f * timeReachHight), but if I use this formula, it yields unexpected results
		if(timeElapse >= 3.0f * timeReachHight)
		{
			m_bOnLand = true ;
			return ;
		}

		// jump up or fall down?
		static bool up = true ;

		if(!m_bOnLand)
		{
			if(up)
			{
				m_fDisplacementUp = m_fInitVelocityUp * timeElapse - 0.5f * m_fGravityAcceleration * timeElapse * timeElapse ;
				
				// reach high point, start to fall down
				if(timeElapse >= timeReachHight)
				{
					up = false ;
				}
			}

			else // fall down
			{
				float t = timeElapse - timeReachHight ;
				m_fDisplacementUp = height - 0.5f * m_fGravityAcceleration * t * t ;
			}
		}
	}

	// Update on land velocity, for terrain walking
	D3DXMATRIX mRotDelta;
	m_vRotVelocity = m_vMouseDelta * m_fRotationScaler ;

	D3DXVECTOR3 vAccel = m_vKeyboardDirection;

	// Normalize vector so if moving 2 dirs (left & forward), 
	// the camera doesn't move faster than if moving in 1 dir
	D3DXVec3Normalize( &vAccel, &vAccel );

	// Scale the acceleration vector
	vAccel *= m_fMoveScaler;

	m_vVelocity = vAccel;
}

void FirstPersonCamera::OnFrameMove(float timeDelta)
{
	// Calculate the mouse moving delta
	UpdateMouseDelta() ;

	UpdateVelocity() ;

	// Simple euler method to calculate position delta
	D3DXVECTOR3 vPosDelta = m_vVelocity * timeDelta ;

	// Update the pitch & yaw angle based on mouse movement
	float fYawDelta = m_vRotVelocity.x;
	float fPitchDelta = m_vRotVelocity.y;

	m_fCameraPitchAngle += fPitchDelta ;
    m_fCameraYawAngle += fYawDelta ;

	// Make a rotation matrix based on the camera's yaw & pitch
	D3DXMATRIX mCameraRot;
	D3DXMatrixRotationYawPitchRoll( &mCameraRot, m_fCameraYawAngle, m_fCameraPitchAngle, 0 );

	// Transform vectors based on camera's rotation matrix
	D3DXVECTOR3 vWorldUp, vWorldAhead;
	D3DXVECTOR3 vLocalUp = D3DXVECTOR3( 0, 1, 0 );
	D3DXVECTOR3 vLocalAhead = D3DXVECTOR3( 0, 0, 1 );
	D3DXVec3TransformCoord( &vWorldUp, &vLocalUp, &mCameraRot );
	D3DXVec3TransformCoord( &vWorldAhead, &vLocalAhead, &mCameraRot );

	// Transform the position delta by the camera's rotation 
	D3DXVECTOR3 vPosDeltaWorld;
	
	D3DXVec3TransformCoord( &vPosDeltaWorld, &vPosDelta, &mCameraRot );

	// Move the eye position 
	m_vEyePoint += vPosDeltaWorld;
	
	m_vEyePoint.y = 0.0f;

	// Update the lookAt position based on the eye position 
	m_vLookAt = m_vEyePoint + vWorldAhead;

	// Update the view matrix
	D3DXMatrixLookAtLH( &m_matView, &m_vEyePoint, &m_vLookAt, &vWorldUp );

	// The m_vKeyboardDirection must be clear to zero each time a key is released
	// because this is a member variable, and we don't want to use the accumulate value
	// we only want the increment value
	m_vKeyboardDirection = D3DXVECTOR3(0.0f, 0.0f, 0.0f) ;
}