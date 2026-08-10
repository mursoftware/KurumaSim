#pragma once


#include "Model.h"


class Brake
{
private:
	float m_MaxTorque{};
	float m_Ratio{};
	float m_TorqueLen{};

	class BodyRB* m_Body{};
	class TireRB* m_Tire{};


	Model		m_Model[5];

	Matrix44	m_WorldMatrix;
	Matrix44	m_OldWorldMatrix;

	float		m_LockRotation{};

public:

	void SetRigidBody(BodyRB* Body, TireRB* Tire)
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

