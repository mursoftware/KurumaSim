#pragma once

#include "Pch.h"


struct CompPosition
{
	short X, Y, Z, W;

	bool operator==(const CompPosition& Vector) const
	{
		return X == Vector.X && Y == Vector.Y && Z == Vector.Z;
	}

	CompPosition& operator=(const Vector3& Vector)
	{
		X = (short)(Vector.X * MAXSHORT);
		Y = (short)(Vector.Y * MAXSHORT);
		Z = (short)(Vector.Z * MAXSHORT);

		return *this;
	}


	operator Vector3() const
	{
		Vector3 out;

		out.X = (float)X / MAXSHORT;
		out.Y = (float)Y / MAXSHORT;
		out.Z = (float)Z / MAXSHORT;

		return out;
	}
};

struct CompNormal
{
	short X, Y;

	bool operator==(const CompNormal& Vector) const
	{
		return X == Vector.X && Y == Vector.Y;
	}



	Vector2 OctWrap(const Vector2& v)
	{
		Vector2 out;

		out.X = (1.0f - abs(v.Y)) * (v.X >= 0.0f ? 1.0f : -1.0f);
		out.Y = (1.0f - abs(v.X)) * (v.Y >= 0.0f ? 1.0f : -1.0f);

		return out;
	}


	//Octahedron normal vector encoding
	//https://knarkowicz.wordpress.com/2014/04/16/octahedron-normal-vector-encoding/

	Vector2 Encode(const Vector3& n)
	{
		Vector2 normal;

		float l = abs(n.X) + abs(n.Y) + abs(n.Z);
		normal.X = n.X / l;
		normal.Y = n.Y / l;

		normal = n.Z >= 0.0f ? normal : OctWrap(normal);

		return normal;
	}

	CompNormal& operator=(const Vector3D& Vector)
	{
		Vector2 normal = Encode(Vector);

		X = (short)(normal.X * MAXSHORT);
		Y = (short)(normal.Y * MAXSHORT);

		return *this;
	}


};


struct CompTexCoord
{
	short X, Y;

	bool operator==(const CompTexCoord& Vector) const
	{
		return X == Vector.X && Y == Vector.Y;
	}

	CompTexCoord& operator=(const Vector2& Vector)
	{
		X = (short)(Vector.X * 50.0f);
		Y = (short)(Vector.Y * 50.0f);

		return *this;
	}


	operator Vector2() const
	{
		Vector2 out;

		out.X = X / 50.0f;
		out.Y = Y / 50.0f;

		return out;
	}
};


struct VERTEX_3D
{
	CompPosition Position;
	CompNormal Normal;
	CompTexCoord TexCoord;

	bool operator==(const VERTEX_3D& Vertex) const
	{
		return Position == Vertex.Position && Normal == Vertex.Normal && TexCoord == Vertex.TexCoord;
	}

};

/*
struct VERTEX_3D
{
	Vector3 Position;
	Vector3 Normal;
	Vector2 TexCoord;

	//Vector4 Color;
	//float	 Occlusion;

	bool operator==(const VERTEX_3D& Vertex) const
	{
		return Position == Vertex.Position && Normal == Vertex.Normal && TexCoord == Vertex.TexCoord;
	}
};
*/

struct MATERIAL
{
	Vector4		BaseColor;
	Vector4		EmissionColor;

	float		Metallic;
	float		Specular;
	float		Roughness;
	float		NormalWeight;
};




struct ENV_CONSTANT
{
	float		Fog;
	Vector3		LightDirection;

	Vector3		LightColor;
	float		Time;

	Vector3		ScatteringLight;
	float       CloudDensity;
	float       CloudHeight;
};


struct CAMERA_CONSTANT
{
	Vector4		CameraPosition;

	float		Exposure;
	float		IBLWhiteBalance;
	float		WhiteBalance;
	float		Gamma;

	BOOL		AutoExposure;
	float		LensFlare;
	float		MotionBlur;
	float		Vignette;

	float		FocalLength;
	float		FocalDistance;
	float		FocalBlur;
	float		Dummy;

