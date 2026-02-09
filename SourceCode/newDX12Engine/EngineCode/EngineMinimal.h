#pragma once
// 项目基础设置

#define USE_SIMPLE_LIB_MATH 0
#define CLOSE_DISABLE_DEBUG_INFOR 

// 是否使用第三方库(FBX)
#define THIRD_PARTY_LIBRARY 1

// 是否使用编辑器引擎
#define EDITOR_ENGINE 1

// 定义窗口坐标
#define WINDOWS_LOCATION_X 0
#define WINDOWS_LOCATION_Y 0


// 定义了需要加载的头文件
#include "simple_library/public/simple_library.h"
#pragma comment(lib, "EngineSimpleLibrary.lib")

// 链接EngineGeneralSetting与EngineCoreObject
#include "Core/Path/EnginePath.h"
#pragma comment(lib, "DX12EngineGeneralSetting.lib")
#pragma comment(lib, "DX12EngineCoreObject.lib")

#include "Debug/Log/SimpleLog.h"

#include "../imgui/imgui.h"
#include "../imgui/backends/imgui_impl_win32.h"
#include "../imgui/backends/imgui_impl_dx12.h"

#include <d3d12.h>
#include <tchar.h>

#include <vector>
#include <sstream>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

#include <wrl.h>
#include <dxgi1_4.h>
#include "Rendering/DirectX12/d3dx12.h"
#include "Rendering/DirectX12/DDSTextureLoader.h"
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include "Debug/EngineDebug.h"



using namespace Microsoft::WRL;
using namespace std;
using namespace DirectX;
using namespace DirectX::PackedVector;


extern class CEngine* Engine;

