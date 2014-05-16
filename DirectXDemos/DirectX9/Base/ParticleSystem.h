#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include "Particle.h"

class __declspec(dllexport) ParticleSystem
{
public:
	ParticleSystem(void);
	virtual ~ParticleSystem(void);

public:
	virtual void Init() = 0 ;
	virtual void Update(float timeDelta) = 0 ;
	virtual void Render() = 0 ;
	virtual void AddParticle() = 0 ;
	virtual void ResetParticle(Particle* particle) = 0 ;
};

#endif // end PARTICLESYSTEM_H