	Vector2		SSBufferSize;
	int			AntiAliasing;
	float       TemporalRatio;

	int         MotionBlurCount;
	int         TemporalFrame;

};


struct OBJECT_CONSTANT
{
	Matrix44 WVP;
	Matrix44 OldWVP;
	Matrix44 World;
	//Matrix44 NormalWorld;
	Matrix44 ShadowWVP[3];
	Matrix44 InvVP;

	Vector4 Param;
};

struct SUBSET_CONSTANT
{
	MATERIAL Material;
};


struct BLOB_SHADOW_CONSTANT
{
	Matrix44 BodyW;
	Matrix44 TireW[4];
};




struct RESOURCE
{
	ComPtr<ID3D12Resource>	Resource;
	unsigned int			SRVIndex;
	unsigned int			SRVIndexArray[6][10];
	D3D12_CPU_DESCRIPTOR_HANDLE RTVHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE RTVHandleArray[6][10];
};








struct SHADER_RESOURCE_VIEW
{
	unsigned int			Index;
	~SHADER_RESOURCE_VIEW();
};


struct RENDER_TARGET_VIEW
{
	D3D12_CPU_DESCRIPTOR_HANDLE Handle;
	~RENDER_TARGET_VIEW() {}
};


struct TEXTURE
{
	ComPtr<ID3D12Resource>	Resource;
	SHADER_RESOURCE_VIEW	ShaderResourceView;
};


struct CONSTANT_BUFFER
{
	ComPtr<ID3D12Resource>	Resource;
	SHADER_RESOURCE_VIEW	ShaderResourceView;
};


struct RENDER_TARGET
{
	ComPtr<ID3D12Resource>		Resource;
	SHADER_RESOURCE_VIEW		ShaderResourceView;
	RENDER_TARGET_VIEW			RenderTargetView;
};

struct RENDER_TARGET_CUBE
{
	ComPtr<ID3D12Resource>		Resource;
	SHADER_RESOURCE_VIEW		ShaderResourceView;
	SHADER_RESOURCE_VIEW		ShaderResourceViewArray[6][10];
	RENDER_TARGET_VIEW			RenderTargetViewArray[6][10];
};




struct VERTEX_BUFFER
{
	ComPtr<ID3D12Resource>		Resource;
	unsigned int				Stride;
	unsigned int				Size;
};

struct INDEX_BUFFER
{
	ComPtr<ID3D12Resource>		Resource;
	unsigned int				Stride;
	unsigned int				Size;
};








class RenderManager
{

private:
	static RenderManager* m_Instance;




	HWND								m_WindowHandle{};


	bool								m_WindowMode{};
	int									m_SSBufferWidth{};
	int									m_SSBufferHeight{};
	int									m_BackBufferWidth{};
	int									m_BackBufferHeight{};
	float								m_SSRatio{};

	int									m_EnvBufferSize{};
	int									m_EnvBufferMipLevel{};

	int									m_IBLBufferWidth{};
	int									m_IBLBufferHeight{};


	UINT64								m_Frame[2]{};
	UINT								m_RTIndex{};

	unsigned long						m_TotalIndexCount{};


	std::string							m_CpuName;
	std::string							m_GpuName;



	ComPtr<IDXGIFactory4>				m_Factory;
	ComPtr<IDXGIAdapter3>				m_Adapter;
	ComPtr<ID3D12Device>				m_Device;
	ComPtr<ID3D12CommandQueue>			m_CommandQueue;
	ComPtr<ID3D12Fence>					m_Fence;
	ComPtr<IDXGISwapChain3>				m_SwapChain;
	ComPtr<ID3D12GraphicsCommandList>	m_GraphicsCommandList;
	ComPtr<ID3D12CommandAllocator>		m_GraphicsCommandAllocator[2];

	ComPtr<ID3D12GraphicsCommandList>	m_ResourceCommandList;
	ComPtr<ID3D12CommandAllocator>		m_ResourceCommandAllocator[2];



	static const unsigned int			m_QueryMax = 256;
	ComPtr<ID3D12QueryHeap>				m_QueryHeap;
	ComPtr<ID3D12Resource>				m_QueryBuffer;

