#pragma once
#include <Windows.h>
#include <vector>
#include <d3dx9.h>
#include "Vertex.h"
using namespace std ;

class Terrain
{
public:
	Terrain(void);
	~Terrain(void);
	void BuildGridsBuffer(LPDIRECT3DDEVICE9 pd3dDevice) ;
	void Render(LPDIRECT3DDEVICE9 pd3dDevice) ;

private:
	void GenerateGrids(int numVertRows, int numVertCols, float dx, float dz, const D3DXVECTOR3& center, 
		vector<D3DXVECTOR3>& verts,
		vector<DWORD>& indices) ;

private:
	DWORD mNumVertices;
	DWORD mNumTriangles;

	IDirect3DVertexBuffer9* mVB;
	IDirect3DIndexBuffer9*  mIB;
};
