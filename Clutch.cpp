

#include "Pch.h"
//#include "Common.h"
#include "Setting.h"
#include "Clutch.h"



void Clutch::Load(const char * FileName, const char * PartName)
{
	m_ClutchMaxTorque = GetPrivateProfileFloat(PartName, "TORQUE", FileName);
}





void Clutch::Update(float dt)
{


	m_InputAngularSpeed1 = m_InputDriveTrain1->GetOutputAngularSpeed1();
	m_OutputAngularSpeed1 = m_OutputDriveTrain1->GetInputAngularSpeed1();



	float clutchMaxTorque = m_ClutchMaxTorque * m_Clutch;
	float clutchRotationSpeed = m_InputAngularSpeed1 - m_OutputAngularSpeed1;
	float speed = (m_ClutchRotationSpeed + clutchRotationSpeed) * 0.5f;
	m_ClutchRotationSpeedIntegration += speed * dt;


	if (m_ClutchRatio == 0.0f)
		m_OutputTorque1 = speed * 0.0f;
	else
		m_OutputTorque1 = speed * 300.0f / fabs(m_ClutchRatio);

	if (m_OutputTorque1 > clutchMaxTorque)
	{
		m_OutputTorque1 = clutchMaxTorque;
		m_ClutchRotationSpeedIntegration = 0.0f;
	}
	else if (m_OutputTorque1 < -clutchMaxTorque)
	{
		m_OutputTorque1 = -clutchMaxTorque;
		m_ClutchRotationSpeedIntegration = 0.0f;
	}
	else if(m_ClutchRatio != 0.0f)
	{
		m_OutputTorque1 += m_ClutchRotationSpeedIntegration * 1000.0f;
	}
	


	m_InputTorque1 = -m_OutputTorque1;

	m_ClutchRotationSpeed = clutchRotationSpeed;


}



void Clutch::DrawDebug()
{
	if (ImGui::CollapsingHeader("Clutch"))
	{
		char buf[64]{};

		sprintf(buf, "InputAngularSpeed1 %.0f", m_InputAngularSpeed1);
		ImGui::ProgressBar(m_InputAngularSpeed1 / 1000.0f, ImVec2(0, 16), buf);

		sprintf(buf, "OutputAngularSpeed1 %.0f", m_OutputAngularSpeed1);
		ImGui::ProgressBar(m_OutputAngularSpeed1 / 1000.0f, ImVec2(0, 16), buf);

		sprintf(buf, "Clutch %.2f", m_Clutch);
		ImGui::ProgressBar(m_Clutch, ImVec2(0, 16), buf);

		ImGui::SliderFloat("ClutchTorqueMax", &m_ClutchMaxTorque, 0.0f, 1000.0f, "%.0f Nm");

		sprintf(buf, "ClutchTorque %.0f Nm", m_OutputTorque1);
		ImGui::ProgressBar(m_OutputTorque1 / 2000.0f + 0.5f, ImVec2(0, 16), buf);
	}
}




float Clutch::GetInputTorque1()
{
	return m_InputTorque1;
}

float Clutch::GetOutputTorque1()
{
	return m_OutputTorque1;
}

