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

	const D3DXMATRIX& getViewMatrix() const;
	const D3DXMATRIX& getProjMatrix() const;
	void SetViewParams(D3DXVECTOR3& pos, D3DXVECTOR3& target, D3DXVECTOR3& up);
	void SetProjParams(float fov, float aspect, float nearZ, float farZ);
	void update(float dt, float offsetHeight);

private:
	void buildView();

private:
	D3DXMATRIX mView;
	D3DXMATRIX mProj;

	// Relative to world space.
	D3DXVECTOR3 mPosW;
	D3DXVECTOR3 mRightW;
	D3DXVECTOR3 mUpW;
	D3DXVECTOR3 mLookW;

	float mSpeed;
	
	DXInput* mDXInput ;

};

#endif // CAMERA_H