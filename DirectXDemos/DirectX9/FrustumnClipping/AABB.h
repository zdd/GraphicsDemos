#ifndef __AABB_H__
#define __AABB_H__

#include <d3dx9.h>

class AABB
{
public:
	AABB(void);
	~AABB(void);
	D3DXVECTOR3 center() const;
	D3DXVECTOR3 extent() const; // what is this?
	void transform(const D3DXMATRIX& M, AABB& out);

	D3DXVECTOR3 minPt;
	D3DXVECTOR3 maxPt;
};

#endif // end __AABB_H__
