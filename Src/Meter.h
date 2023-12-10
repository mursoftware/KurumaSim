#pragma once




class Meter
{
private:

	std::unique_ptr<TEXTURE>	m_PanleTexture;
	std::unique_ptr<TEXTURE>	m_NeedleTexture;
	std::unique_ptr<TEXTURE>	m_NumberTexture;

	float						m_RPM{};
	float						m_Speed{};
	int							m_Gear{};

public:
	Meter();
	~Meter();


	void Update(float dt);
	void DrawUI();


	void SetRPM(float RPM) { m_RPM = RPM; };
	void SetSpeed(float Speed) { m_Speed = Speed; };
	void SetGear(int Gear) { m_Gear = Gear; };

};

