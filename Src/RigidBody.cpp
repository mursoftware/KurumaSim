

#include "Pch.h"
//#include "Common.h"

#include "RigidBody.h"


void RigidBody::Init(Vector3 Position, Quaternion Rotation)
{
	m_Position = Position;
	m_OldPosition = Position;
	m_Velocity = Vector3(0.0f, 0.0f, 0.0f);

	m_Rotation = Rotation;
	m_OldRotation = Rotation;
	m_AngularVelocity = Vector3(0.0f, 0.0f, 0.0f);

}


void RigidBody::CalcInertiaBox(float Mass, Vector3 Size)
{
	m_Mass = Mass;

	float x2 = Size.X * Size.X;
	float y2 = Size.Y * Size.Y;
	float z2 = Size.Z * Size.Z;
	m_Inertia.X = y2 + z2;
	m_Inertia.Y = z2 + x2;
	m_Inertia.Z = x2 + y2;
	m_Inertia *= Mass / 12.0f;
}


void RigidBody::CalcInertiaCylinderX(float Mass, float Height, float Radius)
{
	m_Mass = Mass;

	float h2 = Height * Height;
	float r2 = Radius * Radius;

	m_Inertia.X = Mass * r2 / 2.0f;
	m_Inertia.Y = Mass * (r2 / 4.0f + h2 / 12.0f);
	m_Inertia.Z = m_Inertia.Y;

}



Matrix44 RigidBody::GetMatrix()
{
	Matrix44 worldMatrix, transMatrix, rotMatrix;

	worldMatrix = Matrix44::Identity();

	rotMatrix = Matrix44::RotationQuaternion(m_Rotation);
	worldMatrix = Matrix44::Multiply(worldMatrix, rotMatrix);

	transMatrix = Matrix44::TranslateXYZ((float)m_Position.X, (float)m_Position.Y, (float)m_Position.Z);
	worldMatrix = Matrix44::Multiply(worldMatrix, transMatrix);

	return worldMatrix;
}



void RigidBody::AddTorqueWorld(Vector3 Torque, float dt)
{
	Matrix44 m = GetMatrix();
	Matrix44 im = Matrix44::Inverse(m);
	Vector3 lt = Vector3::TransformNormal(im, Torque);

	m_AngularVelocity += lt / m_Inertia * dt;
}



void RigidBody::AddTorqueWorld(Vector3 Position, Vector3 Torque, float dt)
{
	Vector3 r = Position - m_Position;

	Matrix44 mtx = GetMatrix();
	Matrix44 im = Matrix44::Inverse(mtx);
	Vector3 lt = Vector3::TransformNormal(im, Torque);

	float r2 = r.LengthSq();
	Vector3 inertia = m_Inertia;
	inertia.X += m_Mass * (r2 - r.X * r.X);
	inertia.Y += m_Mass * (r2 - r.Y * r.Y);
	inertia.Z += m_Mass * (r2 - r.Z * r.Z);

	m_AngularVelocity += lt / inertia * dt;
}



void RigidBody::AddTorqueLocal(Vector3 Torque, float dt)
{
	m_AngularVelocity += Torque / m_Inertia * dt;
}

void RigidBody::AddForceWorld(Vector3 Force, float dt)
{
	m_Velocity += Force / m_Mass * dt;
}

void RigidBody::AddForceWorld(Vector3 Position, Vector3 Force, float dt)
{
	m_Velocity += Force / m_Mass * dt;

	Vector3 v = Position - m_Position;
	Vector3 wt = Vector3::Cross(v, Force);

	Matrix44 m = GetMatrix();
	Matrix44 im = Matrix44::Inverse(m);
	Vector3 lt = Vector3::TransformNormal(im, wt);

	m_AngularVelocity += lt / m_Inertia * dt;

}


void RigidBody::RotateAxisWorld(Vector3 Axis)
{
	Quaternion dr = Quaternion::RotationAxis(Axis);
	m_Rotation = Quaternion::Multiply(dr, m_Rotation);
}





void RigidBody::StorePositionRotation()
{
	m_OldPosition = m_Position;
	m_OldRotation = m_Rotation;
}


void RigidBody::UpdatePositionRotation(float dt)
{
	m_Position += m_Velocity * dt;

	if (!m_Lock)
	{
		for (int i = 0; i < 10; i++)
		{
			Vector3 d = Vector3::Cross(m_AngularVelocity, m_AngularVelocity * m_Inertia);
			m_AngularVelocity += -d / m_Inertia * (dt / 10.0f);
		}

		Quaternion dr = Quaternion::RotationAxis(m_AngularVelocity * dt);
		m_Rotation = Quaternion::Multiply(m_Rotation, dr);
		m_Rotation.Normalize();
	}
}


void RigidBody::CalcVelocity(float dt, Vector3* Force, Vector3* Torque)
{
	Vector3 velocity = (m_Position - m_OldPosition) / dt;
	*Force = (velocity - m_Velocity) / dt * m_Mass;

	m_Velocity = velocity;




	Quaternion iq = m_OldRotation;
	iq.Inverse();
	Quaternion dr = Quaternion::Multiply(iq, m_Rotation);
	Vector3 angularVelocity = Vector3::RotationQuaternion(dr) / dt;

	Vector3 d = Vector3::Cross(angularVelocity, angularVelocity * m_Inertia);
	Vector3 torque = (angularVelocity - m_AngularVelocity) / dt * m_Inertia -d;

	Matrix44 rotMatrix = Matrix44::RotationQuaternion(m_Rotation);
	*Torque = Vector3::TransformNormal(rotMatrix, torque);

	m_AngularVelocity = angularVelocity;


}



Vector3 RigidBody::GetAngularVelocityWorld()
{
	Matrix44 rotMatrix = Matrix44::RotationQuaternion(m_Rotation);
	Vector3 wav = Vector3::TransformNormal(rotMatrix, m_AngularVelocity);

	return wav;
}
