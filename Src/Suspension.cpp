

#include "Pch.h"
//#include "Common.h"
#include "Setting.h"
#include "Camera.h"
#include "RigidBody.h"
#include "BodyRB.h"
#include "TireRB.h"
#include "Suspension.h"




void Suspension::Load(const char * FileName, const char * PartName, float ScaleX)
{

	m_ScaleX = ScaleX;


	m_UpperMountOffset = GetPrivateProfileVector3(PartName, "UPPER_MOUNT_OFFSET", FileName);
	m_UpperMountOffset.X *= ScaleX;

	m_LowerMountOffset = GetPrivateProfileVector3(PartName, "LOWER_MOUNT_OFFSET ", FileName);
	m_LowerMountOffset.X *= ScaleX;


	m_TireAxisLength = GetPrivateProfileFloat(PartName, "TIRE_AXIS_LENGTH", FileName);
	m_LowerArmLength = GetPrivateProfileFloat(PartName, "LOWER_ARM_LENGTH", FileName);
	m_StrutLength = GetPrivateProfileFloat(PartName, "STRUT_LENGTH", FileName);
	m_SpringNaturalLength = GetPrivateProfileFloat(PartName, "SPRING_NATURAL_LENGTH", FileName);

	m_Stroke = GetPrivateProfileFloat(PartName, "STROKE", FileName);
	m_SpringRatio = GetPrivateProfileFloat(PartName, "SPRING_RATIO", FileName);
	m_DampingRatioExt = GetPrivateProfileFloat(PartName, "DAMPING_EXT", FileName);
	m_DampingRatioRet = GetPrivateProfileFloat(PartName, "DAMPING_RET", FileName);

	m_CamberAngle = GetPrivateProfileFloat(PartName, "CAMBER", FileName);
	m_ToeAngle = GetPrivateProfileFloat(PartName, "TOE", FileName);


	m_Stabilizer = GetPrivateProfileFloat(PartName, "STABILIZER", FileName);



	m_PointModel.Load("Asset\\sphere.obj", THREAD_PRIORITY_ABOVE_NORMAL, false, true);



	m_SpringLength = 1.0f;
	m_SteeringAngle = 0.0f;

}

void Suspension::Update(float dt)
{

	Matrix44 bodyMatrix = m_Body->GetMatrix();
	Vector3 bodyPos = m_Body->GetPosition();
	Vector3 bodyRight = bodyMatrix.Right();
	Vector3 bodyUp = bodyMatrix.Up();
	Vector3 bodyFront = bodyMatrix.Front();


	Matrix44 matrix;
	matrix = Matrix44::TranslateXYZ(m_UpperMountOffset.X, m_UpperMountOffset.Y, m_UpperMountOffset.Z) * bodyMatrix;
	m_UpperMountPos = matrix.Position();

	Vector3 tirePos = m_Tire->GetPosition();
	m_TireAxisBodyPos = tirePos - m_Tire->GetMatrix().Right() * m_TireAxisLength * m_ScaleX;
	Vector3 direction = m_TireAxisBodyPos - m_UpperMountPos;
	float springLength = (direction.Length() - m_SpringNaturalLength);
	direction.Normalize();


	if (springLength > 0.1f)
	{
		springLength = 0.1f;
	}
	if (springLength < -0.15f)
	{
		springLength = -0.15f;
	}

	float speed = (springLength - m_SpringLength) / dt;

	float springForce = springLength * m_SpringRatio;

	if (speed < 0.0f)
		springForce += speed * m_DampingRatioExt;
	else
		springForce += speed * m_DampingRatioRet;



	m_SpringLength = springLength;

	m_Body->AddForceWorld(m_UpperMountPos, direction * springForce, dt);
	m_Tire->AddForceWorld(m_TireAxisBodyPos, -direction * springForce, dt);




	float dif = m_SuspensionOther->GetSpringLength() - m_SpringLength;
	Vector3 stabilizerForce = bodyUp * dif * m_Stabilizer;

	m_Body->AddForceWorld(m_UpperMountPos, stabilizerForce, dt);
	m_Tire->AddForceWorld(m_TireAxisBodyPos, -stabilizerForce, dt);


}

