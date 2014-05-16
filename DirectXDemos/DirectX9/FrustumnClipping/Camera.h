//=============================================================================
// Camera.h by Frank Luna (C) 2004 All Rights Reserved.
//=============================================================================

#ifndef CAMERA_H
#define CAMERA_H

#include <d3dx9.h>
#include "DXInput.h"
#include "AABB.h"

// Forward declaration.
class Terrain;

class Camera
{
public:
	Camera();

	const D3DXMATRIX& view() const;
	const D3DXMATRIX& proj() const;
	void SetViewParams(D3DXVECTOR3& pos, D3DXVECTOR3& target, D3DXVECTOR3& up);
	void SetProjParams(float fov, float aspect, float nearZ, float farZ);
	void update(float dt, float offsetHeight);
	bool isVisible(const AABB& box)	const;

private:
	void buildView();
	void buildWorldFrustumPlanes();

private:
	D3DXMATRIX mView;
	D3DXMATRIX mProj;

	// Relative to world space.
	D3DXVECTOR3 mPosW;
	D3DXVECTOR3 mRightW;
	D3DXVECTOR3 mUpW;
	D3DXVECTOR3 mLookW;

	// 6 planes for frustum
	// [0] = near
	// [1] = far
	// [2] = left
	// [3] = right
	// [4] = top
	// [5] = bottom
	D3DXPLANE mFrustumPlanes[6]; 

	float mSpeed;
	
	DXInput* mDXInput ;

};

#endif // CAMERA_H