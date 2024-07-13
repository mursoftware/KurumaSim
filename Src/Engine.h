#pragma once


#include "DriveTrain.h"


class Engine : public DriveTrain
{
private:

	float		m_TorqueMap[21]{};
	float		m_Inertia{};
	float		m_Brake{};
	float		m_BrakeRatio{};
	float		m_Limiter{};
	bool		m_LimiterEnable{};
	float		m_Throttle{};
	float		m_DelayThrottle{};
	float		m_Acc{};

	float		m_OutputTorque1{};
	float		m_OutputAngularSpeed1{};

	float		m_Rotation{};

public:


	void Load(const char* FileName, const char* PartName);
	void Update(float dt);
	void DrawDebug();

	void SetThrottle(float Throttle) { m_Throttle = Throttle; }

	float GetOutputTorque1() override;
	float GetOutputAngularSpeed1() override { return m_OutputAngularSpeed1; }


	void  SetOutputAngularSpeed1(float AngularSpeed) { m_OutputAngularSpeed1 = AngularSpeed; }

	float GetThrottle() { return m_Throttle; }
	float GetDelayThrottle() { return m_DelayThrottle; }

};