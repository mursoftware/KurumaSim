#include "Pch.h"
#include "Main.h"

#include "GameManager.h"
#include "GameScene.h"
#include "CarSelectScene.h"


GameManager* GameManager::m_Instance{};





GameManager::GameManager()
{
	m_Instance = this;
	m_LastTime = timeGetTime();


	//m_RenderManger.ResourceCommandBegin();

	//SetScene<GameScene>();
	SetScene<CarSelectScene>();

	//m_RenderManger.ResourceCommandEnd();

	StartDraw();


}




GameManager::~GameManager()
{
	m_RunningDraw = false;

	WaitDraw();
}




void GameManager::StartDraw()
{
	std::thread thDraw(&GameManager::Draw, this);
	thDraw.detach();

}




void GameManager::WaitDraw()
{
	m_RunningDraw = false;

	{
		std::unique_lock<std::mutex> lock(m_DrawMutex);

		m_RenderManger.WaitGPU();
	}

}




void GameManager::UpdateFade()
{

	float dt = 1.0f / 60.0f / 10.0f;

	if (m_Fade == 1)
	{

		if (m_FadeTime > 0.5f)
		{
			{
				//std::unique_lock<std::mutex> lock(m_DrawMutex);

				//m_RenderManger.WaitGPU();

				WaitDraw();

				//m_CommandMutex.lock();
				//m_RenderManger.WaitGPU();

				//m_RenderManger.ResourceCommandBegin();

				m_Scene = std::move(m_NextScene);
				m_Scene->Load();

				m_FrameIndex = 0;

				//m_RenderManger.ResourceCommandEnd();

				//m_CommandMutex.unlock();

				StartDraw();

			}

			m_FadeTime = 0.5f;
			m_Fade = -1;

			//return;
		}

		m_FadeTime += dt;

	}
	else if (m_Fade == -1)
	{
		m_FadeTime -= dt;

		if (m_FadeTime < 0.0f)
		{
			m_FadeTime = 0.0f;
			m_Fade = 0;
		}

	}

}





void GameManager::DrawFade()
{
	//フェード描画
	if (m_Fade != 0)
	{
		m_RenderManger.SetPipelineState("Ui");

		m_RenderManger.SetGraphicsRootDescriptorTable(RenderManager::CBV_REGISTER_MAX + 0, 0);
		m_RenderManger.DrawSprite(
			Vector3(0.0f, 0.0f, 1.0f),
			Vector3((float)m_RenderManger.GetBackBufferWidth(), (float)m_RenderManger.GetBackBufferHeight(), 1.0f),
			Vector3(0.0f, 0.0f, 0.0f),
			Vector4(0.0f, 0.0f, 0.0f, m_FadeTime * 2.0f));

	}

}






void GameManager::Update()
{
	{
		std::lock_guard<std::mutex> lock(m_UpdateMutex);

		m_InputManager.Update();
		m_Scene->Update();
	}

	UpdateFade();
}




void GameManager::Draw()
{
	SetThreadDescription(GetCurrentThread(), L"DrawThread");




	m_RunningDraw = true;






	while (m_RunningDraw)
	{

		{
			std::unique_lock<std::mutex> lock(m_DrawMutex);



			{
				std::lock_guard<std::mutex> lock(m_UpdateMutex);


				m_Scene->PreDraw();
			}





			{
				//std::lock_guard<std::mutex> lock(m_CommandMutex);



				m_RenderManger.PreDraw();


				m_RenderManger.StartCommandTime("Total");


				m_RenderManger.StartCommandTime("ShadowMap");

				{
					m_RenderManger.DrawDepthBegin(0);
					m_Scene->DrawDepth(0);
					m_RenderManger.DrawDepthEnd(0);
				}

				{
					int i = m_FrameIndex % 2 + 1;
					m_RenderManger.DrawDepthBegin(i);
					m_Scene->DrawDepth(i);
					m_RenderManger.DrawDepthEnd(i);
				}

				m_RenderManger.EndCommandTime("ShadowMap");





				////if(m_FrameIndex < 60)
				//{
				//	m_RenderManger.StartCommandTime("EnvMapStatic");
				//	for (int i = 0; i < 6; i++)
				//	{
				//		if (m_FrameIndex % 2 == i % 2)
				//		{
				//			m_RenderManger.DrawEnvStaticBegin(i);
				//			m_Scene->DrawEnvStatic(i);
				//			m_RenderManger.DrawEnvStaticEnd(i);
				//		}
				//	}
				//	m_RenderManger.EndCommandTime("EnvMapStatic");


				//	m_RenderManger.StartCommandTime("IBLStatic");
				//	m_RenderManger.DrawIBLStatic();
				//	m_RenderManger.EndCommandTime("IBLStatic");
				//}



				{
					m_RenderManger.StartCommandTime("EnvMap");
					for (int i = 0; i < 6; i++)
					{
						if (m_FrameIndex % 2 == i % 2)
						{
							m_RenderManger.DrawEnvBegin(i);
							m_Scene->DrawEnv(i);
							m_RenderManger.DrawEnvEnd(i);
						}
					}
					m_RenderManger.EndCommandTime("EnvMap");




					m_RenderManger.StartCommandTime("IBL");
					m_RenderManger.DrawIBL();
					m_RenderManger.EndCommandTime("IBL");
				}

				m_RenderManger.DrawIBLStatic();


#if 1
				m_RenderManger.StartCommandTime("ZPrePass");
				m_RenderManger.DrawZPrePassBegin();
				m_Scene->DrawZPrePass();
				m_RenderManger.DrawZPrePassEnd();
				m_RenderManger.EndCommandTime("ZPrePass");

#endif

				m_RenderManger.StartCommandTime("Scene");
				m_RenderManger.DrawBegin();
				m_Scene->Draw();
				m_RenderManger.DrawEnd();
				m_RenderManger.EndCommandTime("Scene");


				m_RenderManger.StartCommandTime("AntiAliasing");
				m_RenderManger.DrawAntiAliasing();
				m_RenderManger.EndCommandTime("AntiAliasing");


				m_RenderManger.StartCommandTime("ShrinkBlur");
				m_RenderManger.DrawShrinkBlur();
				m_RenderManger.EndCommandTime("ShrinkBlur");


				m_RenderManger.StartCommandTime("Shrink");
				m_RenderManger.DrawShrinkBegin();
				m_Scene->DrawShrink();
				m_RenderManger.DrawShrinkEnd();
				m_RenderManger.EndCommandTime("Shrink");


				m_RenderManger.StartCommandTime("Post");
				m_RenderManger.DrawPost();
				m_RenderManger.EndCommandTime("Post");


				m_RenderManger.EndCommandTime("Total");

				m_Scene->DrawUI();
				DrawFade();
				m_InputManager.DrawDebug();

				m_RenderManger.DrawUI(m_Scene->GetDebugVisible());




			}


			m_FrameIndex++;


		}


	}



}


