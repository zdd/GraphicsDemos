#include "Balls0.h"

Balls::Balls(IDirect3DDevice9* pDevice):mesh(NULL)
{
	vbSize = 2048 ;
	vbOffset = 0 ;
	vbBatchSize = 256 ;

	device = pDevice ;
	D3DXCreateSphere(device, 0.5f, 10, 10, &mesh, NULL) ;
}

Balls::~Balls(void)
{
	SAFE_RELEASE(mesh) ;
	SAFE_RELEASE(texture) ;
	SAFE_RELEASE(pVB) ;
}

void Balls::Init()
{
	// Create vertex buffer
	device->CreateVertexBuffer(
		vbSize * sizeof(POINTVERTEX),
		D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
		D3DFVF_POINTVERTEX,
		D3DPOOL_DEFAULT, // D3DPOOL_MANAGED can't be used with D3DUSAGE_DYNAMIC 
		&pVB,
		0);

	// Create texture
	D3DXCreateTextureFromFile(device, "../Common/Media/particle.bmp", &texture) ;
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
			ResetParticle((Particle*)(&(*citor))) ;
	}
}

void Balls::Update(float timeDelta, int numParticletoEmit)
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
			ResetParticle((Particle*)(&(*citor))) ;
	}

	// Emit new particle
	for (int i = 0 ; i < numParticletoEmit && buffer.size() < vbSize; ++i)
	{
		AddParticle() ;
	}
}

void Balls::Update(float timeDelta, int numParticletoEmit, D3DXVECTOR3* EmitPosition)
{
	for (vector<Particle>::iterator citor = buffer.begin(); citor != buffer.end(); ++citor)
	{
		if (citor->isLive) // Only update live particles
		{
			
			citor->age += timeDelta ;
			citor->position.x = citor->velocity.x * citor->age * 10.0f;
			citor->position.y = citor->velocity.y * citor->age * 10.0f + citor->gravity.y * citor->age * citor->age * 10.0f ;

			if (citor->age > citor->lifeTime)
			{
				citor->isLive = false ;
			}
		}
		else
			ResetParticle((Particle*)(&(*citor)), EmitPosition) ;
	}

	// Emit new particle
	for (int i = 0 ; i < numParticletoEmit && buffer.size() < vbSize; ++i)
	{
		Particle particle ;
		ResetParticle(&particle, EmitPosition) ;
		buffer.push_back(particle) ;
	}
}

void Balls::Render()
{
	// Set render state
	device->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
	device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
	device->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE) ;
	device->SetRenderState( D3DRS_POINTSCALEENABLE, TRUE) ;
	device->SetRenderState( D3DRS_POINTSIZE,	 FloatToDword(0.5f) );
	device->SetRenderState( D3DRS_POINTSIZE_MIN, FloatToDword(0.00f) );
	device->SetRenderState( D3DRS_POINTSCALE_A,  FloatToDword(0.00f) );
	device->SetRenderState( D3DRS_POINTSCALE_B,  FloatToDword(0.00f) );
	device->SetRenderState( D3DRS_POINTSCALE_C,  FloatToDword(1.00f) );

	// Set texture
	device->SetTexture(0, texture) ;
	device->SetStreamSource( 0, pVB, 0, sizeof(POINTVERTEX));
	device->SetFVF(D3DFVF_POINTVERTEX) ;

	// Start at beginning if we reach the end of vb
	if(vbOffset >= vbSize)
		vbOffset = 0 ;

	POINTVERTEX* v ;

	pVB->Lock(
		vbOffset * sizeof(POINTVERTEX),
		vbBatchSize * sizeof(POINTVERTEX),
		(void**)&v,
		vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD
		) ;

	DWORD numParticleinBatch = 0 ;

	for (vector<Particle>::iterator citor = buffer.begin(); citor != buffer.end(); ++citor)
	{
		if (citor->isLive) // Only draw live particles
		{
			v->pos = citor->position ;
			v->color = 0xff00ff00 ;
			v++ ;

			numParticleinBatch++ ;

			if (numParticleinBatch == vbBatchSize)
			{
				pVB->Unlock() ;
				device->DrawPrimitive( D3DPT_POINTLIST, vbOffset, vbBatchSize) ;
				
				vbOffset += vbBatchSize ;

				if (vbOffset >= vbSize)
					vbOffset = 0 ;

				pVB->Lock(
					vbOffset * sizeof(POINTVERTEX),
					vbBatchSize * sizeof(POINTVERTEX),
					(void**)&v,
					vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD
					) ;

				numParticleinBatch = 0 ;
			}
		}
	}

	pVB->Unlock() ;

	// Render the left particles
	if (numParticleinBatch)
	{
		device->DrawPrimitive(
			D3DPT_POINTLIST,
			vbOffset,
			numParticleinBatch
			) ;
	}

	// Restore state
	device->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
}

void Balls::ResetParticle(Particle* particle)
{
	particle->isLive = true ;
	particle->age = 0.0f ;
	particle->lifeTime = 5.0f ;
	particle->position = D3DXVECTOR3(0, 0, 20) ;

	D3DXVECTOR3 min = D3DXVECTOR3(-1.0f, -1.0f, -1.0f);
	D3DXVECTOR3 max = D3DXVECTOR3( 1.0f,  1.0f,  1.0f);

	GetRandomVector(&particle->velocity, &min, &max);

	// normalize to make spherical
	D3DXVec3Normalize(&particle->velocity, &particle->velocity);

	// Normalize gravity
	D3DXVec3Normalize(&particle->gravity, &particle->gravity);
}

void Balls::ResetParticle(Particle* particle, D3DXVECTOR3* pos)
{
	particle->isLive = true ;
	particle->age = 0.0f ;
	particle->lifeTime = 5.0f ; /*GetRandomFloat(0.1f, 1.0f) ;*/
	particle->position = *pos ;

	D3DXVECTOR3 min = D3DXVECTOR3(-1.0f, -1.0f, -1.0f);
	D3DXVECTOR3 max = D3DXVECTOR3( 1.0f,  1.0f, 1.0f);
	GetRandomVector(&particle->velocity, &min, &max);

	// normalize to make spherical
	D3DXVec3Normalize(&particle->velocity, &particle->velocity);
	particle->velocity.z = 0.0f;

	// Normalize gravity
	D3DXVec3Normalize(&particle->gravity, &particle->gravity);
}

void Balls::AddParticle()
{
	Particle particle ;
	ResetParticle(&particle) ;
	buffer.push_back(particle) ;
}