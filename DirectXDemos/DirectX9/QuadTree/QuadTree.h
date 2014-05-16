#ifndef __QUADTREE_H__
#define __QUADTREE_H__

#include <d3dx9.h>
#include <limits.h>
#include "Frustum.h"
#include "AABB.h"

// Tree node for quad tree, why this struct cannot put into the QuadTree class as
// a private member? it will cause compile error!, fuck!
struct TreeNode 
{
	TreeNode():
		mvertexBuffer(NULL), 
		mindexBuffer(NULL), 
		mmesh(NULL),
		mtriangleCount(0),
		mcenterX(0),
		mcenterZ(0),
		mwidth(-1),
		hasChild(false),
		id(-1)
	{

	}

	// Vertex buffer to hold the vertex in the node
	IDirect3DVertexBuffer9* mvertexBuffer;

	// Index buffer
	IDirect3DIndexBuffer9* mindexBuffer;

	// Mesh
	ID3DXMesh* mmesh;

	// Number of vertex in this node
	int mvertexCount;

	// Number of triangles in this node
	int mtriangleCount;

	// Center of the node, we regardless the y coordinates, since 
	// the terrain was on the XOZ plane, and we don't use y for frustum clip
	int mcenterX;
	int mcenterZ;

	// width of the node mesh, it's a square, so we just use one variable here
	int mwidth;
	bool hasChild;

	// for debug
	int id;

	// Max vertex count in node
	static const int MAXVERTEX = 8;
	static int NODEID;

	// Four children nodes
	TreeNode* nodes[4];
};

class QuadTree
{
public:
	QuadTree(void);
	virtual ~QuadTree(void);
	
	void initialize(IDirect3DDevice9* device);
	bool createTree(TreeNode* root, int centerX, int centerZ, int width, IDirect3DDevice9* device);
	bool createTree1(TreeNode* root, int centerX, int centerZ, int width, IDirect3DDevice9* device);
	void renderNode(TreeNode* root, Frustum* frustum, IDirect3DDevice9* device);
	void render(Frustum* frustum, IDirect3DDevice9* device);
	void releaseNode(TreeNode* node);
	void release();

	inline int getDrawCount() const
	{
		return mdrawCount;
	}

	inline TreeNode* getRoot() const
	{
		return mRoot;
	}

private:
	
	// Vertex format for quad tree
	struct Vertex
	{
		D3DXVECTOR3 position;
		//D3DXVECTOR3 normal;
		//D3DXVECTOR2 texture;
	};

#define Vertex_FVF D3DFVF_XYZ

	TreeNode*	mRoot;			// root of the quad tree
	float		mTriangleCount; // Number of triangles in quad tree
	float		mdx;			// distance of two adjacent vertex in x coordinates
	float		mdz;			//	distance of two adjacent vertex in z coordinates
	int  	    mdrawCount;		// How many triangles currently rendering
};

#endif // __QUADTREE_H__