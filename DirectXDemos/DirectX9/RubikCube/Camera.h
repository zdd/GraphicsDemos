#ifndef __CAMERA_H__
#define __CAMERA_H__

#ifndef _WIN32_WINNT
#define _WIN32_WINNT   0x0600
#endif

#include "ArcBall.h"

class Camera
{
public:
	Camera(void);
	~Camera(void);

public:
	void Reset() ;
	void OnFrameMove() ;
	LRESULT HandleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) ;
	void SetViewParams(D3DXVECTOR3* pvEyePt, D3DXVECTOR3* pvLookatPt, D3DXVECTOR3* pvUp);
	void SetProjParams(float fFOV, float fAspect, float fNearPlane, float fFarPlane) ;
	void SetWindow(int nWidth, int nHeight, float fArcballRadius = 0.9f) ;
	const D3DXMATRIX* GetWorldMatrix() const ;
	const D3DXMATRIX* GetViewMatrix() const ;
	const D3DXMATRIX* GetProjMatrix() const ;
	const D3DXVECTOR3* GetEyePoint() const ;
	void SetWorldMatrix(D3DXMATRIX *matModelWorld) ;
	D3DXQUATERNION GetRotationQuat();
	const D3DXMATRIX* GetModelRotMatrix() const ;
private:
	bool m_bDragSinceLastUpdate ;
	float m_fRadius;				// Distance from the camera to model 
	float m_fMaxRadius ;			// The Maximum distance from the camera to the model
	float m_fMinRadius ;			// The Minimum distance from the camera to the model
	int m_nMouseWheelDelta;			// Amount of middle wheel scroll (+/-)
	D3DXVECTOR3 m_vModelCenter ;	// Model center
	
	D3DXVECTOR3 m_vEyePt ;			// Eye position
	D3DXVECTOR3 m_vLookatPt ;		// Look at position
	D3DXVECTOR3 m_vUp ;				// Up vector

	float m_fFOV;					// Field of view
	float m_fAspect;				// Aspect ratio
	float m_fNearPlane;				// Near plane
	float m_fFarPlane;				// Far plane
	
	// 这里是不是有些重复呢，稍后仔细斟酌一下
	D3DXMATRIX m_matView ;			// Camera View matrix
	D3DXMATRIX m_matProj ;			// Camera Projection matrix
	D3DXMATRIX m_matWorld ;			// World matrix of model
	D3DXMATRIX m_matModelRotate ;	// Rotation matrix of model
	D3DXMATRIX m_matModelRotLast ;	// model last rotation matrix
	D3DXMATRIX m_matCameraRotLast ;	// Camera last rotation matrix

	ArcBall m_WorldArcBall ;		// World arc ball 
	ArcBall m_ViewArcBall ;			// View arc ball
};

#endif // end __CAMERA_H__