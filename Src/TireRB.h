#pragma once
#include "RigidBody.h"
#include "DriveTrain.h"
#include "Model.h"



struct TIRE_BRUSH
{
	Vector3		Position;
	bool		Hit{};
	Vector3		HitPosition;
	Vector3		Displacement;

	float		dx{};
};




class TireRB : public RigidBody, public DriveTrain
{
private:

	RigidBody*	m_BodyRB{};
	float		m_Radius{};

	Vector3		m_DrawPosition{};
	Quaternion	m_DrawRotation{};
	Vector3		m_OldDrawPosition{};
	Quaternion	m_OldDrawRotation{};

	static const int	BRUSH_COUNT = 100;
	TIRE_BRUSH	m_Brush[BRUSH_COUNT];

	float		m_Srip{};

	float		m_ScaleX{};

	Model		m_Model[5];

	Matrix44	m_WorldMatrix;
	Matrix44	m_OldWorldMatrix;

	Vector3		m_BrushSpringRatio;//(N/m)
	float		m_BrushDamping{};
	float		m_BrushDampingMax{};
	float		m_BrushStaticFrictionRatio{};
	float		m_BrushDynamicFrictionRatio{};

	//std::unique_ptr<TEXTURE>	m_ShadowTexture;


public:

	void SetBodyRB(RigidBody* BodyRB) { m_BodyRB = BodyRB; }
	void Load(const char * FileName, const char * PartName, float ScaleX);
	void InitBursh();
	void Update(float dt);
	void DrawDebug(const char* DebugName);
	void PreDraw();
	void Draw(class Camera* DrawCamera, int LodLevel = 2);


	float GetSrip() { return m_Srip; }

	bool RayHitCheck(Vector3 facePosition, Vector3 faceNormal, Vector3 position, Vector3 direction, Vector3* hitPosition, Vector3* hitNormal);

	float GetRadius() { return m_Radius; }

	float GetInputAngularSpeed1() override { return m_AngularVelocity.X; }


	//TEXTURE* GetShadowTexture() { return m_ShadowTexture.get(); }

};