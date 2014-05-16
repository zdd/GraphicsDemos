#ifndef __MATH_H__
#define __MATH_H__

#include <d3dx9.h>

// Triangle
struct Triangle
{
	D3DXVECTOR3 _v1 ;
	D3DXVECTOR3 _v2 ;
	D3DXVECTOR3 _v3 ;

	Triangle(D3DXVECTOR3 v1, D3DXVECTOR3 v2, D3DXVECTOR3 v3)
	{
		_v1 = v1 ;
		_v2 = v2 ;
		_v3 = v3 ;
	}
} ;

// Rectangle
struct Rect 
{
	D3DXVECTOR3 _v1 ; // top-left 
	D3DXVECTOR3 _v2 ; // top-right
	D3DXVECTOR3 _v3 ; // left-bottom
	D3DXVECTOR3 _v4 ; // right-bottom

	Rect(D3DXVECTOR3 v1, D3DXVECTOR3 v2, D3DXVECTOR3 v3, D3DXVECTOR3 v4)
	{
		_v1 = v1 ;
		_v2 = v2 ;
		_v3 = v3 ;
		_v4 = v4 ;
	}
};

// Picking ray
class Ray
{
public:
	BOOL Intersection(Rect *rect, D3DXVECTOR3 *hitPoint, float *dist) ;
	BOOL Ray::Intersection(Triangle *triangle, D3DXVECTOR3 *hitPoint, float *dist) ;

public:
	D3DXVECTOR3 _origin;
	D3DXVECTOR3 _direction;
};

#endif //end __MATH_H__