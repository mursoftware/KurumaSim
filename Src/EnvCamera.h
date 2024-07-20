#pragma once

#include "Camera.h"


class EnvCamera : public Camera
{

private:

	Vector3 m_Axis;

public:

	EnvCamera();

	void Update(float dt);
	void PreDraw();
	void Draw();
	void DrawDebug();

	void SetAxis(Vector3 Axis) { m_Axis = Axis; }

};

