

#include "Pch.h"

#include "GameManager.h"
#include "RenderManager.h"
#include "GameScene.h"
#include "Field.h"

#include "BodyRB.h"
#include "Camera.h"
#include "ShadowCamera.h"

#include "Setting.h"


void BodyRB::Load(const char * FileName, const char * PartName)
{

	std::string path = FileName;
	path = path.substr(0, path.rfind('\\') + 1);


	{
		m_Offset = GetPrivateProfileVector3(PartName, "OFFSET", FileName);
		m_Size = GetPrivateProfileVector3(PartName, "SIZE", FileName);

		float bodyMass = GetPrivateProfileFloat(PartName, "MASS", FileName);
		CalcInertiaBox(bodyMass, m_Size);

		m_Inertia *= GetPrivateProfileVector3(PartName, "INERTIA_RATIO", FileName);;


		m_CD = GetPrivateProfileFloat(PartName, "CD", FileName);

		m_ShadowSize = GetPrivateProfileFloat(PartName, "SHADOW_SIZE", FileName);


		m_DownForceRatioF = GetPrivateProfileFloat(PartName, "DOWN_FORCE_RATIO_FRONT", FileName);
		m_DownForceRatioR = GetPrivateProfileFloat(PartName, "DOWN_FORCE_RATIO_REAR", FileName);


		m_AirDensity = 1.205f;


		m_Visible = true;
	}

	

	{
		std::string fileName;
		fileName = path;
		fileName += GetPrivateProfileStdString(PartName, "MODEL", FileName);
		m_Model[4].Load((fileName + "_4.obj").c_str(), THREAD_PRIORITY_ABOVE_NORMAL);
		m_Model[3].Load((fileName + "_3.obj").c_str(), THREAD_PRIORITY_LOWEST);
		m_Model[2].Load((fileName + "_2.obj").c_str(), THREAD_PRIORITY_LOWEST);
		m_Model[1].Load((fileName + "_1.obj").c_str(), THREAD_PRIORITY_LOWEST);
		m_Model[0].Load((fileName + "_0.obj").c_str(), THREAD_PRIORITY_LOWEST);
	}
	
	



	{
		RenderManager* render = RenderManager::GetInstance();



		std::string fileName;
		fileName = path;
		fileName += GetPrivateProfileStdString(PartName, "SHADOW", FileName);
		m_ShadowTexture = render->LoadTexture(fileName.c_str());
	}


}





void BodyRB::Update(float dt)
{
	GameScene* scene = (GameScene*)GameManager::GetInstance()->GetScene();
	Field* field = scene->GetGameObject<Field>();





	Vector3 v = GetVelocity();
	float vl = v.Length();
	v.Normalize();


	//air resistance
	{
		Vector3 airForce = v * vl * vl * 0.5f * m_AirDensity * m_CD * m_Size.X * m_Size.Y;
		AddForceWorld(-airForce, dt);
		m_AirFoeceLen = airForce.Length();
	}


	//downforce
	{
		Matrix44 bodyMatrix = GetMatrix();

		Vector3 pointF = Vector3::TransformCoord(bodyMatrix, Vector3(0.0f, 0.0f, 1.0f));
		Vector3 pointR = Vector3::TransformCoord(bodyMatrix, Vector3(0.0f, 0.0f, -1.0f));

		Vector3 downForce = GetMatrix().Up() * vl * vl * 0.5f * m_AirDensity;

		Vector3 downForceF = downForce * -m_DownForceRatioF;
		AddForceWorld(pointF, downForceF, dt);
		m_DownForceLenF = downForceF.Length();

		Vector3 downForceR = downForce * -m_DownForceRatioR;
		AddForceWorld(pointR, downForceR, dt);
		m_DownForceLenR = downForceR.Length();
	}





	StorePositionRotation();
	UpdatePositionRotation(dt);





	//if(false)
	{
		Matrix44 bodyMatrix = GetMatrix();
		Vector3 bodyPos = GetPosition();

		//Vector3 pointArray[8];
		//pointArray[0] = Vector3(-m_Size.X / 2.0f, -m_Size.Y / 6.0f, m_Size.Z / 2.0f);
		//pointArray[1] = Vector3(m_Size.X / 2.0f, -m_Size.Y / 6.0f, m_Size.Z / 2.0f);
		//pointArray[2] = Vector3(-m_Size.X / 2.0f, -m_Size.Y / 6.0f, -m_Size.Z / 2.0f);
		//pointArray[3] = Vector3(m_Size.X / 2.0f, -m_Size.Y / 6.0f, -m_Size.Z / 2.0f);

		//pointArray[4] = Vector3(-m_Size.X / 2.0f, m_Size.Y / 2.0f, m_Size.Z / 2.0f);
		//pointArray[5] = Vector3(m_Size.X / 2.0f, m_Size.Y / 2.0f, m_Size.Z / 2.0f);
		//pointArray[6] = Vector3(-m_Size.X / 2.0f, m_Size.Y / 2.0f, -m_Size.Z / 2.0f);
		//pointArray[7] = Vector3(m_Size.X / 2.0f, m_Size.Y / 2.0f, -m_Size.Z / 2.0f);
		
		Vector3 pointArray[4];
		pointArray[0] = Vector3(-m_Size.X / 2.0f, 0.0f, m_Size.Z / 2.0f);
		pointArray[1] = Vector3(m_Size.X / 2.0f, 0.0f, m_Size.Z / 2.0f);
		pointArray[2] = Vector3(-m_Size.X / 2.0f, 0.0f, -m_Size.Z / 2.0f);
		pointArray[3] = Vector3(m_Size.X / 2.0f, 0.0f, -m_Size.Z / 2.0f);

		for (Vector3& point : pointArray)
		{
			point = Vector3::TransformCoord(bodyMatrix, point);
		}


		bool hit{};
		Vector3 hitPosition;
		Vector3 hitNormal;
		Vector3 hitDirection;
		float hitLength{};
		Vector3 direction;
		Vector3 offset;
		Vector3 offsetPosition = bodyPos;
		Vector3 force;
		Vector3 rotateAxis;


		for (Vector3& point : pointArray)
		{
			direction = point - bodyPos;
			hit = field->RayHitCheck(bodyPos, direction, &hitPosition, &hitNormal);

			if (hit)
			{
				hitDirection = point - hitPosition;
				hitLength = Vector3::Dot(hitDirection, hitNormal);
				offset = hitNormal * -hitLength;

				offsetPosition += offset;


				rotateAxis = Vector3::Cross(direction, offset) / direction.Length() / 5.0f;

				RotateAxisWorld(rotateAxis);
			}

		}

		SetPosition(offsetPosition);

	}


}



