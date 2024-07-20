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

	std::thread m_VirtualCameraReceiveUDPThread{};
	Vector3		m_VirtualCameraRotation;
	bool		m_VirtualCameraReceiveUDPThreadStop{};
	std::string m_IpAdress;
	Quaternion	m_Quaternion;

	Vector3		m_VirtualCameraAcceleration;
	Vector3		m_VirtualCameraVelocity;
	Quaternion	m_VirtualCameraQuaternion;

public:


	CarCamera();
	~CarCamera();

	void Update(float dt);
	void PreDraw();
	void Draw();
	void DrawDebug();

	void SetLoacation(class Field* CameraField);


	void VirtualCameraReceiveUDP();

};

