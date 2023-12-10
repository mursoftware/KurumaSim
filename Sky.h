#pragma once

#include "GameObject.h"
#include "Model.h"


class Sky : public GameObject
{
private:

	Model						m_Model;
	std::unique_ptr<TEXTURE>	m_Texture;

	Matrix44		m_WorldMatrix;
	Matrix44		m_OldWorldMatrix;


public:
	Sky();
	~Sky();

	void Update();
	void PreDraw();
	void Draw(class Camera* DrawCamera);


};

