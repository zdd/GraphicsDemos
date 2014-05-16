#ifndef __FRUSTUMN_H__
#define __FRUSTUMN_H__

#include <d3dx9.h>
#include "AABB.h"

class Frustum
{
public:
	Frustum()
	{
	}

	~Frustum(void)
	{
	}

	// Build the frustum planes
	inline void buildFrustumPlanes(const D3DXMATRIX& viewMatrix, const D3DXMATRIX& projMatrx)
	{
		// Calculate the view * project matrix
		D3DXMATRIX VP = viewMatrix * projMatrx ;

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

	// The view matrix is changed every time when the camera moves
	// So we need to update the frustum planes accordingly
	// We simply call buildFrustumPlanes to rebuild the planes
	// *** Is this function really needed? can we use buildFrustumPlanes directly?
	inline void updatePlanes(const D3DXMATRIX& viewMatrix, const D3DXMATRIX& projMatrx)
	{
		buildFrustumPlanes(viewMatrix, projMatrx);
	}

	// Determine whether the Axis aligned bounding box was intersect with the frustum
	inline bool isVisable(AABB& box) const
	{
		//D3DXVECTOR3 center = box.getCenter();
		//D3DXVECTOR3 halfSize = box.getHalfSize();

		//for (int i = 0; i < 6; ++i)
		//{
		//	// Get distance from box center to plane
		//	float a = mFrustumPlanes[i].a;
		//	float b = mFrustumPlanes[i].b;
		//	float c = mFrustumPlanes[i].c;
		//	float d = mFrustumPlanes[i].d;

		//	float distance = a * center.x + b * center.y + c * center.z + d;
		//	float maxAbsDist = abs(a * halfSize.x) + abs(b * halfSize.y) + abs(c * halfSize.z);

		//	if (distance <= -maxAbsDist)
		//	{
		//		return false ;
		//	}
		//}

		//return true ;

		//// Test assumes frustum planes face inward.

		D3DXVECTOR3 P;
		D3DXVECTOR3 Q;

		//      N  *Q                    *P
		//      | /                     /
		//      |/                     /
		// -----/----- Plane     -----/----- Plane    
		//     /                     / |
		//    /                     /  |
		//   *P                    *Q  N
		//
		// PQ forms diagonal most closely aligned with plane normal.

		// For each frustum plane, find the box diagonal (there are four main
		// diagonals that intersect the box center point) that points in the
		// same direction as the normal along each axis (i.e., the diagonal 
		// that is most aligned with the plane normal).  Then test if the box
		// is in front of the plane or not.
		for(int i = 0; i < 6; ++i)
		{
			// For each coordinate axis x, y, z...
			for(int j = 0; j < 3; ++j)
			{
				// Make PQ point in the same direction as the plane normal on this axis.
				if( mFrustumPlanes[i][j] >= 0.0f )
				{
					/*P[j] = box.mMinPoint[j];
					Q[j] = box.mMaxPoint[j];*/

					P[j] = box.getMinPoint()[j];
					Q[j] = box.getMaxPoint()[j];
				}
				else 
				{
					P[j] = box.getMaxPoint()[j];
					Q[j] = box.getMinPoint()[j];
				}
			}

			// If box is in negative half space, it is behind the plane, and thus, completely
			// outside the frustum.  Note that because PQ points roughly in the direction of the 
			// plane normal, we can deduce that if Q is outside then P is also outside--thus we
			// only need to test Q.
			if( D3DXPlaneDotCoord(&mFrustumPlanes[i], &Q) < 0.0f  ) // outside
				return false;
		}

		return true;
	}

	/* Determine whether a 3d point was inside the frustum
	   If the point placed at the positive side of the all six
	   planes of the frustum, we treat it inside the frustum, otherwise not */
	inline bool isPointInside(const D3DXVECTOR3& vec) const
	{
		for (int i = 0; i < 6; ++i)
		{
			float result = D3DXPlaneDotCoord(&mFrustumPlanes[i], &vec);
			if (result <= 0.0f)
			{
				return false;
			}
		}

		return true;
	}

	// Determine whether a 3d rectangle was visible
	// the rectangle center is (centerX, centerY, centerZ), and half width is radius
	inline bool isVisable(D3DXVECTOR3& center, const float radius) const
	{
		// Calculate the 4 points of the quad
		/*
		A------B
		|      |
		|      |
		|      |
		D------C
		*/
		D3DXVECTOR3 A(center.x - radius, center.y, center.z + radius);
		D3DXVECTOR3 B(center.x + radius, center.y, center.z + radius);
		D3DXVECTOR3 C(center.x + radius, center.y, center.z - radius);
		D3DXVECTOR3 D(center.x - radius, center.y, center.z - radius);

		D3DXVECTOR3 pointList[4] = { A, B, C, D };
		for (int i = 0; i < 6; ++i)
		{
			if (isNegativeSide(&mFrustumPlanes[i], pointList, 4))
			{
				return false ;
			}
		}

		return true; 
	}

	// Determine whether all points in pointList is in negative side of plane
	inline bool isNegativeSide(const D3DXPLANE* plane, D3DXVECTOR3* pointList, int numPoints) const
	{
		for (int i = 0; i < numPoints; ++i)
		{
			if (D3DXPlaneDotCoord(plane, &pointList[i]) > 0.0f)
			{
				return false ;
			}
		}

		return true;
	}

private:
	D3DXPLANE mFrustumPlanes[6];
};

#endif // end __FRUSTUMN_H__
