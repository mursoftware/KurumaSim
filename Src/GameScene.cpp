#include "Pch.h"
#include "GameManager.h"
#include "GameScene.h"
#include "CarSelectScene.h"
#include "Camera.h"
#include "CarCamera.h"
#include "EnvCamera.h"
#include "ShadowCamera.h"
#include "Sky.h"
#include "Field.h"
#include "Car.h"
#include "SmokeManager.h"




GameScene::GameScene()
{
}



GameScene::~GameScene()
{
}




void GameScene::Load()
{

	m_CarCamera = AddGameObject<CarCamera>();
	m_CurrentCamera = m_CarCamera;



	m_ShadowCamera[0] = AddGameObject<ShadowCamera>();
	m_ShadowCamera[0]->SetIndex(0);

	m_ShadowCamera[1] = AddGameObject<ShadowCamera>();
	m_ShadowCamera[1]->SetIndex(1);

	m_ShadowCamera[2] = AddGameObject<ShadowCamera>();
	m_ShadowCamera[2]->SetIndex(2);

	m_CurrentShadowCamera = m_ShadowCamera;



	m_EnvCamera[0] = AddGameObject<EnvCamera>();
	m_EnvCamera[0]->SetAxis(Vector3(1.0f, 0.0f, 0.0f));
	m_EnvCamera[0]->SetUp(Vector3(0.0f, 1.0f, 0.0f));

	m_EnvCamera[1] = AddGameObject<EnvCamera>();
	m_EnvCamera[1]->SetAxis(Vector3(-1.0f, 0.0f, 0.0f));
	m_EnvCamera[1]->SetUp(Vector3(0.0f, 1.0f, 0.0f));

	m_EnvCamera[2] = AddGameObject<EnvCamera>();
	m_EnvCamera[2]->SetAxis(Vector3(0.0f, 1.0f, 0.0f));
	m_EnvCamera[2]->SetUp(Vector3(0.0f, 0.0f, -1.0f));

	m_EnvCamera[3] = AddGameObject<EnvCamera>();
	m_EnvCamera[3]->SetAxis(Vector3(0.0f, -1.0f, 0.0f));
	m_EnvCamera[3]->SetUp(Vector3(0.0f, 0.0f, 1.0f));

	m_EnvCamera[4] = AddGameObject<EnvCamera>();
	m_EnvCamera[4]->SetAxis(Vector3(0.0f, 0.0f, 1.0f));
	m_EnvCamera[4]->SetUp(Vector3(0.0f, 1.0f, 0.0f));

	m_EnvCamera[5] = AddGameObject<EnvCamera>();
	m_EnvCamera[5]->SetAxis(Vector3(0.0f, 0.0f, -1.0f));
	m_EnvCamera[5]->SetUp(Vector3(0.0f, 1.0f, 0.0f));






	m_Field = AddGameObject<Field>();
	m_Field->Load(1);



	m_Sky = AddGameObject<Sky>();



	m_Car = AddGameObject<Car>();
	m_Car->InitPositionRotation(Vector3(0.0f, 0.40f, 0.0f), -PI * 0.5f);



	m_SmokeManager = AddGameObject<SmokeManager>();





	InputManager* inputManager = InputManager::GetInstance();

	if (m_Replay)
	{
		m_Car->SetReplay(true);
		m_CarCamera->SetViewMode(4);
	}
	else
	{
		m_Car->SetReplay(false);
	}



	m_CarCamera->SetLoacation(m_Field);
}




