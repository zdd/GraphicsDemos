#include "QuadTree.h"
#include "stdio.h"

int TreeNode::NODEID = 0;
QuadTree::QuadTree(void):mdx(1.0f), mdz(1.0f), mdrawCount(0)
{
}

QuadTree::~QuadTree(void)
{
	release();
}

void QuadTree::initialize(IDirect3DDevice9* device)
{
	// Build the quad tree
	mRoot = new TreeNode();	
}

// This version use mesh to draw
// Note, the radius here is the half width of the terrain, that means
// the terrain center is (centerX, centerZ) and radius is the distance from center to the edge
bool QuadTree::createTree(TreeNode* root, int centerX, int centerZ, int radius, IDirect3DDevice9* device)
{
	// Initialize root node
	root->mcenterX = centerX;
	root->mcenterZ = centerZ;
	root->mwidth = radius;

	if (radius > TreeNode::MAXVERTEX)
	{
		root->hasChild = true;

		for (int i  = 0; i < 4; ++i)
		{
			// Calculate the centerX,  centerZ and width of the sub nodes
			int subCenterX = (((i % 2) < 1) ? -1 : 1) * (radius / 2) + centerX;
			int subCenterZ = (((i % 4) < 2) ? -1 : 1) * (radius / 2) + centerZ;
			int subRadius = radius / 2;

			root->nodes[i] = new TreeNode();
			createTree(root->nodes[i], subCenterX, subCenterZ, subRadius, device);
		}
	}

	else
	{
		root->hasChild = false ;
		root->id = TreeNode::NODEID++;

		// Calculate number of vertex
		int vertexCount = (radius * 2 + 1) * (radius * 2 + 1);
		int triangleCount = (radius * 2) * (radius * 2) * 2;
		int indexCount = triangleCount * 3;

		root->mvertexCount = vertexCount;
		root->mtriangleCount = triangleCount;

		// Create vertex array
		Vertex* vertices = new Vertex[vertexCount];

		// vertex count
		int k = 0;

		// Start and end coordinates of a quad
		int startX = centerX - radius;
		int endX   = centerX + radius;
		int startZ = centerZ - radius;
		int endZ   = centerZ + radius;

		// Fill the vertex array
		// The rows go from bottom to top
		for (int i = startZ; i <= endZ ; ++i)
		{
			// The column go from left to right
			for (int j = startX; j <= endX; ++j)
			{
				vertices[k].position.x = (float)j;
				vertices[k].position.z = (float)i;
				vertices[k].position.y = 0.0f;

				++k;
			}
		}

		// Create index array
		DWORD* indices = new DWORD[triangleCount * 3];
		
		// index array index
		k = 0;

		DWORD numCellsperRow = 2 * radius;
		DWORD numCellsperCol = 2 * radius;

		DWORD numVertexperRow = numCellsperRow + 1;
		DWORD numVertexperCol = numCellsperCol + 1;

		// Fill the index array
		for (DWORD i = 0; i < numCellsperRow; ++i)
		{
			for (DWORD j = 0; j < numCellsperCol; ++j)
			{
				indices[k]     =       i * numVertexperCol + j;			// 0
				indices[k + 1] =	   i * numVertexperCol + (j + 1);	// 1
				indices[k + 2] = (i + 1) * numVertexperCol + j;			// 2


				indices[k + 3] = (i + 1) * numVertexperCol + j;			// 3
				indices[k + 4] =       i * numVertexperCol + (j + 1);	// 4
				indices[k + 5] = (i + 1) * numVertexperCol + (j + 1);	// 5

				// next quad
				k += 6;
			}
		}

		#define VERTEX_FVF  D3DFVF_XYZ

		// Create mesh
		HRESULT hr = D3DXCreateMeshFVF(
			root->mtriangleCount, 
			root->mvertexCount, 
			D3DXMESH_MANAGED | D3DXMESH_32BIT,
			VERTEX_FVF, 
			device,
			&root->mmesh);
		if (FAILED(hr))
		{
			return false;
		}
	
		// Lock mesh vertex buffer and copy data
		VOID* pVertices;
		if( FAILED( root->mmesh->LockVertexBuffer(0, (void**)&pVertices) ) )
			return false;
		memcpy( pVertices, vertices, vertexCount * sizeof(Vertex) );
		root->mmesh->UnlockVertexBuffer();

		// Delete vertices array
		delete []vertices;
		vertices = NULL;

		// Lock mesh index buffer and copy data
		DWORD *pIndices;
		if( FAILED( root->mmesh->LockIndexBuffer(0, (void **)&pIndices) ) )
			return false;
		memcpy(pIndices, indices, indexCount * sizeof(DWORD) );
		root->mmesh->UnlockIndexBuffer();

		// Delete indices array
		delete []indices;
		indices = NULL;
	}

	return true;
}