void Suspension::UpdateGeometry(float dt)
{
	Matrix44 matrix;
	Vector3 dir;
	Matrix44 bodyMatrix = m_Body->GetMatrix();
	Vector3 bodyPos = m_Body->GetPosition();
	Vector3 bodyRight = bodyMatrix.Right();
	Vector3 bodyUp = bodyMatrix.Up();
	Vector3 bodyFront = bodyMatrix.Front();

	Vector3 tirePos = m_Tire->GetPosition();
	Vector3 tireRight = m_Tire->GetMatrix().Right();
	m_TireAxisBodyPos = tirePos - tireRight * m_TireAxisLength * m_ScaleX;

	matrix = Matrix44::TranslateXYZ(m_UpperMountOffset.X, m_UpperMountOffset.Y, m_UpperMountOffset.Z) * bodyMatrix;
	m_UpperMountPos = matrix.Position();

	dir = m_TireAxisBodyPos - m_UpperMountPos;
	float springLength = (dir.Length() - m_SpringNaturalLength);

	if (springLength > 0.1f)
	{
		dir.Normalize();
		m_TireAxisBodyPos -= dir * (springLength - 0.1f);
	}
	if (springLength < -0.15f)
	{
		dir.Normalize();
		m_TireAxisBodyPos -= dir * (springLength + 0.15f);
	}


	dir = m_TireAxisBodyPos - bodyPos;

	float z = Vector3::Dot(dir, bodyFront) - m_LowerMountOffset.Z;
	m_TireAxisBodyPos -= bodyFront * z;


	Vector3 strutDir = m_TireAxisBodyPos - m_UpperMountPos;
	strutDir.Normalize();
	m_LowerArmTirePos = m_TireAxisBodyPos + strutDir * m_StrutLength;


	matrix = Matrix44::TranslateXYZ(m_LowerMountOffset.X, m_LowerMountOffset.Y, m_LowerMountOffset.Z) * bodyMatrix;
	m_LowerArmBodyPos = matrix.Position();

	Vector3 lowerArmDir = m_LowerArmTirePos - m_LowerArmBodyPos;
	lowerArmDir.Normalize();
	lowerArmDir *= m_LowerArmLength;


	m_LowerArmTirePos = m_LowerArmBodyPos;
	m_LowerArmTirePos += lowerArmDir;

	strutDir = m_LowerArmTirePos - m_UpperMountPos;
	strutDir.Normalize();

	m_TireAxisBodyPos = m_LowerArmTirePos - strutDir * m_StrutLength;

	Vector3 vx, vy, vz;
	vy = -strutDir;
	vz = Vector3::Cross(bodyRight, vy);
	vz.Normalize();
	vx = Vector3::Cross(vy, vz);

	matrix = Matrix44::Identity();
	matrix = Matrix44::Multiply(matrix, Matrix44::RotationXYZ(0.0f, 0.0f, -m_CamberAngle * m_ScaleX));
	matrix = Matrix44::Multiply(matrix, Matrix44::RotationXYZ(0.0f, m_SteeringAngle + m_ToeAngle * m_ScaleX, 0.0f));
	matrix = Matrix44::Multiply(matrix, Matrix44::FromAxis(vx, vy, vz));

	tirePos = m_TireAxisBodyPos + tireRight * m_TireAxisLength * m_ScaleX;

	m_Tire->SetPosition(tirePos);


	Vector3 axis = Vector3::Cross(matrix.Right(), tireRight);
	m_Tire->RotateAxisWorld(-axis);


}



void Suspension::DrawDebug(const char* DebugName)
{
	if (ImGui::CollapsingHeader(DebugName))
	{
		std::string name;

		name = "SpringRatio ";
		name += DebugName;
		ImGui::SliderFloat(name.c_str(), &m_SpringRatio, 0.0f, 50000.0f, "%.0f N/m");

		name = "DampingRatioExt ";
		name += DebugName;
		ImGui::SliderFloat(name.c_str(), &m_DampingRatioExt, 0.0f, 5000.0f, "%.0f N/m/s");

		name = "DampingRatioRet ";
		name += DebugName;
		ImGui::SliderFloat(name.c_str(), &m_DampingRatioRet, 0.0f, 5000.0f, "%.0f N/m/s");




		name = "Stabilizer ";
		name += DebugName;
		ImGui::SliderFloat(name.c_str(), &m_Stabilizer, 0.0f, 10000.0f, "%.0f N/m");


		char buf[64]{};

		sprintf(buf, "SpringLength %.3f m", m_SpringLength);
		ImGui::ProgressBar(m_SpringLength * 2.0f + 0.5f, ImVec2(0, 16), buf);
	}
}



