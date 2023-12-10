#pragma once

#define WIN32_LEAN_AND_MEAN 

#include <fstream>
#include <memory>

#include <vector>
#include <list>
#include <deque>
#include <map>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <algorithm>

#include <string>

#include <io.h>
#include <typeinfo>

#include <Windows.h>
#include <mmsystem.h>

#include <d3d12.h>
#include <d3d12shader.h>
#include <dxgi1_4.h>
#include <wrl/client.h>
using namespace Microsoft::WRL;

#include <DirectXMath.h>
using namespace DirectX;

#include <Xinput.h>


#include "MatrixVector.h"


#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx12.h"


#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "d3d12.lib" )
#pragma comment( lib, "dxgi.lib" )
#pragma comment (lib, "xinput.lib")
