#pragma once

#include "nMouse.h"
#include "nKeyboard.h"

class nInput
{
public:
	nInput(void);
	~nInput(void);

	bool InitInput();
	void CleanInput();

	bool Process();

	nMouse* GetMouse() { return m_pMouse; }
	nKeyboard* GetKeyboard() { return m_pKeyboard; }

	IDirectInput8* GetDirectInput() { return m_pInput; }

protected:
	IDirectInput8*		m_pInput;

	nMouse*		m_pMouse;
	nKeyboard*	m_pKeyboard;
};
