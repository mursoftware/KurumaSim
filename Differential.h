#pragma once


#include "DriveTrain.h"


class Differential : public DriveTrain
{
private:

	float		m_TractionRatio{};

	float		m_LsdInitialTorque{};
	float		m_LsdRatioDrive{};
	float		m_LsdRatioCoast{};
	float		m_LsdTorque{};

	float		m_OutputTorque1{};
	float		m_OutputTorque2{};
	float		m_LsdTorqueRatio{};

	std::string m_DebugName;

public:


	void Load(const char* FileName, const char* PartName);
	void Update(float dt);
	void DrawDebug(const char* DebugName);

	float GetOutputTorque1() override;
	float GetOutputTorque2() override;
	float GetInputAngularSpeed1() override;


};