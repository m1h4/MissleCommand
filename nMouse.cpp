#include "nGlobals.h"
#include "nMain.h"
#include "nMouse.h"

nMouse::nMouse(void) :
m_pDevice(NULL),
m_Smooth(true)	// Default
{
}

nMouse::~nMouse(void)
{
	CleanMouse();
}

bool nMouse::InitMouse()
{
	HRESULT hr = nGetInstance()->GetInput()->GetDirectInput()->CreateDevice(GUID_SysMouse,(LPDIRECTINPUTDEVICE8*)&m_pDevice,0);
	if(FAILED(hr))
		return false;

	// Set the data format that we will be retriving when we will be reading the state of the device
	hr = m_pDevice->SetDataFormat(&c_dfDIMouse2);
	if(FAILED(hr))
		return false;

	// Set the cooperate levels to exclusive
	hr = m_pDevice->SetCooperativeLevel(nGetInstance()->GetWindowHandle(),DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if(FAILED(hr))
		return false;

	// At last acquire the device for input reading
	hr = m_pDevice->Acquire();
	if(FAILED(hr))
		return false;

	// Get device instance
	DIDEVICEINSTANCE inst;
	inst.dwSize = sizeof(inst);
	m_pDevice->GetDeviceInfo(&inst);

	m_DeviceName = inst.tszProductName;

	// Zero the mouse states
	ZeroMemory(m_ButtonMask,sizeof(m_ButtonMask));
	ZeroMemory(m_Positions,sizeof(m_Positions));

	return true;
}

void nMouse::CleanMouse()
{
	if(m_pDevice)
		m_pDevice->Unacquire();

	nSafeRelease(m_pDevice);
}

bool nMouse::Update()
{
	nAssert(m_pDevice);

	// The structure that will hold the mouse data
	DIMOUSESTATE2 dims2;

	// Get the mouse data
	HRESULT hr = m_pDevice->GetDeviceState(sizeof(DIMOUSESTATE2),&dims2);
	if(hr != S_OK)
	{
		// Keep trying to reacquire the device
		hr = m_pDevice->Acquire();
        while(hr == DIERR_INPUTLOST) 
			hr = m_pDevice->Acquire();

		return false;
	}

	m_Positions[0] = m_Positions[1];

	m_Positions[1].x += (float)dims2.lX;
	m_Positions[1].y += (float)dims2.lY;

	// Clip the cursor pos if it's outside the screen
	if(m_Positions[1].x < 0.0f)
		m_Positions[1].x = 0.0f;

	else if(m_Positions[1].x > nGetInstance()->GetGraphics()->GetDisplaySize().cx)
		m_Positions[1].x = nGetInstance()->GetGraphics()->GetDisplaySize().cx;

	if(m_Positions[1].y < 0.0f)
		m_Positions[1].y = 0.0f;

	else if(m_Positions[1].y > nGetInstance()->GetGraphics()->GetDisplaySize().cy)
		m_Positions[1].y = nGetInstance()->GetGraphics()->GetDisplaySize().cy;

	m_ButtonMask[0] = m_ButtonMask[1];
	m_ButtonMask[1] = NULL;

	// Return mouse button states
	if(((BYTE)dims2.rgbButtons[0] >> 4) != 0)
		m_ButtonMask[1] |= ButtonLeft;

	if(((BYTE)dims2.rgbButtons[1] >> 4) != 0)
		m_ButtonMask[1] |= ButtonRight;

	if(((BYTE)dims2.rgbButtons[2] >> 4) != 0)
		m_ButtonMask[1] |= ButtonMiddle;

	return true;
}

nString nMouse::GetButtonName(Button button) const
{
	nAssert(m_pDevice);

	DIDEVICEOBJECTINSTANCE doi;
	ZeroMemory(&doi,sizeof(DIDEVICEOBJECTINSTANCE));
	doi.dwSize = sizeof(DIDEVICEOBJECTINSTANCE);
	unsigned long dwOfs = DIMOFS_BUTTON0;

	switch(button)
	{
	case ButtonLeft:
		dwOfs += 0;
		break;

	case ButtonRight:
		dwOfs += 1;
		break;

	case ButtonMiddle:
		dwOfs += 2;
		break;
	}
	
	m_pDevice->GetObjectInfo(&doi,dwOfs,DIPH_BYOFFSET);

	return doi.tszName;
}

nString nMouse::GetAxisName(Axis axis) const
{
	nAssert(m_pDevice);

	DIDEVICEOBJECTINSTANCE doi;
	ZeroMemory(&doi,sizeof(DIDEVICEOBJECTINSTANCE));
	doi.dwSize = sizeof(DIDEVICEOBJECTINSTANCE);
	unsigned long dwOfs;

	switch(axis)
	{
	case AxisX:
		dwOfs = DIMOFS_X;
		break;

	case AxisY:
		dwOfs = DIMOFS_Y;
		break;

	case AxisZ:
		dwOfs = DIMOFS_Z;
		break;
	}

	m_pDevice->GetObjectInfo(&doi,dwOfs,DIPH_BYOFFSET);

	return doi.tszName;
}