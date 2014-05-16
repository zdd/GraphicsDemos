#ifndef BASECAMERA_H
#define BASECAMERA_H

#include <d3dx9.h>
#include "ArcBall.h"

enum CAMERA_KEYS
{
	MOVE_LEFT = 0 ,
	MOVE_RIGHT ,
	MOVE_FORWARD ,
	MOVE_BACKFORWARD ,
	MOVE_UP ,
	UNKOWN = 0xFF ,
};

class BaseCamera
{
public:
	BaseCamera(void);
	~BaseCamera(void);

public:
	void SetWorldMatrix(D3DXMATRIX* matWorld) ;
	void SetViewParams(D3DXVECTOR3* pvEyePt, D3DXVECTOR3* pvLookatPt, D3DXVECTOR3* pvUp);
	void SetProjParams(float fFOV, float fAspect, float fNearPlane, float fFarPlane) ;
	const D3DXMATRIX* GetWorldMatrix() const ;
	const D3DXMATRIX* GetViewMatrix() const ;
	const D3DXMATRIX* GetProjMatrix() const ;
	void SetPosition(D3DXVECTOR3* vPos) ;
	const D3DXVECTOR3* GetPosition() const ;
	const D3DXVECTOR3* GetLookAt() const ;
	const CAMERA_KEYS MapKey(UINT key) const ;

protected:
	D3DXVECTOR3 m_vEyePoint ;		// Eye position
	D3DXVECTOR3 m_vLookAt ;			// Look at position
	D3DXVECTOR3 m_vUpVector ;		// Up vector

	float m_fFOV;					// Field of view
	float m_fAspect;				// Aspect ratio
	float m_fNearPlane;				// Near plane
	float m_fFarPlane;				// Far plane
	
	D3DXMATRIX m_matView ;			// Camera View matrix
	D3DXMATRIX m_matProj ;			// Camera Projection matrix
	D3DXMATRIX m_matWorld ;			// World matrix of model
};

#endif // BASECAMERA_H