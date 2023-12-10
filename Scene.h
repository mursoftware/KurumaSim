#pragma once

#include "InputManager.h"
#include "GameObject.h"


class Scene
{
protected:

	std::list<std::unique_ptr<GameObject>> m_GameObject;

	class Camera* m_CurrentCamera{};
	class ShadowCamera** m_CurrentShadowCamera{};

	static bool		m_DebugVisible;

public:

	Scene() {};
	virtual ~Scene() {};

	virtual void Load() {};

	class Camera* GetCurrentCamera() { return m_CurrentCamera; }
	class ShadowCamera** GetCurrentShadowCamera() { return m_CurrentShadowCamera; }

	bool GetDebugVisible() { return m_DebugVisible; }


	virtual void Update()
	{

		if (InputManager::GetInstance()->GetKeyTrigger(VK_F1))
		{
			m_DebugVisible = !m_DebugVisible;
		}

		for (std::unique_ptr<GameObject>& gameObject : m_GameObject)
		{
			gameObject->Update();
		}
	}


	virtual void PreDraw()
	{
		for (std::unique_ptr<GameObject>& gameObject : m_GameObject)
		{
			gameObject->PreDraw();
		}
	}

	virtual void DrawEnv(int Index) {}
	virtual void DrawDepth(int Index) {}
	virtual void DrawZPrePass() {}
	virtual void Draw() {}
	virtual void DrawShrink() {}





	virtual void DrawUI()
	{
		for (std::unique_ptr<GameObject>& gameObject : m_GameObject)
		{
			gameObject->DrawUI();
		}




		if(m_DebugVisible)
		for (std::unique_ptr<GameObject>& gameObject : m_GameObject)
		{
			gameObject->DrawDebug();
		}
	}




	template <class T>
	T* AddGameObject()
	{
		std::unique_ptr<T> gameObject = std::make_unique<T>();
		T* p = gameObject.get();

		m_GameObject.push_back(std::move(gameObject));

		return p;
	}

	template <class T>
	T* GetGameObject()
	{
		for (std::unique_ptr<GameObject>& gameObject : m_GameObject)
		{
			if (typeid(*gameObject.get()) == typeid(T))
			{
				return dynamic_cast<T*>(gameObject.get());
			}
		}

		return nullptr;
	}

};