#include "Pch.h"
#include "GameManager.h"
#include "RenderManager.h"
#include "SmokeManager.h"
#include "Camera.h"
#include "ShadowCamera.h"






SmokeManager::SmokeManager()
{

	RenderManager* render = RenderManager::GetInstance();



	{
		HRESULT hr{};
		D3D12_HEAP_PROPERTIES heapProperties{};
		D3D12_RESOURCE_DESC   resourceDesc{};

		heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProperties.CreationNodeMask = 0;
		heapProperties.VisibleNodeMask = 0;

		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Height = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;

		//頂点バッファの作成
		resourceDesc.Width = sizeof(VERTEX_3D) * 4;
		hr = render->GetDevice()->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE,
			&resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
			IID_PPV_ARGS(&m_VertexBuffer));
		assert(SUCCEEDED(hr));


		//頂点データの書き込み
		VERTEX_3D* buffer{};
		hr = m_VertexBuffer->Map(0, nullptr, (void**)&buffer);
		assert(SUCCEEDED(hr));

		buffer[0].Position = Vector3{ -1.0f,  1.0f,  0.0f };
		buffer[1].Position = Vector3{ 1.0f,  1.0f,  0.0f };
		buffer[2].Position = Vector3{ -1.0f,  -1.0f, 0.0f };
		buffer[3].Position = Vector3{ 1.0f,  -1.0f, 0.0f };
		buffer[0].Normal = Vector3{ -1.0f, 1.0f, -0.1f };
		buffer[1].Normal = Vector3{ 1.0f, 1.0f, -0.1f };
		buffer[2].Normal = Vector3{ -1.0f, -1.0f, -0.1f };
		buffer[3].Normal = Vector3{ 1.0f, -1.0f, -0.1f };
		buffer[0].TexCoord = { 0.0f, 1.0f };
		buffer[1].TexCoord = { 1.0f, 1.0f };
		buffer[2].TexCoord = { 0.0f, 0.0f };
		buffer[3].TexCoord = { 1.0f, 0.0f };
		buffer[0].Color = Vector4{ 1.0f, 1.0f, 0.1f, 0.1f };
		buffer[1].Color = Vector4{ 1.0f, 1.0f, 0.1f, 0.1f };
		buffer[2].Color = Vector4{ 1.0f, 1.0f, 0.1f, 0.1f };
		buffer[3].Color = Vector4{ 1.0f, 1.0f, 0.1f, 0.1f };

		m_VertexBuffer->Unmap(0, nullptr);
	}



	m_Texture = render->LoadTexture("asset/smoke.dds");



}

SmokeManager::~SmokeManager()
{

}




void SmokeManager::Create(Vector3 Position, float Alpha)
{
	if (m_SmokeList.size() > 1000)
		return;

	m_SmokeList.emplace_back();
	Smoke& smoke = m_SmokeList.back();

	smoke.Position = Position;
	smoke.Alpha = Alpha;
	smoke.Size = 0.5f;

	smoke.Velocity.X = ((float)rand() / RAND_MAX * 2.0f - 1.0f) * 1.0f;
	smoke.Velocity.Y = ((float)rand() / RAND_MAX) * 1.0f;
	smoke.Velocity.Z = ((float)rand() / RAND_MAX * 2.0f - 1.0f) * 1.0f;

	smoke.Rotation = (float)rand() / RAND_MAX * 2.0f * PI;
	smoke.RotationSpeed = ((float)rand() / RAND_MAX * 2.0f - 1.0f) * 3.0f;

	smoke.Destroy = false;
}



void SmokeManager::Update(float dt)
{


	for (auto& smoke : m_SmokeList)
	{
		smoke.Position += smoke.Velocity * dt;
		smoke.Size += 0.5f * dt;
		smoke.Alpha *= pow(0.5f, dt);
		smoke.Rotation += smoke.RotationSpeed * dt;

		if (smoke.Alpha < 0.01f)
		{
			smoke.Destroy = true;
		}
	}


	m_SmokeList.remove_if([](Smoke& somke) { return somke.Destroy; });

}



void SmokeManager::PreDraw()
{

	m_PreDrawSmokeList = m_SmokeList;


}


