#include "Mesh.h"

Mesh::Mesh(void):m_mesh(NULL), m_pAdjBuffer(NULL), m_pMtrlBuffer(NULL), m_iNumMtrls(0), m_vMtrls(NULL)
{
}

Mesh::~Mesh(void)
{
	if(m_pMtrlBuffer != NULL)
	{
		m_pMtrlBuffer->Release();
		m_pMtrlBuffer = NULL ;
	}
	if(m_pAdjBuffer != NULL)
	{
		m_pAdjBuffer->Release();
		m_pAdjBuffer = NULL ;
	}
	if(m_mesh != NULL)
	{
		m_mesh->Release();
		m_mesh = NULL ;
	}
}

void Mesh::LoadFromXFile(LPDIRECT3DDEVICE9 pDevice, LPCTSTR fileName)
{
	// Load mesh from .x file
	HRESULT hr = D3DXLoadMeshFromX(fileName, D3DXMESH_MANAGED, pDevice, &m_pAdjBuffer, &m_pMtrlBuffer, 0, &m_iNumMtrls, &m_mesh) ;

	if(FAILED(hr))
		MessageBox(NULL, L"Load mesh from x file failed!", L"error", 0) ;

	// Load materials
	D3DXMATERIAL* mtrls = (D3DXMATERIAL*)m_pMtrlBuffer->GetBufferPointer();
	for (DWORD i = 0; i < m_iNumMtrls; i++)
	{
		mtrls[i].MatD3D.Ambient = mtrls[i].MatD3D.Diffuse;
		m_vMtrls.push_back(mtrls[i].MatD3D);
	}
}

void Mesh::Draw(LPDIRECT3DDEVICE9 pDevice)
{
	for(DWORD i = 0; i < m_iNumMtrls; i++)
	{
		pDevice->SetMaterial(&m_vMtrls[i]);
		m_mesh->DrawSubset(i);
	}
}