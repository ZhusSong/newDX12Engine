
#include "DX12EngineGeneralSetting.h"
#include "DX12EngineGeneralSettingMarco.h"

struct RI_ENGINESETTING_API FEnginePathHelper
{
	static std::string GetEngineRootPath();

	static std::string GetEngineBinariesPath();

	static std::string GetEngineLogsPath();

	static std::string GetEngineAssetPath();

	static std::wstring GetEngineShadersPath();

	static std::string RelativeToAbsolutePath(const std::string& InPath);
};