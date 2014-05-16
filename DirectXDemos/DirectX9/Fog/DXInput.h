#ifndef DXINPUT_H
#define DXINPUT_H

#include <d3dx9.h>

#define DIRECTINPUT_VERSION 0x0800 
#include <dinput.h>

#include <DxErr.h>

#define SAFE_RELEASE(p) { if (p) { (p)->Release(); (p) = NULL; } }

// This Macro wraps the Trace function to show error box
#define ERRORBOX(msg) 

// Define left, right and wheel button 
#define BUTTON_LEFT 0
#define BUTTON_RIGHT 1 
#define BUTTON_WHEEL 2 

class DXInput
{
public:
	DXInput(void);
	~DXInput(void);
	void Update() ;					// Update keyboard
	bool KeyDown(int key) ;			// Is a keyboard key pressed?
	bool ButtonDown(int button) ;	// Is a mouse button down?
	float MouseDX() const;
	float MouseDY() const;

private:
	bool Init() ;			// Initialize Direct Input
	bool InitKeyboard() ;	// Initialize keyboard
	bool InitMouse() ;		// Initialize mouse
	void Release() ;

private:
	LPDIRECTINPUT8			m_pDIObject ;
	LPDIRECTINPUTDEVICE8	m_pDIKeyboardDevice ;	// Keyboard device
	LPDIRECTINPUTDEVICE8	m_pDIMouseDevice ;		// Mouse device
	char					m_KeyBuffer[256] ;		// Buffer for keys
	DIMOUSESTATE2			m_MouseState ;			// Mouse state
};

#endif // DXINPUT_H
