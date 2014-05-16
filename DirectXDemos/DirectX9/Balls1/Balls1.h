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
	void Update(float timeDelta) ;
	void Render() ;
	void ResetParticle(Particle* particle) ;
	void AddParticle() ;

private:
	vector<Particle> buffer ;
	static const int maxParticleNum = 100 ;

	IDirect3DDevice9* device ;
	ID3DXMesh*	mesh ;
};

#endif // end BALLS_H