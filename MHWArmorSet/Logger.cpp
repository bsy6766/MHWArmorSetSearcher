// pch
#include "stdafx.h"

#include "Logger.h"
#include "Utility.h"

MHW::Logger::Logger()
{
	wchar_t buffer[MAX_PATH];
	GetModuleFileNameW(NULL, (LPWSTR)buffer, MAX_PATH);
	auto pos = std::wstring(buffer).find_last_of(L"\\/");
	workingDirectory = std::wstring(buffer).substr(0, pos);

	std::wstring logFilePath = workingDirectory + L"/log.txt";

	OutputDebugString((L"WD: " + workingDirectory + L"\n").c_str());

	if (PathFileExistsW((LPWSTR)logFilePath.c_str()))
	{
		// file path exstis
		DeleteFileW((LPWSTR)logFilePath.c_str());
	}

	std::wifstream ifs(logFilePath);
	ifs.close();

	logger = spdlog::basic_logger_mt("Log", logFilePath);

	logger->set_pattern("[%P] [%l] [%c] %v");
	logger->info((L"Log file created at " + logFilePath).c_str());
}

void MHW::Logger::info(const std::string & log)
{
	logger->info(log);
}

void MHW::Logger::warn(const std::string & log)
{
	logger->warn(log);
}

void MHW::Logger::error(const std::string & log)
{
	logger->error(log);
}

void MHW::Logger::errorCode(const MHW::ERROR_CODE errCode)
{
	logger->error("Error code: " + std::to_string(static_cast<int>(errCode)));
}

void MHW::Logger::errorCode(const int errCode)
{
	logger->error("Error code: " + std::to_string(errCode));
}

std::wstring MHW::Logger::getWorkingDirectory()
{
	return workingDirectory;
}

void MHW::Logger::flush()
{
	logger->flush();
}