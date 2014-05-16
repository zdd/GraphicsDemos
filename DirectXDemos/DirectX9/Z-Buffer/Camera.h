#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <d3dx9.h>

class Camera
{
public:
	Camera(void);
	~Camera(void);
	void GetViewMatrix(D3DXMATRIX *view) ;
	void Walk(float unit) ;
	void Strafe(float unit) ;
	LRESULT HandleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) ;

private:
	D3DXVECTOR3 m_vPos ;	// Camera position
	D3DXVECTOR3 m_vRight ;	// The right vector,	x-axis
	D3DXVECTOR3 m_vUp ;		// The up vector,		y-axis // NO USE for landscape camera
	D3DXVECTOR3 m_vLook ;	// The look vector,		z-axis
};

#endif // __CAMERA_H__