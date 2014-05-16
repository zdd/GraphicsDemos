#ifndef PARTICLE_H
#define PARTICLE_H

#pragma warning(disable: 4251)

#include <d3dx9.h>

class __declspec(dllexport) Particle
{
public:
	Particle(void);
	virtual ~Particle(void);

public:
	bool isLive	;				// Draw particle when isLive is true
	float lifeTime ;			// How long the particle will last
	float age ;					// Time since the particle was born, if age > lifeTime, particle was dead

	D3DXVECTOR3 position ;		// Current position 
	D3DXVECTOR3 velocity ;		// Current velocity
	D3DXVECTOR3 gravity;		// g = 9.8
	D3DXVECTOR3 initVelocity;	// Initial velocity
};

#endif // end PARTICLE_H