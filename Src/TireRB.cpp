

#include "Pch.h"
//#include "Common.h"
#include "GameManager.h"
#include "GameScene.h"
#include "Field.h"
#include "TireRB.h"
#include "Setting.h"
#include "Camera.h"
#include "ShadowCamera.h"




void TireRB::Load(const char* FileName, const char* PartName, float ScaleX)
{

	std::string path = FileName;
	path = path.substr(0, path.rfind('\\') + 1);


	std::string fileName;
	fileName = path;
	fileName += GetPrivateProfileStdString(PartName, "MODEL", FileName);
	m_Model[4].Load((fileName + "_4.obj").c_str(), THREAD_PRIORITY_ABOVE_NORMAL, false, true);
	m_Model[3].Load((fileName + "_3.obj").c_str(), THREAD_PRIORITY_LOWEST, false, true);
	m_Model[2].Load((fileName + "_2.obj").c_str(), THREAD_PRIORITY_LOWEST, false, true);
	m_Model[1].Load((fileName + "_1.obj").c_str(), THREAD_PRIORITY_LOWEST, false, true);
	m_Model[0].Load((fileName + "_0.obj").c_str(), THREAD_PRIORITY_LOWEST, false, true);

	m_ScaleX = ScaleX;


	m_Radius = GetPrivateProfileFloat(PartName, "RADIUS", FileName);
	float width = GetPrivateProfileFloat(PartName, "WIDTH", FileName);
	float mass = GetPrivateProfileFloat(PartName, "MASS", FileName);

	Init(Vector3(0.0f, 0.0f, 0.0f), Quaternion::Identity());
	CalcInertiaCylinderX(mass, width, m_Radius);

	m_Inertia *= GetPrivateProfileVector3(PartName, "INERTIA_RATIO", FileName);;


	m_BrushStaticFrictionRatio = GetPrivateProfileFloat(PartName, "STATIC_FRICTION", FileName);
	m_BrushDynamicFrictionRatio = GetPrivateProfileFloat(PartName, "DYNAMIC_FRICTION", FileName);

	m_BrushSpringRatio = GetPrivateProfileVector3(PartName, "SPRING_RATIO", FileName);;

	m_BrushDamping = 1000.0f;
	m_BrushDampingMax = 5000.0f;


	InitBursh();




}

void TireRB::InitBursh()
{

	for (int i = 0; i < BRUSH_COUNT; i++)
	{
		Vector3 bp;
		bp.X = 0.0f;
		bp.Y = sinf(-i * 2.0f * PI / BRUSH_COUNT) * m_Radius;
		bp.Z = cosf(i * 2.0f * PI / BRUSH_COUNT) * m_Radius;
		m_Brush[i].Position = bp;

		m_Brush[i].HitPosition = Vector3(0.0f, 0.0f, 0.0f);
		m_Brush[i].Displacement = Vector3(0.0f, 0.0f, 0.0f);
		m_Brush[i].Hit = false;
	}

}



void TireRB::Update(float dt)
{
	GameScene* scene = (GameScene*)GameManager::GetInstance()->GetScene();
	Field* field = scene->GetGameObject<Field>();




	Vector3 facePosition;
	Vector3 faceNormal;
	Vector3 hitPosition;
	Vector3 hitNormal;
	bool hit{};


	m_Srip = 0.0f;




	float inputTorque1 = 0.0f;
	if (m_InputDriveTrain1)
		inputTorque1 = m_InputDriveTrain1->GetOutputTorque1();

	if (m_InputDriveTrain2)
		inputTorque1 = m_InputDriveTrain2->GetOutputTorque2();





	StorePositionRotation();



	float ddt = dt / 5.0f;

	for (int d = 0; d < 5; d++)
	{
		AddTorqueWorld(GetMatrix().Right() * inputTorque1, ddt);
		m_BodyRB->AddTorqueWorld(GetPosition(), GetMatrix().Right() * -inputTorque1, ddt);


		Matrix44 matrix = GetMatrix();
		Vector3 position = GetPosition();
		Vector3 tireRight = matrix.Right();

		//Vector3 dir = Vector3::Cross(matrix.Right(), m_BodyRB->GetMatrix().Front());
		//dir.Normalize();


		//hit = field->RayHitCheck(position, dir * m_Radius * 1.0f, &facePosition, &faceNormal);

		//if (hit && faceNormal.Y > 0.5f)
		{
			for (TIRE_BRUSH& brush : m_Brush)
			{
				Vector3 bp = Vector3::TransformCoord(matrix, brush.Position);
				Vector3 vtb = bp - position;

				//hit = RayHitCheck(facePosition, faceNormal, position, vtb, &hitPosition, &hitNormal);
				hit = field->RayHitCheck(position, vtb, &hitPosition, &hitNormal);

				if (hit)
				{
					if (!brush.Hit)
					{
						brush.Hit = true;
						brush.HitPosition = hitPosition;
					}

					Vector3 dv = (brush.HitPosition - bp);

					float dy = Vector3::Dot(dv, hitNormal);
					Vector3 dvy = hitNormal * dy;

					float dx = Vector3::Dot(dv - dvy, tireRight);
					Vector3 dvx = tireRight * dx;

					brush.dx = dx;

					Vector3 dvz = dv - dvy - dvx;
					float dz = dvz.Length();


					//Vector3 rfv = (dv - brush.Displacement) / ddt * m_BrushDamping;
					//float rfl = rfv.Length();
					//if (rfl > m_BrushDampingMax)
					//{
					//	rfl = m_BrushDampingMax;
					//	rfv.Normalize();
					//	rfv *= rfl;
					//}

					brush.Displacement = dv;

					Vector3 fy = dvy * m_BrushSpringRatio.Y;
					Vector3 fx = dvx * m_BrushSpringRatio.X;
					Vector3 fz = dvz * m_BrushSpringRatio.Z;

					Vector3 fh = fx + fz;
					float fhl = fh.Length();

					float fyl = fy.Length();	
					if (fyl > 1000.0f)
					{
						fy /= fyl;
						fyl = 1000.0f;
						fy *= fyl;
					}
					
					if (fhl > fyl * m_BrushStaticFrictionRatio)
					{
						float fhld = fyl * m_BrushDynamicFrictionRatio;
						fh.Normalize();
						fh *= fhld;
						Vector3 fxd = tireRight * Vector3::Dot(fh, tireRight);
						Vector3 fzd = fh - fxd;
						Vector3 srip = (fxd - fx) / m_BrushSpringRatio.X + (fzd - fz) / m_BrushSpringRatio.Z;
						brush.HitPosition += srip;

						m_Srip += srip.Length();
					}

					AddForceWorld(hitPosition, fy + fh/* + rfv*/, ddt);

				}
				else
				{
					brush.Hit = false;
					brush.HitPosition = Vector3(0.0f, 0.0f, 0.0f);
					brush.Displacement = Vector3(0.0f, 0.0f, 0.0f);
				}
			}


		}/*
		else
		{

			for (TIRE_BRUSH& brush : m_Brush)
			{
				brush.Hit = false;
				brush.HitPosition = Vector3(0.0f, 0.0f, 0.0f);
				brush.Displacement = Vector3(0.0f, 0.0f, 0.0f);
			}

		}*/


		UpdatePositionRotation(ddt);
	}

}