// Output a float
void OutputInt(int i)
{
	CHAR textBuffer[100] ;

	//_snprintf(textBuffer, sizeof(textBuffer), "%s%i\n", "i:", i);
	_snprintf_s(textBuffer, sizeof(textBuffer), _TRUNCATE, "%s%i\n", "i:", i);

	OutputDebugString(textBuffer) ;
}

void OutputVector(const D3DXVECTOR3& vec3)
{
	CHAR textBuffer[100] ;
	//_snprintf(textBuffer, sizeof(textBuffer), "%s%f %s%f %s%f \n", "x:", vec3.x, "y:", vec3.y, "z:", vec3.z);
	_snprintf_s(textBuffer, sizeof(textBuffer), _TRUNCATE, "%s%f %s%f %s%f \n", "x:", vec3.x, "y:", vec3.y, "z:", vec3.z);
	OutputDebugString(textBuffer) ;
}

void QuadTree::renderNode( TreeNode* node, Frustum* frustum, IDirect3DDevice9* device )
{
	//// Build AABB for node
	//D3DXVECTOR3 boxMin(
	//	node->mcenterX - node->mwidth, 
	//	0, 
	//	node->mcenterZ - node->mwidth
	//	);
	//D3DXVECTOR3 boxMax(
	//	node->mcenterX + node->mwidth, 
	//	0, 
	//	node->mcenterZ + node->mwidth
	//	);
	//AABB box(boxMin, boxMax);

	//// Determine whether current node was visible, if not visible, return.
	//bool isVisible = frustum->isVisable(box);

	D3DXVECTOR3 center((float)node->mcenterX, 0.0f, (float)node->mcenterZ);
	float radius = (float)node->mwidth;

	bool isVisible = frustum->isVisable(center, radius);
	if (!isVisible)
	{
		return;
	}

	// if has child node, render child nodes, can we use node->node[0] != NULL?
	if (node->hasChild)
	{
		// Render child nodes
		for (int i = 0; i < 4; ++i)
		{
			renderNode(node->nodes[i], frustum, device);
		}
	}

	else // render current node
	{
		node->mmesh->DrawSubset(0);
		mdrawCount += node->mtriangleCount;
		device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	}
}

// Render the entire tree
void QuadTree::render( Frustum* frustum, IDirect3DDevice9* device )
{
	mdrawCount = 0;
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	renderNode(mRoot, frustum, device);
}

void QuadTree::releaseNode(TreeNode* node)
{
	if (node->hasChild)
	{
		for (int i = 0; i < 4; ++i)
		{
			releaseNode(node->nodes[i]);
		}
	}
	else
	{
		// Release node mesh
		if (node->mmesh)
		{
			node->mmesh->Release();
			node->mmesh = NULL;
		}

		// Delete node
		delete node;
		node = NULL;
	}
}

void QuadTree::release()
{
	if (mRoot)
	{
		releaseNode(mRoot);
		delete mRoot;
		mRoot = NULL;
	}
}

