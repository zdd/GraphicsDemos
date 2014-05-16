#include "AABB.h"

AABB::AABB(void):minPt(D3DXVECTOR3(0, 0, 0)), maxPt(D3DXVECTOR3(0, 0, 0))
{
	
}

AABB::~AABB(void)
{
}

D3DXVECTOR3 AABB::center() const
{
	return (minPt + maxPt) * 0.5f;
}

D3DXVECTOR3 AABB::extent() const
{
	return (maxPt - minPt) * 0.5f;
}

void AABB::transform( const D3DXMATRIX& M, AABB& out )
{
	// Convert to center/extent representation.
	D3DXVECTOR3 c = center();
	D3DXVECTOR3 e = extent();

	// Transform center in usual way.
	D3DXVec3TransformCoord(&c, &c, &M);

	// Transform extent.
	D3DXMATRIX absM;
	D3DXMatrixIdentity(&absM);
	absM(0,0) = fabsf(M(0,0)); absM(0,1) = fabsf(M(0,1)); absM(0,2) = fabsf(M(0,2));
	absM(1,0) = fabsf(M(1,0)); absM(1,1) = fabsf(M(1,1)); absM(1,2) = fabsf(M(1,2));
	absM(2,0) = fabsf(M(2,0)); absM(2,1) = fabsf(M(2,1)); absM(2,2) = fabsf(M(2,2));
	D3DXVec3TransformNormal(&e, &e, &absM);

	// Convert back to AABB representation.
	out.minPt = c - e;
	out.maxPt = c + e;
}