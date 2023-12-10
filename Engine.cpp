

#include "Pch.h"
//#include "Common.h"
#include "Engine.h"
#include "Setting.h"

#include "InputManager.h"



void Engine::Load(const char * FileName, const char * PartName)
{
	m_TorqueMap[0] = GetPrivateProfileFloat(PartName, "TORQUE_0", FileName);
	m_TorqueMap[1] = GetPrivateProfileFloat(PartName, "TORQUE_500", FileName);
	m_TorqueMap[2] = GetPrivateProfileFloat(PartName, "TORQUE_1000", FileName);
	m_TorqueMap[3] = GetPrivateProfileFloat(PartName, "TORQUE_1500", FileName);
	m_TorqueMap[4] = GetPrivateProfileFloat(PartName, "TORQUE_2000", FileName);
	m_TorqueMap[5] = GetPrivateProfileFloat(PartName, "TORQUE_2500", FileName);
	m_TorqueMap[6] = GetPrivateProfileFloat(PartName, "TORQUE_3000", FileName);
	m_TorqueMap[7] = GetPrivateProfileFloat(PartName, "TORQUE_3500", FileName);
	m_TorqueMap[8] = GetPrivateProfileFloat(PartName, "TORQUE_4000", FileName);
	m_TorqueMap[9] = GetPrivateProfileFloat(PartName, "TORQUE_4500", FileName);
	m_TorqueMap[10] = GetPrivateProfileFloat(PartName, "TORQUE_5000", FileName);
	m_TorqueMap[11] = GetPrivateProfileFloat(PartName, "TORQUE_5500", FileName);
	m_TorqueMap[12] = GetPrivateProfileFloat(PartName, "TORQUE_6000", FileName);
	m_TorqueMap[13] = GetPrivateProfileFloat(PartName, "TORQUE_6500", FileName);
	m_TorqueMap[14] = GetPrivateProfileFloat(PartName, "TORQUE_7000", FileName);
	m_TorqueMap[15] = GetPrivateProfileFloat(PartName, "TORQUE_7500", FileName);
	m_TorqueMap[16] = GetPrivateProfileFloat(PartName, "TORQUE_8000", FileName);
	m_TorqueMap[17] = GetPrivateProfileFloat(PartName, "TORQUE_8500", FileName);
	m_TorqueMap[18] = GetPrivateProfileFloat(PartName, "TORQUE_9000", FileName);
	m_TorqueMap[19] = GetPrivateProfileFloat(PartName, "TORQUE_9500", FileName);
	m_TorqueMap[20] = GetPrivateProfileFloat(PartName, "TORQUE_10000", FileName);


	m_Inertia = GetPrivateProfileFloat(PartName, "INERTIA", FileName);
	m_BrakeRatio = GetPrivateProfileFloat(PartName, "BRAKE_RATIO", FileName);
	m_Limiter = GetPrivateProfileFloat(PartName, "LIMITER", FileName);

}





void Engine::Update(float dt)
{


	float rpm = m_OutputAngularSpeed1 * 30.0f / PI;

	//レブリミッター
	if (rpm > m_Limiter)
		m_Throttle = 0;


	m_DelayThrottle += (m_Throttle - m_DelayThrottle) * 0.1f;
	//m_DelayThrottle = m_Throttle;

	//トルクマップ→トルク
	int emi = (int)(rpm / 500.0f);
	float et = m_TorqueMap[emi] + (m_TorqueMap[emi + 1] - m_TorqueMap[emi]) * (rpm / 500.0f - emi);

	float mt = 0.0f;
	if (m_OutputAngularSpeed1 > 100.0f)
		mt = powf(m_OutputAngularSpeed1 - 100.0f, 0.2f) * 4.0f;

	m_OutputTorque1 = (et + m_BrakeRatio * m_OutputAngularSpeed1 + mt) * m_DelayThrottle - m_BrakeRatio * m_OutputAngularSpeed1 - mt;



	//エンジン回転速度
	float acc = (m_OutputTorque1 + m_OutputDriveTrain1->GetInputTorque1()) / m_Inertia;
	m_OutputAngularSpeed1 += (m_Acc + acc) * 0.5f * dt;
	m_Acc = acc;

	if (m_OutputAngularSpeed1 < 0.0f)
		m_OutputAngularSpeed1 = 0.0f;



}



void Engine::DrawDebug()
{
	if (ImGui::CollapsingHeader("Engine"))
	{
		char buf[32]{};


		ImGui::PlotLines("TorqueMap", m_TorqueMap, sizeof(m_TorqueMap) / sizeof(float), 0, NULL, 0.0f, m_TorqueMap[10] * 1.5f, ImVec2(0, 60));

		sprintf(buf, "Torque %.0f Nm", m_OutputTorque1);
		ImGui::ProgressBar(m_OutputTorque1 / 2000.0f + 0.5f, ImVec2(0, 16), buf);


		float rpm = m_OutputAngularSpeed1 * 30.0f / PI;
		sprintf(buf, "%.0f RPM", rpm);
		ImGui::ProgressBar(rpm / 10000.0f, ImVec2(0, 16), buf);

		sprintf(buf, "Throttle %.2f", m_DelayThrottle);
		ImGui::ProgressBar(m_DelayThrottle, ImVec2(0, 16), buf);
	}

}




float Engine::GetOutputTorque1()
{
	return m_OutputTorque1;
}
