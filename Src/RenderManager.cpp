#include "Pch.h"
#include "Main.h"
#include "RenderManager.h"

#include "d3dx12.h"
#include "DDSTextureLoader12.h"

#include "Setting.h"


#include <intrin.h>




RenderManager* RenderManager::m_Instance{};


RenderManager::RenderManager()
{
	m_Instance = this;

	Initialize();
}




RenderManager::~RenderManager()
{
/*
#if defined(_DEBUG)
	//ReportLiveDeviceObjects
	{
		ComPtr<ID3D12DebugDevice> debugInterface;

		if (SUCCEEDED(m_Device->QueryInterface(IID_PPV_ARGS(&debugInterface))))
		{
			debugInterface->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL);
		}
	}
#endif
*/
}


std::string ConvertWStringToString(const std::wstring& wstr)
{
	if (wstr.empty())
	{
		return "";
	}

	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);

	return strTo;
}


void RenderManager::Initialize()
{



	{
		std::string path;
		path = ".\\Setting.ini";


		m_WindowMode = GetPrivateProfileInt("RENDER", "WINDOW_MODE", 0, path.c_str());

		m_SSBufferWidth = GetPrivateProfileInt("RENDER", "SS_BUFFER_WIDTH", 0, path.c_str());
		m_SSBufferHeight = GetPrivateProfileInt("RENDER", "SS_BUFFER_HEIGHT", 0, path.c_str());

		m_BackBufferWidth = GetPrivateProfileInt("RENDER", "BACK_BUFFER_WIDTH", 0, path.c_str());
		m_BackBufferHeight = GetPrivateProfileInt("RENDER", "BACK_BUFFER_HEIGHT", 0, path.c_str());

		m_SSRatio = GetPrivateProfileFloat("RENDER", "SUPER_SAMPLING_RATIO", path.c_str());

		m_EnvBufferSize = GetPrivateProfileInt("RENDER", "ENV_BUFFER_SIZE", 0, path.c_str());
		m_EnvBufferMipLevel = (int)log2f((float)m_EnvBufferSize) + 1;

		m_ShadowDepthBufferSize = GetPrivateProfileInt("RENDER", "SHADOW_BUFFER_SIZE ", 0, path.c_str());


		int hdrbpp = GetPrivateProfileInt("RENDER", "HDR_BUFFER_BPP ", 0, path.c_str());
		if (hdrbpp == 32)
			m_HDRBufferFormat = DXGI_FORMAT_R11G11B10_FLOAT;
		else if (hdrbpp == 64)
			m_HDRBufferFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
		else
			assert(false);
	}




	m_WindowHandle = GetWindow();
	m_Frame[0] = 1;
	m_Frame[1] = 0;
	m_RTIndex = 0;


	m_Viewport.TopLeftX = 0.f;
	m_Viewport.TopLeftY = 0.f;
	m_Viewport.Width = (FLOAT)m_BackBufferWidth;
	m_Viewport.Height = (FLOAT)m_BackBufferHeight;
	m_Viewport.MinDepth = 0.f;
	m_Viewport.MaxDepth = 1.f;

	m_ScissorRect.top = 0;
	m_ScissorRect.left = 0;
	m_ScissorRect.right = m_BackBufferWidth;
	m_ScissorRect.bottom = m_BackBufferHeight;


	m_SSViewport.TopLeftX = 0.f;
	m_SSViewport.TopLeftY = 0.f;
	//m_SSViewport.Width = (FLOAT)m_BackBufferWidth * m_SSRatio;
	//m_SSViewport.Height = (FLOAT)m_BackBufferHeight * m_SSRatio;
	m_SSViewport.Width = (FLOAT)m_SSBufferWidth;
	m_SSViewport.Height = (FLOAT)m_SSBufferHeight;
	m_SSViewport.MinDepth = 0.f;
	m_SSViewport.MaxDepth = 1.f;

	m_SSScissorRect.top = 0;
	m_SSScissorRect.left = 0;
	//m_SSScissorRect.right = (LONG)(m_BackBufferWidth * m_SSRatio);
	//m_SSScissorRect.bottom = (LONG)(m_BackBufferHeight * m_SSRatio);
	m_SSScissorRect.right = (LONG)(m_SSBufferWidth);
	m_SSScissorRect.bottom = (LONG)(m_SSBufferHeight);


	m_IBLBufferWidth = 16;
	m_IBLBufferHeight = 8;






	HRESULT hr;



#if defined(_DEBUG)
	{
		ComPtr<ID3D12Debug1>	debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
			//debugController->SetEnableGPUBasedValidation(true);
		}
	}
	/*
	//DRED
	{
		ComPtr<ID3D12DeviceRemovedExtendedDataSettings1> d3dDredSettings1;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&d3dDredSettings1))))
		{
			d3dDredSettings1->SetAutoBreadcrumbsEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
			d3dDredSettings1->SetBreadcrumbContextEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
			d3dDredSettings1->SetPageFaultEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
		}
	}*/
