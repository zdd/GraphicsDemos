#ifndef DXINPUT_H
#define DXINPUT_H

#include <d3dx9.h>
#include <dinput.h>

// Define left, right and wheel button 
#define BUTTON_LEFT 0
#define BUTTON_RIGHT 1 
#define BUTTON_WHEEL 2 

class DXInput
{
public:
	DXInput(void);
	~DXInput(void);
	bool Init() ;			// Initialize Direct Input
	void Update() ;			// Update keyboard
	bool KeyDown(int key) ;	// Asked for a pressed key
	bool ButtonDown(int button) ;	// Ask for a mouse button press
	void Release() ;

private:
	bool InitKeyboard() ;	// Initialize keyboard
	bool InitMouse() ;		// Initialize mouse

private:
	LPDIRECTINPUT8			m_pDIObject ;
	LPDIRECTINPUTDEVICE8	m_pDIKeyboardDevice ;	// Keyboard device
	LPDIRECTINPUTDEVICE8	m_pDIMouseDevice ;		// Mouse device
	char					m_KeyBuffer[256] ;		// Buffer for keys
	DIMOUSESTATE2			m_MouseState ;			// Mouse state
};

#endif // DXINPUT_H
