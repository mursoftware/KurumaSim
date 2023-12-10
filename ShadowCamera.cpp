#include "Pch.h"
#include "Main.h"
#include "GameManager.h"
#include "InputManager.h"
#include "RenderManager.h"
#include "ShadowCamera.h"
#include "Car.h"
#include "CarCamera.h"

#include "GameScene.h"




ShadowCamera::ShadowCamera()
{
	m_Position = Vector3{ 0.0f, 1.5f, -3.0f };
	m_Target = Vector3{ 0.0f, 0.5f, 0.0f };
	m_Rotation = Vector3{ 0.0f, 0.0f, 0.0f };
	m_Up = Vector3{ 0.0f, 1.0f, 0.0f };
}






void ShadowCamera::Update()
{

	GameManager* gameManager = GameManager::GetInstance();
	GameScene* scene = (GameScene*)gameManager->GetScene();
	Car* car = scene->GetGameObject<Car>();
	CarCamera* carCamera = scene->GetGameObject<CarCamera>();

	Vector3 cameraPosition = carCamera->GetPosition();
	Vector3 cameraTarget = carCamera->GetTarget();
	Vector3 cameraDirection = cameraTarget - cameraPosition;
	Vector3 carPosition = car->GetPosition();



	cameraDirection.Y = 0.0f;
	cameraDirection.Normalize();


	cameraPosition.Y = carPosition.Y;


	m_Up = -cameraDirection;

	switch (m_Index)
	{
	case 0:
		//m_Target = cameraPosition + cameraDirection * 8.0f;
		m_Target = carPosition;
		m_Position = m_Target + m_LightDirection * 12.5f;
		break;

	case 1:
		m_Target = cameraPosition + cameraDirection * 20.0f;
		m_Position = m_Target + m_LightDirection * 100.0f;
		break;

	case 2:
		m_Target = cameraPosition + cameraDirection * 130.0f;
		m_Position = m_Target + m_LightDirection * 500.0f;
		break;

	default:
		break;
	}

}





void ShadowCamera::PreDraw()
{
	GameManager* gameManager = GameManager::GetInstance();
	unsigned long frame = gameManager->GetFrameIndex();

	if (m_Index != 0)
		if (frame % 2 + 1 != m_Index)
			return;


	m_OldViewMatrix = m_ViewMatrix;

	m_DrawPosition = m_Position;

	m_ViewMatrix = Matrix44::LookAt(m_Position, m_Target, m_Up);


	//m_ProjectionMatrix = Matrix44::PerspectiveFov(PI * 0.5f, 1.0f, 10.0f, 30.0f);


	switch (m_Index)
	{
	case 0:
		m_ProjectionMatrix = Matrix44::OrthoCenter(5.0f, 5.0f, 25.0f);
		break;

	case 1:
		m_ProjectionMatrix = Matrix44::OrthoCenter(40.0f, 40.0f, 200.0f);
		break;

	case 2:
		m_ProjectionMatrix = Matrix44::OrthoCenter(200.0f, 200.0f, 1000.0f);
		break;


	default:
		break;
	}


	//{
	//	float dx{}, dy{};

	//	dx = ((float)rand() / RAND_MAX - 0.5f) * 2.0f / 512.0f;
	//	dy = ((float)rand() / RAND_MAX - 0.5f) * 2.0f / 512.0f;

	//	m_ProjectionMatrix *= Matrix44::TranslateXYZ(dx, dy, 0.0f);
	//}

}




void ShadowCamera::Draw()
{
	//定数バッファ設定
	CAMERA_CONSTANT constant{};
	constant.CameraPosition = { m_DrawPosition.X, m_DrawPosition.Y, m_DrawPosition.Z, 0.0f };
	RenderManager::GetInstance()->SetConstant(1, &constant, sizeof(constant));


}



void ShadowCamera::DrawDebug()
{


}




