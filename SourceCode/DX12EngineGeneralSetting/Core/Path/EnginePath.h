#pragma once
#include "DX12EngineGeneralSetting.h"
#include "DX12EngineGeneralSettingMarco.h"

struct RI_ENGINESETTING_API FEnginePathHelper
{
	static std::string GetEngineRootPath();

	static std::string GetEngineBinariesPath();

	static std::string GetEngineLogsPath();

	// 引擎源代码路径
	// エンジンソースコードパス
	static std::string GetEngineSourcePath();

	static std::string GetEngineIntermediatePath();

	// 引擎反射代码路径
	// エンジンリフレクションコードパス
	static std::string GetEngineCodeReflectionPath();

	static std::string GetEngineAssetPath();

	static std::wstring GetEngineShadersPath();

	static std::string RelativeToAbsolutePath(const std::string& InPath);
};