void TireRB::DrawDebug(const char* DebugName)
{
	ImGui::Begin(DebugName);
	{
		char buf[32];
		sprintf(buf, "Srip %.3f", m_Srip);
		ImGui::ProgressBar(m_Srip * 5.0f, ImVec2(0, 15), buf);

		for (TIRE_BRUSH& brush : m_Brush)
		{
			if (!brush.Hit)
				continue;

			ImGui::ProgressBar(brush.dx * 50.0f + 0.5f, ImVec2(0, 5), "");
		}
	}
	ImGui::End();
}


bool TireRB::RayHitCheck(Vector3 facePosition, Vector3 faceNormal, Vector3 position, Vector3 direction, Vector3* hitPosition, Vector3* hitNormal)
{

	Vector3 vp0;
	float dnp{}, dnd{}, t{};

	vp0 = facePosition - position;
	dnp = Vector3::Dot(faceNormal, vp0);
	dnd = Vector3::Dot(faceNormal, direction);

	if (dnd >= 0.0f)
		return false;

	t = dnp / dnd;

	if (t < 0.0f || 1.0f < t)
		return false;


	Vector3 hp;

	hp = position + direction * t;




	*hitPosition = hp;
	*hitNormal = faceNormal;

	return true;

}



void TireRB::PreDraw()
{
	m_OldWorldMatrix = m_WorldMatrix;


	m_WorldMatrix = Matrix44::Identity();
	m_WorldMatrix *= Matrix44::Scale(-m_ScaleX, 1.0f, -m_ScaleX);
	m_WorldMatrix *= GetMatrix();


	m_OldDrawPosition = m_DrawPosition;
	m_OldDrawRotation = m_DrawRotation;

	m_DrawPosition = m_Position;
	m_DrawRotation = m_Rotation;

}


void TireRB::Draw(Camera* DrawCamera, int LodLevel)
{

	RenderManager* render = RenderManager::GetInstance();

	ShadowCamera** shadowCamera = GameManager::GetInstance()->GetScene()->GetCurrentShadowCamera();



	int level = LodLevel;
	for (; level < 5; level++)
	{
		if (m_Model[level].IsLoaded())
		{
			break;
		}
	}

	if (level >= 5)
		return;



	float size = m_Model[level].GetSize();
	Matrix44 scale = Matrix44::Scale(size, size, size);

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




	Matrix44 oldWorldMatrix, transMatrix, rotMatrix;
	{
		Matrix44 transMatrix, rotMatrix;

		oldWorldMatrix = Matrix44::Scale(-m_ScaleX, 1.0f, -m_ScaleX);

		Quaternion slr = Quaternion::Slerp(m_OldDrawRotation, m_Rotation, 0.9f);
		rotMatrix = Matrix44::RotationQuaternion(slr);
		oldWorldMatrix = Matrix44::Multiply(oldWorldMatrix, rotMatrix);

		transMatrix = Matrix44::TranslateXYZ((float)m_OldDrawPosition.X, (float)m_OldDrawPosition.Y, (float)m_OldDrawPosition.Z);
		oldWorldMatrix = Matrix44::Multiply(oldWorldMatrix, transMatrix);
	}





	OBJECT_CONSTANT constant;
	constant.WVP = Matrix44::Transpose(scale * m_WorldMatrix * view * projection);
	constant.OldWVP = Matrix44::Transpose(scale * oldWorldMatrix * oldView * oldProjection);
	constant.World = Matrix44::Transpose(scale * m_WorldMatrix);
	constant.ShadowWVP[0] = Matrix44::Transpose(scale * m_WorldMatrix * shadowView0 * shadowProjection0);
	constant.ShadowWVP[1] = Matrix44::Transpose(scale * m_WorldMatrix * shadowView1 * shadowProjection1);
	constant.ShadowWVP[2] = Matrix44::Transpose(scale * m_WorldMatrix * shadowView2 * shadowProjection2);
	constant.Param = { 0.0f, 0.0f, 0.2f, 1.0f };
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

