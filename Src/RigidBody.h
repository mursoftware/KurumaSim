#pragma once


#include "MatrixVector.h"


class RigidBody
{
protected:
	Vector3D		m_Position;			//(m)
	Vector3D		m_OldPosition;		//(m)
	Vector3			m_Velocity;			//(m/s) WorldSpace
	float			m_Mass{};			//(kg)

	Quaternion		m_Rotation;			//(rad)
	Quaternion		m_OldRotation;		//(rad)
	Vector3			m_AngularVelocity;	//(rad/s) LocalSpace
	Vector3			m_Inertia;			//(kgm2)



public:

	void Init(Vector3 Position, Quaternion Rotation);
	void CalcInertiaBox(float Mass, Vector3 Size);
	void CalcInertiaCylinderX(float Mass, float Height, float Radius);

	Matrix44 GetMatrix();
	void AddTorqueWorld(Vector3 Torque, float dt);
	void AddTorqueWorld(Vector3 Position, Vector3 Torque, float dt);
	void AddTorqueLocal(Vector3 Torque, float dt);
	void AddForceWorld(Vector3 Force, float dt);
	void AddForceWorld(Vector3 Position, Vector3 Force, float dt);

	void RotateAxisWorld(Vector3 Axis);



	void StorePositionRotation();
	void UpdatePositionRotation(float dt);
	void CalcVelocity(float dt, Vector3* Force, Vector3* Torque);


	Vector3 GetPosition() { return m_Position; }
	void SetPosition(Vector3 Position) { m_Position = Position; }

	Quaternion GetRotation() { return m_Rotation; }
	void SetRotation(Quaternion Rotation) { m_Rotation = Rotation; }

	float GetMass() { return m_Mass; }
	Vector3 GetVelocity() { return m_Velocity; }

	Vector3 GetAngularVelocityLocal() { return m_AngularVelocity; }
	Vector3 GetAngularVelocityWorld();


};