	struct CommandTime
	{
		std::string Name;
		float		Time;
	};
	std::vector<CommandTime>			m_CommandTime;

	UINT64								m_Gpufreq{};




	HANDLE								m_FenceEvent{};


	ComPtr<ID3D12Resource>				m_RenderTarget[2];
	ComPtr<ID3D12DescriptorHeap>		m_RenderTargetDescriptorHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE			m_RenderTargetHandle[2];

	ComPtr<ID3D12DescriptorHeap>		m_DepthBufferDescriptorHeap;

	ComPtr<ID3D12Resource>				m_DepthBuffer[2];
	D3D12_CPU_DESCRIPTOR_HANDLE			m_DepthBufferHandle[2];
	unsigned int						m_DepthBufferSRVIndex[2]{};

	int									m_ShadowDepthBufferSize{};
	ComPtr<ID3D12Resource>				m_ShadowDepthBuffer[3];
	D3D12_CPU_DESCRIPTOR_HANDLE			m_ShadowDepthBufferHandle[3];
	unsigned int						m_ShadowDepthBufferSRVIndex[3]{};






	D3D12_RECT							m_ScissorRect{};
	D3D12_VIEWPORT						m_Viewport{};

	D3D12_RECT							m_SSScissorRect{};
	D3D12_VIEWPORT						m_SSViewport{};



	ComPtr<ID3D12DescriptorHeap>		m_SRVDescriptorHeap;
	std::list<unsigned int>				m_SRVDescriptorPool;
	static const unsigned int			m_SRVDescriptorMax = 10000;

	ComPtr<ID3D12DescriptorHeap>		m_RTVDescriptorHeap;
	unsigned int						m_RTVDescriptorNum{};
	static const unsigned int			m_RTVDescriptorMax = 1000;




	static const unsigned int			CONSTANT_BUFFER_SIZE = 2000;
	ComPtr<ID3D12Resource>				m_ConstantBuffer[2];
	byte*								m_ConstantBufferPointer[2]{};
	unsigned int						m_ConstantBufferView[2][CONSTANT_BUFFER_SIZE]{};
	unsigned int						m_ConstantBufferIndex[2]{};





	ComPtr<ID3D12RootSignature>			m_RootSignature;



	std::unique_ptr<TEXTURE>			m_WhiteTexture;

	RESOURCE							m_EnvBuffer{};
	RESOURCE							m_IBLBuffer{};

	RESOURCE							m_EnvStaticBuffer{};
	RESOURCE							m_IBLStaticBuffer{};

	DXGI_FORMAT							m_HDRBufferFormat{};
	RESOURCE							m_HDRBuffer{};
	RESOURCE							m_TemporalBuffer[2]{};
	RESOURCE							m_ShrinkBuffer{};
	RESOURCE							m_VelocityBuffer{};
	//RESOURCE							m_DepthDummyBuffer{};

	RESOURCE							m_ExposureBuffer{};

	int									m_TemporalFrameIndex{};



	std::unordered_map<std::string, ComPtr<ID3D12PipelineState>>	m_PipelineState;
	ID3D12PipelineState*				m_CurPipelineState{};





	ComPtr<ID3D12Resource>				m_SpriteVertexBuffer;







