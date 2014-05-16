#include "Particle.h"

Particle::Particle(void):
isLive(true),
lifeTime(1.0f),
age(0.0f),
position(D3DXVECTOR3(0, 0, 10)), 
velocity(D3DXVECTOR3(0, 0, 0)), 
gravity(D3DXVECTOR3(0, -1.8f, 0)),
initVelocity(D3DXVECTOR3(0, 0, 0))
{
}

Particle::~Particle(void)
{
}
