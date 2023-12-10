#pragma once

#include "Camera.h"


class CarCamera : public Camera
{

private:

	std::vector<Vector3> m_Location;

	Vector3		m_DronePosition;
	Vector3		m_DroneRotationSpeed;

	Vector3		m_TargetRotation;
	float		m_TargetDistance{};

public:


	CarCamera();

	void Update();
	void PreDraw();
	void Draw();
	void DrawDebug();

	void SetLoacation(class Field* CameraField);

};

