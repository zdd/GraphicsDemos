#ifndef __MESH_H__
#define __MESH_H__

#include <vector>
#include <d3dx9.h>

using namespace std ;

// this mesh is a wrapper of D3D mesh
class Mesh
{
public:
	Mesh(void);
	~Mesh(void);
public:
	// Load mesh from .x file
	void LoadFromXFile(LPDIRECT3DDEVICE9 pDevice, LPCTSTR fileName);

	// draw current mesh
	void Draw(LPDIRECT3DDEVICE9 pDevice) ; 
private:
	ID3DXMesh*		m_mesh ;
	ID3DXBuffer*	m_pAdjBuffer  ;
	ID3DXBuffer*	m_pMtrlBuffer ;
	DWORD			m_iNumMtrls   ;
	vector<D3DMATERIAL9> m_vMtrls;
};

#endif // end __MESH_H__