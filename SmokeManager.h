#pragma once

#include "GameObject.h"
#include "Model.h"



struct Smoke
{
	Vector3		Position;
	Vector3		ViewPosition;
	Vector3		Velocity;
	float		Size{};
	float		Alpha{};
	float		Rotation{};
	float		RotationSpeed{};
	bool		Destroy{};
};




class SmokeManager : public GameObject
{
private:

	ComPtr<ID3D12Resource>		m_VertexBuffer;
	std::unique_ptr<TEXTURE>	m_Texture;

	std::list<Smoke>			m_SmokeList;
	std::list<Smoke>			m_PreDrawSmokeList;

public:
	SmokeManager();
	~SmokeManager();

	void Update(float dt);
	void PreDraw();
	void Draw(class Camera* DrawCamera);

	void Create(Vector3 Position, float Alpha);

};

