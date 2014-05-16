#ifndef FIRSTPERSON_CAMERA_H
#define FIRSTPERSON_CAMERA_H

#include <d3dx9.h>

class FirstPersonViewCamera
{
public:
	FirstPersonViewCamera(void);
	~FirstPersonViewCamera(void);

	void CalcViewMatrix(D3DXMATRIX* viewMatrix);
	void MoveForward(float unit) ;	// Move towards the positive z-axis
	void MoveBack(float unit) ;		// Move towards the negative z-axis
	void MoveLeft(float unit) ;		// Move towards the negative x-axis
	void MoveRight(float unit) ;	// Move towards the positive x-axis
	void Restore() ;				// Restore view matrix

private:
	D3DXVECTOR3 m_vRight ;	// Right vector
	D3DXVECTOR3 m_vUp ;		// Up vector
	D3DXVECTOR3 m_vLook ;	// Forward vector

	D3DXVECTOR3 m_vPos ;	// Camera position
};

#endif //FIRSTPERSON_CAMERA_H