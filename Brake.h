#pragma once


#include "Model.h"


class Brake
{
private:
	float m_MaxTorque{};
	float m_Ratio{};
	float m_TorqueLen{};

	class RigidBody* m_Body{};
	class RigidBody* m_Tire{};


	Model		m_Model[5];

	Matrix44	m_WorldMatrix;
	Matrix44	m_OldWorldMatrix;

public:

	void SetRigidBody(RigidBody* Body, RigidBody* Tire)
	{
		m_Body = Body;
		m_Tire = Tire;
	}

	void Load(const char* FileName, const char* PartName);
	void Update(float dt);
	void PreDraw();
	void Draw(class Camera* DrawCamera, int LodLevel = 2);
	void DrawDebug(const char* DebugName);

	void SetRatio(float Ratio) {m_Ratio = Ratio;}


};

