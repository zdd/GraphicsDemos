#include "Math.h"

// Determine Whether a ray intersect with a rectangle
// Divide the rectangle into two triangles and if the ray intersect with any of them
BOOL Ray::Intersection(Rect *rect, D3DXVECTOR3 *hitPoint, float *dist)
{
	// Divide the rectangle into two triangles
	Triangle t1(rect->_v1, rect->_v2, rect->_v3);
	Triangle t2(rect->_v1, rect->_v3, rect->_v4) ;

	return Intersection(&t1, hitPoint, dist) || Intersection(&t2, hitPoint, dist);
}

// Determine whether the ray intersect with a triangle
BOOL Ray::Intersection(Triangle *triangle, D3DXVECTOR3 *hitPoint, float *dist)
{
	float u = 0.0f ;
	float v = 0.0f ;
	if(D3DXIntersectTri(&triangle->_v1, &triangle->_v2, &triangle->_v3, &_origin, &_direction, &u, &v, dist))
	{
		*hitPoint = u * triangle->_v2 + v * triangle->_v3 + (1 - u - v) * triangle->_v1 ;
		return TRUE ;
	}
	else
		return FALSE ;
}