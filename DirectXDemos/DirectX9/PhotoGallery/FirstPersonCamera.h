#ifndef FIRSTPERSONVIEWCAMERA_H
#define FIRSTPERSONVIEWCAMERA_H

#include <d3dx9.h>
#include "BaseCamera.h" 
#include "DXInput.h"

class Terrain ;

// This is a first person-view camera, with this camera, you can
// 1. move mouse left button to change view
// 2. press WASD or arrow keys to move on the ground
class FirstPersonCamera : public BaseCamera
{
public:
	FirstPersonCamera(void);
	~FirstPersonCamera(void);
	void UpdateMouseDelta() ;
	void UpdateVelocity() ;
	void OnFrameMove(float timeDelta) ;
	LRESULT HandleInput(DXInput* dInput) ;

private:
	float m_fMoveScaler ;				// Move scaler
	float m_fRotationScaler ;			// Scalar for rotation
	float m_fFramesToSmoothMouseData ;	// Number of frames to smooth the mouse delta
	float m_fCameraYawAngle ;			// Yaw angle
	float m_fCameraPitchAngle ;			// Pitch angle

	// The mouse move is restrict on the screen plane, so it's a 2D vector
	D3DXVECTOR2 m_vMouseDelta ;			// Mouse moving increment during rotation
	D3DXVECTOR2 m_vRotVelocity ;		// Camera rotation velocity, use mouse
	POINT m_ptLastMousePosition ;		// Previous mouse position in rotation 

	D3DXVECTOR3 m_vVelocity ;			// This is the moving velocity, use keyboard
	float		m_fInitVelocityUp ;		// The initial up velocity for jump
	float		m_fVelocityUp ;			// vertical velocity during a jump
	float		m_fGravityAcceleration ;// The gravity acceleration
	DWORD		m_dwStartTime ;			// Start time of free fallen motion
	float		m_fDisplacementUp ;		// Vertical displacement
	bool		m_bOnLand ;				// Person is on terrain ? or in the air(jump)

	D3DXVECTOR3 m_vKeyboardDirection ;	// Moving direction 
	
};

#endif // FIRSTPERSONVIEWCAMERA_H

