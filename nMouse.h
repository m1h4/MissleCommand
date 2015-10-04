#pragma once

class nMouse
{
public:
	nMouse(void);
	~nMouse(void);

	enum Button
	{
		ButtonLeft = 1,
		ButtonRight = 2,
		ButtonMiddle = 4,
	};

	enum Axis
	{
		AxisX,
		AxisY,
		AxisZ,
	};

	bool InitMouse();
	void CleanMouse();

	bool Update();

	bool GetButton(Button button) { return m_ButtonMask[1] & button; }
	bool GetNewButton(Button button) { return m_ButtonMask[1] & button && !(m_ButtonMask[0] & button); }

	void SetPosition(const nVector2& position) { m_Positions[0] = m_Positions[1] = position; }
	nVector2 GetPosition() { if(m_Smooth) return (m_Positions[0] + m_Positions[1]) / 2.0f; else return m_Positions[1]; }

	void SetSmooth(bool smooth) { m_Smooth = smooth; }
	bool GetSmooth() const { return m_Smooth; }

	nString GetAxisName(Axis axis) const;
	nString GetButtonName(Button button) const;

	const nString& GetDeviceName() const { return m_DeviceName; }

protected:
	IDirectInputDevice8*	m_pDevice;

	nString					m_DeviceName;

	bool					m_Smooth;

	nVector2				m_Positions[2];		// Last two positions
	unsigned long			m_ButtonMask[2];	// Last two button states
};
