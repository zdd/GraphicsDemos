#ifndef BALLS_H
#define BALLS_H

#include <vector>

#include "ParticleSystem.h"
#include "Utility.h"

#define SAFE_RELEASE(P) if(P){ P->Release(); P = NULL;}

using namespace std ;


struct POINTVERTEX
{
	D3DXVECTOR3 pos ;	// position
	DWORD color ;		// color
};

#define D3DFVF_POINTVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE) 

class Balls : public ParticleSystem
{
public:
	Balls(IDirect3DDevice9* pDevice);
	~Balls(void);
	void Init() ;
	void Update(float timeDelta) ;
	void Update(float timeDelta, int numParticletoEmit) ;
	void Update(float timeDelta, int numParticletoEmit, D3DXVECTOR3* EmitPosition) ;
	void Render() ;
	void AddParticle() ;
	void ResetParticle(Particle* particle);
	void ResetParticle(Particle* particle, D3DXVECTOR3* pos) ;

private:
	vector<Particle> buffer ;
	static const int maxParticleNum = 100 ;

	IDirect3DDevice9* device ;
	ID3DXMesh*	mesh ;
	IDirect3DTexture9* texture ;
	LPDIRECT3DVERTEXBUFFER9 pVB ;

	unsigned int vbSize ;
	unsigned int vbOffset ;
	unsigned int vbBatchSize ;
};

#endif // end BALLS_H