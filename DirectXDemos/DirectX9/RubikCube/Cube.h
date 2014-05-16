#ifndef __CUBE_H__
#define __CUBE_H__

#include "Mesh.h"

class Cube
{
public:
	Cube(void);
	~Cube(void);
	void LoadMesh(LPDIRECT3DDEVICE9 pDevice, WCHAR* fileName);
	void Rotate(D3DXMATRIX* rotMatrix) ;
	void Draw(LPDIRECT3DDEVICE9 pDevice) ;
private:
	Mesh		m_mesh ;		// mesh for cube, used to load the .x file	
	D3DXMATRIX	m_matWorld ;	// world matrix for unit cube, for rotation
};

#endif // end __CUBE_H__