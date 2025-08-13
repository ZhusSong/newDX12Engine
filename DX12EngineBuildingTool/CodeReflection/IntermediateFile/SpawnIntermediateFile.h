#pragma once
// 25.7.15 ¿Ó

#include "../../simple_library/public/simple_library.h"
#include "../CppSourceCodeType.h"

namespace IntermediateFile
{
	bool Builder(
		const FClassAnalysis& InClassAnalysis,
		std::vector<std::string>& OutAnalysisRawH,
		std::vector<std::string>& OutAnalysisRawCPP);
}