#ifndef LOGGER_H
#define LOGGER_H

// cpp
#include <memory>
#include <string>
#include <spdlog\spdlog.h>

#include "GlobalEnums.h"
#include "ErrorCode.h"

// foward delcaration
namespace spdlog
{
	class logger;
}

namespace MHW
{
	class Logger
	{
	private:
		// Constructor
		Logger();

		// Destructor
		~Logger() = default;

		// working directory
		std::wstring workingDirectory;

		// basic logger
		std::shared_ptr<spdlog::logger> logger;

	public:
		static Logger& getInstance()
		{
			static Logger instance;
			return instance;
		}

		Logger(Logger const&) = delete;             // Copy construct
		Logger(Logger&&) = delete;                  // Move construct
		Logger& operator=(Logger const&) = delete;  // Copy assign
		Logger& operator=(Logger &&) = delete;      // Move assign

													// log info
		void info(const std::string& log);
		void infoW(const std::wstring& log);

		// log warn
		void warn(const std::string& log);

		// log error
		void error(const std::string& log);
		void errorCode(const MHW::ERROR_CODE errCode);
		void errorCode(const int errCode);

		// get wd
		std::wstring getWorkingDirectory();

		// flush
		void flush();
	};
}

#endif