void BodyRB::DrawDebug()
{


	if (ImGui::CollapsingHeader("Body"))
	{
		char buf[32];

		ImGui::Checkbox("Visible", &m_Visible);

		Vector3 v = GetVelocity();
		float vl = v.Length();
		sprintf(buf, "Velocity %.1f m/s", vl);
		ImGui::ProgressBar(vl / 100.0f, ImVec2(0, 16), buf);

		ImGui::SliderFloat("CD", &m_CD, 0.0f, 1.0f, "%.3f");
		ImGui::SliderFloat("AirDensity", &m_AirDensity, 0.0f, 10.0f, "%.3f kg/m3");

		sprintf(buf, "AirResistance %.0f N", m_AirFoeceLen);
		ImGui::ProgressBar(m_AirFoeceLen / 10000.0f, ImVec2(0, 16), buf);


		ImGui::SliderFloat("DownForceRatioF", &m_DownForceRatioF, 0.0f, 10.0f, "%.3f");
		sprintf(buf, "DownForceF %.0f N", m_DownForceLenF);
		ImGui::ProgressBar(m_DownForceLenF / 10000.0f, ImVec2(0, 16), buf);

		ImGui::SliderFloat("DownForceRatioR", &m_DownForceRatioR, 0.0f, 10.0f, "%.3f");
		sprintf(buf, "DownForceR %.0f N", m_DownForceLenR);
		ImGui::ProgressBar(m_DownForceLenR / 10000.0f, ImVec2(0, 16), buf);


		ImGui::SliderFloat3("BodyOffset", (float*)&m_Offset, -0.5f, 0.5f, "%.3f m");




		Vector3 hitPosition;
		Vector3 hitNormal;
		Vector3 dir;
		
		GameScene* scene = (GameScene*)GameManager::GetInstance()->GetScene();
		Field* field = scene->GetGameObject<Field>();
		field->RayHitCheck(GetPosition(), -GetMatrix().Up(), &hitPosition, &hitNormal);
		

		dir = GetPosition() - hitPosition;

		sprintf(buf, "GCenterHeight %.3f m", dir.Length());
		ImGui::LabelText("", buf);
	}

}




void BodyRB::PreDraw()
{
	m_OldWorldMatrix = m_WorldMatrix;

	m_WorldMatrix = Matrix44::Identity();
	m_WorldMatrix *= Matrix44::TranslateXYZ(m_Offset.X, m_Offset.Y, m_Offset.Z);
	m_WorldMatrix *= GetMatrix();

}



void BodyRB::Draw(Camera* DrawCamera, int LodLevel)
{
	if (!m_Visible)
		return;




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
	constant.Param = { 0.0f, 0.0f, 0.1f, 1.0f };
	render->SetConstant(2, &constant, sizeof(constant));




	std::unordered_map<std::string, MATERIAL> overridMaterial;
	MATERIAL material{};

	material.BaseColor = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
	material.EmissionColor = Vector4(10.0f, 10.0f, 10.0f, 1.0f);
	overridMaterial["camc"] = material;

	material.BaseColor = Vector4(0.5f, 0.0f, 0.0f, 1.0f);
	material.EmissionColor = Vector4(1.0f, 0.1f, 0.1f, 1.0f);
	overridMaterial["tail"] = material;
	overridMaterial["stop_side"] = material;

	if (m_BrakeLamp)
	{
		material.BaseColor = Vector4(0.5f, 0.0f, 0.0f, 1.0f);
		material.EmissionColor = Vector4(10.0f, 1.0f, 1.0f, 1.0f);
		overridMaterial["stop_side"] = material;
		overridMaterial["stop_top"] = material;
	}


	for (int level = LodLevel; level < 5; level++)
	{
		if (m_Model[level].IsLoaded())
		{
			m_Model[level].Draw(false, &overridMaterial);
			break;
		}
	}

	//m_Model[LodLevel].Draw(false, &overridMaterial);

}

