#include "Terrain.h"

Terrain::Terrain(void):mquadTree(NULL), mfrustum(NULL), mdrawCount(0)
{
}

Terrain::~Terrain(void)
{
	mquadTree->release();
}

void Terrain::initialize(IDirect3DDevice9* pDevice)
{
	// Initialize quad tree
	mquadTree = new QuadTree();
	mquadTree->initialize(pDevice);

	// Build quad tree
	TreeNode* root = mquadTree->getRoot();
	mquadTree->createTree(root, 0, 0, 64, pDevice);

	// Initialize frustum
	mfrustum = new Frustum();
}

void Terrain::render(IDirect3DDevice9* pDevice, Camera* camera)
{
	// Get view matrix from camera
	D3DXMATRIX matView = camera->getViewMatrix() ;

	// Get projection matrix from camera
	D3DXMATRIX matProj = camera->getProjMatrix() ;

	// Compute frustum
	mfrustum->updatePlanes(matView, matProj);

	mquadTree->render(mfrustum, pDevice);
}