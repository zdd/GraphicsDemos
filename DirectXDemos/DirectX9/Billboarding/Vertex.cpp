//=============================================================================
// Vertex.cpp by Frank Luna (C) 2005 All Rights Reserved.
//=============================================================================

#include "Vertex.h"

#define ReleaseCOM(x) { if(x){ x->Release();x = 0; } }

// Initialize static variables.
IDirect3DVertexDeclaration9* VertexPos::Decl = 0;


void InitAllVertexDeclarations(LPDIRECT3DDEVICE9 pD3DDevice)
{
	//===============================================================
	// VertexPos

	D3DVERTEXELEMENT9 VertexPosElements[] = 
	{
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		D3DDECL_END()
	};	
	pD3DDevice->CreateVertexDeclaration(VertexPosElements, &VertexPos::Decl);
}

void DestroyAllVertexDeclarations()
{
	ReleaseCOM(VertexPos::Decl);
}