void SmokeManager::Draw(Camera* DrawCamera)
{

	RenderManager* render = RenderManager::GetInstance();
	ShadowCamera** shadowCamera = GameManager::GetInstance()->GetScene()->GetCurrentShadowCamera();



	render->SetPipelineState("Smoke");




	//マトリクス設定
	Matrix44 view = DrawCamera->GetViewMatrix();
	Matrix44 oldView = DrawCamera->GetOldViewMatrix();

	Matrix44 projection = DrawCamera->GetProjectionMatrix();
	Matrix44 oldProjection = DrawCamera->GetOldProjectionMatrix();


	Matrix44 shadowView0 = shadowCamera[0]->GetViewMatrix();
	Matrix44 shadowProjection0 = shadowCamera[0]->GetProjectionMatrix();

	Matrix44 shadowView1 = shadowCamera[1]->GetViewMatrix();
	Matrix44 shadowProjection1 = shadowCamera[1]->GetProjectionMatrix();

	Matrix44 shadowView2 = shadowCamera[2]->GetViewMatrix();
	Matrix44 shadowProjection2 = shadowCamera[2]->GetProjectionMatrix();




	// Zソート
	{
		Vector3 cameraPosition = DrawCamera->GetPosition();

		for (auto& smoke : m_PreDrawSmokeList)
		{
			smoke.ViewPosition = Vector3::TransformCoord(view, smoke.Position);
		}

		m_PreDrawSmokeList.sort
		(
			[](auto const& left, auto const& right)
			{
				return left.ViewPosition.Z > right.ViewPosition.Z;
			}
		);
	}




	//マトリクス設定
	Matrix44 invView = Matrix44::Inverse(view);
	invView.M[3][0] = 0.0f;
	invView.M[3][1] = 0.0f;
	invView.M[3][2] = 0.0f;



	//頂点バッファ設定
	D3D12_VERTEX_BUFFER_VIEW vertexView{};
	vertexView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
	vertexView.StrideInBytes = sizeof(VERTEX_3D);
	vertexView.SizeInBytes = sizeof(VERTEX_3D) * 4;
	render->GetGraphicsCommandList()->IASetVertexBuffers(0, 1, &vertexView);





	//トポロジ設定
	render->GetGraphicsCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);



	//テクスチャ設定
	render->SetGraphicsRootDescriptorTable(RenderManager::CBV_REGISTER_MAX + 2, m_Texture->ShaderResourceView.Index);





	//定数バッファ設定
	OBJECT_CONSTANT constant;
	constant.Param = { 0.0f, 0.0f, 0.0f, 0.0f };

	//マテリアル設定
	MATERIAL material{};
	material.EmissionColor = { 0.0f, 0.0f, 0.0f, 0.0f };
	material.Metallic = 0.0f;
	material.Specular = 0.5f;
	material.Roughness = 1.0f;
	material.NormalWeight = 0.0f;


	for (auto& smoke : m_PreDrawSmokeList)
	{
		Matrix44 world = Matrix44::Identity();
		world *= Matrix44::Scale(smoke.Size, smoke.Size, smoke.Size);
		world *= Matrix44::RotationXYZ(0.0f, 0.0f, smoke.Rotation);
		world *= invView;
		world *= Matrix44::TranslateXYZ(smoke.Position.X, smoke.Position.Y, smoke.Position.Z);

		constant.World = Matrix44::Transpose(world);
		constant.WVP = Matrix44::Transpose(world * view * projection);
		constant.OldWVP = Matrix44::Transpose(world * oldView * oldProjection);
		constant.ShadowWVP[0] = Matrix44::Transpose(world * shadowView0 * shadowProjection0);
		constant.ShadowWVP[1] = Matrix44::Transpose(world * shadowView1 * shadowProjection1);
		constant.ShadowWVP[2] = Matrix44::Transpose(world * shadowView2 * shadowProjection2);
		render->SetConstant(2, &constant, sizeof(constant));


		material.BaseColor = { 0.5f, 0.5f, 0.5f, smoke.Alpha };
		//material.BaseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		render->SetConstant(3, &material, sizeof(material));


		//描画
		render->GetGraphicsCommandList()->DrawInstanced(4, 1, 0, 0);
	}

}
