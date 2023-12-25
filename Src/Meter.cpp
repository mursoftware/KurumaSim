
#include "Pch.h"

#include "GameManager.h"
#include "RenderManager.h"
#include "Camera.h"
#include "Meter.h"



Meter::Meter()
{

	m_PanleTexture = RenderManager::GetInstance()->LoadTexture("Asset\\meter.dds");
	m_NeedleTexture = RenderManager::GetInstance()->LoadTexture("Asset\\needle.dds");
	m_NumberTexture = RenderManager::GetInstance()->LoadTexture("Asset\\number_digi.dds");

}



Meter::~Meter()
{
}



void Meter::Update(float dt)
{


}



void Meter::DrawUI()
{
	RenderManager* render = RenderManager::GetInstance();
	Camera* camera = GameManager::GetInstance()->GetScene()->GetCurrentCamera();

	int cameraViewMode = camera->GetViewMode();


	if (!(cameraViewMode == 1 || cameraViewMode == 0))
		return;


	Vector3 position;
	float size = 160.0f;


	if (cameraViewMode == 0)
		position = Vector3(780.0f, -360.0f, 0.0f);

	if (cameraViewMode == 1)
		position = Vector3(0.0f, -380.0f, 0.0f);



	{
		render->SetGraphicsRootDescriptorTable(RenderManager::CBV_REGISTER_MAX + 0, m_PanleTexture->ShaderResourceView.Index);
		render->DrawSprite(
			position,
			Vector3(size, size, 1.0f),
			Vector3(0.0f, 0.0f, 0.0f),
			Vector4(1.0f, 1.0f, 1.0f, 0.8f));
	}






	{
		int gear = m_Gear;
		if (gear == -1)
			gear = 8;

		render->SetGraphicsRootDescriptorTable(RenderManager::CBV_REGISTER_MAX + 0, m_NumberTexture->ShaderResourceView.Index);

		render->DrawSprite(
			position + Vector3(0.0, 70.0f, 0.0f),
			Vector3(size / 9.0f, size / 4.5f, 1.0f),
			Vector3(0.0f, 0.0f, 0.0f),
			Vector4(1.0f, 1.0f, 1.0f, 0.8f),
			Vector2(gear * 0.1f, 0.0f),
			Vector2(0.1f, 1.0f));
	}




	{
		int speedkmph{};
		speedkmph = (int)(m_Speed / 1000.0f * 60.0f * 60.0f);
		speedkmph = abs(speedkmph);

		render->SetGraphicsRootDescriptorTable(RenderManager::CBV_REGISTER_MAX + 0, m_NumberTexture->ShaderResourceView.Index);

		for (int i = 0; i < 3; i++)
		{
			int num = speedkmph % 10;
			speedkmph /= 10;

			render->DrawSprite(
				position + Vector3(130.0, -80.0f, 0.0f) + Vector3(-30.0f * i, 0.0f, 0.0f),
				Vector3(size / 10.0f, size / 5.0f, 1.0f),
				Vector3(0.0f, 0.0f, 0.0f),
				Vector4(1.0f, 1.0f, 1.0f, 0.8f),
				Vector2(num * 0.1f, 0.0f),
				Vector2(0.1f, 1.0f));
		}

	}



	{
		float needleRotation = -PI + m_RPM / 200.0f;

		render->SetGraphicsRootDescriptorTable(RenderManager::CBV_REGISTER_MAX + 0, m_NeedleTexture->ShaderResourceView.Index);
		render->DrawSprite(
			position,
			Vector3(size / 4.0f, size, 1.0f),
			Vector3(0.0f, 0.0f, needleRotation),
			Vector4(1.0f, 1.0f, 1.0f, 0.8f));
	}


}