void GameScene::Update()
{
	//Scene::Update();

	if (InputManager::GetInstance()->GetKeyTrigger(VK_F1))
	{
		m_DebugVisible = !m_DebugVisible;
	}



	float dt = 1.0f / 60.0f / 10.0f;

	m_Car->Update(true, true, dt);



	float focalLength = m_CarCamera->GetFocalLength();
	m_ShadowCamera[1]->SetFocalLength(focalLength);
	m_ShadowCamera[2]->SetFocalLength(focalLength);


	Vector3 lightDirection = m_Field->GetLightDirection();
	m_ShadowCamera[0]->SetLightDirection(lightDirection);
	m_ShadowCamera[1]->SetLightDirection(lightDirection);
	m_ShadowCamera[2]->SetLightDirection(lightDirection);


	m_CarCamera->Update();
	m_ShadowCamera[0]->Update();
	m_ShadowCamera[1]->Update();
	m_ShadowCamera[2]->Update();



	m_Field->Update(dt);
	m_Sky->Update();


	m_SmokeManager->Update(dt);


	m_CurrentCamera = m_CarCamera;



	InputManager* inputManager = InputManager::GetInstance();
	GameManager* gameManager = GameManager::GetInstance();

	if (inputManager->GetButtonTrigger(6) || inputManager->GetKeyTrigger(VK_F3))
	{
		GameScene* scene = gameManager->FadeScene<GameScene>();
		if (scene)
		{
			scene->m_Replay = true;
		}
	}
	
	if (inputManager->GetButtonTrigger(7) || inputManager->GetKeyTrigger(VK_F4) || inputManager->GetKeyTrigger(VK_RETURN))
	{
		gameManager->FadeScene<CarSelectScene>();
	}
	

}


void GameScene::DrawEnvStatic(int Index)
{


	m_EnvCamera[Index]->SetPosition(Vector3(0.0f, 2.0f, 0.0f));
	m_EnvCamera[Index]->PreDraw();
	m_EnvCamera[Index]->Draw();



	m_Field->Draw(m_EnvCamera[Index]);
	m_Sky->Draw(m_EnvCamera[Index]);

	//m_Car->DrawShadow(m_EnvCamera[Index]);

}

void GameScene::DrawEnv(int Index)
{


	Vector3 envCamPosition = m_Car->GetPosition();
	envCamPosition.Y += 1.0f;
	m_EnvCamera[Index]->SetPosition(envCamPosition);
	m_EnvCamera[Index]->PreDraw();
	m_EnvCamera[Index]->Draw();



	m_Field->Draw(m_EnvCamera[Index]);
	m_Sky->Draw(m_EnvCamera[Index]);

	//m_Car->DrawShadow(m_EnvCamera[Index]);

}



void GameScene::DrawDepth(int Index)
{

	m_ShadowCamera[Index]->Draw();


	m_Field->DrawDepth(m_ShadowCamera[Index]);

	if(Index == 0)
		m_Car->DrawDepth(m_ShadowCamera[Index], 4);

}



void GameScene::DrawZPrePass()
{
	//Scene::Draw();

	m_CarCamera->Draw();


	m_Field->DrawDepth(m_CarCamera);
	//m_Sky->DrawDepth(m_CarCamera);
	//m_Car->DrawDepth(m_CarCamera, 0);

}



void GameScene::Draw()
{
	//Scene::Draw();

	m_CarCamera->Draw();


	m_Field->Draw(m_CarCamera);
	m_Sky->Draw(m_CarCamera);

	//m_Car->DrawShadow(m_CarCamera);
	m_Car->Draw(m_CarCamera, 0);

}



void GameScene::DrawShrink()
{

	m_SmokeManager->Draw(m_CarCamera);

}

void GameScene::DrawUI()
{
	Scene::DrawUI();


	ImGui::Begin("Control");

	ImGui::Text("[Game Pad]");
	ImGui::Text("RStickUp or RT : Throttle");
	ImGui::Text("RStickDown or LT : Brake");
	ImGui::Text("LStick : Steering");
	ImGui::Text("RB or A : ShiftUp");
	ImGui::Text("LB or B : ShiftDown");
	ImGui::Text("D-Pad Up/Down : ChangeCamera");

	ImGui::Separator();

	ImGui::Text("[Keyboard]");
	ImGui::Text("ArrawUp : Throttle");
	ImGui::Text("ArrawDown : Brake");
	ImGui::Text("ArrawLeftRight : Steering");
	ImGui::Text("X : ShiftUp");
	ImGui::Text("Z : ShiftDown");
	ImGui::Text("W/S : ChangeCamera");

	ImGui::Separator();

	ImGui::Text("Enter : Next Sceen");
	ImGui::Text("F3 : Replay");

	ImGui::End();

}

