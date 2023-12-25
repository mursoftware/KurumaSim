
#include "Pch.h"
//#include "Common.h"
#include "GameManager.h"
#include "Setting.h"
#include "Brake.h"
#include "RigidBody.h"
#include "Camera.h"
#include "ShadowCamera.h"




void Brake::Load(const char * FileName, const char * PartName)
{
	m_MaxTorque = GetPrivateProfileFloat(PartName, "TORQUE", FileName);



	std::string path = FileName;
	path = path.substr(0, path.rfind('\\') + 1);

	std::string fileName;
	fileName = path;
	fileName += GetPrivateProfileStdString(PartName, "MODEL", FileName);
	m_Model[4].Load((fileName + "_4.obj").c_str(), THREAD_PRIORITY_NORMAL);
	m_Model[3].Load((fileName + "_3.obj").c_str(), THREAD_PRIORITY_LOWEST);
	m_Model[2].Load((fileName + "_2.obj").c_str(), THREAD_PRIORITY_LOWEST);
	m_Model[1].Load((fileName + "_1.obj").c_str(), THREAD_PRIORITY_LOWEST);
	m_Model[0].Load((fileName + "_0.obj").c_str(), THREAD_PRIORITY_LOWEST);

}




void Brake::Update(float dt)
{
	Vector3 torque;
	float brakeRatio = 1000.0f;

	m_TorqueLen = m_Tire->GetAngularVelocityLocal().X * brakeRatio * m_Ratio;

	//if (m_TorqueLen != 0.0f)
	{
		if (m_TorqueLen > m_MaxTorque * m_Ratio)
			m_TorqueLen = m_MaxTorque * m_Ratio;

		if (m_TorqueLen < -m_MaxTorque * m_Ratio)
			m_TorqueLen = -m_MaxTorque * m_Ratio;

		torque = m_Tire->GetMatrix().Right() * -m_TorqueLen;

		m_Tire->AddTorqueWorld(torque, dt);
		m_Body->AddTorqueWorld(m_Tire->GetPosition(), -torque, dt);

	}



}





void Brake::DrawDebug(const char* DebugName)
{
	if (ImGui::CollapsingHeader(DebugName))
	{
		std::string name;

		name = "MaxTorque ";
		name += DebugName;
		ImGui::SliderFloat(name.c_str(), &m_MaxTorque, 0.0f, 3000.0f, "%.0f Nm");

		char buf[32]{};
		sprintf(buf, "Torque %.0f Nm", m_TorqueLen);
		ImGui::ProgressBar(m_TorqueLen / 3000.0f, ImVec2(0, 16), buf);
	}
}




void Brake::PreDraw()
{
	m_OldWorldMatrix = m_WorldMatrix;


	Matrix44 tireMatrix = m_Tire->GetMatrix();
	Vector3 right, top, front, position;

	right = tireMatrix.Right();
	front = Vector3::Cross(right, Vector3(0.0f, 1.0f, 0.0f));
	front.Normalize();
	top = Vector3::Cross(front, right);

	position = tireMatrix.Position();


	m_WorldMatrix = Matrix44::Identity();

	m_WorldMatrix.M[0][0] = right.X;
	m_WorldMatrix.M[0][1] = right.Y;
	m_WorldMatrix.M[0][2] = right.Z;

	m_WorldMatrix.M[1][0] = top.X;
	m_WorldMatrix.M[1][1] = top.Y;
	m_WorldMatrix.M[1][2] = top.Z;

	m_WorldMatrix.M[2][0] = front.X;
	m_WorldMatrix.M[2][1] = front.Y;
	m_WorldMatrix.M[2][2] = front.Z;


	m_WorldMatrix *= Matrix44::TranslateXYZ(position.X, position.Y, position.Z);

}



void Brake::Draw(Camera* DrawCamera, int LodLevel)
{

	RenderManager* render = RenderManager::GetInstance();

	ShadowCamera** shadowCamera = GameManager::GetInstance()->GetScene()->GetCurrentShadowCamera();



	Matrix44 view = DrawCamera->GetViewMatrix();
	Matrix44 oldView = DrawCamera->GetOldViewMatrix();

	Matrix44 projection = DrawCamera->GetProjectionMatrix();
	Matrix44 oldProjection = DrawCamera->GetOldProjectionMatrix();


	Matrix44 shadowView0 = shadowCamera[0]->GetViewMatrix();
	Matrix44 shadowProjection0 = shadowCamera[0]->GetProjectionMatrix();

	Matrix44 shadowView1 = shadowCamera[1]->GetViewMatrix();
	Matrix44 shadowProjection1 = shadowCamera[1]->GetProjectionMatrix();

	Matrix44 shadowView2 = shadowCamera[2]->GetViewMatrix();
	Matrix44 shadowProjection2 = shadowCamera[2]->GetProjectionMatrix();



	OBJECT_CONSTANT constant;
	constant.WVP = Matrix44::Transpose(m_WorldMatrix * view * projection);
	constant.OldWVP = Matrix44::Transpose(m_OldWorldMatrix * oldView * oldProjection);
	constant.World = Matrix44::Transpose(m_WorldMatrix);
	constant.ShadowWVP[0] = Matrix44::Transpose(m_WorldMatrix * shadowView0 * shadowProjection0);
	constant.ShadowWVP[1] = Matrix44::Transpose(m_WorldMatrix * shadowView1 * shadowProjection1);
	constant.ShadowWVP[2] = Matrix44::Transpose(m_WorldMatrix * shadowView2 * shadowProjection2);
	constant.Param = { 0.0f, 0.0f, 0.0f, 1.0f };
	render->SetConstant(2, &constant, sizeof(constant));



	for (int level = LodLevel; level < 5; level++)
	{
		if (m_Model[level].IsLoaded())
		{
			m_Model[level].Draw();
			break;
		}
	}


	//m_Model[LodLevel].Draw();

}
