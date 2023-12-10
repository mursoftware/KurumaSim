#pragma once


#include "DriveTrain.h"


class Transmission : public DriveTrain
{
private:

	int			m_Gear{};
	int			m_GearCount{};
	float		m_GearRatio{};
	float		m_GearRatioMap[7]{};
	float		m_GearRatioFinal{};

	float		m_InputAngularSpeed1{};


public:


	void Load(const char* FileName, const char* PartName);
	void Update(float dt);
	void DrawDebug();

	int GetGear() { return m_Gear; }
	void SetGear(int Gear) { m_Gear = Gear; }
	int GetGearCount() { return m_GearCount; }
	float GetGearRatio() { return m_GearRatio; }


	float GetOutputTorque1() override;
	float GetInputAngularSpeed1() override;

};