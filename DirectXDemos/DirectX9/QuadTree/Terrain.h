#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#include "QuadTree.h"
#include "Frustum.h"
#include "Camera.h"
#include <d3dx9.h>

class Terrain
{
public:
	Terrain(void);
	~Terrain(void);
	void initialize(IDirect3DDevice9* pDevice);
	void render(IDirect3DDevice9* pDevice, Camera* camera);
	inline int getDrawCount() const
	{
		return mquadTree->getDrawCount();
	}

private:
	QuadTree*	mquadTree;
	Frustum*	mfrustum;
	int			mdrawCount;
};


#endif // end __TERRAIN_H__