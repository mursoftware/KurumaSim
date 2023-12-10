#pragma once


#include "DriveTrain.h"


class Clutch : public DriveTrain
{
private:

	float		m_Clutch{};
	float		m_ClutchRatio{};
	float		m_ClutchMaxTorque{};
	float		m_ClutchRotationSpeed{};
	float		m_ClutchRotationSpeedIntegration{};

	float		m_InputTorque1{};
	float		m_OutputTorque1{};
	float		m_InputAngularSpeed1{};
	float		m_OutputAngularSpeed1{};

public:

	void Load(const char* FileName, const char* PartName);
	void Update(float dt);
	void DrawDebug();

	void SetClutch(float Clutch) { m_Clutch = Clutch; }
	void SetClutchRatio(float ClutchRatio) { m_ClutchRatio = ClutchRatio; }


	float GetInputTorque1() override;
	float GetOutputTorque1() override;
};

