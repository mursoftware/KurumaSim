#pragma once

#include "GameObject.h"
#include "Model.h"


class Field : public GameObject
{
private:

	Model			m_Model;

	float			m_Fog{};
	Vector2			m_LightRotation{};
	float			m_LightTemperature{};
	float			m_LightIntensity{};
	float			m_Time{};
	float			m_CloudDensity{};
	float			m_CloudHeight{};


	std::unique_ptr<TEXTURE>	m_ShadowTexture;


public:
	Field();
	~Field();

	void Load(int Index);

	void Update(float dt);
	void DrawDepth(class Camera* DrawCamera);
	void Draw(class Camera* DrawCamera);
	void DrawDebug();

	bool RayHitCheck(Vector3 position, Vector3 direction, Vector3* hitPosition, Vector3* hitNormal);



	Model* GetModel() { return &m_Model; }




	Vector3 GetLightDirection();
};

