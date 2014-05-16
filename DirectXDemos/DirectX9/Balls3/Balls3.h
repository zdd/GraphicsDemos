#ifndef BALLS_H
#define BALLS_H

#include <vector>

#include "ParticleSystem.h"
#include "Utility.h"

#define SAFE_RELEASE(P) if(P){ P->Release(); P = NULL;}

using namespace std ;

class Balls : public ParticleSystem
{
public:
	Balls(IDirect3DDevice9* pDevice);
	~Balls(void);
	void Init() ;
	void Init(D3DXVECTOR3* pos) ;
	void Update(float timeDelta) ;
	void Update(float timeDelta, D3DXVECTOR3* pos) ;
	void Update(float timeDelta, D3DXVECTOR3* pos, D3DXVECTOR3* velocity) ;
	void Render() ;
	void ResetParticle(Particle* particle) ;
	void ResetParticle(Particle* particle, D3DXVECTOR3* pos) ;
	void ResetParticle(Particle* particle, D3DXVECTOR3* pos, D3DXVECTOR3* velocity) ;
	void AddParticle() ;
	void AddParticle(D3DXVECTOR3* pos) ;

private:
	vector<Particle> buffer ;
	static const int maxParticleNum = 200 ;

	IDirect3DDevice9* device ;
	ID3DXMesh*	mesh ;
};

#endif // end BALLS_H