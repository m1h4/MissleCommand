#pragma once

class nKeyboard
{
public:
	nKeyboard(void);
	~nKeyboard(void);

	bool InitKeyboard();
	void CleanKeyboard();

	bool Update();

	bool GetKey(unsigned long key) { return m_Keys[1][key] & 0x80; }
	bool GetNewKey(unsigned long key) { return m_Keys[0][key] & 0x80 && !(m_Keys[1][key] & 0x80); }

	nString GetKeyName(unsigned long key);

	const nString& GetDeviceName() const { return m_DeviceName; }

protected:
	IDirectInputDevice8*	m_pDevice;

	nString					m_DeviceName;

	unsigned char	m_Keys[2][256];	// Last two key states
};
