#include "nGlobals.h"
#include "nMain.h"
#include "nKeyboard.h"

nKeyboard::nKeyboard(void)
{
}

nKeyboard::~nKeyboard(void)
{
	CleanKeyboard();
}

bool nKeyboard::InitKeyboard()
{
	HRESULT hr = nGetInstance()->GetInput()->GetDirectInput()->CreateDevice(GUID_SysKeyboard,(LPDIRECTINPUTDEVICE8*)&m_pDevice,0);
	if(FAILED(hr))
		return false;

	// Set the data format that we will be retreving when reading input from this device
	hr = m_pDevice->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(hr))
		return false;

	// Set the cooperate levels for the keyboard
	hr = m_pDevice->SetCooperativeLevel(nGetInstance()->GetWindowHandle(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if(FAILED(hr))
		return false;

	// Finally acquire the device to prepare it for input reading
	hr = m_pDevice->Acquire();
	if(FAILED(hr))
		return false;

	// Get device instance
	DIDEVICEINSTANCE inst;
	inst.dwSize = sizeof(inst);
	m_pDevice->GetDeviceInfo(&inst);

	m_DeviceName = inst.tszProductName;

	// Zero the key states
	ZeroMemory(m_Keys,sizeof(m_Keys));

	return true;
}

void nKeyboard::CleanKeyboard()
{
	if(m_pDevice)
		m_pDevice->Unacquire();

	nSafeRelease(m_pDevice);
}

bool nKeyboard::Update()
{
	nAssert(m_pDevice);

	CopyMemory(&m_Keys[0],&m_Keys[1],sizeof(m_Keys[0]));

	// Get the keystates from the device
	HRESULT hr = m_pDevice->GetDeviceState(sizeof(m_Keys[1]),&m_Keys[1]);
	if(hr != S_OK)
	{
		// If input is lost then acquire and keep trying 
		hr = m_pDevice->Acquire();
        while(hr == DIERR_INPUTLOST)
			hr = m_pDevice->Acquire();

		return false;
	}

	return true;
}

nString nKeyboard::GetKeyName(unsigned long key)
{
	nAssert(m_pDevice);

	DIDEVICEOBJECTINSTANCE doi;
	ZeroMemory(&doi,sizeof(DIDEVICEOBJECTINSTANCE));
	doi.dwSize = sizeof(DIDEVICEOBJECTINSTANCE);
	unsigned long dwOfs = key;

	m_pDevice->GetObjectInfo(&doi,dwOfs,DIPH_BYOFFSET);

	return doi.tszName;
}