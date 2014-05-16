//=============================================================================
// Camera.h by Frank Luna (C) 2004 All Rights Reserved.
//=============================================================================

#ifndef CAMERA_H
#define CAMERA_H

#include <d3dx9.h>
#include "DXInput.h"

// Forward declaration.
class Terrain;

class Camera
{
public:
	Camera();

	const D3DXMATRIX& GetViewMatrix() const;
	const D3DXMATRIX& GetProjMatrix() const;
	void SetViewParams(D3DXVECTOR3& pos, D3DXVECTOR3& target, D3DXVECTOR3& up);
	void SetProjParams(float fov, float aspect, float nearZ, float farZ);
	void Update(float dt, float offsetHeight);
	const D3DXVECTOR3 GetLookAt() const ;
	const D3DXVECTOR3 GetEyePoint() const ;
	const D3DXVECTOR3 GetUpVector() const ;

private:
	void BuildViewMatrix();

private:
	D3DXMATRIX m_matView;
	D3DXMATRIX m_matProj;

	// Relative to world space.
	D3DXVECTOR3 m_vEyePoint;
	D3DXVECTOR3 m_vRight;
	D3DXVECTOR3 m_vUp;
	D3DXVECTOR3 m_vForward;

	float mSpeed;
	
	DXInput* mDXInput ;

};

#endif // CAMERA_H