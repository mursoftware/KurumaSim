#pragma once

#include "Camera.h"


class ShadowCamera : public Camera
{

private:

	int m_Index{};

	Vector3 m_LightDirection;

public:

	ShadowCamera();

	void Update(float dt);
	void PreDraw();
	void Draw();
	void DrawDebug();

	void SetIndex(int Index) { m_Index = Index; }

	void SetLightDirection(Vector3 LightDirection) { m_LightDirection = LightDirection; }
};

