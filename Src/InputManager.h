#pragma once






class InputManager
{

private:
	static InputManager* m_Instance;

	BYTE m_OldKeyState[256]{};
	BYTE m_KeyState[256]{};

	POINT m_OldMousePosition{};
	POINT m_MousePosition{};


	Vector2 m_AxisLeft;
	Vector2 m_AxisRight;
	float	m_TriggerLeft{};
	float	m_TriggerRight{};
	bool	m_Button[12]{};
	bool	m_ButtonOld[12]{};



	float	m_Torque{};
	int		m_TorqueCount{};

	float	m_TorqueAverage{};
	float	m_TorqureRatio{};

public:
	static InputManager* GetInstance() { return m_Instance; }

	InputManager();
	~InputManager();

	void Update();
	void DrawDebug();

	bool GetKeyPress(BYTE KeyCode);
	bool GetKeyTrigger(BYTE KeyCode);

	POINT GetMouseMove();

	void SetTorque(float Torque);


	Vector2 GetAxisLeft() { return m_AxisLeft; }
	Vector2 GetAxisRight() { return m_AxisRight; }
	float GetTriggerLeft() { return m_TriggerLeft; }
	float GetTriggerRight() { return m_TriggerRight; }
	bool GetButtonPress(int index) { return m_Button[index]; }
	bool GetButtonTrigger(int index) { return (m_Button[index] && !m_ButtonOld[index]); }
	bool GetButtonRelease(int index) { return (!m_Button[index] && m_ButtonOld[index]); }

};