/*
This version use vertex and index buffer to draw
*/
bool QuadTree::createTree1( TreeNode* root, int centerX, int centerZ, int radius, IDirect3DDevice9* device )
{
	// Initialize root node
	root->mcenterX = centerX;
	root->mcenterZ = centerZ;
	root->mwidth = radius;

	if (radius > TreeNode::MAXVERTEX)
	{
		root->hasChild = true;

		for (int i  = 0; i < 4; ++i)
		{
			// Calculate the centerX,  centerZ and width of the sub nodes
			int subCenterX = (((i % 2) < 1) ? -1 : 1) * (radius / 2) + centerX;
			int subCenterZ = (((i % 4) < 2) ? -1 : 1) * (radius / 2) + centerZ;
			int subRadius = radius / 2;

			root->nodes[i] = new TreeNode();
			createTree1(root->nodes[i], subCenterX, subCenterZ, subRadius, device);
		}
	}

	else
	{
		root->hasChild = false ;
		root->id = TreeNode::NODEID++;
		OutputDebugString("node: ");
		OutputInt(root->id);

		// Calculate number of vertex
		int vertexCount = (radius * 2 + 1) * (radius * 2 + 1);
		int triangleCount = (radius * 2) * (radius * 2) * 2;
		int indexCount = triangleCount * 3;

		root->mvertexCount = vertexCount;
		root->mtriangleCount = triangleCount;

		// Create vertex array
		Vertex* vertices = new Vertex[vertexCount];

		// vertex count
		int k = 0;

		// Start and end coordinates of a quad
		int startX = centerX - radius;
		int endX   = centerX + radius;
		int startZ = centerZ - radius;
		int endZ   = centerZ + radius;

		// Fill the vertex array
		// The rows go from bottom to top
		for (int i = startZ; i <= endZ ; ++i)
		{
			// The column go from left to right
			for (int j = startX; j <= endX; ++j)
			{
				vertices[k].position.x = (float)j;
				vertices[k].position.z = (float)i;
				vertices[k].position.y = 0.0f;

				++k;

				D3DXVECTOR3 vec3((float)j, 0.0f, (float)i);
				OutputVector(vec3);
			}
		}

		// Create index array
		DWORD* indices = new DWORD[triangleCount * 3];

		// index array index
		k = 0;

		DWORD numCellsperRow = 2 * radius;
		DWORD numCellsperCol = 2 * radius;

		DWORD numVertexperRow = numCellsperRow + 1;
		DWORD numVertexperCol = numCellsperCol + 1;

		// Fill the index array

		for (DWORD i = 0; i < numCellsperRow; ++i)
		{
			for (DWORD j = 0; j < numCellsperCol; ++j)
			{
				indices[k]     =       i * numVertexperCol + j;			// 0
				indices[k + 1] =	   i * numVertexperCol + (j + 1);	// 1
				indices[k + 2] = (i + 1) * numVertexperCol + j;			// 2


				indices[k + 3] = (i + 1) * numVertexperCol + j;			// 3
				indices[k + 4] =       i * numVertexperCol + (j + 1);	// 4
				indices[k + 5] = (i + 1) * numVertexperCol + (j + 1);	// 5

				// next quad
				k += 6;
			}
		}

		// Create vertex buffer
#define D3D_FVF  D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2
		if( FAILED( device->CreateVertexBuffer( vertexCount * sizeof(Vertex),
			D3DUSAGE_WRITEONLY, 
			D3D_FVF,
			D3DPOOL_MANAGED, 
			&(root->mvertexBuffer), 
			NULL ) ) )
		{
			return false;
		}

		// Lock vertex buffer and copy data
		VOID* pVertices;
		if( FAILED( root->mvertexBuffer->Lock( 0, 0, (void**)&pVertices, 0 ) ) )
			return false;
		memcpy( pVertices, vertices, vertexCount * sizeof(Vertex) );
		root->mvertexBuffer->Unlock();

		// Delete vertices array
		delete []vertices;
		vertices = NULL;

		// Create index buffer
		if( FAILED( device->CreateIndexBuffer( indexCount * sizeof(DWORD), 
			D3DUSAGE_WRITEONLY, 
			D3DFMT_INDEX32, 
			D3DPOOL_MANAGED, 
			&(root->mindexBuffer), 
			0) ) )
		{
			return false ;
		}

		// Lock index buffer and copy data
		DWORD *pIndices;
		if( FAILED( root->mindexBuffer->Lock( 0, 0, (void **)&pIndices, 0) ) )
			return false;
		memcpy(pIndices, indices, indexCount * sizeof(DWORD) );
		root->mindexBuffer->Unlock() ;

		// Delete indices array
		delete []indices;
		indices = NULL;
	}

	return true;
}