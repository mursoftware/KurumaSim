#include "Pch.h"
#include "Main.h"
#include "GameManager.h"
#include "InputManager.h"
#include "RenderManager.h"
#include "EnvCamera.h"
#include "car.h"

#include "GameScene.h"




EnvCamera::EnvCamera()
{
	m_Position = Vector3{ 0.0f, 1.5f, -3.0f };
	m_Target = Vector3{ 0.0f, 0.5f, 0.0f };
	m_Rotation = Vector3{ 0.0f, 0.0f, 0.0f };
	m_Up = Vector3{ 0.0f, 1.0f, 0.0f };
}






void EnvCamera::Update()
{


}





void EnvCamera::PreDraw()
{
	m_OldViewMatrix = m_ViewMatrix;

	m_DrawPosition = m_Position;

	m_Target = m_Position + m_Axis;

	m_ViewMatrix = Matrix44::LookAt(m_Position, m_Target, m_Up);

	m_OldProjectionMatrix = m_ProjectionMatrix;

	m_ProjectionMatrix = Matrix44::PerspectiveFov(PI * 0.5f, 1.0f, 1.0f, 200.0f);

}




void EnvCamera::Draw()
{

	CAMERA_CONSTANT constant{};
	constant.CameraPosition = { m_DrawPosition.X, m_DrawPosition.Y, m_DrawPosition.Z, 0.0f };
	RenderManager::GetInstance()->SetConstant(1, &constant, sizeof(constant));


}



void EnvCamera::DrawDebug()
{


}




