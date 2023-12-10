#include "Pch.h"
#include "GameManager.h"
#include "RenderManager.h"
#include "Sky.h"
#include "Camera.h"






Sky::Sky()
{
	m_Rotation = Vector3{ 0.0f, 0.0f, 0.0f };


	m_Model.Load("Asset\\sky.obj");

	//m_Texture = RenderManager::GetInstance()->LoadTexture("asset/sunflowers_nonsun_8k_EXR_BC6H_1.DDS");
	//m_Texture = RenderManager::GetInstance()->LoadTexture("asset/black.DDS");


}

Sky::~Sky()
{

}



void Sky::Update()
{

}

void Sky::PreDraw()
{
	//Vector3 cameraPosition = m_Camera->GetPosition();

	m_OldWorldMatrix = m_WorldMatrix;

	m_WorldMatrix = Matrix44::Identity();
	m_WorldMatrix *= Matrix44::Scale(700.0f, 700.0f, 700.0f);
	m_WorldMatrix *= Matrix44::RotationXYZ(0.0f, 0.0f, 0.0f);
	//m_WorldMatrix *= Matrix44::TranslateXYZ(cameraPosition.X, cameraPosition.Y, cameraPosition.Z);
}



void Sky::Draw(Camera* DrawCamera)
{
	RenderManager* render = RenderManager::GetInstance();
	render->SetPipelineState("Sky");


	//マトリクス設定
	Matrix44 view = DrawCamera->GetViewMatrix();
	Matrix44 projection = DrawCamera->GetProjectionMatrix();
	Matrix44 oldView = DrawCamera->GetOldViewMatrix();


	Vector3 cameraPosition = DrawCamera->GetPosition();
	Matrix44 trans = Matrix44::TranslateXYZ(cameraPosition.X, cameraPosition.Y, cameraPosition.Z);


	//定数バッファ設定
	OBJECT_CONSTANT constant;
	constant.WVP = Matrix44::Transpose(m_WorldMatrix * trans * view * projection);
	constant.OldWVP = Matrix44::Transpose(m_OldWorldMatrix * trans * oldView * projection);
	constant.World = Matrix44::Transpose(m_WorldMatrix * trans);
	constant.Param = { 0.0f, 0.0f, 0.4f, 1.0f };
	render->SetConstant(2, &constant, sizeof(constant));


	//RenderManager::GetInstance()->SetGraphicsRootDescriptorTable(6, m_Texture->ShaderResourceView.Index);


	//描画
	m_Model.Draw(true);

}
