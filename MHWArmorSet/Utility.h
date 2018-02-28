#ifndef UTILITY_H
#define UTILITY_H

#include <vector>
#include <string>
#include <sstream>
#include <atlstr.h>
#include <chrono>

namespace Utility
{
	// split string line by comma
	static std::vector<std::string> splitByComma(const std::string str)
	{
		std::vector<std::string> split;

		std::stringstream ss(str);
		std::string token;

		while (std::getline(ss, token, ','))
		{
			split.push_back(token);
		}

		return split;
	}

	// Build skill description
	//std::vector<std::wstring> buildSkillDescription(const std::wstring& description);

	// Convert string utf to multibyte string
	static std::wstring utf8_decode(const std::string &str)
	{
		if (str.empty()) 
			return std::wstring();

		int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
		std::wstring wstrTo(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
		return wstrTo;
	}

	static const std::string wtos(const std::wstring& wstr)
	{
		if (wstr.empty()) return "";
		int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
		std::string strTo(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
		return strTo;
	}

	// check if pos is in rect
	static bool isPointInRect(RECT& rect, POINT& point)
	{
		return (rect.left <= point.x && point.x <= rect.right && rect.top <= point.y && point.y <= rect.bottom);
	}

	class Time
	{
	public:
		static inline std::chrono::steady_clock::time_point now()
		{
			return std::chrono::steady_clock::now();
		}

		static inline const std::wstring toMicroSecondString(const std::chrono::steady_clock::time_point start, const std::chrono::steady_clock::time_point end, const bool addUnit = true)
		{
			std::wstringstream ss;
			ss << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
			return ss.str() + L"us";
		}

		static inline const std::wstring toMilliSecondString(const std::chrono::steady_clock::time_point start, const std::chrono::steady_clock::time_point end, const bool addUnit = true)
		{
			std::wstringstream ss;
			ss << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
			return ss.str() + L"ms";
		}

		static inline const std::wstring toNanoSecondSTring(const std::chrono::steady_clock::time_point start, const std::chrono::steady_clock::time_point end, const bool addUnit = true)
		{
			std::wstringstream ss;
			ss << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
			return ss.str() + L"ns";
		}
	};
}

#endif