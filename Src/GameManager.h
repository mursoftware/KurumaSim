#pragma once

#include "InputManager.h"
#include "RenderManager.h"
#include "SoundManager.h"
#include "Scene.h"


class GameManager
{
private:
	static GameManager* m_Instance;

	InputManager m_InputManager;
	RenderManager m_RenderManger;
	SoundManager m_SoundManager;

	DWORD m_LastTime{};



	std::unique_ptr<Scene> m_Scene;
	std::unique_ptr<Scene> m_NextScene;


	int m_DrawCount{};


	int m_Fade{};
	float m_FadeTime{};




	bool m_RunningDraw{};

	std::mutex m_FileMutex;
	std::mutex m_UpdateMutex;
	//std::mutex m_CommandMutex;
	std::mutex m_DrawMutex;


	unsigned long m_FrameIndex{};



public:
	static GameManager* GetInstance() { return m_Instance; }

	GameManager();
	~GameManager();


	void UpdateFade();
	void DrawFade();

	void Update();
	void Draw();

	void StartDraw();
	void WaitDraw();



	template <class T>
	T* SetScene()
	{
		m_Scene = std::make_unique<T>();
		m_Scene->Load();

		return (T*)m_Scene.get();
	}


	template <class T>
	T* FadeScene()
	{
		if (m_Fade != 0)
			return nullptr;

		m_NextScene = std::make_unique<T>();

		m_Fade = 1;
		m_FadeTime = 0.0f;


		return (T*)m_NextScene.get();
	}



	Scene* GetScene() { return m_Scene.get(); }
	//std::mutex* GetCommandMutex() { return &m_CommandMutex; }
	std::mutex* GetFileMutex() { return &m_FileMutex; }
	//std::mutex* GetDrawMutex() { return &m_DrawMutex; }

	unsigned long GetFrameIndex() { return m_FrameIndex; }

};

