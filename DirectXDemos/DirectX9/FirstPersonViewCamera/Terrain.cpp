#include "Terrain.h"

Terrain::Terrain(void)
{
	mVB = NULL ;
	mIB = NULL ;
}

Terrain::~Terrain(void)
{
	Release() ;
}

void Terrain::Release()
{
	if (mVB != NULL)
	{
		mVB->Release() ;
		mVB = NULL ;
	}

	if (mIB != NULL)
	{
		mIB->Release() ;
		mIB = NULL ;
	}

	DestroyAllVertexDeclarations() ;
}

void Terrain::GenerateGrids(int numVertRows, int numVertCols,
				float dx, float dz, 
				const D3DXVECTOR3& center, 
				std::vector<D3DXVECTOR3>& verts,
				std::vector<DWORD>& indices)
{
	int numVertices = numVertRows*numVertCols;
	int numCellRows = numVertRows-1;
	int numCellCols = numVertCols-1;

	int numTris = numCellRows*numCellCols*2;

	float width = (float)numCellCols * dx;
	float depth = (float)numCellRows * dz;

	//===========================================
	// Build vertices.

	// We first build the grid geometry centered about the origin and on
	// the xz-plane, row-by-row and in a top-down fashion.  We then translate
	// the grid vertices so that they are centered about the specified 
	// parameter 'center'.

	verts.resize( numVertices );

	// Offsets to translate grid from quadrant 4 to center of 
	// coordinate system.
	float xOffset = -width * 0.5f; 
	float zOffset =  depth * 0.5f;

	int k = 0;
	for(float i = 0; i < numVertRows; ++i)
	{
		for(float j = 0; j < numVertCols; ++j)
		{
			// Negate the depth coordinate to put in quadrant four.  
			// Then offset to center about coordinate system.
			verts[k].x =  j * dx + xOffset;
			verts[k].z = -i * dz + zOffset;
			verts[k].y =  0.0f;

			// Translate so that the center of the grid is at the
			// specified 'center' parameter.
			/*D3DXMATRIX T;
			D3DXMatrixTranslation(&T, center.x, center.y, center.z);
			D3DXVec3TransformCoord(&verts[k], &verts[k], &T);*/

			++k; // Next vertex
		}
	}

	//===========================================
	// Build indices.

	indices.resize(numTris * 3);

	// Generate indices for each quad.
	k = 0;
	for(DWORD i = 0; i < (DWORD)numCellRows; ++i)
	{
		for(DWORD j = 0; j < (DWORD)numCellCols; ++j)
		{
			indices[k]     =   i   * numVertCols + j;
			indices[k + 1] =   i   * numVertCols + j + 1;
			indices[k + 2] = (i+1) * numVertCols + j;

			indices[k + 3] = (i+1) * numVertCols + j;
			indices[k + 4] =   i   * numVertCols + j + 1;
			indices[k + 5] = (i+1) * numVertCols + j + 1;

			// next quad
			k += 6;
		}
	}
}

void Terrain::BuildGridsBuffer(LPDIRECT3DDEVICE9 pd3dDevice)
{
	InitAllVertexDeclarations(pd3dDevice) ;

	std::vector<D3DXVECTOR3> verts;
	std::vector<DWORD> indices;

	GenerateGrids(100, 100, 1.0f, 1.0f, D3DXVECTOR3(0.0f, 0.0f, 0.0f), verts, indices);

	// Save vertex count and triangle count for DrawIndexedPrimitive arguments.
	mNumVertices  = 100 * 100;
	mNumTriangles = 99 * 99 * 2;

	// Obtain a pointer to a new vertex buffer.
	pd3dDevice->CreateVertexBuffer(mNumVertices * sizeof(VertexPos), 
		D3DUSAGE_WRITEONLY,	0, D3DPOOL_MANAGED, &mVB, 0);

	// Now lock it to obtain a pointer to its internal data, and write the
	// grid's vertex data.
	VertexPos* v = 0;
	mVB->Lock(0, 0, (void**)&v, 0);

	for(DWORD i = 0; i < mNumVertices; ++i)
		v[i] = verts[i];

	mVB->Unlock();


	// Obtain a pointer to a new index buffer.
	pd3dDevice->CreateIndexBuffer(mNumTriangles*3*sizeof(WORD), D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16, D3DPOOL_MANAGED, &mIB, 0);

	// Now lock it to obtain a pointer to its internal data, and write the
	// grid's index data.

	WORD* k = 0;
	mIB->Lock(0, 0, (void**)&k, 0);

	for(DWORD i = 0; i < mNumTriangles*3; ++i)
		k[i] = (WORD)indices[i];

	mIB->Unlock();
}

void Terrain::Render(LPDIRECT3DDEVICE9 pd3dDevice)
{
	// Let Direct3D know the vertex buffer, index buffer and vertex 
	// declaration we are using.
	pd3dDevice->SetStreamSource(0, mVB, 0, sizeof(VertexPos));
	pd3dDevice->SetIndices(mIB);
	pd3dDevice->SetVertexDeclaration(VertexPos::Decl);
	pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, mNumVertices, 0, mNumTriangles);
}