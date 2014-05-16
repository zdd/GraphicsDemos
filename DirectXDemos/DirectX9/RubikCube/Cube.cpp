#include "Cube.h"

Cube::Cube(void)
{
	D3DXMatrixIdentity(&m_matWorld);
}

Cube::~Cube(void)
{
}

void Cube::LoadMesh(LPDIRECT3DDEVICE9 pDevice, WCHAR* fileName)
{
	m_mesh.LoadFromXFile(pDevice, fileName) ;
}

void Cube::Rotate(D3DXMATRIX* rotMatrix)
{
	m_matWorld *= *rotMatrix ;
}

void Cube::Draw(LPDIRECT3DDEVICE9 pDevice)
{
	pDevice->SetTransform(D3DTS_WORLD, &m_matWorld) ;
	m_mesh.Draw(pDevice) ;
}