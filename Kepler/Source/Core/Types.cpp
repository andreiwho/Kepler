#include "Types.h"

#include <cstdio>

namespace Kepler
{

	std::string ConvertToAnsiString(const std::wstring& WideString)
	{
		std::string OutString;
		OutString.resize(WideString.length());
		CHECK(std::wcstombs(OutString.data(), WideString.data(), OutString.length()));
		return OutString;
	}

	std::wstring ConvertToWideString(const std::string& AnsiString)
	{
		std::wstring OutString;
		OutString.resize(AnsiString.length());
		CHECK(std::mbstowcs(OutString.data(), AnsiString.data(), OutString.length()));
		return OutString;
	}

}