// Axis-Aligned Bounding Box 
// 2012-05-06

#ifndef __AABB_H__
#define __AABB_H__

#include <d3dx9.h>

class AABB 
{
public:
	AABB(D3DXVECTOR3& boxMin, D3DXVECTOR3& boxMax):mMinPoint(boxMin), mMaxPoint(boxMax)
	{

	}

	// Get the maximum corner of the box
	inline const D3DXVECTOR3& getMaxPoint() const
	{
		return mMaxPoint;
	}

	// Get the minimum corner of the box
	inline const D3DXVECTOR3& getMinPoint() const
	{
		return mMinPoint;
	}

	// Set the maximum corner of the box
	inline void setMaxPoint(D3DXVECTOR3& vec)
	{
		mMaxPoint = vec;
	}

	// Set the minimum corner of the box
	inline void setMinPoint(D3DXVECTOR3& vec)
	{
		mMinPoint = vec;
	}

	// Get center of box
	inline D3DXVECTOR3 getCenter() const
	{
		return (mMaxPoint + mMinPoint) * 0.5f;
	}

	// Get extent of box, extent is the half size of width, height and depth
	inline D3DXVECTOR3 getHalfSize() const
	{
		return (mMaxPoint - mMinPoint) * 0.5f;
	}

	// Transform, *** what's the algorithm here for extent transform ***
	inline void transform(const D3DXMATRIX& mat)
	{
		// Convert to center/extent representation.
		D3DXVECTOR3 c = getCenter();
		D3DXVECTOR3 e = getHalfSize();

		// Transform center in usual way.
		D3DXVec3TransformCoord(&c, &c, &mat);

		// Transform extent.
		D3DXMATRIX absM;
		D3DXMatrixIdentity(&absM);

		absM(0,0) = fabsf(mat(0,0)); 
		absM(0,1) = fabsf(mat(0,1)); 
		absM(0,2) = fabsf(mat(0,2));

		absM(1,0) = fabsf(mat(1,0)); 
		absM(1,1) = fabsf(mat(1,1)); 
		absM(1,2) = fabsf(mat(1,2));

		absM(2,0) = fabsf(mat(2,0)); 
		absM(2,1) = fabsf(mat(2,1)); 
		absM(2,2) = fabsf(mat(2,2));

		D3DXVec3TransformNormal(&e, &e, &absM);

		// Convert back to AABB representation.
		this->mMaxPoint = c + e;
		this->mMinPoint = c - e;
	}

private:
	D3DXVECTOR3 mMaxPoint;
	D3DXVECTOR3 mMinPoint;
};
#endif // end __AABB_H__