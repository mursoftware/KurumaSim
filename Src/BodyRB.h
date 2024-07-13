#pragma once
#include "RigidBody.h"
#include "Model.h"



class BodyRB : public RigidBody
{
private:

	Vector3		m_Size;
	Vector3		m_Offset;
	float		m_CD{};
	float		m_ShadowSize{};
	float		m_AirDensity{};
	float		m_AirFoeceLen{};
	float		m_DownForceLenF{};
	float		m_DownForceLenR{};
	float		m_DownForceRatioF{};
	float		m_DownForceRatioR{};
	bool		m_BrakeLamp{};

	bool		m_Visible{};
	Model		m_Model[5];

	Matrix44	m_WorldMatrix;
	Matrix44	m_OldWorldMatrix;

	std::unique_ptr<TEXTURE>	m_ShadowTexture;


	MATERIAL	m_Material{};



public:

	void Load(const char* FileName, const char* PartName);

	void Update(float dt);
	void PreDraw();
	void Draw(class Camera* DrawCamera, int LodLevel=2);
	void DrawDebug();

	void SetBrakeLamp(bool BrakeLamp) { m_BrakeLamp = BrakeLamp; }

	
	TEXTURE* GetShadowTexture() { return m_ShadowTexture.get(); }
};