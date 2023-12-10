#pragma once

#include "Scene.h"




class CarSelectScene : public Scene
{
private:

	class CarCamera* m_CarCamera{};
	class EnvCamera* m_EnvCamera[6]{};
	class ShadowCamera* m_ShadowCamera[3]{};

	class Sky*		m_Sky{};
	class Field*	m_Field{};
	class Car*		m_Car{};

public:

	CarSelectScene();
	~CarSelectScene();

	void Load();

	void Update() override;
	void DrawEnv(int Index) override;
	void DrawZPrePass() override;
	void DrawDepth(int Index) override;
	void Draw() override;
	void DrawUI() override;


};

