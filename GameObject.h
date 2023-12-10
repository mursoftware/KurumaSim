#pragma once

#include "Pch.h"



class GameObject
{

protected:

	Vector3	m_Position;
	Vector3	m_Rotation;

public:
	GameObject() {};
	virtual ~GameObject() {};

	virtual void Update() {};
	virtual void PreDraw() {};
	virtual void DrawDepth(class Camera* DrawCamera) {};
	virtual void Draw(class Camera* DrawCamera) {};
	virtual void DrawUI() {};
	virtual void DrawDebug() {};


	Vector3 GetPosition() { return m_Position; }
	void SetPosition(Vector3 Position) { m_Position = Position; }

	Vector3 GetRotation() { return m_Rotation; }
	void SetRotation(Vector3 Rotation) { m_Rotation = Rotation; }

};

