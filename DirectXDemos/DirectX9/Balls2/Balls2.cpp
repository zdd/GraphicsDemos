#include "Balls2.h"

Balls::Balls(IDirect3DDevice9* pDevice):mesh(NULL)
{
	device = pDevice ;
	D3DXCreateSphere(device, 0.5f, 10, 10, &mesh, NULL) ;
}

Balls::~Balls(void)
{
	SAFE_RELEASE(mesh) ;
}

void Balls::Init()
{
	for (int i = 0; i < maxParticleNum; ++i)
	{
		AddParticle() ;
	}
}

void Balls::Update(float timeDelta)
{
	for (vector<Particle>::iterator citor = buffer.begin(); citor != buffer.end(); ++citor)
	{
		if (citor->isLive) // Only update live particles
		{
			citor->position += timeDelta * citor->velocity * 20.0f;
			citor->age += timeDelta ;
			if (citor->age > citor->lifeTime)
			{
				citor->isLive = false ;
			}
		}
		else
		{	
			ResetParticle((Particle*)(&(*citor))) ;
		}
	}
}

void Balls::Update(float timeDelta, D3DXVECTOR3* pos)
{
	for (vector<Particle>::iterator citor = buffer.begin(); citor != buffer.end(); ++citor)
	{
		if (citor->isLive) // Only update live particles
		{
			citor->position += timeDelta * citor->velocity * 30.0f;
			citor->age += timeDelta ;
			if (citor->age > citor->lifeTime)
			{
				citor->isLive = false ;
			}
		}
		else
		{	
			ResetParticle((Particle*)(&(*citor)), pos) ;
		}
	}
}

void Balls::Render()
{
	for (vector<Particle>::iterator citor = buffer.begin(); citor != buffer.end(); ++citor)
	{
		if (citor->isLive) // Only draw live particles
		{
			D3DXMATRIX word ;
			D3DXMatrixIdentity(&word) ;
			word._41 = citor->position.x ;
			word._42 = citor->position.y ;
			word._43 = citor->position.z ;
			device->SetTransform(D3DTS_WORLD, &word) ;
			mesh->DrawSubset(0) ;
		}
	}
}

void Balls::ResetParticle(Particle *particle)
{
	particle->isLive = true ;
	particle->age = 0.0f ;
	particle->lifeTime = GetRandomFloat(0.1f, 2.0f) ;
	particle->position = D3DXVECTOR3(0, 0, 20) ;

	D3DXVECTOR3 min = D3DXVECTOR3(-1.0f, -1.0f, -1.0f);
	D3DXVECTOR3 max = D3DXVECTOR3( 1.0f,  1.0f,  1.0f);
	GetRandomVector(&particle->velocity, &min, &max);

	// normalize to make spherical
	D3DXVec3Normalize(&particle->velocity, &particle->velocity);

	particle->velocity ;
	float radius = GetRandomFloat(0.1f, 0.5f) ;
}

void Balls::ResetParticle(Particle* particle, D3DXVECTOR3* pos)
{
	particle->isLive = true ;
	particle->age = 0.0f ;
	particle->lifeTime = GetRandomFloat(0.1f, 1.0f) ;
	particle->position = *pos ;

	D3DXVECTOR3 min = D3DXVECTOR3(-1.0f, -1.0f, -1.0f);
	D3DXVECTOR3 max = D3DXVECTOR3( 1.0f,  1.0f,  1.0f);
	GetRandomVector(&particle->velocity, &min, &max);

	// normalize to make spherical
	D3DXVec3Normalize(&particle->velocity, &particle->velocity);

	particle->velocity ;
	float radius = GetRandomFloat(0.1f, 0.5f) ;
}

void Balls::AddParticle()
{
	Particle particle ;
	ResetParticle(&particle) ;
	buffer.push_back(particle) ;
}