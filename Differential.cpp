

#include "Pch.h"
//#include "Common.h"
#include "Differential.h"
#include "Setting.h"




void Differential::Load(const char * FileName, const char * PartName)
{

	m_TractionRatio = GetPrivateProfileFloat(PartName, "TRACTION_RATIO", FileName);

	m_LsdInitialTorque = GetPrivateProfileFloat(PartName, "INITIAL_TORQUE", FileName);
	m_LsdRatioDrive = GetPrivateProfileFloat(PartName, "RATIO_DRIVE", FileName);
	m_LsdRatioCoast = GetPrivateProfileFloat(PartName, "RATIO_COAST", FileName);

}





void Differential::Update(float dt)
{

	float inputTorque1{};
	if (m_InputDriveTrain1)
		inputTorque1 = m_InputDriveTrain1->GetOutputTorque1();

	if (m_InputDriveTrain2)
		inputTorque1 = m_InputDriveTrain2->GetOutputTorque2();



	float outputAngularSpeed1{};
	if (m_OutputDriveTrain1)
		outputAngularSpeed1 = m_OutputDriveTrain1->GetInputAngularSpeed1();

	float outputAngularSpeed2{};
	if (m_OutputDriveTrain2)
		outputAngularSpeed2 = m_OutputDriveTrain2->GetInputAngularSpeed1();


	float lsdTorqueMax{};

	if (inputTorque1 > 0.0f)
	{
		lsdTorqueMax = m_LsdInitialTorque + inputTorque1 * m_LsdRatioDrive * 0.5f;
	}
	else
	{
		lsdTorqueMax = m_LsdInitialTorque - inputTorque1 * m_LsdRatioCoast * 0.5f;
	}


	m_LsdTorque = (outputAngularSpeed1 - outputAngularSpeed2) * 100.0f;


	if (m_LsdTorque > lsdTorqueMax)
	{
		m_LsdTorque = lsdTorqueMax;
	}
	if (m_LsdTorque < -lsdTorqueMax)
	{
		m_LsdTorque = -lsdTorqueMax;
	}


	m_OutputTorque1 = inputTorque1 * m_TractionRatio - m_LsdTorque;
	m_OutputTorque2 = inputTorque1 * (1.0f - m_TractionRatio) + m_LsdTorque;

	m_LsdTorqueRatio = (fabs(inputTorque1) * 0.5f + fabs(m_LsdTorque)) / (fabs(inputTorque1) * 0.5f - fabs(m_LsdTorque));

}

void Differential::DrawDebug(const char* DebugName)
{
	if (ImGui::CollapsingHeader(DebugName))
	{
		std::string name;

		name = "TractionRatio ";
		name += DebugName;
		ImGui::SliderFloat(name.c_str(), &m_TractionRatio, 0.0f, 1.0f, "%.3f");

		name = "LsdInitialTorque ";
		name += DebugName;
		ImGui::SliderFloat(name.c_str(), &m_LsdInitialTorque, 0.0f, 1000.0f, "%.0f Nm");

		name = "LsdRatioDrive ";
		name += DebugName;
		ImGui::SliderFloat(name.c_str(), &m_LsdRatioDrive, 0.0f, 1.0f, "%.3f");

		name = "LsdRatioCoast ";
		name += DebugName;
		ImGui::SliderFloat(name.c_str(), &m_LsdRatioCoast, 0.0f, 1.0f, "%.3f");


		char buf[64];

		sprintf(buf, "Torque1 %.0f Nm", m_OutputTorque1);
		ImGui::ProgressBar(m_OutputTorque1 / 10000.0f + 0.5f, ImVec2(0, 16), buf);

		sprintf(buf, "Torque2 %.0f Nm", m_OutputTorque2);
		ImGui::ProgressBar(m_OutputTorque2 / 10000.0f + 0.5f, ImVec2(0, 16), buf);

		sprintf(buf, "LSD Torque %.0f Nm", m_LsdTorque);
		ImGui::ProgressBar(m_LsdTorque / 10000.0f + 0.5f, ImVec2(0, 16), buf);

		sprintf(buf, "TorqueRatio %.2f", m_LsdTorqueRatio);
		ImGui::ProgressBar(m_LsdTorqueRatio / 10.0f, ImVec2(0, 16), buf);
	}
}


float Differential::GetOutputTorque1()
{
	return m_OutputTorque1;
}


float Differential::GetOutputTorque2()
{
	return m_OutputTorque2;
}


float Differential::GetInputAngularSpeed1()
{
	float outputAngularSpeed1{};
	if (m_OutputDriveTrain1)
		outputAngularSpeed1 = m_OutputDriveTrain1->GetInputAngularSpeed1();

	float outputAngularSpeed2{};
	if (m_OutputDriveTrain2)
		outputAngularSpeed2 = m_OutputDriveTrain2->GetInputAngularSpeed1();

	float inputAngularSpeed1 = outputAngularSpeed1 * m_TractionRatio + outputAngularSpeed2 * (1.0f - m_TractionRatio);
	return inputAngularSpeed1;
}