#endif





	{
		int data[4] = { 0 };
		char cpu_name[49] = { 0 };

		// Get the information associated with each extended function
		for (int i = 0x80000002; i <= 0x80000004; ++i)
		{
			__cpuid(data, i);

			// Interpret CPU brand string
			memcpy(cpu_name + (i - 0x80000002) * 16, data, sizeof(data));
		}

		m_CpuName = cpu_name;
	}





	{
		UINT flag{};
		hr = CreateDXGIFactory2(flag, IID_PPV_ARGS(&m_Factory));
		assert(SUCCEEDED(hr));

		hr = m_Factory->EnumAdapters(0, (IDXGIAdapter**)m_Adapter.GetAddressOf());
		assert(SUCCEEDED(hr));

		DXGI_ADAPTER_DESC desc;
		m_Adapter->GetDesc(&desc);
		m_GpuName = ConvertWStringToString(desc.Description);

		hr = D3D12CreateDevice(m_Adapter.Get(), D3D_FEATURE_LEVEL_11_1, IID_PPV_ARGS(&m_Device));
		assert(SUCCEEDED(hr));
	}


	m_Device->SetStablePowerState(true);





	{
		D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};

		commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		commandQueueDesc.Priority = 0;
		commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		commandQueueDesc.NodeMask = 0;

		hr = m_Device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&m_CommandQueue));
		assert(SUCCEEDED(hr));

		m_FenceEvent = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
		assert(m_FenceEvent);

		hr = m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence));
		assert(SUCCEEDED(hr));
	}




	{
		hr = m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_GraphicsCommandAllocator[0]));
		assert(SUCCEEDED(hr));
		m_GraphicsCommandAllocator[0]->SetName(L"GraphicsCommandAllocator[0]");

		hr = m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_GraphicsCommandAllocator[1]));
		assert(SUCCEEDED(hr));
		m_GraphicsCommandAllocator[1]->SetName(L"GraphicsCommandAllocator[1]");

		hr = m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_GraphicsCommandAllocator[0].Get(), nullptr, IID_PPV_ARGS(&m_GraphicsCommandList));
		assert(SUCCEEDED(hr));
		m_GraphicsCommandList->SetName(L"GraphicsCommandList");

		//hr = m_GraphicsCommandList->Close();
		//assert(SUCCEEDED(hr));
	}





	{
		hr = m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_ResourceCommandAllocator[0]));
		assert(SUCCEEDED(hr));
		m_ResourceCommandAllocator[0]->SetName(L"ResourceCommandAllocator[0]");

		hr = m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_ResourceCommandAllocator[1]));
		assert(SUCCEEDED(hr));
		m_ResourceCommandAllocator[1]->SetName(L"ResourceCommandAllocator[1]");

		hr = m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_ResourceCommandAllocator[0].Get(), nullptr, IID_PPV_ARGS(&m_ResourceCommandList));
		assert(SUCCEEDED(hr));
		m_ResourceCommandList->SetName(L"ResourceCommandList");


		//hr = m_ResourceCommandList->Close();
		//assert(SUCCEEDED(hr));

	}







	{
		D3D12_QUERY_HEAP_DESC queryHeapDesc{};
		queryHeapDesc.Type = D3D12_QUERY_HEAP_TYPE_TIMESTAMP;
		queryHeapDesc.Count = m_QueryMax * 2;
		m_Device->CreateQueryHeap(&queryHeapDesc, IID_PPV_ARGS(&m_QueryHeap));



		D3D12_RESOURCE_DESC desc{};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Alignment = 0;
		desc.Width = sizeof(UINT64) * m_QueryMax * 2;
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		D3D12_HEAP_PROPERTIES heap{};
		heap.Type = D3D12_HEAP_TYPE_READBACK;
		m_Device->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_QueryBuffer));



		m_CommandQueue->GetTimestampFrequency(&m_Gpufreq);

	}






	{
		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		ComPtr<IDXGISwapChain> swapChain{};

		swapChainDesc.BufferDesc.Width = m_BackBufferWidth;
		swapChainDesc.BufferDesc.Height = m_BackBufferHeight;
		swapChainDesc.OutputWindow = m_WindowHandle;
		swapChainDesc.Windowed = m_WindowMode;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;


		hr = m_Factory->CreateSwapChain(m_CommandQueue.Get(), &swapChainDesc, &swapChain);
		assert(SUCCEEDED(hr));

		hr = swapChain.As(&m_SwapChain);
		assert(SUCCEEDED(hr));

		m_RTIndex = m_SwapChain->GetCurrentBackBufferIndex();
	}






	{
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
		heapDesc.NumDescriptors = 2;
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		heapDesc.NodeMask = 0;

		hr = m_Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_RenderTargetDescriptorHeap));
		assert(SUCCEEDED(hr));

		UINT size = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		for (UINT i = 0; i < 2; ++i)
		{
			hr = m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_RenderTarget[i]));
			assert(SUCCEEDED(hr));

			m_RenderTargetHandle[i] = m_RenderTargetDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			m_RenderTargetHandle[i].ptr += size * i;
			m_Device->CreateRenderTargetView(m_RenderTarget[i].Get(), nullptr, m_RenderTargetHandle[i]);
		}
	}








	{
		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
		descriptorHeapDesc.NumDescriptors = 5;///////////////////////////////////
		descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		descriptorHeapDesc.NodeMask = 0;

		hr = m_Device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_DepthBufferDescriptorHeap));
		assert(SUCCEEDED(hr));
	}




	{
		D3D12_HEAP_PROPERTIES heapProperties{};
		D3D12_RESOURCE_DESC resourceDesc{};
		D3D12_CLEAR_VALUE clearValue{};

		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProperties.CreationNodeMask = 0;
		heapProperties.VisibleNodeMask = 0;

		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		resourceDesc.Width = (int)(m_SSBufferWidth);
		resourceDesc.Height = (int)(m_SSBufferHeight);
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		clearValue.Format = DXGI_FORMAT_D32_FLOAT;
		clearValue.DepthStencil.Depth = 1.0f;
		clearValue.DepthStencil.Stencil = 0;

		hr = m_Device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, &clearValue, IID_PPV_ARGS(&m_DepthBuffer[0]));
		assert(SUCCEEDED(hr));

		hr = m_Device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, &clearValue, IID_PPV_ARGS(&m_DepthBuffer[1]));
		assert(SUCCEEDED(hr));



		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.Texture2D.MipSlice = 0;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

		m_DepthBufferHandle[0] = m_DepthBufferDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		m_Device->CreateDepthStencilView(m_DepthBuffer[0].Get(), &dsvDesc, m_DepthBufferHandle[0]);

		m_DepthBufferHandle[1] = m_DepthBufferDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		m_DepthBufferHandle[1].ptr += m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		m_Device->CreateDepthStencilView(m_DepthBuffer[1].Get(), &dsvDesc, m_DepthBufferHandle[1]);

	}






	for(int i = 0; i < 3; i++)
	{
		D3D12_HEAP_PROPERTIES heapProperties{};
		D3D12_RESOURCE_DESC resourceDesc{};
		D3D12_CLEAR_VALUE clearValue{};

		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProperties.CreationNodeMask = 0;
		heapProperties.VisibleNodeMask = 0;

		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		resourceDesc.Width = m_ShadowDepthBufferSize * 2;
		resourceDesc.Height = m_ShadowDepthBufferSize;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		clearValue.Format = DXGI_FORMAT_D32_FLOAT;
		clearValue.DepthStencil.Depth = 1.0f;
		clearValue.DepthStencil.Stencil = 0;

		hr = m_Device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, &clearValue, IID_PPV_ARGS(&m_ShadowDepthBuffer[i]));
		assert(SUCCEEDED(hr));



		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.Texture2D.MipSlice = 0;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

		m_ShadowDepthBufferHandle[i] = m_DepthBufferDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		m_ShadowDepthBufferHandle[i].ptr += m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV) * (i + 2);

		m_Device->CreateDepthStencilView(m_ShadowDepthBuffer[i].Get(), &dsvDesc, m_ShadowDepthBufferHandle[i]);



	}




	{
		D3D12_DESCRIPTOR_HEAP_DESC desc;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		desc.NumDescriptors = m_SRVDescriptorMax;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NodeMask = 0;

		m_Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_SRVDescriptorHeap));

		for (int i = 0; i < m_SRVDescriptorMax; i++)
			m_SRVDescriptorPool.push_back(i);
	}

	{
		D3D12_DESCRIPTOR_HEAP_DESC desc;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		desc.NumDescriptors = m_RTVDescriptorMax;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.NodeMask = 0;

		m_Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_RTVDescriptorHeap));

		m_RTVDescriptorNum = 0;
	}






	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// Setup Platform/Renderer backends
		ImGui_ImplWin32_Init(m_WindowHandle);
		ImGui_ImplDX12_Init(m_Device.Get(), 2,
			DXGI_FORMAT_R8G8B8A8_UNORM, m_SRVDescriptorHeap.Get(),
			m_SRVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
			m_SRVDescriptorHeap->GetGPUDescriptorHandleForHeapStart());


		//m_SRVDescriptorNum++;

		m_SRVDescriptorPool.pop_front();
	}





	m_DepthBufferSRVIndex[0] = CreateShaderResourceView(m_DepthBuffer[0].Get());
	m_DepthBufferSRVIndex[1] = CreateShaderResourceView(m_DepthBuffer[1].Get());

	m_ShadowDepthBufferSRVIndex[0] = CreateShaderResourceView(m_ShadowDepthBuffer[0].Get());
	m_ShadowDepthBufferSRVIndex[1] = CreateShaderResourceView(m_ShadowDepthBuffer[1].Get());
	m_ShadowDepthBufferSRVIndex[2] = CreateShaderResourceView(m_ShadowDepthBuffer[2].Get());




	m_WhiteTexture = LoadTexture("asset/white.dds");


	//m_EnvTexture = LoadTexture("asset/snow_field_2k_EXR_BC6H_1.DDS");
	//m_EnvTexture = LoadTexture("asset/CubeMap.DDS");

	//m_IBLTexture = LoadTexture("asset/snow_field_256_ibl_DDS_BC6H_1.DDS");





	for (int i = 0; i < 2; i++)
	{
		m_ConstantBuffer[i] = CreateConstantResource(512 * CONSTANT_BUFFER_SIZE);

		hr = m_ConstantBuffer[i]->Map(0, nullptr, (void**)&m_ConstantBufferPointer[i]);
		assert(SUCCEEDED(hr));


		for (int j = 0; j < CONSTANT_BUFFER_SIZE; j++)
		{
			m_ConstantBufferView[i][j] = CreateConstantBufferView(m_ConstantBuffer[i].Get(), j * 512, 512);
		}

		m_ConstantBufferIndex[i] = 0;
	}






	////for (int i = 0; i < 2; i++)
	//{
	//	m_DepthDummyBuffer.Resource = CreateTextureResource(m_ShadowDepthBufferSize, m_ShadowDepthBufferSize,
	//		DXGI_FORMAT_R8_UNORM,
	//		D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

	//	m_DepthDummyBuffer.RTVHandle = CreateRenderTargetView(m_DepthDummyBuffer.Resource.Get());


	//	SetResourceBarrier(m_GraphicsCommandList.Get(), m_DepthDummyBuffer.Resource.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET, 0);
	//}





	//for (int i = 0; i < 2; i++)
	{
		m_EnvBuffer.Resource = CreateTextureResource(m_EnvBufferSize, m_EnvBufferSize,
			m_HDRBufferFormat,
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, m_EnvBufferMipLevel, 6);

		m_EnvBuffer.Resource->SetName(L"EnvBuffer");

		m_EnvBuffer.SRVIndex = CreateShaderResourceViewCube(m_EnvBuffer.Resource.Get());

		for (int j = 0; j < 6; j++)
		{
			for (int k = 0; k < m_EnvBufferMipLevel; k++)
			{
				m_EnvBuffer.SRVIndexArray[j][k] = CreateShaderResourceViewArray(m_EnvBuffer.Resource.Get(), k, j);
				m_EnvBuffer.RTVHandleArray[j][k] = CreateRenderTargetViewArray(m_EnvBuffer.Resource.Get(), k, j);
			}
		}
	}


	//for (int i = 0; i < 2; i++)
	{
		m_IBLBuffer.Resource = CreateTextureResource(m_IBLBufferWidth, m_IBLBufferHeight,
			m_HDRBufferFormat,
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		m_IBLBuffer.Resource->SetName(L"IBLBuffer");

		m_IBLBuffer.SRVIndex = CreateShaderResourceView(m_IBLBuffer.Resource.Get());
		m_IBLBuffer.RTVHandle = CreateRenderTargetView(m_IBLBuffer.Resource.Get());
	}

	//for (int i = 0; i < 2; i++)
	{
		m_IBLStaticBuffer.Resource = CreateTextureResource(m_IBLBufferWidth, m_IBLBufferHeight,
			m_HDRBufferFormat,
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		m_IBLStaticBuffer.Resource->SetName(L"IBLStaticBuffer");

		m_IBLStaticBuffer.SRVIndex = CreateShaderResourceView(m_IBLStaticBuffer.Resource.Get());
		m_IBLStaticBuffer.RTVHandle = CreateRenderTargetView(m_IBLStaticBuffer.Resource.Get());
	}


	//for (int i = 0; i < 2; i++)
	{
		int width = (int)(m_SSBufferWidth);
		int height = (int)(m_SSBufferHeight);


		m_HDRBuffer.Resource = CreateTextureResource(width, height,
			m_HDRBufferFormat,
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		m_HDRBuffer.Resource->SetName(L"HDRBuffer");

		m_HDRBuffer.SRVIndex = CreateShaderResourceView(m_HDRBuffer.Resource.Get());
		m_HDRBuffer.RTVHandle = CreateRenderTargetView(m_HDRBuffer.Resource.Get());
	}


	for (int i = 0; i < 2; i++)
	{
		int width = (int)(m_BackBufferWidth);
		int height = (int)(m_BackBufferHeight);


		m_TemporalBuffer[i].Resource = CreateTextureResource(width, height,
			m_HDRBufferFormat,
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, 10);

		m_TemporalBuffer[i].Resource->SetName(L"TAABuffer");

		m_TemporalBuffer[i].SRVIndex = CreateShaderResourceView(m_TemporalBuffer[i].Resource.Get());


		for (int j = 0; j < 10; j++)
		{
			m_TemporalBuffer[i].SRVIndexArray[0][j] = CreateShaderResourceViewArray(m_TemporalBuffer[i].Resource.Get(), j);
			m_TemporalBuffer[i].RTVHandleArray[0][j] = CreateRenderTargetViewArray(m_TemporalBuffer[i].Resource.Get(), j);
		}
	}



	//for (int i = 0; i < 2; i++)
	{
		int width = m_SSBufferWidth / 4;
		int height = m_SSBufferHeight / 4;

		float clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		m_ShrinkBuffer.Resource = CreateTextureResource(width, height,
			DXGI_FORMAT_R16G16B16A16_FLOAT,
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, 1, 1, clearColor);

		m_ShrinkBuffer.Resource->SetName(L"ShrinkBuffer");


		m_ShrinkBuffer.SRVIndex = CreateShaderResourceView(m_ShrinkBuffer.Resource.Get());
		m_ShrinkBuffer.RTVHandle = CreateRenderTargetView(m_ShrinkBuffer.Resource.Get());
	}



	//for (int i = 0; i < 2; i++)
	{
		int width = (int)(m_SSBufferWidth);
		int height = (int)(m_SSBufferHeight);

		m_VelocityBuffer.Resource = CreateTextureResource(width, height,
			DXGI_FORMAT_R16G16_FLOAT,
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		m_VelocityBuffer.Resource->SetName(L"VelocityBuffer");


		m_VelocityBuffer.SRVIndex = CreateShaderResourceView(m_VelocityBuffer.Resource.Get());
		m_VelocityBuffer.RTVHandle = CreateRenderTargetView(m_VelocityBuffer.Resource.Get());
	}




	{

		m_ExposureBuffer.Resource = CreateTextureResource(4, 4,
			m_HDRBufferFormat,
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		m_ExposureBuffer.Resource->SetName(L"ExposureBuffer");


		m_ExposureBuffer.SRVIndex = CreateShaderResourceView(m_ExposureBuffer.Resource.Get());
		m_ExposureBuffer.RTVHandle = CreateRenderTargetView(m_ExposureBuffer.Resource.Get());
	}




	{

		D3D12_ROOT_PARAMETER		rootParameters[CBV_REGISTER_MAX + SRV_REGISTER_MAX]{};
		D3D12_DESCRIPTOR_RANGE		range[CBV_REGISTER_MAX + SRV_REGISTER_MAX]{};


		for (unsigned int i = 0; i < CBV_REGISTER_MAX; i++)
		{
			range[i].NumDescriptors = 1;
			range[i].BaseShaderRegister = i;
			range[i].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
			range[i].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			rootParameters[i].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rootParameters[i].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			rootParameters[i].DescriptorTable.NumDescriptorRanges = 1;
			rootParameters[i].DescriptorTable.pDescriptorRanges = &range[i];
		}



		for (unsigned int i = 0; i < SRV_REGISTER_MAX; i++)
		{
			range[CBV_REGISTER_MAX + i].NumDescriptors = 1;
			range[CBV_REGISTER_MAX + i].BaseShaderRegister = i;
			range[CBV_REGISTER_MAX + i].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			range[CBV_REGISTER_MAX + i].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			rootParameters[CBV_REGISTER_MAX + i].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rootParameters[CBV_REGISTER_MAX + i].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			rootParameters[CBV_REGISTER_MAX + i].DescriptorTable.NumDescriptorRanges = 1;
			rootParameters[CBV_REGISTER_MAX + i].DescriptorTable.pDescriptorRanges = &range[CBV_REGISTER_MAX + i];
		}


		D3D12_STATIC_SAMPLER_DESC	samplerDesc[4]{};
		//samplerDesc[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc[0].Filter = D3D12_FILTER_ANISOTROPIC;
		samplerDesc[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc[0].MipLODBias = 0.0f;//-0.1f Bias for TSR ////////////////////////////////////////////////////////////////////////
		samplerDesc[0].MaxAnisotropy = 2;
		samplerDesc[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		samplerDesc[0].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		samplerDesc[0].MinLOD = 0.0f;
		samplerDesc[0].MaxLOD = D3D12_FLOAT32_MAX;
		samplerDesc[0].ShaderRegister = 0;
		samplerDesc[0].RegisterSpace = 0;
		samplerDesc[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		samplerDesc[1].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc[1].MipLODBias = 0.0f;
		samplerDesc[1].MaxAnisotropy = 0;
		samplerDesc[1].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		samplerDesc[1].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		samplerDesc[1].MinLOD = 0.0f;
		samplerDesc[1].MaxLOD = D3D12_FLOAT32_MAX;
		samplerDesc[1].ShaderRegister = 1;
		samplerDesc[1].RegisterSpace = 0;
		samplerDesc[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		samplerDesc[2].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc[2].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc[2].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc[2].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc[2].MipLODBias = 0.0f;
		samplerDesc[2].MaxAnisotropy = 0;
		samplerDesc[2].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		samplerDesc[2].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		samplerDesc[2].MinLOD = 0.0f;
		samplerDesc[2].MaxLOD = D3D12_FLOAT32_MAX;
		samplerDesc[2].ShaderRegister = 2;
		samplerDesc[2].RegisterSpace = 0;
		samplerDesc[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		samplerDesc[3].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		samplerDesc[3].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc[3].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc[3].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		samplerDesc[3].MipLODBias = 0.0f;
		samplerDesc[3].MaxAnisotropy = 0;
		samplerDesc[3].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		samplerDesc[3].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		samplerDesc[3].MinLOD = 0.0f;
		samplerDesc[3].MaxLOD = D3D12_FLOAT32_MAX;
		samplerDesc[3].ShaderRegister = 3;
		samplerDesc[3].RegisterSpace = 0;
		samplerDesc[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;


		D3D12_ROOT_SIGNATURE_DESC	rootSignatureDesc{};
		rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		rootSignatureDesc.NumParameters = _countof(rootParameters);
		rootSignatureDesc.pParameters = rootParameters;
		rootSignatureDesc.NumStaticSamplers = _countof(samplerDesc);
		rootSignatureDesc.pStaticSamplers = samplerDesc;


		HRESULT hr;
		ComPtr<ID3DBlob> blob{};
		hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, nullptr);
		assert(SUCCEEDED(hr));

		hr = m_Device->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature));
		assert(SUCCEEDED(hr));
	}



	{
		DXGI_FORMAT RTVFormats[] =
		{
			DXGI_FORMAT_R8_UNORM,
		};

		D3D12_RENDER_TARGET_BLEND_DESC blendDesc{};
		blendDesc.BlendEnable = FALSE;
		blendDesc.SrcBlend = D3D12_BLEND_ONE;
		blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		blendDesc.BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		blendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
		blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		blendDesc.LogicOpEnable = FALSE;
		blendDesc.LogicOp = D3D12_LOGIC_OP_CLEAR;

		m_PipelineState["Depth"] = CreatePipeline("Shader\\DepthVS.cso", "Shader\\DepthPS.cso", RTVFormats, _countof(RTVFormats), blendDesc, true, false, true);
	}



	{
		DXGI_FORMAT RTVFormats[] =
		{
			m_HDRBufferFormat,
			DXGI_FORMAT_R16G16_FLOAT
		};

		D3D12_RENDER_TARGET_BLEND_DESC blendDesc{};
		blendDesc.BlendEnable = TRUE;
		blendDesc.SrcBlend = D3D12_BLEND_ONE;
		blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		blendDesc.BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		blendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
		blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		blendDesc.LogicOpEnable = FALSE;
		blendDesc.LogicOp = D3D12_LOGIC_OP_CLEAR;

		m_PipelineState["Sky"] = CreatePipeline("Shader\\SkyVS.cso", "Shader\\SkyPS.cso", RTVFormats, _countof(RTVFormats), blendDesc, true, false);
		m_PipelineState["Shadow"] = CreatePipeline("Shader\\ShadowVS.cso", "Shader\\ShadowPS.cso", RTVFormats, _countof(RTVFormats), blendDesc, false, false);
		m_PipelineState["Field"] = CreatePipeline("Shader\\FieldVS.cso", "Shader\\FieldPS.cso", RTVFormats, _countof(RTVFormats), blendDesc, true, false);
		m_PipelineState["Car"] = CreatePipeline("Shader\\CarVS.cso", "Shader\\CarPS.cso", RTVFormats, _countof(RTVFormats), blendDesc, true, false);
	}


	{
		DXGI_FORMAT RTVFormats[] =
		{
			m_HDRBufferFormat
		};

		D3D12_RENDER_TARGET_BLEND_DESC blendDesc{};
		blendDesc.BlendEnable = TRUE;
		blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		blendDesc.BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		blendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
		blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		blendDesc.LogicOpEnable = FALSE;
		blendDesc.LogicOp = D3D12_LOGIC_OP_CLEAR;

		m_PipelineState["AntiAliasing"] = CreatePipeline("Shader\\AntiAliasingVS.cso", "Shader\\AntiAliasingPS.cso", RTVFormats, _countof(RTVFormats), blendDesc, false, false);
	}


	{
		DXGI_FORMAT RTVFormats[] =
		{
			DXGI_FORMAT_R16G16B16A16_FLOAT
		};

		D3D12_RENDER_TARGET_BLEND_DESC blendDesc{};
		blendDesc.BlendEnable = TRUE;
		blendDesc.SrcBlend = D3D12_BLEND_ONE;
		blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		blendDesc.BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		blendDesc.DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
		blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		blendDesc.LogicOpEnable = FALSE;
		blendDesc.LogicOp = D3D12_LOGIC_OP_CLEAR;

		m_PipelineState["Smoke"] = CreatePipeline("Shader\\SmokeVS.cso", "Shader\\SmokePS.cso", RTVFormats, _countof(RTVFormats), blendDesc, false, false);
	}



	{
		DXGI_FORMAT RTVFormats[] =
		{
			m_HDRBufferFormat
		};

		D3D12_RENDER_TARGET_BLEND_DESC blendDesc{};
		blendDesc.BlendEnable = TRUE;
		blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		blendDesc.BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		blendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
		blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		blendDesc.LogicOpEnable = FALSE;
		blendDesc.LogicOp = D3D12_LOGIC_OP_CLEAR;

		m_PipelineState["IBL"] = CreatePipeline("Shader\\IblVS.cso", "Shader\\IblPS.cso", RTVFormats, _countof(RTVFormats), blendDesc, false, false);
		//m_PipelineState["TSS"] = CreatePipeline("Shader\\TssVS.cso", "Shader\\TssPS.cso", RTVFormats, _countof(RTVFormats), blendDesc, false, false);
		m_PipelineState["Shrink"] = CreatePipeline("Shader\\ShrinkVS.cso", "Shader\\ShrinkPS.cso", RTVFormats, _countof(RTVFormats), blendDesc, false, false);
		m_PipelineState["ShrinkBlur"] = CreatePipeline("Shader\\ShrinkBlurVS.cso", "Shader\\ShrinkBlurPS.cso", RTVFormats, _countof(RTVFormats), blendDesc, false, false);
	}


	{
		DXGI_FORMAT RTVFormats[] =
		{
			DXGI_FORMAT_R8G8B8A8_UNORM
		};

		D3D12_RENDER_TARGET_BLEND_DESC blendDesc{};
		blendDesc.BlendEnable = TRUE;
		blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		blendDesc.BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		blendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
		blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		blendDesc.LogicOpEnable = FALSE;
		blendDesc.LogicOp = D3D12_LOGIC_OP_CLEAR;

		m_PipelineState["Post"] = CreatePipeline("Shader\\PostVS.cso", "Shader\\PostPS.cso", RTVFormats, _countof(RTVFormats), blendDesc, false, false);


		blendDesc.BlendEnable = TRUE;

		m_PipelineState["Ui"] = CreatePipeline("Shader\\UiVS.cso", "Shader\\UiPS.cso", RTVFormats, _countof(RTVFormats), blendDesc, false, false);
	}





	{
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

		resourceDesc.Width = sizeof(VERTEX_3D) * 4;
		hr = m_Device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE,
			&resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
			IID_PPV_ARGS(&m_SpriteVertexBuffer));
		assert(SUCCEEDED(hr));


		VERTEX_3D* buffer{};
		hr = m_SpriteVertexBuffer->Map(0, nullptr, (void**)&buffer);
		assert(SUCCEEDED(hr));

		buffer[0].Position = Vector3{ -1.0f,  1.0f,  0.0f };
		buffer[1].Position = Vector3{ 1.0f,  1.0f, 0.0f };
		buffer[2].Position = Vector3{ -1.0f,  -1.0f, 0.0f };
		buffer[3].Position = Vector3{ 1.0f,  -1.0f, 0.0f };
		buffer[0].Normal = Vector3{ 0.0f, 1.0f, 0.0f };
		buffer[1].Normal = Vector3{ 0.0f, 1.0f, 0.0f };
		buffer[2].Normal = Vector3{ 0.0f, 1.0f, 0.0f };
		buffer[3].Normal = Vector3{ 0.0f, 1.0f, 0.0f };
		buffer[0].TexCoord = { 0.0f, 0.0f };
		buffer[1].TexCoord = { 1.0f, 0.0f };
		buffer[2].TexCoord = { 0.0f, 1.0f };
		buffer[3].TexCoord = { 1.0f, 1.0f };

		m_SpriteVertexBuffer->Unmap(0, nullptr);
	}



}






void RenderManager::WaitGPU()
{

	m_CommandQueue->Signal(m_Fence.Get(), m_Frame[m_RTIndex]);

	m_Fence->SetEventOnCompletion(m_Frame[m_RTIndex], m_FenceEvent);
	WaitForSingleObjectEx(m_FenceEvent, INFINITE, FALSE);

	m_Frame[m_RTIndex]++;

}








void RenderManager::PreDraw()
{



	//if (false)
	{
		RECT rect;
		GetClientRect(GetWindow(), &rect);

		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame((float)(rect.right - rect.left) / m_BackBufferWidth, (float)(rect.bottom - rect.top) / m_BackBufferHeight, (float)m_BackBufferWidth, (float)m_BackBufferHeight);
		ImGui::NewFrame();

		//ImGui::ShowDemoWindow();
	}


	{
		ID3D12DescriptorHeap* dh[] = { m_SRVDescriptorHeap.Get() };
		m_GraphicsCommandList->SetDescriptorHeaps(_countof(dh), dh);
	}


	m_GraphicsCommandList->SetGraphicsRootSignature(m_RootSignature.Get());


	m_ConstantBufferIndex[m_RTIndex] = 0;



	SetResourceBarrier(m_GraphicsCommandList.Get(), m_DepthBuffer[m_RTIndex].Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE, 0);



	m_TotalIndexCount = 0;
}





void RenderManager::DrawDepthBegin(int Index)
{


	{
		D3D12_VIEWPORT viewport = m_SSViewport;
		D3D12_RECT scissorRect = m_SSScissorRect;

		viewport.Width = (float)m_ShadowDepthBufferSize * 2;
		viewport.Height = (float)m_ShadowDepthBufferSize;

		scissorRect.right = m_ShadowDepthBufferSize * 2;
		scissorRect.bottom = m_ShadowDepthBufferSize;


		m_GraphicsCommandList->RSSetViewports(1, &viewport);
		m_GraphicsCommandList->RSSetScissorRects(1, &scissorRect);


		SetResourceBarrier(m_GraphicsCommandList.Get(), m_ShadowDepthBuffer[Index].Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE, 0);


		m_GraphicsCommandList->OMSetRenderTargets(0, nullptr, false, &m_ShadowDepthBufferHandle[Index]);

		m_GraphicsCommandList->ClearDepthStencilView(m_ShadowDepthBufferHandle[Index], D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);


		m_GraphicsCommandList->SetPipelineState(m_PipelineState["Depth"].Get());

	}



}




void RenderManager::DrawDepthEnd(int Index)
{

	SetResourceBarrier(m_GraphicsCommandList.Get(), m_ShadowDepthBuffer[Index].Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, 0);

}





void RenderManager::DrawEnvBegin(int Index)
{


	{
		D3D12_VIEWPORT viewport = m_Viewport;
		D3D12_RECT scissorRect = m_ScissorRect;

		viewport.Width = (float)m_EnvBufferSize;
		viewport.Height = (float)m_EnvBufferSize;

		scissorRect.right = m_EnvBufferSize;
		scissorRect.bottom = m_EnvBufferSize;

		m_GraphicsCommandList->RSSetViewports(1, &viewport);
		m_GraphicsCommandList->RSSetScissorRects(1, &scissorRect);


		SetResourceBarrier(m_GraphicsCommandList.Get(), m_EnvBuffer.Resource.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET, Index * m_EnvBufferMipLevel);


		FLOAT clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_GraphicsCommandList->ClearDepthStencilView(m_DepthBufferHandle[m_RTIndex], D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
		m_GraphicsCommandList->ClearRenderTargetView(m_EnvBuffer.RTVHandleArray[Index][0], clearColor, 0, nullptr);


		D3D12_CPU_DESCRIPTOR_HANDLE rt[1] =
		{
			m_EnvBuffer.RTVHandleArray[Index][0],
		};
		m_GraphicsCommandList->OMSetRenderTargets(1, rt, false, &m_DepthBufferHandle[m_RTIndex]);



		SetGraphicsRootDescriptorTable(RenderManager::CBV_REGISTER_MAX + 0, m_EnvBuffer.SRVIndex);
		SetGraphicsRootDescriptorTable(RenderManager::CBV_REGISTER_MAX + 1, m_IBLBuffer.SRVIndex);
		SetGraphicsRootDescriptorTable(RenderManager::CBV_REGISTER_MAX + 2, m_IBLStaticBuffer.SRVIndex);
		SetGraphicsRootDescriptorTable(RenderManager::CBV_REGISTER_MAX + 5, m_ShadowDepthBufferSRVIndex[0]);
		SetGraphicsRootDescriptorTable(RenderManager::CBV_REGISTER_MAX + 6, m_ShadowDepthBufferSRVIndex[1]);
		SetGraphicsRootDescriptorTable(RenderManager::CBV_REGISTER_MAX + 7, m_ShadowDepthBufferSRVIndex[2]);

	}

}




void RenderManager::DrawEnvEnd(int Index)
{

	{

		SetResourceBarrier(m_GraphicsCommandList.Get(), m_EnvBuffer.Resource.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, Index * m_EnvBufferMipLevel);
	}


	//if(false)
	{
		m_GraphicsCommandList->SetPipelineState(m_PipelineState["Shrink"].Get());



		D3D12_VIEWPORT viewport = m_Viewport;
		D3D12_RECT scissorRect = m_ScissorRect;

		viewport.Width = (float)m_EnvBufferSize;
		viewport.Height = (float)m_EnvBufferSize;

		scissorRect.right = m_EnvBufferSize;
		scissorRect.bottom = m_EnvBufferSize;


		for (int i = 1; i < m_EnvBufferMipLevel; i++)
		{
			viewport.Width /= 2;
			viewport.Height /= 2;

			scissorRect.right /= 2;
			scissorRect.bottom /= 2;


			m_GraphicsCommandList->RSSetViewports(1, &viewport);
			m_GraphicsCommandList->RSSetScissorRects(1, &scissorRect);


			SetResourceBarrier(m_GraphicsCommandList.Get(), m_EnvBuffer.Resource.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET, Index * m_EnvBufferMipLevel + i);


			m_GraphicsCommandList->OMSetRenderTargets(1, &m_EnvBuffer.RTVHandleArray[Index][i], false, nullptr);


			SetGraphicsRootDescriptorTable(RenderManager::CBV_REGISTER_MAX + 0, m_EnvBuffer.SRVIndexArray[Index][i - 1]);



			DrawScreen((unsigned int)viewport.Width, (unsigned int)viewport.Height);


			SetResourceBarrier(m_GraphicsCommandList.Get(), m_EnvBuffer.Resource.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, Index * m_EnvBufferMipLevel + i);

		}
	}

}



void RenderManager::DrawIBL()
{

	{
		D3D12_VIEWPORT viewport = m_Viewport;
		D3D12_RECT scissorRect = m_ScissorRect;

		viewport.Width = (float)m_IBLBufferWidth;
		viewport.Height = (float)m_IBLBufferHeight;

		scissorRect.right = m_IBLBufferWidth;
		scissorRect.bottom = m_IBLBufferHeight;

		m_GraphicsCommandList->RSSetViewports(1, &viewport);
		m_GraphicsCommandList->RSSetScissorRects(1, &scissorRect);


		m_GraphicsCommandList->SetPipelineState(m_PipelineState["IBL"].Get());


		SetResourceBarrier(m_GraphicsCommandList.Get(), m_IBLBuffer.Resource.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);

		m_GraphicsCommandList->OMSetRenderTargets(1, &m_IBLBuffer.RTVHandle, false, nullptr);




		SetGraphicsRootDescriptorTable(RenderManager::CBV_REGISTER_MAX + 0, m_EnvBuffer.SRVIndex);


		DrawScreen((unsigned int)viewport.Width, (unsigned int)viewport.Height);



		SetResourceBarrier(m_GraphicsCommandList.Get(), m_IBLBuffer.Resource.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	}

}



void RenderManager::DrawIBLStatic()
{

	{
		D3D12_VIEWPORT viewport = m_Viewport;
		D3D12_RECT scissorRect = m_ScissorRect;

		viewport.Width = (float)m_IBLBufferWidth;
		viewport.Height = (float)m_IBLBufferHeight;

		scissorRect.right = m_IBLBufferWidth;
		scissorRect.bottom = m_IBLBufferHeight;

		m_GraphicsCommandList->RSSetViewports(1, &viewport);
		m_GraphicsCommandList->RSSetScissorRects(1, &scissorRect);


		m_GraphicsCommandList->SetPipelineState(m_PipelineState["Shrink"].Get());


		SetResourceBarrier(m_GraphicsCommandList.Get(), m_IBLStaticBuffer.Resource.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);

		m_GraphicsCommandList->OMSetRenderTargets(1, &m_IBLStaticBuffer.RTVHandle, false, nullptr);




		SetGraphicsRootDescriptorTable(RenderManager::CBV_REGISTER_MAX + 0, m_IBLBuffer.SRVIndex);


		DrawScreen((unsigned int)viewport.Width, (unsigned int)viewport.Height);



		SetResourceBarrier(m_GraphicsCommandList.Get(), m_IBLStaticBuffer.Resource.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	}

}






void RenderManager::DrawZPrePassBegin()
{

	{
		m_GraphicsCommandList->RSSetViewports(1, &m_SSViewport);
		m_GraphicsCommandList->RSSetScissorRects(1, &m_SSScissorRect);


		//SetResourceBarrier(m_HDRBuffer.Resource.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET, 0);
		//SetResourceBarrier(m_VelocityBuffer.Resource.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);

/*
		D3D12_CPU_DESCRIPTOR_HANDLE rt[2] =
		{
			m_HDRBuffer.RTVHandleArray[0][0],
			m_VelocityBuffer.RTVHandle,
		};
*/
		m_GraphicsCommandList->OMSetRenderTargets(0, nullptr, false, &m_DepthBufferHandle[m_RTIndex]);


		FLOAT clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_GraphicsCommandList->ClearDepthStencilView(m_DepthBufferHandle[m_RTIndex], D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
		//m_GraphicsCommandList->ClearRenderTargetView(m_HDRBuffer.RTVHandleArray[0][0], clearColor, 0, nullptr);
		//m_GraphicsCommandList->ClearRenderTargetView(m_VelocityBuffer.RTVHandle, clearColor, 0, nullptr);


		m_GraphicsCommandList->SetPipelineState(m_PipelineState["Depth"].Get());

/*
		//SetGraphicsRootDescriptorTable(4, m_EnvTexture.SRVIndex);
		SetGraphicsRootDescriptorTable(4, m_EnvBuffer.SRVIndex);
		SetGraphicsRootDescriptorTable(5, m_IBLBuffer.SRVIndex);
		SetGraphicsRootDescriptorTable(9, m_IBLTexture.SRVIndex);
*/
	}


}

void RenderManager::DrawZPrePassEnd()
{
}








void RenderManager::DrawBegin()
{

	m_TemporalFrameIndex = (m_TemporalFrameIndex + 1) % 2;


	{
		m_GraphicsCommandList->RSSetViewports(1, &m_SSViewport);
		m_GraphicsCommandList->RSSetScissorRects(1, &m_SSScissorRect);


		SetResourceBarrier(m_GraphicsCommandList.Get(), m_HDRBuffer.Resource.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
		SetResourceBarrier(m_GraphicsCommandList.Get(), m_VelocityBuffer.Resource.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);


		D3D12_CPU_DESCRIPTOR_HANDLE rt[2] =
		{
			m_HDRBuffer.RTVHandle,
			m_VelocityBuffer.RTVHandle,
		};
		m_GraphicsCommandList->OMSetRenderTargets(2, rt, false, &m_DepthBufferHandle[m_RTIndex]);


		FLOAT clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		//m_GraphicsCommandList->ClearDepthStencilView(m_DepthBufferHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
		m_GraphicsCommandList->ClearRenderTargetView(m_HDRBuffer.RTVHandle, clearColor, 0, nullptr);
		m_GraphicsCommandList->ClearRenderTargetView(m_VelocityBuffer.RTVHandle, clearColor, 0, nullptr);



		SetGraphicsRootDescriptorTable(RenderManager::CBV_REGISTER_MAX + 0, m_EnvBuffer.SRVIndex);
		SetGraphicsRootDescriptorTable(RenderManager::CBV_REGISTER_MAX + 1, m_IBLBuffer.SRVIndex);
		SetGraphicsRootDescriptorTable(RenderManager::CBV_REGISTER_MAX + 3, m_IBLStaticBuffer.SRVIndex);
		SetGraphicsRootDescriptorTable(RenderManager::CBV_REGISTER_MAX + 5, m_ShadowDepthBufferSRVIndex[0]);
		SetGraphicsRootDescriptorTable(RenderManager::CBV_REGISTER_MAX + 6, m_ShadowDepthBufferSRVIndex[1]);
		SetGraphicsRootDescriptorTable(RenderManager::CBV_REGISTER_MAX + 7, m_ShadowDepthBufferSRVIndex[2]);

	}



}




void RenderManager::DrawEnd()
{


	{

		//SetResourceBarrier(m_GraphicsCommandList.Get(), m_DepthDummyBuffer.Resource.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		SetResourceBarrier(m_GraphicsCommandList.Get(), m_HDRBuffer.Resource.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, 0);
		SetResourceBarrier(m_GraphicsCommandList.Get(), m_VelocityBuffer.Resource.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		SetResourceBarrier(m_GraphicsCommandList.Get(), m_DepthBuffer[m_RTIndex].Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, 0);

	}

}





void RenderManager::DrawAntiAliasing()
{

	{
		m_GraphicsCommandList->RSSetViewports(1, &m_Viewport);
		m_GraphicsCommandList->RSSetScissorRects(1, &m_ScissorRect);


		SetResourceBarrier(m_GraphicsCommandList.Get(), m_TemporalBuffer[m_TemporalFrameIndex].Resource.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET, 0);


		m_GraphicsCommandList->SetPipelineState(m_PipelineState["AntiAliasing"].Get());

		SetGraphicsRootDescriptorTable(RenderManager::CBV_REGISTER_MAX + 0, m_HDRBuffer.SRVIndex);
		SetGraphicsRootDescriptorTable(RenderManager::CBV_REGISTER_MAX + 1, m_TemporalBuffer[(m_TemporalFrameIndex + 1) % 2].SRVIndex);
		SetGraphicsRootDescriptorTable(RenderManager::CBV_REGISTER_MAX + 2, m_VelocityBuffer.SRVIndex);
		SetGraphicsRootDescriptorTable(RenderManager::CBV_REGISTER_MAX + 3, m_DepthBufferSRVIndex[m_RTIndex]);
		SetGraphicsRootDescriptorTable(RenderManager::CBV_REGISTER_MAX + 4, m_DepthBufferSRVIndex[(m_RTIndex + 1) % 2]);


		D3D12_CPU_DESCRIPTOR_HANDLE rt[1] =
		{
			m_TemporalBuffer[m_TemporalFrameIndex].RTVHandleArray[0][0],
		};
		m_GraphicsCommandList->OMSetRenderTargets(1, rt, false, nullptr);




		DrawScreen((unsigned int)m_Viewport.Width, (unsigned int)m_Viewport.Height);


		SetResourceBarrier(m_GraphicsCommandList.Get(), m_TemporalBuffer[m_TemporalFrameIndex].Resource.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, 0);

	}

}





void RenderManager::DrawShrinkBlur()
{


	{

		//SetResourceBarrier(m_GraphicsCommandList.Get(), m_HDRBuffer.Resource.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, 0);

	}




	//if(false)
	{
		m_GraphicsCommandList->SetPipelineState(m_PipelineState["ShrinkBlur"].Get());



		D3D12_VIEWPORT viewport = m_Viewport;
		D3D12_RECT scissorRect = m_ScissorRect;

		int i;
		for (i = 1; i < 10; i++)
		{
			viewport.Width /= 2;
			viewport.Height /= 2;

			scissorRect.right /= 2;
			scissorRect.bottom /= 2;


			m_GraphicsCommandList->RSSetViewports(1, &viewport);
			m_GraphicsCommandList->RSSetScissorRects(1, &scissorRect);


			SetResourceBarrier(m_GraphicsCommandList.Get(), m_TemporalBuffer[m_TemporalFrameIndex].Resource.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET, i);


			m_GraphicsCommandList->OMSetRenderTargets(1, &m_TemporalBuffer[m_TemporalFrameIndex].RTVHandleArray[0][i], false, nullptr);


			FLOAT clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
			//m_GraphicsCommandList->ClearDepthStencilView(m_DepthBufferHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
			//m_GraphicsCommandList->ClearRenderTargetView(handle, clearColor, 0, nullptr);

			SetGraphicsRootDescriptorTable(RenderManager::CBV_REGISTER_MAX + 0, m_TemporalBuffer[m_TemporalFrameIndex].SRVIndexArray[0][i - 1]);



			DrawScreen((unsigned int)viewport.Width, (unsigned int)viewport.Height);


			SetResourceBarrier(m_GraphicsCommandList.Get(), m_TemporalBuffer[m_TemporalFrameIndex].Resource.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, i);




		}
	}




	{

		m_GraphicsCommandList->SetPipelineState(m_PipelineState["Shrink"].Get());

		D3D12_VIEWPORT viewport = m_SSViewport;
		D3D12_RECT scissorRect = m_SSScissorRect;

		{
			viewport.Width = 4;
			viewport.Height = 4;

			scissorRect.right = 4;
			scissorRect.bottom = 4;

			m_GraphicsCommandList->RSSetViewports(1, &viewport);
			m_GraphicsCommandList->RSSetScissorRects(1, &scissorRect);


			SetResourceBarrier(m_GraphicsCommandList.Get(), m_ExposureBuffer.Resource.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);


			m_GraphicsCommandList->OMSetRenderTargets(1, &m_ExposureBuffer.RTVHandle, false, nullptr);


			FLOAT clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
			//m_GraphicsCommandList->ClearDepthStencilView(m_DepthBufferHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
			//m_GraphicsCommandList->ClearRenderTargetView(handle, clearColor, 0, nullptr);

			SetGraphicsRootDescriptorTable(RenderManager::CBV_REGISTER_MAX + 0, m_TemporalBuffer[m_TemporalFrameIndex].SRVIndexArray[0][9]);



			DrawScreen((unsigned int)viewport.Width, (unsigned int)viewport.Height, 0.05f);


			SetResourceBarrier(m_GraphicsCommandList.Get(), m_ExposureBuffer.Resource.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

		}
	}


}







void RenderManager::DrawShrinkBegin()
{


	{
		D3D12_VIEWPORT viewport = m_SSViewport;
		D3D12_RECT scissorRect = m_SSScissorRect;

		viewport.Width /= 4;
		viewport.Height /= 4;

		scissorRect.right /= 4;
		scissorRect.bottom /= 4;


		m_GraphicsCommandList->RSSetViewports(1, &viewport);
		m_GraphicsCommandList->RSSetScissorRects(1, &scissorRect);


		SetResourceBarrier(m_GraphicsCommandList.Get(), m_ShrinkBuffer.Resource.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);


		D3D12_CPU_DESCRIPTOR_HANDLE rt[2] =
		{
			m_ShrinkBuffer.RTVHandle,
		};
		m_GraphicsCommandList->OMSetRenderTargets(1, rt, false, nullptr);


		FLOAT clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		m_GraphicsCommandList->ClearRenderTargetView(m_ShrinkBuffer.RTVHandle, clearColor, 0, nullptr);




		SetGraphicsRootDescriptorTable(RenderManager::CBV_REGISTER_MAX + 0, m_EnvBuffer.SRVIndex);
		//SetGraphicsRootDescriptorTable(5, m_HDRBuffer.SRVIndex);
		//SetGraphicsRootDescriptorTable(5, m_IBLBuffer.SRVIndex);
		SetGraphicsRootDescriptorTable(RenderManager::CBV_REGISTER_MAX + 1, m_DepthBufferSRVIndex[m_RTIndex]);
		SetGraphicsRootDescriptorTable(RenderManager::CBV_REGISTER_MAX + 3, m_IBLStaticBuffer.SRVIndex);
		SetGraphicsRootDescriptorTable(RenderManager::CBV_REGISTER_MAX + 5, m_ShadowDepthBufferSRVIndex[0]);
		SetGraphicsRootDescriptorTable(RenderManager::CBV_REGISTER_MAX + 6, m_ShadowDepthBufferSRVIndex[1]);
		SetGraphicsRootDescriptorTable(RenderManager::CBV_REGISTER_MAX + 7, m_ShadowDepthBufferSRVIndex[2]);

	}



}




void RenderManager::DrawShrinkEnd()
{


	{

		SetResourceBarrier(m_GraphicsCommandList.Get(), m_ShrinkBuffer.Resource.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	}


}






void RenderManager::DrawPost()
{


	{
		D3D12_VIEWPORT viewport = m_Viewport;
		D3D12_RECT scissorRect = m_ScissorRect;


		m_GraphicsCommandList->RSSetViewports(1, &viewport);
		m_GraphicsCommandList->RSSetScissorRects(1, &scissorRect);



		SetResourceBarrier(m_GraphicsCommandList.Get(), m_RenderTarget[m_RTIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

		m_GraphicsCommandList->OMSetRenderTargets(1, &m_RenderTargetHandle[m_RTIndex], TRUE, nullptr);


		FLOAT clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		//m_GraphicsCommandList->ClearDepthStencilView(m_DepthBufferHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
		//m_GraphicsCommandList->ClearRenderTargetView(m_RenderTargetHandle[m_RTIndex], clearColor, 0, nullptr);

		m_GraphicsCommandList->SetPipelineState(m_PipelineState["Post"].Get());


		SetGraphicsRootDescriptorTable(RenderManager::CBV_REGISTER_MAX + 0, m_TemporalBuffer[m_TemporalFrameIndex].SRVIndex);
		SetGraphicsRootDescriptorTable(RenderManager::CBV_REGISTER_MAX + 1, m_VelocityBuffer.SRVIndex);
		SetGraphicsRootDescriptorTable(RenderManager::CBV_REGISTER_MAX + 2, m_DepthBufferSRVIndex[m_RTIndex]);
		SetGraphicsRootDescriptorTable(RenderManager::CBV_REGISTER_MAX + 3, m_ShrinkBuffer.SRVIndex);
		SetGraphicsRootDescriptorTable(RenderManager::CBV_REGISTER_MAX + 4, m_ExposureBuffer.SRVIndex);


		DrawScreen((unsigned int)viewport.Width, (unsigned int)viewport.Height);

	}

}





void RenderManager::DrawUI(bool Debug)
{
	HRESULT hr;


	{
		if (Debug)
		{
			//ImGui::ShowDemoWindow();
			
			
			ImGui::Begin("Renderer");

			ImGui::Text(m_CpuName.c_str());
			ImGui::Text(m_GpuName.c_str());

			ImGui::NewLine();

			//ImGui::Text("SRVDescriptor %d", m_SRVDescriptorMax - m_SRVDescriptorPool.size());
			//ImGui::Text("RTVDescriptor %d", m_RTVDescriptorNum);
			// 
			ImGui::Text("TotalTriangle %.2f M", m_TotalIndexCount / 3.0f / 1000000.0f);


			{
				LARGE_INTEGER qpf;
				LARGE_INTEGER qpc;

				QueryPerformanceFrequency(&qpf);
				QueryPerformanceCounter(&qpc);
				LONGLONG time = qpc.QuadPart;
				static LONGLONG lastTime;

				float drawTime = (float)(time - lastTime) / qpf.QuadPart;
				lastTime = time;



				static float value[180];
				for (int i = 0; i < 179; i++)
					value[i] = value[i + 1];

				value[179] = 1.0f / drawTime;


				float at = (value[179] + value[178] + value[177] + value[176] + value[175] + value[174] + value[173] + value[172] + value[171] + value[170]) / 10.0f;
				ImGui::Text("%.1f fps / %.1f ms", at, 1000.0f / at);

				ImGui::PlotLines("", value, sizeof(value) / sizeof(float), 0, NULL, 0.0f, 100.0f, ImVec2(0, 50));

			}



			{
				static float value[180];
				for (int i = 0; i < 179; i++)
					value[i] = value[i + 1];

				value[179] = m_CommandTime[0].Time;


				float at = (value[179] + value[178] + value[177] + value[176] + value[175] + value[174] + value[173] + value[172] + value[171] + value[170]) / 10.0f;
				ImGui::Text("Draw %.1f ms", at);

				ImGui::PlotLines("", value, sizeof(value) / sizeof(float), 0, NULL, 0.0f, 50.0f, ImVec2(0, 50));

			}

			
			float total = m_CommandTime[0].Time;

			for (int i = 1; i < m_CommandTime.size(); i++)
			{
				char buf[32]{};
				sprintf(buf, "%s %.1f ms", m_CommandTime[i].Name.c_str(), m_CommandTime[i].Time);
				ImGui::ProgressBar(m_CommandTime[i].Time / total, ImVec2(0, 16), buf);

			}



			ImGui::End();





			ImGui::Begin("Image Buffer");

			ImGui::Text("ShadowDepthBuffer[0]");
			ImGui::Image((void*)GetGpuDescriptorHandle(m_ShadowDepthBufferSRVIndex[0]).ptr, ImVec2(256.0f, 256.0f));

			ImGui::Text("ShadowDepthBuffer[1]");
			ImGui::Image((void*)GetGpuDescriptorHandle(m_ShadowDepthBufferSRVIndex[1]).ptr, ImVec2(256.0f, 256.0f));

			ImGui::Text("ShadowDepthBuffer[2]");
			ImGui::Image((void*)GetGpuDescriptorHandle(m_ShadowDepthBufferSRVIndex[2]).ptr, ImVec2(256.0f, 256.0f));


			for (int i = 0; i < 6; i++)
			{
				ImGui::Text("EnvBuffer[%d]", i);
				ImGui::Image((void*)GetGpuDescriptorHandle(m_EnvBuffer.SRVIndexArray[i][0]).ptr, ImVec2(128.0f, 128.0f));
			}

			ImGui::Text("IBLBuffer");
			ImGui::Image((void*)GetGpuDescriptorHandle(m_IBLBuffer.SRVIndex).ptr, ImVec2(256.0f, 128.0f));

			ImGui::Text("IBLStaticBuffer");
			ImGui::Image((void*)GetGpuDescriptorHandle(m_IBLStaticBuffer.SRVIndex).ptr, ImVec2(256.0f, 128.0f));


			ImGui::Text("HDRBuffer");
			ImGui::Image((void*)GetGpuDescriptorHandle(m_HDRBuffer.SRVIndex).ptr, ImVec2(256.0f, 128.0f));

			for (int i = 0; i < 10; i++)
			{
				ImGui::Text("TAABuffer[%d]", i);
				ImGui::Image((void*)GetGpuDescriptorHandle(m_TemporalBuffer[m_TemporalFrameIndex].SRVIndexArray[0][i]).ptr, ImVec2(256.0f, 128.0f));
			}


			ImGui::Text("ExposureBuffer");
			ImGui::Image((void*)GetGpuDescriptorHandle(m_ExposureBuffer.SRVIndex).ptr, ImVec2(256.0f, 128.0f));

			ImGui::Text("ShrinkBuffer");
			ImGui::Image((void*)GetGpuDescriptorHandle(m_ShrinkBuffer.SRVIndex).ptr, ImVec2(256.0f, 128.0f));

			ImGui::Text("VelocityBuffer");
			ImGui::Image((void*)GetGpuDescriptorHandle(m_VelocityBuffer.SRVIndex).ptr, ImVec2(256.0f, 128.0f));

			ImGui::Text("DepthBuffer[0]");
			ImGui::Image((void*)GetGpuDescriptorHandle(m_DepthBufferSRVIndex[0]).ptr, ImVec2(256.0f, 128.0f));

			ImGui::Text("DepthBuffer[1]");
			ImGui::Image((void*)GetGpuDescriptorHandle(m_DepthBufferSRVIndex[1]).ptr, ImVec2(256.0f, 128.0f));

			ImGui::End();



			//ID3D12DescriptorHeap* dh[] = { m_ImGUISRVDescriptorHeap.Get() };
			//m_GraphicsCommandList->SetDescriptorHeaps(1, dh);
		}
		ImGui::Render();

		if (Debug)
		{
			ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_GraphicsCommandList.Get());
		}


		SetResourceBarrier(m_GraphicsCommandList.Get(), m_RenderTarget[m_RTIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	}





	m_ResourceCommandMutex.lock();

	{
		hr = m_ResourceCommandList->Close();
		assert(SUCCEEDED(hr));


		m_GraphicsCommandList->ResolveQueryData(m_QueryHeap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, 0, m_QueryMax * 2, m_QueryBuffer.Get(), 0);

		hr = m_GraphicsCommandList->Close();
		assert(SUCCEEDED(hr));

		ID3D12CommandList* const command_lists[2] = { m_ResourceCommandList.Get(), m_GraphicsCommandList.Get() };
		m_CommandQueue->ExecuteCommandLists(2, command_lists);
	}




	m_CommandQueue->Signal(m_Fence.Get(), m_Frame[m_RTIndex]);



	hr = m_SwapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING);
	assert(SUCCEEDED(hr));




	UINT64 frame = m_Frame[m_RTIndex];

	m_RTIndex = m_SwapChain->GetCurrentBackBufferIndex();


	if (m_Fence->GetCompletedValue() < m_Frame[m_RTIndex])
	{
		m_Fence->SetEventOnCompletion(m_Frame[m_RTIndex], m_FenceEvent);
		WaitForSingleObjectEx(m_FenceEvent, INFINITE, FALSE);
	}

	m_Frame[m_RTIndex] = frame + 1;


	//Sleep(100);






	{
		void* ptr{};
		m_QueryBuffer->Map(0, nullptr, &ptr);

		UINT64* data = (UINT64*)ptr;

		for(int i = 0; i < m_CommandTime.size(); i++)
		{
			m_CommandTime[i].Time = (data[i * 2 + 1] - data[i * 2]) * 1000.0f / m_Gpufreq;
		}

		m_QueryBuffer->Unmap(0, nullptr);
	}






	hr = m_GraphicsCommandAllocator[m_RTIndex]->Reset();
	assert(SUCCEEDED(hr));

	hr = m_GraphicsCommandList->Reset(m_GraphicsCommandAllocator[m_RTIndex].Get(), nullptr);
	assert(SUCCEEDED(hr));




	hr = m_ResourceCommandAllocator[m_RTIndex]->Reset();
	assert(SUCCEEDED(hr));

	hr = m_ResourceCommandList->Reset(m_ResourceCommandAllocator[m_RTIndex].Get(), nullptr);
	assert(SUCCEEDED(hr));


	m_ResourceCommandMutex.unlock();
}





void RenderManager::SetResourceBarrier(ID3D12GraphicsCommandList* CommandList, ID3D12Resource* Resource, D3D12_RESOURCE_STATES BeforeState, D3D12_RESOURCE_STATES AfterState, unsigned int Subresource)
{
	D3D12_RESOURCE_BARRIER resourceBarrier{};

	resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	resourceBarrier.Transition.pResource = Resource;
	resourceBarrier.Transition.Subresource = Subresource;
	resourceBarrier.Transition.StateBefore = BeforeState;
	resourceBarrier.Transition.StateAfter = AfterState;

	CommandList->ResourceBarrier(1, &resourceBarrier);

}





ComPtr<ID3D12Resource> RenderManager::CreateConstantResource(unsigned int Size)
{

	D3D12_HEAP_PROPERTIES properties{};
	properties.Type = D3D12_HEAP_TYPE_UPLOAD;
	properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	properties.CreationNodeMask = 0;
	properties.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC desc{};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Width = Size;

	ComPtr<ID3D12Resource> resource;

	HRESULT hr = m_Device->CreateCommittedResource(&properties,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));


	return resource;
}





ComPtr<ID3D12Resource> RenderManager::CreateTextureResource(unsigned int Width, unsigned int Height, DXGI_FORMAT Format, D3D12_RESOURCE_FLAGS Flags, unsigned int MipLeve, unsigned int ArraySize, const float* ClearColor)
{

	D3D12_HEAP_PROPERTIES properties{};
	properties.Type = D3D12_HEAP_TYPE_DEFAULT;
	properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	properties.CreationNodeMask = 0;
	properties.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC desc{};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Width = Width;
	desc.Height = Height;
	desc.DepthOrArraySize = ArraySize;
	desc.MipLevels = MipLeve;
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Flags = Flags;
	desc.Format = Format;

	D3D12_CLEAR_VALUE clearValue{};

	if (ClearColor)
	{
		clearValue.Color[0] = ClearColor[0];
		clearValue.Color[1] = ClearColor[1];
		clearValue.Color[2] = ClearColor[2];
		clearValue.Color[3] = ClearColor[3];
	}
	else
	{
		clearValue.Color[0] = 0.0f;
		clearValue.Color[1] = 0.0f;
		clearValue.Color[2] = 0.0f;
		clearValue.Color[3] = 1.0f;
	}
	clearValue.Format = Format;

	ComPtr<ID3D12Resource> resource;

	HRESULT hr = m_Device->CreateCommittedResource(&properties,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		&clearValue,
		IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));


	return resource;
}





std::unique_ptr<TEXTURE> RenderManager::LoadTexture(const char* FileName)
{
	std::unique_ptr<TEXTURE> resource = std::make_unique<TEXTURE>();

	std::unique_ptr<uint8_t[]> ddsData;
	std::vector<D3D12_SUBRESOURCE_DATA> subresouceData;

	wchar_t wFileName[MAX_PATH];
	size_t size;
	mbstowcs_s(&size, wFileName, FileName, MAX_PATH);

	HRESULT hr = LoadDDSTextureFromFile(m_Device.Get(), wFileName, &resource->Resource, ddsData, subresouceData);
	//assert(SUCCEEDED(hr));
	if (FAILED(hr))
	{
		resource->Resource = nullptr;
		resource->ShaderResourceView.Index = 0;
		return resource;
	}

	resource->Resource->SetName(wFileName);



	D3D12_RESOURCE_DESC desc = resource->Resource->GetDesc();

	unsigned int bpp, block;

	if (desc.Format == DXGI_FORMAT_BC1_UNORM)
	{
		bpp = 4;
		block = 4;
	}
	else if (desc.Format == DXGI_FORMAT_BC6H_UF16)
	{
		bpp = 8;
		block = 4;
	}
	else
	{
		bpp = 32;
		block = 1;
	}
	




	//ResourceCommandBegin();
	{
		//std::lock_guard<std::mutex> lock(m_ResourceCommandMutex);

		for (unsigned int a = 0; a < desc.DepthOrArraySize; a++)
		{
			for (unsigned int m = 0; m < desc.MipLevels; m++)
			{
				unsigned int s = a * desc.MipLevels + m;

				unsigned int width = (unsigned int)(subresouceData[s].RowPitch * 8 / bpp / block);
				unsigned int height = (unsigned int)(subresouceData[s].SlicePitch / subresouceData[s].RowPitch * block);

				D3D12_BOX box = { 0, 0, 0, width, height, 1 };

				hr = resource->Resource->WriteToSubresource(s, &box, subresouceData[s].pData, (unsigned int)subresouceData[s].RowPitch, (unsigned int)subresouceData[s].SlicePitch);
				assert(SUCCEEDED(hr));
			}
		}



		SetResourceBarrier(m_ResourceCommandList.Get(), resource->Resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	}
	//ResourceCommandEnd();




	if(desc.DepthOrArraySize > 1)
		resource->ShaderResourceView.Index = CreateShaderResourceViewCube(resource->Resource.Get());
	else
		resource->ShaderResourceView.Index = CreateShaderResourceView(resource->Resource.Get());



	return std::move(resource);
}





std::unique_ptr<CONSTANT_BUFFER> RenderManager::CreateConstantBuffer(unsigned int Size)
{
	std::unique_ptr<CONSTANT_BUFFER> constantBuffer = std::make_unique<CONSTANT_BUFFER>();

	constantBuffer->Resource = RenderManager::GetInstance()->CreateConstantResource(Size);
	constantBuffer->ShaderResourceView.Index = RenderManager::GetInstance()->CreateConstantBufferView(constantBuffer->Resource.Get(), 0, Size);

	return std::move(constantBuffer);
}







ComPtr<ID3D12PipelineState> RenderManager::CreatePipeline(const char* VertexShaderFile, const char* PixelShaderFile,
	const DXGI_FORMAT* RTVFormats, unsigned int NumRenderTargets, D3D12_RENDER_TARGET_BLEND_DESC BlendDesc, bool DepthEnable, bool AlphaCovEnable, bool CullEnable, int DepthBias)
{

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc{};


	std::vector<char> vertexShader;
	std::vector<char> pixelShader;


	{
		std::ifstream file(VertexShaderFile, std::ios_base::in | std::ios_base::binary);
		assert(file);

		file.seekg(0, std::ios_base::end);
		int filesize = (int)file.tellg();
		file.seekg(0, std::ios_base::beg);

		vertexShader.resize(filesize);
		file.read(&vertexShader[0], filesize);

		file.close();


		pipelineStateDesc.VS.pShaderBytecode = &vertexShader[0];
		pipelineStateDesc.VS.BytecodeLength = filesize;
	}


	{
		std::ifstream file(PixelShaderFile, std::ios_base::in | std::ios_base::binary);
		assert(file);

		file.seekg(0, std::ios_base::end);
		int filesize = (int)file.tellg();
		file.seekg(0, std::ios_base::beg);

		pixelShader.resize(filesize);
		file.read(&pixelShader[0], filesize);

		file.close();


		pipelineStateDesc.PS.pShaderBytecode = &pixelShader[0];
		pipelineStateDesc.PS.BytecodeLength = filesize;
	}


	D3D12_INPUT_ELEMENT_DESC InputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0,  0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,    0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		//{ "OCCLUSION",    0, DXGI_FORMAT_R32_FLOAT,    0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};
	pipelineStateDesc.InputLayout.pInputElementDescs = InputElementDesc;
	pipelineStateDesc.InputLayout.NumElements = _countof(InputElementDesc);



	pipelineStateDesc.SampleDesc.Count = 1;
	pipelineStateDesc.SampleDesc.Quality = 0;
	pipelineStateDesc.SampleMask = UINT_MAX;



	pipelineStateDesc.NumRenderTargets = NumRenderTargets;

	if (NumRenderTargets == 0)
	{
		pipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
		pipelineStateDesc.PS.pShaderBytecode = nullptr;
		pipelineStateDesc.PS.BytecodeLength = 0;

	}
	else
	{
		for (unsigned int i = 0; i < NumRenderTargets; i++)
			pipelineStateDesc.RTVFormats[i] = RTVFormats[i];
	}


	pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	pipelineStateDesc.pRootSignature = m_RootSignature.Get();


	if(CullEnable)
		pipelineStateDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	else
		pipelineStateDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

	pipelineStateDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	pipelineStateDesc.RasterizerState.FrontCounterClockwise = FALSE;
	pipelineStateDesc.RasterizerState.DepthBias = DepthBias;
	pipelineStateDesc.RasterizerState.DepthBiasClamp = 0;
	pipelineStateDesc.RasterizerState.SlopeScaledDepthBias = 0;
	pipelineStateDesc.RasterizerState.DepthClipEnable = FALSE;
	pipelineStateDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	pipelineStateDesc.RasterizerState.AntialiasedLineEnable = FALSE;
	pipelineStateDesc.RasterizerState.MultisampleEnable = FALSE;



	for (int i = 0; i < _countof(pipelineStateDesc.BlendState.RenderTarget); ++i)
	{
		pipelineStateDesc.BlendState.RenderTarget[i] = BlendDesc;
		
		BlendDesc.BlendEnable = FALSE;
		BlendDesc.SrcBlend = D3D12_BLEND_ONE;
		BlendDesc.DestBlend = D3D12_BLEND_ZERO;
		BlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
		BlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		BlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
		BlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		BlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		BlendDesc.LogicOpEnable = FALSE;
		BlendDesc.LogicOp = D3D12_LOGIC_OP_CLEAR;
		
	}
	pipelineStateDesc.BlendState.AlphaToCoverageEnable = AlphaCovEnable;
	pipelineStateDesc.BlendState.IndependentBlendEnable = FALSE;



	pipelineStateDesc.DepthStencilState.DepthEnable = DepthEnable;
	pipelineStateDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	pipelineStateDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	pipelineStateDesc.DepthStencilState.StencilEnable = FALSE;
	pipelineStateDesc.DepthStencilState.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
	pipelineStateDesc.DepthStencilState.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;

	pipelineStateDesc.DepthStencilState.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	pipelineStateDesc.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	pipelineStateDesc.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	pipelineStateDesc.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	pipelineStateDesc.DepthStencilState.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	pipelineStateDesc.DepthStencilState.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	pipelineStateDesc.DepthStencilState.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	pipelineStateDesc.DepthStencilState.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	pipelineStateDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;




	ComPtr<ID3D12PipelineState> pipelineState;
	HRESULT hr = m_Device->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(hr));


	return pipelineState;
}



unsigned int RenderManager::CreateConstantBufferView(ID3D12Resource* Resource, unsigned int Offset, unsigned int Size)
{

	unsigned int index = m_SRVDescriptorPool.front();
	m_SRVDescriptorPool.pop_front();




	D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
	desc.BufferLocation = Resource->GetGPUVirtualAddress() + Offset;
	desc.SizeInBytes = Size;



	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_SRVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	unsigned int size = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	handle.ptr += size * index;

	m_Device->CreateConstantBufferView(&desc, handle);


	return index;
}




unsigned int RenderManager::CreateShaderResourceView(ID3D12Resource* Resource)
{

	unsigned int index = m_SRVDescriptorPool.front();
	m_SRVDescriptorPool.pop_front();



	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_SRVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	unsigned int size = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	handle.ptr += size * index;

	D3D12_RESOURCE_DESC resDesc = Resource->GetDesc();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = resDesc.MipLevels;


	if (srvDesc.Format == DXGI_FORMAT_R32_TYPELESS)
	{
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		//srvDesc.Texture2D.MipLevels = 1;
	}


	m_Device->CreateShaderResourceView(Resource, &srvDesc, handle);


	return index;
}



unsigned int RenderManager::CreateShaderResourceViewCube(ID3D12Resource* Resource)
{

	unsigned int index = m_SRVDescriptorPool.front();
	m_SRVDescriptorPool.pop_front();



	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_SRVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	unsigned int size = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	handle.ptr += size * index;

	D3D12_RESOURCE_DESC resDesc = Resource->GetDesc();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MipLevels = resDesc.MipLevels;


	m_Device->CreateShaderResourceView(Resource, &srvDesc, handle);


	return index;
}



unsigned int RenderManager::CreateShaderResourceViewArray(ID3D12Resource* Resource, unsigned int MipLevel, unsigned int Index)
{

	unsigned int index = m_SRVDescriptorPool.front();
	m_SRVDescriptorPool.pop_front();



	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_SRVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	unsigned int size = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	handle.ptr += size * index;

	D3D12_RESOURCE_DESC resDesc = Resource->GetDesc();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = resDesc.Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.MostDetailedMip = MipLevel;
	srvDesc.Texture2DArray.MipLevels = 1;
	srvDesc.Texture2DArray.FirstArraySlice = Index;
	srvDesc.Texture2DArray.ArraySize = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	m_Device->CreateShaderResourceView(Resource, &srvDesc, handle);


	return index;
}




void RenderManager::ReleaseShaderResourceView(unsigned int SRVIndex)
{
	m_SRVDescriptorPool.push_front(SRVIndex);
}





D3D12_CPU_DESCRIPTOR_HANDLE RenderManager::CreateRenderTargetView(ID3D12Resource* Resource, unsigned int MipLevel)
{
	assert(m_RTVDescriptorNum < m_RTVDescriptorMax);

	unsigned int index = m_RTVDescriptorNum;
	m_RTVDescriptorNum++;



	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	unsigned int size = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	handle.ptr += size * index;


	m_Device->CreateRenderTargetView(Resource, nullptr, handle);


	return handle;
}


D3D12_CPU_DESCRIPTOR_HANDLE RenderManager::CreateRenderTargetViewArray(ID3D12Resource* Resource, unsigned int MipLevel, unsigned int Index)
{
	assert(m_RTVDescriptorNum < m_RTVDescriptorMax);

	unsigned int index = m_RTVDescriptorNum;
	m_RTVDescriptorNum++;



	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	unsigned int size = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	handle.ptr += size * index;



	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = Resource->GetDesc().Format;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION::D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
	rtvDesc.Texture2DArray.MipSlice = MipLevel;
	rtvDesc.Texture2DArray.FirstArraySlice = Index;
	rtvDesc.Texture2DArray.ArraySize = 1;

	m_Device->CreateRenderTargetView(Resource, &rtvDesc, handle);


	return handle;
}



void RenderManager::SetGraphicsRootDescriptorTable(unsigned int RootParameterIndex, unsigned int ViewIndex)
{
	if (ViewIndex == 0)
		ViewIndex = m_WhiteTexture->ShaderResourceView.Index;

	D3D12_GPU_DESCRIPTOR_HANDLE handle = m_SRVDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	unsigned int size = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	handle.ptr += size * ViewIndex;

	m_GraphicsCommandList->SetGraphicsRootDescriptorTable(RootParameterIndex, handle);
}



D3D12_GPU_DESCRIPTOR_HANDLE RenderManager::GetGpuDescriptorHandle(unsigned int ViewIndex)
{
	if (ViewIndex == 0)
		ViewIndex = m_WhiteTexture->ShaderResourceView.Index;

	D3D12_GPU_DESCRIPTOR_HANDLE handle = m_SRVDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	unsigned int size = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	handle.ptr += size * ViewIndex;

	return handle;
}



void RenderManager::SetConstant(unsigned int RootParameterIndex, void* Constant, unsigned int Size)
{
	assert(m_ConstantBufferIndex[m_RTIndex] < CONSTANT_BUFFER_SIZE);

	memcpy(m_ConstantBufferPointer[m_RTIndex] + 512 * m_ConstantBufferIndex[m_RTIndex], Constant, Size);

	SetGraphicsRootDescriptorTable(RootParameterIndex, m_ConstantBufferView[m_RTIndex][m_ConstantBufferIndex[m_RTIndex]]);

	m_ConstantBufferIndex[m_RTIndex]++;
}


void RenderManager::DrawScreen(unsigned int Width, unsigned int Height, float Alpha)
{

	Matrix44 world = Matrix44::Identity();

	OBJECT_CONSTANT constant;
	constant.WVP = Matrix44::Transpose(world);
	constant.World = Matrix44::Transpose(world);
	constant.Param = { (float)Width, (float)Height, 0.0f, Alpha };
	SetConstant(2, &constant, sizeof(constant));


	D3D12_VERTEX_BUFFER_VIEW vertexView{};
	vertexView.BufferLocation = m_SpriteVertexBuffer->GetGPUVirtualAddress();
	vertexView.StrideInBytes = sizeof(VERTEX_3D);
	vertexView.SizeInBytes = sizeof(VERTEX_3D) * 4;
	m_GraphicsCommandList->IASetVertexBuffers(0, 1, &vertexView);


	m_GraphicsCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	m_GraphicsCommandList->DrawInstanced(4, 1, 0, 0);
}




void RenderManager::SetPipelineState(const char* PiplineName)
{
	m_CurPipelineState = m_PipelineState[PiplineName].Get();
	assert(m_CurPipelineState);

	m_GraphicsCommandList->SetPipelineState(m_CurPipelineState);
}


void RenderManager::SetPipelineState(ID3D12PipelineState* PiplineState)
{
	m_CurPipelineState = PiplineState;
	m_GraphicsCommandList->SetPipelineState(m_CurPipelineState);
}


ID3D12PipelineState* RenderManager::GetPipelineState()
{
	return m_CurPipelineState;
}





void RenderManager::DrawSprite(Vector3 Position, Vector3 Size, Vector3 Rotation, Vector4 Color, Vector2 TexPosition, Vector2 TexSize)
{

	Matrix44 world = Matrix44::Identity();
	world *= Matrix44::Scale(Size.X, Size.Y, Size.Z);
	world *= Matrix44::RotationXYZ(Rotation.X, Rotation.Y, Rotation.Z);
	world *= Matrix44::TranslateXYZ(Position.X, Position.Y, Position.Z);

	Matrix44 projection = Matrix44::OrthoCenter(1920.0f, 1080.0f, 1.0f);


	OBJECT_CONSTANT constant{};
	constant.WVP = Matrix44::Transpose(world * projection);
	constant.World = Matrix44::Transpose(world);
	constant.Param = Vector4{ TexPosition.X, TexPosition.Y, TexSize.X, TexSize.Y };
	SetConstant(2, &constant, sizeof(constant));


	D3D12_VERTEX_BUFFER_VIEW vertexView{};
	vertexView.BufferLocation = m_SpriteVertexBuffer->GetGPUVirtualAddress();
	vertexView.StrideInBytes = sizeof(VERTEX_3D);
	vertexView.SizeInBytes = sizeof(VERTEX_3D) * 4;
	m_GraphicsCommandList->IASetVertexBuffers(0, 1, &vertexView);


	MATERIAL material{};
	material.BaseColor = Color;
	SetConstant(3, &material, sizeof(material));


	m_GraphicsCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);


	m_GraphicsCommandList->DrawInstanced(4, 1, 0, 0);

}



void RenderManager::StartCommandTime(const char* Name)
{

	int index = -1;
	
	for (int i = 0; i < m_CommandTime.size(); i++)
	{
		if (m_CommandTime[i].Name == Name)
		{
			index = i;
			break;
		}
	}

	if (index == -1)
	{
		index = (int)m_CommandTime.size();

		CommandTime commandTime{};
		commandTime.Name = Name;
		m_CommandTime.push_back(commandTime);
	}

	m_GraphicsCommandList->EndQuery(m_QueryHeap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, index * 2);

}

void RenderManager::EndCommandTime(const char* Name)
{
	int index = -1;

	for (int i = 0; i < m_CommandTime.size(); i++)
	{
		if (m_CommandTime[i].Name == Name)
		{
			index = i;
			break;
		}
	}

	assert(index != -1);

	m_GraphicsCommandList->EndQuery(m_QueryHeap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, index * 2 + 1);

}



SHADER_RESOURCE_VIEW::~SHADER_RESOURCE_VIEW()
{
	if(Index != 0)
		RenderManager::GetInstance()->ReleaseShaderResourceView(Index);
}
