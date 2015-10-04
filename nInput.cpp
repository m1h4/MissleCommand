#include "nGlobals.h"
#include "nInput.h"

nInput::nInput(void) :
m_pMouse(NULL),
m_pKeyboard(NULL),
m_pInput(NULL)
{
}

nInput::~nInput(void)
{
	CleanInput();
}

bool nInput::InitInput()
{
	// Create our main direct input object
	HRESULT hr = DirectInput8Create(GetModuleHandle(0),DIRECTINPUT_VERSION,IID_IDirectInput8,(void **)&m_pInput,0);
	if(FAILED(hr))
		return false;

	m_pMouse = nMalloc(nMouse);

	if(!m_pMouse->InitMouse())
		return false;

	m_pKeyboard = nMalloc(nKeyboard);

	if(!m_pKeyboard->InitKeyboard())
		return false;

	return true;
}

void nInput::CleanInput()
{
	if(m_pMouse)
		m_pMouse->CleanMouse();

	if(m_pKeyboard)
		m_pKeyboard->CleanKeyboard();

	nSafeFree(m_pMouse);
	nSafeFree(m_pKeyboard);
	nSafeRelease(m_pInput);
}

bool nInput::Process()
{
	if(m_pMouse)
		m_pMouse->Update();

	if(m_pKeyboard)
		m_pKeyboard->Update();

	return true;
}