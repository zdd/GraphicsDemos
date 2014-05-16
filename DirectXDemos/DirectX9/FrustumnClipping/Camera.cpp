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
	buildWorldFrustumPlanes() ;
}

void Camera::SetProjParams(float fov, float aspect, float nearZ, float farZ)
{
	D3DXMatrixPerspectiveFovLH(&mProj, fov, aspect, nearZ, farZ);
	buildWorldFrustumPlanes() ;
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

	// Rebuild the frustum planes
	buildWorldFrustumPlanes() ;
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

// for algorithm of this function, please google "Fast Extraction of Viewing Frustum Planes from the World-
// View-Projection Matrix"
void Camera::buildWorldFrustumPlanes()
{
	D3DXMATRIX VP = mView * mProj ;

	D3DXVECTOR4 col0(VP(0,0), VP(1,0), VP(2,0), VP(3,0));
	D3DXVECTOR4 col1(VP(0,1), VP(1,1), VP(2,1), VP(3,1));
	D3DXVECTOR4 col2(VP(0,2), VP(1,2), VP(2,2), VP(3,2));
	D3DXVECTOR4 col3(VP(0,3), VP(1,3), VP(2,3), VP(3,3));

	// Planes face inward.
	mFrustumPlanes[0] = (D3DXPLANE)(col2);        // near
	mFrustumPlanes[1] = (D3DXPLANE)(col3 - col2); // far
	mFrustumPlanes[2] = (D3DXPLANE)(col3 + col0); // left
	mFrustumPlanes[3] = (D3DXPLANE)(col3 - col0); // right
	mFrustumPlanes[4] = (D3DXPLANE)(col3 - col1); // top
	mFrustumPlanes[5] = (D3DXPLANE)(col3 + col1); // bottom

	for(int i = 0; i < 6; i++)
		D3DXPlaneNormalize(&mFrustumPlanes[i], &mFrustumPlanes[i]);
}

//bool Camera::isVisible( const AABB& box ) const
//{
//	// Test assumes frustum planes face inward.
//
//	D3DXVECTOR3 P;
//	D3DXVECTOR3 Q;
//
//	//      N  *Q                    *P
//	//      | /                     /
//	//      |/                     /
//	// -----/----- Plane     -----/----- Plane    
//	//     /                     / |
//	//    /                     /  |
//	//   *P                    *Q  N
//	//
//	// PQ forms diagonal most closely aligned with plane normal.
//
//	// For each frustum plane, find the box diagonal (there are four main
//	// diagonals that intersect the box center point) that points in the
//	// same direction as the normal along each axis (i.e., the diagonal 
//	// that is most aligned with the plane normal).  Then test if the box
//	// is in front of the plane or not.
//	for(int i = 0; i < 6; ++i)
//	{
//		// For each coordinate axis x, y, z...
//		for(int j = 0; j < 3; ++j)
//		{
//			// Make PQ point in the same direction as the plane normal on this axis.
//			if( mFrustumPlanes[i][j] >= 0.0f )
//			{
//				P[j] = box.minPt[j];
//				Q[j] = box.maxPt[j];
//			}
//			else 
//			{
//				P[j] = box.maxPt[j];
//				Q[j] = box.minPt[j];
//			}
//		}
//
//		// If box is in negative half space, it is behind the plane, and thus, completely
//		// outside the frustum.  Note that because PQ points roughly in the direction of the 
//		// plane normal, we can deduce that if Q is outside then P is also outside--thus we
//		// only need to test Q.
//		if( D3DXPlaneDotCoord(&mFrustumPlanes[i], &Q) < 0.0f  ) // outside
//			return false;
//	}
//	return true;
//}


/*
This way determine whether all the eight points of the box
are all in the negative side of one of the six planes, if it is true
the box was invisible.
*/
bool Camera::isVisible(const AABB& box) const
{
	// Get the x, y, z of box's min point.
	float minX = box.minPt.x;
	float minY = box.minPt.y;
	float minZ = box.minPt.z;

	// Get the x, y,z of box's max point.
	float maxX = box.maxPt.x;
	float maxY = box.maxPt.y;
	float maxZ = box.maxPt.z;

	// Get 8 points of the box
	D3DXVECTOR3 points[8] = 
	{
		D3DXVECTOR3(minX, minY, minZ),
		D3DXVECTOR3(maxX, minY, minZ),
		D3DXVECTOR3(maxX, minY, maxZ),
		D3DXVECTOR3(minX, minY, maxZ),
		D3DXVECTOR3(minX, maxY, minZ),
		D3DXVECTOR3(maxX, maxY, minZ),
		D3DXVECTOR3(maxX, maxY, maxZ),
		D3DXVECTOR3(minX, maxY, maxZ),
	};

	// Determine if all of the 8 points are in the negative side
	// of the box, if it is true, the box was invisible. otherwise, visible.
	for (int i = 0; i < 6; ++i)
	{
		if (D3DXPlaneDotCoord(&mFrustumPlanes[i], &points[0]) > 0.0f)
		{
			continue;
		}
		if (D3DXPlaneDotCoord(&mFrustumPlanes[i], &points[1]) > 0.0f)
		{
			continue;
		}
		if (D3DXPlaneDotCoord(&mFrustumPlanes[i], &points[2]) > 0.0f)
		{
			continue;
		}
		if (D3DXPlaneDotCoord(&mFrustumPlanes[i], &points[3]) > 0.0f)
		{
			continue;
		}
		if (D3DXPlaneDotCoord(&mFrustumPlanes[i], &points[4]) > 0.0f)
		{
			continue;
		}
		if (D3DXPlaneDotCoord(&mFrustumPlanes[i], &points[5]) > 0.0f)
		{
			continue;
		}
		if (D3DXPlaneDotCoord(&mFrustumPlanes[i], &points[6]) > 0.0f)
		{
			continue;
		}
		if (D3DXPlaneDotCoord(&mFrustumPlanes[i], &points[7]) > 0.0f)
		{
			continue;
		}

		return false ;
	}

	return true;
}