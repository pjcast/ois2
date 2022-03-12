#pragma once
#include <memory>
#include <string_view>
#include "OIS2.h"

namespace OIS2
{
	//! Type'd variadic type
	class LogArgument
	{
	public:
		enum Type { INT, INT64, DOUBLE, CONST_CHAR_PTR, STRING_VIEW };

	private:
		Type _type;

		union 
		{
			int int_value;
			int64_t int64_value;
			double dbl_value;
			const char *c_str_value;
		} _u;

		std::string_view _sv_value;
	
	public:
		LogArgument(int value) : _type(INT) { _u.int_value = value; }
		LogArgument(int64_t value) : _type(INT64) { _u.int64_value = value; }
		LogArgument(double value) : _type(DOUBLE) { _u.dbl_value = value; }
		LogArgument(const char *value) : _type(CONST_CHAR_PTR) { _u.c_str_value = value; }
		LogArgument(const std::string_view value) : _type(STRING_VIEW) { _sv_value = value; }

		Type GetType() { return _type; }

		inline int GetValueInt() const { ValidateType(INT); return _u.int_value; }
		inline int64_t GetValueInt64() const { ValidateType(INT64); return _u.int64_value; }
		inline double GetValueDouble() const { ValidateType(DOUBLE); return _u.dbl_value; }
		inline const char* GetValueConstCharPtr() const { ValidateType(CONST_CHAR_PTR); return _u.c_str_value; }
		inline const std::string_view& GetValueStringView() const { ValidateType(STRING_VIEW); return _sv_value; }

	private:
		inline void ValidateType(Type type) const
		{
			if (type != _type)
				throw std::runtime_error("Incorrect LogArgument type requested");
		}
	};

	//! Abstract class for providing logging. Any custom overrides must ensure thread safety.
	class ILogger
	{
	public:
		enum LogLevel
		{
			None = 0,
			Trace,
			Debug,
			Warning,
			Error
		};

	protected:
		LogLevel _logLevel;

	public:
		ILogger(LogLevel logLevel = LogLevel::Warning) :
			_logLevel(logLevel)
		{
		}

		virtual ~ILogger() = 0;

		LogLevel GetLogLevel() { return _logLevel; }

		std::string_view GetLogLevelString(LogLevel logLevel)
		{
			switch(logLevel)
			{
				case LogLevel::Debug: return "Debug";
				case LogLevel::Trace: return "Trace";
				case LogLevel::Warning: return "Warn";
				case LogLevel::Error: return "Error";
				default: return "Default";
			}
		}

		inline bool ShouldLog(LogLevel logLevel) 
		{
			return (logLevel >= _logLevel && _logLevel != LogLevel::None);
		}

		//! Basic Log() method for logging single string
		virtual void Log(LogLevel logLevel, std::string_view message) = 0;

		//! Templated variadic Log() method for logging a variable number of params
		//! message params are positional: "Hi {0}, {1}. Repat: {0}"
		template <typename... Args>
		void Log(LogLevel logLevel, std::string_view message, const Args&... args)
		{
			if (!ShouldLog(logLevel))
				return;

			// Convert to a typed array - to be sent to derived classes
			LogArgument argArray[] = {args...};
			LogFormat(logLevel, message, argArray, sizeof...(Args));
		}

		//! Thread Safe, basic stdout/stderr logger.
		static std::unique_ptr<ILogger> UseStdLogger(LogLevel logLevel = LogLevel::Warning);

	protected:
		//! If overriden, you must handle format parsing.
		virtual void LogFormat(LogLevel logLevel, const std::string_view &message, const LogArgument args [], size_t numberOfArgs);

		//! Convert arguments to strings (basic value to string - no special formatting)
		void GetArgStrings(const LogArgument args[], size_t numberOfArgs, std::vector<std::string> &outArgStrings);
	};
}
