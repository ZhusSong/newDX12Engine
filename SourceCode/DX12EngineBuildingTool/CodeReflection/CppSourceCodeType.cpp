#include "CppSourceCodeType.h"

namespace
{
	std::string StripComments(
		const std::vector<std::string>& InContent)
	{
		std::string Result;
		bool bInBlockComment = false;

		for (const std::string& Line : InContent)
		{
			for (size_t Index = 0; Index < Line.size(); ++Index)
			{
				if (bInBlockComment)
				{
					if (Index + 1 < Line.size() &&
						Line[Index] == '*' &&
						Line[Index + 1] == '/')
					{
						bInBlockComment = false;
						++Index;
					}

					continue;
				}

				if (Index + 1 < Line.size())
				{
					if (Line[Index] == '/' && Line[Index + 1] == '/')
					{
						break;
					}

					if (Line[Index] == '/' && Line[Index + 1] == '*')
					{
						bInBlockComment = true;
						++Index;
						continue;
					}
				}

				Result.push_back(Line[Index]);
			}

			Result.push_back('\n');
		}

		return Result;
	}
}

bool IsCheckAllowCodeReflection(const std::vector<std::string>& InContent)
{
	const std::string ClearContent = StripComments(InContent);
	const bool bHasCodeReflectionInclude =
		simple_cpp_string_algorithm::string_contain(
			ClearContent, ".CodeReflection.h");
	const bool bHasCodeReflectionMacro =
		simple_cpp_string_algorithm::string_contain(
			ClearContent, "CODEREFLECTION(");

	return bHasCodeReflectionInclude && bHasCodeReflectionMacro;
}
