#include "Camera.h"
#include "Terrain.h"

Camera::Camera()
{
	D3DXMatrixIdentity(&mView);
	D3DXMatrixIdentity(&mProj);

	mPosW   = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	mRightW = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	mUpW    = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	mLookW  = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

	// Client should adjust to a value that makes sense for application's
	// unit scale, and the object the camera is attached to--e.g., car, jet,
	// human walking, etc.
	mSpeed  = 1.0f;

	mDXInput = new DXInput() ;
}

const D3DXMATRIX& Camera::view() const
{
	return mView;
}

const D3DXMATRIX& Camera::proj() const
{
	return mProj;
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

	mPosW   = pos;
	mRightW = R;
	mUpW    = U;
	mLookW  = L;

	buildView();
}

void Camera::SetProjParams(float fov, float aspect, float nearZ, float farZ)
{
	D3DXMatrixPerspectiveFovLH(&mProj, fov, aspect, nearZ, farZ);
}

void Camera::update(float dt, float offsetHeight)
{
	mDXInput->Update() ;

	// Find the net direction the camera is traveling in (since the
	// camera could be running and strafing).
	D3DXVECTOR3 dir(0.0f, 0.0f, 0.0f);

	if( mDXInput->KeyDown(DIK_W) )
		dir += mLookW;
	if( mDXInput->KeyDown(DIK_S) )
		dir -= mLookW;
	if( mDXInput->KeyDown(DIK_D) )
		dir += mRightW;
	if( mDXInput->KeyDown(DIK_A) )
		dir -= mRightW;

	// Move at mSpeed along net direction.
	D3DXVec3Normalize(&dir, &dir);
	D3DXVECTOR3 newPos = mPosW + dir * mSpeed * dt;
	
	mPosW = newPos ;
	if (mPosW.y < 1.0f)
	{
		mPosW.y = 1.0f ;
	}
	if (mPosW.y > 2.0f)
	{
		mPosW.y = 2.0f ;
	}

	// We rotate at a fixed speed.
	float pitch  = mDXInput->MouseDY() / 360.0f;
	float yAngle = mDXInput->MouseDX() / 360.0f;

	// Rotate camera's look and up vectors around the camera's right vector.
	D3DXMATRIX R;
	D3DXMatrixRotationAxis(&R, &mRightW, pitch);
	D3DXVec3TransformCoord(&mLookW, &mLookW, &R);
	D3DXVec3TransformCoord(&mUpW, &mUpW, &R);

	// Rotate camera axes about the world's y-axis.
	D3DXMatrixRotationY(&R, yAngle);
	D3DXVec3TransformCoord(&mRightW, &mRightW, &R);
	D3DXVec3TransformCoord(&mUpW, &mUpW, &R);
	D3DXVec3TransformCoord(&mLookW, &mLookW, &R);

	// Rebuild the view matrix to reflect changes.
	buildView();
}

void Camera::buildView()
{
	// Keep camera's axes orthogonal to each other and of unit length.
	D3DXVec3Normalize(&mLookW, &mLookW);

	D3DXVec3Cross(&mUpW, &mLookW, &mRightW);
	D3DXVec3Normalize(&mUpW, &mUpW);

	D3DXVec3Cross(&mRightW, &mUpW, &mLookW);
	D3DXVec3Normalize(&mRightW, &mRightW);

	// Fill in the view matrix entries.

	float x = -D3DXVec3Dot(&mPosW, &mRightW);
	float y = -D3DXVec3Dot(&mPosW, &mUpW);
	float z = -D3DXVec3Dot(&mPosW, &mLookW);

	mView(0,0) = mRightW.x; 
	mView(1,0) = mRightW.y; 
	mView(2,0) = mRightW.z; 
	mView(3,0) = x;   

	mView(0,1) = mUpW.x;
	mView(1,1) = mUpW.y;
	mView(2,1) = mUpW.z;
	mView(3,1) = y;  

	mView(0,2) = mLookW.x; 
	mView(1,2) = mLookW.y; 
	mView(2,2) = mLookW.z; 
	mView(3,2) = z;   

	mView(0,3) = 0.0f;
	mView(1,3) = 0.0f;
	mView(2,3) = 0.0f;
	mView(3,3) = 1.0f;
}