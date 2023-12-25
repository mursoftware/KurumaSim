

#include "Pch.h"
//#include "Common.h"
#include "Transmission.h"
#include "Setting.h"




void Transmission::Load(const char * FileName, const char * PartName)
{

	m_GearCount = GetPrivateProfileInt(PartName, "GEAR_COUNT", 0, FileName);

	m_GearRatioMap[0] = GetPrivateProfileFloat(PartName, "GEAR_R", FileName);
	m_GearRatioMap[1] = GetPrivateProfileFloat(PartName, "GEAR_1", FileName);
	m_GearRatioMap[2] = GetPrivateProfileFloat(PartName, "GEAR_2", FileName);
	m_GearRatioMap[3] = GetPrivateProfileFloat(PartName, "GEAR_3", FileName);
	m_GearRatioMap[4] = GetPrivateProfileFloat(PartName, "GEAR_4", FileName);
	m_GearRatioMap[5] = GetPrivateProfileFloat(PartName, "GEAR_5", FileName);
	m_GearRatioMap[6] = GetPrivateProfileFloat(PartName, "GEAR_6", FileName);

	m_GearRatioFinal = GetPrivateProfileFloat(PartName, "GEAR_F", FileName);

}





void Transmission::Update(float dt)
{
	//gear ratio
	{
		switch (m_Gear)
		{
		case -1:
			m_GearRatio = m_GearRatioMap[0];
			break;
		case 0:
			m_GearRatio = 0.0f;
			break;
		default:
			m_GearRatio = m_GearRatioMap[m_Gear];
			break;
		}

		//final gear
		m_GearRatio *= m_GearRatioFinal;
	}



	float inputTorque1 = m_InputDriveTrain1->GetOutputTorque1();
	float outputAngularSpeed1 = m_OutputDriveTrain1->GetInputAngularSpeed1();

	if (m_Gear == 0)
	{
		//m_InputAngularSpeed1 += inputTorque1 / 0.1f;
		m_InputAngularSpeed1 = 0.0f;
	}
	else
	{
		m_InputAngularSpeed1 = outputAngularSpeed1 * m_GearRatio;
	}

}



void Transmission::DrawDebug()
{
	if (ImGui::CollapsingHeader("Transmission"))
	{
		char buf[32];
		sprintf(buf, "Gear %d", m_Gear);
		ImGui::LabelText("", buf);
		sprintf(buf, "GearRatio %.2f", m_GearRatio);
		ImGui::LabelText("", buf);
	}

}




float Transmission::GetOutputTorque1()
{
	float inputTorque1 = m_InputDriveTrain1->GetOutputTorque1();
	float outputTorque1 = 0.0f;

	if (m_Gear != 0)
	{
		outputTorque1 = inputTorque1 * m_GearRatio;
	}

	return outputTorque1;
}

float Transmission::GetInputAngularSpeed1()
{
	float outputAngularSpeed1 = m_OutputDriveTrain1->GetInputAngularSpeed1();

	if (m_Gear != 0)
	{
		m_InputAngularSpeed1 = outputAngularSpeed1 * m_GearRatio;
	}

	return	m_InputAngularSpeed1;
}

