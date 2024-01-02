#pragma once

#include "Scene.h"




class GameScene : public Scene
{
private:

	class CarCamera* m_CarCamera{};
	class EnvCamera* m_EnvCamera[6]{};
	class ShadowCamera* m_ShadowCamera[3]{};

	class Sky*		m_Sky{};
	class Field*	m_Field{};
	class Car*		m_Car{};
	class SmokeManager* m_SmokeManager{};

	bool			m_Replay{};

public:

	GameScene();
	~GameScene();

	void Load();

	void Update() override;
	void DrawEnvStatic(int Index) override;
	void DrawEnv(int Index) override;
	void DrawDepth(int Index) override;
	void DrawZPrePass() override;
	void Draw() override;
	void DrawShrink() override;
	void DrawUI() override;


};

