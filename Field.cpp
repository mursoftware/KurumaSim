#include "Pch.h"
#include "GameManager.h"
#include "RenderManager.h"
#include "Field.h"
#include "Camera.h"
#include "ShadowCamera.h"
#include "Car.h"






Field::Field()
{


}



void Field::Load(int Index)
{	

	if(Index == 0)
	{
		m_Model.Load("Asset\\Road\\ShowRoom\\ShowRoom_Out.obj", THREAD_PRIORITY_ABOVE_NORMAL, true);

		m_LightRotation = { 0.6006f * PI * 2.0f, 0.2598f * PI };
		m_LightColor = Vector3{ 0.9f, 0.9f, 1.0f };
		m_LightIntensity = 5.0f;

		m_Fog = 3.0f;
	}
	else if (Index == 1)
	{
		m_Model.Load("Asset\\Road\\Forest\\Forest.obj", THREAD_PRIORITY_ABOVE_NORMAL, true);

		m_LightRotation = { 0.6006f * PI * 2.0f, 0.2598f * PI };
		m_LightColor = Vector3{ 0.9f, 0.9f, 1.0f };
		m_LightIntensity = 5.0f;

		m_Fog = 3.0f;
	}

	m_CloudDensity = 0.5f;
	m_CloudHeight = 0.05f;

}


Field::~Field()
{

}



void Field::Update(float dt)
{
	m_Time += dt;
}



void Field::DrawDepth(Camera* DrawCamera)
{
	RenderManager* render = RenderManager::GetInstance();


	//m_Camera = GameManager::GetInstance()->GetScene()->GetCurrentCamera();


	//マトリクス設定
	Matrix44 view = DrawCamera->GetViewMatrix();
	Matrix44 projection = DrawCamera->GetProjectionMatrix();
	Matrix44 world = Matrix44::Identity();

	Matrix44 oldView = DrawCamera->GetOldViewMatrix();

	//定数バッファ設定
	OBJECT_CONSTANT constant;
	constant.WVP = Matrix44::Transpose(world * view * projection);
	constant.OldWVP = Matrix44::Transpose(world * oldView * projection);
	constant.World = Matrix44::Transpose(world);
	constant.Param = { 0.0f, 0.0f, 0.0f, 0.0f };
	render->SetConstant(2, &constant, sizeof(constant));


	//描画
	m_Model.Draw();

}



void Field::Draw(Camera* DrawCamera)
{	

	//定数バッファ設定
	{
		ENV_CONSTANT constant;

		constant.Fog = m_Fog;

		constant.LightDirection.X = sinf(m_LightRotation.Y) * cosf(m_LightRotation.X);
		constant.LightDirection.Z = sinf(m_LightRotation.Y) * sinf(m_LightRotation.X);
		constant.LightDirection.Y = cosf(m_LightRotation.Y);

		constant.LightColor = m_LightColor * m_LightIntensity;
		
		constant.Time = m_Time;

		constant.CloudDensity = m_CloudDensity;
		constant.CloudHeight = m_CloudHeight;



		//大気散乱
		Vector3 wavelength = Vector3(0.650f, 0.570f, 0.475f);
		Vector3 wavelength4inv;
		wavelength4inv.X = 1.0f / powf(wavelength.X, 4);
		wavelength4inv.Z = 1.0f / powf(wavelength.Z, 4);
		wavelength4inv.Y = 1.0f / powf(wavelength.Y, 4);

		float atomDensityLight = 1.0 + pow(1.0 - constant.LightDirection.Y, 10) * 10.0;

		constant.ScatteringLight.X = constant.LightColor.X * exp(-atomDensityLight * atomDensityLight * wavelength4inv.X * 0.01);
		constant.ScatteringLight.Z = constant.LightColor.Z * exp(-atomDensityLight * atomDensityLight * wavelength4inv.Z * 0.01);
		constant.ScatteringLight.Y = constant.LightColor.Y * exp(-atomDensityLight * atomDensityLight * wavelength4inv.Y * 0.01);



		RenderManager::GetInstance()->SetConstant(0, &constant, sizeof(constant));
	}



	//描画
	{
		RenderManager* render = RenderManager::GetInstance();
		render->SetPipelineState("Field");


		{
			//Camera* camera = GameManager::GetInstance()->GetScene()->GetCurrentCamera();
			ShadowCamera** shadowCamera = GameManager::GetInstance()->GetScene()->GetCurrentShadowCamera();


			//マトリクス設定
			Matrix44 world = Matrix44::Identity();

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


			//定数バッファ設定
			OBJECT_CONSTANT constant;
			constant.WVP = Matrix44::Transpose(world * view * projection);
			constant.OldWVP = Matrix44::Transpose(world * oldView * oldProjection);
			constant.World = Matrix44::Transpose(world);
			constant.ShadowWVP[0] = Matrix44::Transpose(world * shadowView0 * shadowProjection0);
			constant.ShadowWVP[1] = Matrix44::Transpose(world * shadowView1 * shadowProjection1);
			constant.ShadowWVP[2] = Matrix44::Transpose(world * shadowView2 * shadowProjection2);
			constant.Param = { 0.0f, 0.0f, 0.3f, 1.0f };
			render->SetConstant(2, &constant, sizeof(constant));

		}

		{
			BodyRB* bodyRB = GameManager::GetInstance()->GetScene()->GetGameObject<Car>()->GetBodyRB();

			BLOB_SHADOW_CONSTANT constant;

			constant.BodyW = Matrix44::Inverse(bodyRB->GetMatrix());
			//constant.BodyW = bodyRB->GetMatrix();
			constant.BodyW = Matrix44::Transpose(constant.BodyW);
			render->SetConstant(4, &constant, sizeof(constant));


			render->SetGraphicsRootDescriptorTable(RenderManager::CBV_REGISTER_MAX + 8, bodyRB->GetShadowTexture()->ShaderResourceView.Index);

		}

		//描画
		m_Model.Draw();
	}

}

Vector3 Field::GetLightDirection()
{
	Vector3 lightDirection;

	lightDirection.X = sinf(m_LightRotation.Y) * cosf(m_LightRotation.X);
	lightDirection.Z = sinf(m_LightRotation.Y) * sinf(m_LightRotation.X);
	lightDirection.Y = cosf(m_LightRotation.Y);

	return lightDirection;
}



void Field::DrawDebug()
{
	ImGui::Begin("Field");

	ImGui::SliderFloat("Fog", &m_Fog, 0.0f, 10.0f, "%.2f");

	ImGui::SliderFloat("LightRotationX", &m_LightRotation.X, 0.0f, PI * 2.0f, "%.2f");
	ImGui::SliderFloat("LightRotationY", &m_LightRotation.Y, 0.0f, PI * 0.5f, "%.2f");

	ImGui::ColorPicker3("LightColor", (float*)&m_LightColor, ImGuiColorEditFlags_PickerHueWheel);
	ImGui::SliderFloat("LightIntencity", &m_LightIntensity, 0.0f, 10.0f, "%.2f");

	ImGui::SliderFloat("CloudDensity", &m_CloudDensity, 0.0f, 1.0f, "%.2f");
	ImGui::SliderFloat("CloudHeight", &m_CloudHeight, 0.0f, 0.5f, "%.3f");

	ImGui::End();
	
}




bool Field::RayHitCheck(Vector3 position, Vector3 direction, Vector3* hitPosition, Vector3* hitNormal)
{

	return m_Model.RayHitCheck(position, direction, hitPosition, hitNormal);

}

