#include "DXInput.h"

DXInput::DXInput(void)
{
	m_pDIObject = NULL ;
	Init() ;
}

DXInput::~DXInput(void)
{
	Release() ;
}

void DXInput::Release()
{
	// Release keyboard device
	if (m_pDIKeyboardDevice != NULL)
	{
		m_pDIKeyboardDevice->Unacquire() ;
		m_pDIKeyboardDevice->Release() ;
		m_pDIKeyboardDevice = NULL ;
	}

	// Release mouse device
	if (m_pDIMouseDevice != NULL)
	{
		m_pDIMouseDevice->Unacquire() ;
		m_pDIMouseDevice->Release() ;
		m_pDIMouseDevice = NULL ;
	}

	// Release Direct Input object
	if (m_pDIObject != NULL)
	{
		m_pDIObject->Release() ;
		m_pDIObject = NULL ;
	}
}

// Initialize Direct input object
bool DXInput::Init()
{
	// Create Direct Input object
	HRESULT hr = DirectInput8Create( GetModuleHandle(NULL), 
		DIRECTINPUT_VERSION, 
		IID_IDirectInput8, 
		(void **)&m_pDIObject, 
		NULL) ;

	if (FAILED(hr))
	{
		DXTRACE_ERR_MSGBOX(DXGetErrorString(hr), hr) ;
		//ERRORBOX("Create Direct Input object failed!") ;
		return false ;
	}

	// Initialize keyboard
	else if (!InitKeyboard())
	{
		//ERRORBOX("Initialize keyboard failed!") ;
		DXTRACE_ERR_MSGBOX(DXGetErrorString(hr), hr) ;

	}

	// Initialize mouse
	else if (!InitMouse())
	{
		//ERRORBOX("Initialize mouse failed!") ;
		DXTRACE_ERR_MSGBOX(DXGetErrorString(hr), hr) ;

	}

	return true ;
}

// Initialize keyboard device
bool DXInput::InitKeyboard()
{
	// Create keyboard device
	HRESULT hr = m_pDIObject->CreateDevice(GUID_SysKeyboard, &m_pDIKeyboardDevice, NULL) ;
	if (FAILED(hr))
	{
		//ERRORBOX("Create keyboard device failed!") ;
		DXTRACE_ERR_MSGBOX(DXGetErrorString(hr), hr) ;

		return false ;
	}

	// Set keyboard data format
	hr = m_pDIKeyboardDevice->SetDataFormat(&c_dfDIKeyboard) ;
	if(FAILED(hr))
	{
		DXTRACE_ERR_MSGBOX(DXGetErrorString(hr), hr) ;
		return false ;
	}

	// Set keyboard cooperate level
	hr = m_pDIKeyboardDevice->SetCooperativeLevel(GetForegroundWindow(), DISCL_BACKGROUND | DISCL_NONEXCLUSIVE) ;
	if (FAILED(hr))
	{
		DXTRACE_ERR_MSGBOX(DXGetErrorString(hr), hr) ;
		return false ;
	}

	// Acquire for keyboard input
	hr = m_pDIKeyboardDevice->Acquire() ;
	if (FAILED(hr))
	{
		DXTRACE_ERR_MSGBOX(DXGetErrorString(hr), hr) ;
		return false ;
	}

	return true ;
}

bool DXInput::InitMouse()
{
	// Create mouse device
	HRESULT hr = m_pDIObject->CreateDevice(GUID_SysMouse, &m_pDIMouseDevice, NULL) ;
	if(FAILED(hr))
	{
		DXTRACE_ERR_MSGBOX(DXGetErrorString(hr), hr) ;
		return false ;
	}

	// Set mouse data format
	hr = m_pDIMouseDevice->SetDataFormat(&c_dfDIMouse2) ;
	if(FAILED(hr))
	{
		DXTRACE_ERR_MSGBOX(DXGetErrorString(hr), hr) ;
		return false ;
	}

	// Set mouse cooperate level
	hr = m_pDIMouseDevice->SetCooperativeLevel(GetForegroundWindow(),DISCL_BACKGROUND | DISCL_NONEXCLUSIVE) ;
	if(FAILED(hr))
	{
		DXTRACE_ERR_MSGBOX(DXGetErrorString(hr), hr) ;
		return false ;
	}
	
	// Acquire for mouse input
	hr = m_pDIMouseDevice->Acquire() ;
	if(FAILED(hr))
	{
		DXTRACE_ERR_MSGBOX(DXGetErrorString(hr), hr) ;
		return false ;
	}

	return true ;
}

// Update keyboard state
void DXInput::Update()
{
	// Update keyboard
	if (FAILED(m_pDIKeyboardDevice->GetDeviceState(sizeof(m_KeyBuffer), (LPVOID)&m_KeyBuffer)))
	{
		// Keyboard lost, zero out keyboard data structure.
		ZeroMemory(m_KeyBuffer, sizeof(m_KeyBuffer));

		// Try to acquire for next time we poll.
		m_pDIKeyboardDevice->Acquire() ;
		
	}

	// Update mouse
	if(DIERR_INPUTLOST == m_pDIMouseDevice->GetDeviceState(sizeof(m_MouseState),(LPVOID)&m_MouseState))
	{
		ZeroMemory(&m_MouseState, sizeof(m_MouseState)) ;
		m_pDIMouseDevice->Acquire();
	}
}

// Determine whether a key was pressed
bool DXInput::KeyDown(int key)
{
	if (m_KeyBuffer[key] & 0x80)
	{
		return true ;
	}
	return false ;
}

bool DXInput::ButtonDown(int button)
{
	return (m_MouseState.rgbButtons[button] & 0x80) != 0;
}

float DXInput::MouseDX() const
{
	return (float)m_MouseState.lX;
}

float DXInput::MouseDY() const
{
	return (float)m_MouseState.lY;
}