void Suspension::PreDraw()
{

}

void Suspension::Draw(Camera* DrawCamera)
{
	RenderManager* render = RenderManager::GetInstance();

	Matrix44 view = DrawCamera->GetViewMatrix();
	Matrix44 projection = DrawCamera->GetProjectionMatrix();


	{
		Matrix44 worldMatrix, transMatrix, rotMatrix;

		worldMatrix = Matrix44::Identity();
		worldMatrix *= Matrix44::Scale(0.02f, 0.02f, 0.02f);
		worldMatrix *= Matrix44::TranslateXYZ(m_UpperMountPos.X, m_UpperMountPos.Y, m_UpperMountPos.Z);

		OBJECT_CONSTANT constant;
		constant.WVP = Matrix44::Transpose(worldMatrix * view * projection);
		constant.OldWVP = Matrix44::Transpose(worldMatrix * view * projection);
		constant.World = Matrix44::Transpose(worldMatrix);
		constant.Param = { 0.0f, 0.0f, 0.1f, 1.0f };
		render->SetConstant(2, &constant, sizeof(constant));

		m_PointModel.Draw();
	}

	{
		Matrix44 worldMatrix, transMatrix, rotMatrix;

		worldMatrix = Matrix44::Identity();
		worldMatrix *= Matrix44::Scale(0.02f, 0.02f, 0.02f);
		worldMatrix *= Matrix44::TranslateXYZ(m_LowerArmBodyPos.X, m_LowerArmBodyPos.Y, m_LowerArmBodyPos.Z);

		OBJECT_CONSTANT constant;
		constant.WVP = Matrix44::Transpose(worldMatrix * view * projection);
		constant.OldWVP = Matrix44::Transpose(worldMatrix * view * projection);
		constant.World = Matrix44::Transpose(worldMatrix);
		constant.Param = { 0.0f, 0.0f, 0.1f, 1.0f };
		render->SetConstant(2, &constant, sizeof(constant));

		m_PointModel.Draw();
	}

	{
		Matrix44 worldMatrix, transMatrix, rotMatrix;

		worldMatrix = Matrix44::Identity();
		worldMatrix *= Matrix44::Scale(0.02f, 0.02f, 0.02f);
		worldMatrix *= Matrix44::TranslateXYZ(m_LowerArmTirePos.X, m_LowerArmTirePos.Y, m_LowerArmTirePos.Z);

		OBJECT_CONSTANT constant;
		constant.WVP = Matrix44::Transpose(worldMatrix * view * projection);
		constant.OldWVP = Matrix44::Transpose(worldMatrix * view * projection);
		constant.World = Matrix44::Transpose(worldMatrix);
		constant.Param = { 0.0f, 0.0f, 0.1f, 1.0f };
		render->SetConstant(2, &constant, sizeof(constant));

		m_PointModel.Draw();
	}

	{
		Matrix44 worldMatrix, transMatrix, rotMatrix;

		worldMatrix = Matrix44::Identity();
		worldMatrix *= Matrix44::Scale(0.02f, 0.02f, 0.02f);
		worldMatrix *= Matrix44::TranslateXYZ(m_TireAxisBodyPos.X, m_TireAxisBodyPos.Y, m_TireAxisBodyPos.Z);

		OBJECT_CONSTANT constant;
		constant.WVP = Matrix44::Transpose(worldMatrix * view * projection);
		constant.OldWVP = Matrix44::Transpose(worldMatrix * view * projection);
		constant.World = Matrix44::Transpose(worldMatrix);
		constant.Param = { 0.0f, 0.0f, 0.1f, 1.0f };
		render->SetConstant(2, &constant, sizeof(constant));

		m_PointModel.Draw();
	}

}
