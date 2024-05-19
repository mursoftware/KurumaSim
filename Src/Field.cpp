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
	}
	else if (Index == 1)
	{
		m_Model.Load("Asset\\Road\\Forest\\Forest.obj", THREAD_PRIORITY_ABOVE_NORMAL, true);
	}

	m_LightRotation = { 0.6006f * PI * 2.0f, 0.2598f * PI };
	m_LightTemperature = 6000.0f;
	m_LightIntensity = 130000.0f;

	m_CloudDensity = 0.3f;
	m_CloudHeight = 0.1f;

	m_Fog = 0.0003f;

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


	float size = m_Model.GetSize();
	Matrix44 scale = Matrix44::Scale(size, size, size);

	Matrix44 view = DrawCamera->GetViewMatrix();
	Matrix44 projection = DrawCamera->GetProjectionMatrix();
	Matrix44 world = Matrix44::Identity();

	Matrix44 oldView = DrawCamera->GetOldViewMatrix();


	OBJECT_CONSTANT constant;
	constant.WVP = Matrix44::Transpose(scale * world * view * projection);
	constant.OldWVP = Matrix44::Transpose(scale * world * oldView * projection);
	constant.World = Matrix44::Transpose(world);
	constant.Param = { 0.0f, 0.0f, 0.0f, 0.0f };
	render->SetConstant(2, &constant, sizeof(constant));



	m_Model.Draw();

}


float Saturate(float in)
{
	float ret;

	ret = min(in, 1.0f);
	ret = max(in, 0.0f);

	return ret;
}

Vector3 ColorTemperatureToRGB(float temperatureInKelvins)
{
	Vector3 retColor;

	temperatureInKelvins = temperatureInKelvins / 100.0f;

	if (temperatureInKelvins <= 66.0)
	{
		retColor.X = 1.0;
		retColor.Y = Saturate(0.39008157876901960784f * log(temperatureInKelvins) - 0.63184144378862745098f);
	}
	else
	{
		float t = abs(temperatureInKelvins - 60.0f);
		retColor.X = Saturate(1.29293618606274509804f * pow(t, -0.1332047592f));
		retColor.Y = Saturate(1.12989086089529411765f * pow(t, -0.0755148492f));
	}

	if (temperatureInKelvins >= 66.0f)
		retColor.Z = 1.0f;
	else if (temperatureInKelvins <= 19.0f)
		retColor.Z = 0.0f;
	else
		retColor.Z = Saturate(0.54320678911019607843f * log(temperatureInKelvins - 10.0f) - 1.19625408914f);

	return retColor;
}



void Field::Draw(Camera* DrawCamera)
{	

	{
		ENV_CONSTANT constant;

		constant.Fog = m_Fog;

		constant.LightDirection.X = sinf(m_LightRotation.Y) * cosf(m_LightRotation.X);
		constant.LightDirection.Z = sinf(m_LightRotation.Y) * sinf(m_LightRotation.X);
		constant.LightDirection.Y = cosf(m_LightRotation.Y);

		constant.LightColor = ColorTemperatureToRGB(m_LightTemperature) * m_LightIntensity / 10000.0f;//Luminance unit in shader is 1/10000 nit
		
		constant.Time = m_Time;

		constant.CloudDensity = m_CloudDensity;
		constant.CloudHeight = m_CloudHeight;



		//atmospheric dispersion
		Vector3 wavelength = Vector3(0.650f, 0.570f, 0.475f);
		Vector3 wavelength4inv;
		wavelength4inv.X = 1.0f / powf(wavelength.X, 4.0f);
		wavelength4inv.Z = 1.0f / powf(wavelength.Z, 4.0f);
		wavelength4inv.Y = 1.0f / powf(wavelength.Y, 4.0f);

		float atomThicknessRatio = 0.05f;
		float atomDensityLight = atomThicknessRatio + pow(1.0f - constant.LightDirection.Y, 10.0f) * (1.0f - atomThicknessRatio);
		float attenuation = atomDensityLight * atomDensityLight * 0.5f;

		constant.ScatteringLight.X = constant.LightColor.X * exp(-attenuation * wavelength4inv.X);
		constant.ScatteringLight.Z = constant.LightColor.Z * exp(-attenuation * wavelength4inv.Z);
		constant.ScatteringLight.Y = constant.LightColor.Y * exp(-attenuation * wavelength4inv.Y);

		//float lumi = constant.ScatteringLight.X * 0.3 + constant.ScatteringLight.Y * 0.6 + constant.ScatteringLight.Z * 0.1;

		constant.ScatteringLight *= (1.0f - m_CloudDensity);

		//fog
		//float fog = exp(-atomDensityLight * 1000.0f * m_Fog);
		//constant.ScatteringLight *= fog;


		RenderManager::GetInstance()->SetConstant(0, &constant, sizeof(constant));
	}




	{
		RenderManager* render = RenderManager::GetInstance();
		render->SetPipelineState("Field");


		{
			//Camera* camera = GameManager::GetInstance()->GetScene()->GetCurrentCamera();
			ShadowCamera** shadowCamera = GameManager::GetInstance()->GetScene()->GetCurrentShadowCamera();



			Matrix44 world = Matrix44::Identity();

			float size = m_Model.GetSize();
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



			OBJECT_CONSTANT constant;
			constant.WVP = Matrix44::Transpose(scale * world * view * projection);
			constant.OldWVP = Matrix44::Transpose(scale * world * oldView * oldProjection);
			constant.World = Matrix44::Transpose(scale * world);
			constant.ShadowWVP[0] = Matrix44::Transpose(scale * world * shadowView0 * shadowProjection0);
			constant.ShadowWVP[1] = Matrix44::Transpose(scale * world * shadowView1 * shadowProjection1);
			constant.ShadowWVP[2] = Matrix44::Transpose(scale * world * shadowView2 * shadowProjection2);
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


	if (ImGui::CollapsingHeader("SunLight"))
	{
		ImGui::SliderFloat("RotationX", &m_LightRotation.X, 0.0f, PI * 2.0f, "%.2f");
		ImGui::SliderFloat("RotationY", &m_LightRotation.Y, 0.0f, 1.8f, "%.2f");

		ImGui::SliderFloat("Temperature", &m_LightTemperature, 1000.0f, 10000.0f, "%.0f K");
		ImGui::SliderFloat("Intensity", &m_LightIntensity, 0.0f, 200000.0f, "%.0f lx");
	}


	if (ImGui::CollapsingHeader("Cloud"))
	{
		ImGui::SliderFloat("Density", &m_CloudDensity, 0.0f, 1.0f, "%.2f");
		ImGui::SliderFloat("Height", &m_CloudHeight, 0.0f, 0.5f, "%.3f");
	}


	if (ImGui::CollapsingHeader("Fog"))
	{
		ImGui::SliderFloat("FogDensity", &m_Fog, 0.0f, 0.001f, "%.5f");
	}

	ImGui::End();
	
}




bool Field::RayHitCheck(Vector3 position, Vector3 direction, Vector3* hitPosition, Vector3* hitNormal)
{

	return m_Model.RayHitCheck(position, direction, hitPosition, hitNormal);

}

