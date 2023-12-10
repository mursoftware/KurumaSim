#pragma once

#include "GameObject.h"


class Camera : public GameObject
{

protected:

	float		m_Distance = 6.0f;
	float		m_FocalLength = 35.0f;
	float		m_FocalBlur = 20.0f;
	float		m_Exposure = 0.0f;
	float		m_IBLWhiteBalance = 5000.0f;
	float		m_WhiteBalance = 5000.0f;
	float		m_Gamma = 2.2f;
	//bool		m_ACESFilmEnable = true;
	//bool		m_FXAAEnable = false;
	bool		m_AutoExposure = true;
	//bool		m_TSSEnable = false;
	//bool		m_TSS = false;
	int			m_AntiAliasng = 0;
	//int			m_TSSCount = 0;
	float		m_LensFlare = 0.2f;
	float		m_MotionBlur = 0.5f;
	float		m_Vignette = 0.0f;
	float       m_TemporalRatio = 0.9f;
	int         m_MotionBlurCount = 5;

	int         m_TaaFrame = 0;


	Vector3		m_Target;
	Vector3		m_Up;


	Vector3		m_OldPosition;
	Vector3		m_OldVelocity;

	Vector3		m_DrawPosition;


	int			m_ViewMode = 0;

	Matrix44		m_ViewMatrix;
	Matrix44		m_OldViewMatrix;
	Matrix44		m_ProjectionMatrix;
	Matrix44		m_OldProjectionMatrix;


public:


	Camera() {}
	virtual ~Camera() {}

	virtual void Update() {}
	virtual void PreDraw() {}
	virtual void Draw() {}
	virtual void DrawDebug() {}

	Matrix44 GetViewMatrix() { return m_ViewMatrix; }
	Matrix44 GetOldViewMatrix() { return m_OldViewMatrix; }
	Matrix44 GetProjectionMatrix() { return m_ProjectionMatrix; }
	Matrix44 GetOldProjectionMatrix() { return m_ProjectionMatrix; }

	//bool GetTSSEnable() { return m_TSSEnable && m_TSS; }

	void SetUp(Vector3 Up) { m_Up = Up; }

	Vector3 GetTarget() { return m_Target; }
	void SetViewMode(int ViewMode) { m_ViewMode = ViewMode; }
	int GetViewMode() { return m_ViewMode; }


	float GetFocalLength() { return m_FocalLength; }
	void SetFocalLength(float FocalLength) { m_FocalLength = FocalLength; }

};