	void Initialize();
	void SetResourceBarrier(ID3D12GraphicsCommandList* CommandList, ID3D12Resource* Resource, D3D12_RESOURCE_STATES BeforeState, D3D12_RESOURCE_STATES AfterState, unsigned int Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

	ComPtr<ID3D12PipelineState> CreatePipeline(const char* VertexShaderFile, const char* PixelShaderFile,
		const DXGI_FORMAT* RTVFormats, unsigned int NumRenderTargets, D3D12_RENDER_TARGET_BLEND_DESC BlendDesc, bool DepthEnable, bool AlphaCovEnable, bool CullEnable=true, int DepthBias=0);


	void DrawScreen(unsigned int Width, unsigned int Height, float Alpha=1.0f);


	ComPtr<ID3D12Resource> CreateConstantResource(unsigned int Size);
	ComPtr<ID3D12Resource> CreateTextureResource(unsigned int Width, unsigned int Height, DXGI_FORMAT Format, D3D12_RESOURCE_FLAGS Flags, unsigned int MipLevel=1, unsigned int ArraySize=1, const float* ClearColor=nullptr);

	unsigned int CreateConstantBufferView(ID3D12Resource* Resource, unsigned int Offset, unsigned int Size);
	unsigned int CreateShaderResourceView(ID3D12Resource* Resource);
	unsigned int CreateShaderResourceViewCube(ID3D12Resource* Resource);
	unsigned int CreateShaderResourceViewArray(ID3D12Resource* Resource, unsigned int MipLevel = 0, unsigned int Index = 0);

	D3D12_CPU_DESCRIPTOR_HANDLE CreateRenderTargetView(ID3D12Resource* Resource, unsigned int MipLevel = 0);
	D3D12_CPU_DESCRIPTOR_HANDLE CreateRenderTargetViewArray(ID3D12Resource* Resource, unsigned int MipLevel = 0, unsigned int Index = 0);


	std::mutex m_ResourceCommandMutex;


public:



	static RenderManager* GetInstance() { return m_Instance; }

	ID3D12Device* GetDevice() { return m_Device.Get(); }
	ID3D12GraphicsCommandList* GetGraphicsCommandList() { return m_GraphicsCommandList.Get(); }
	ID3D12GraphicsCommandList* GetResourceCommandList() { return m_ResourceCommandList.Get(); }

	int GetSSBufferWidth() { return m_SSBufferWidth; }
	int GetSSBufferHeight() { return m_SSBufferHeight; }
	int GetBackBufferWidth() { return m_BackBufferWidth; }
	int GetBackBufferHeight() { return m_BackBufferHeight; }
	//float GetSSRatio() { return m_SSRatio; }

	std::mutex* GetResourceCommandMutex() { return &m_ResourceCommandMutex; }


	RenderManager();
	~RenderManager();


	static const int					CBV_REGISTER_MAX = 5;
	static const int					SRV_REGISTER_MAX = 10;


/*
	void GraphicsCommandBegin();
	void GraphicsCommandEnd();

	void ResourceCommandBegin();
	void ResourceCommandEnd();
*/
	void WaitGPU();



	void PreDraw();

	void DrawDepthBegin(int Index);
	void DrawDepthEnd(int Index);

	void DrawEnvBegin(int Index);
	void DrawEnvEnd(int Index);
	void DrawIBL();

	void DrawEnvStaticBegin(int Index);
	void DrawEnvStaticEnd(int Index);
	void DrawIBLStatic();

	void DrawZPrePassBegin();
	void DrawZPrePassEnd();

	void DrawBegin();
	void DrawEnd();

	void DrawAntiAliasing();
	void DrawShrinkBlur();

	void DrawShrinkBegin();
	void DrawShrinkEnd();

	void DrawPost();
	void DrawUI(bool Debug);





	std::unique_ptr<TEXTURE> LoadTexture(const char* FileName);
	std::unique_ptr<CONSTANT_BUFFER> CreateConstantBuffer(unsigned int Size);


	void ReleaseShaderResourceView(unsigned int SRVIndex);



	void SetGraphicsRootDescriptorTable(unsigned int RootParameterIndex, unsigned int ViewIndex);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGpuDescriptorHandle(unsigned int ViewIndex);


	void SetConstant(unsigned int RootParameterIndex, void* Constant, unsigned int Size);


	void SetPipelineState(const char* PiplineName);
	void SetPipelineState(ID3D12PipelineState* PiplineState);
	ID3D12PipelineState* GetPipelineState();





	void DrawSprite(Vector3 Position, Vector3 Size, Vector3 Rotation, Vector4 Color = { 1.0f, 1.0f, 1.0f, 1.0f }, Vector2 TexPosition = {0.0f, 0.0f}, Vector2 TexSize = {1.0f, 1.0f});



	void StartCommandTime(const char* Name);
	void EndCommandTime(const char* Name);



	void AddIndexCount(unsigned long IndexCount) { m_TotalIndexCount += IndexCount; }

};

