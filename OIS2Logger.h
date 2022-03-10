#pragma once
#include <memory>
#include <string_view>
#include "OIS2.h"

namespace OIS2
{
	enum LogLevel
	{
		None = 0,
		Trace,
		Debug,
		Warning,
		Error
	};

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

	//! Abstract class for providing logging
	class ILogger
	{
	protected:
		LogLevel _logLevel;

	public:
		ILogger(LogLevel logLevel = LogLevel::Warning) :
			_logLevel(logLevel)
		{
		}

		virtual ~ILogger() = 0;

		LogLevel GetLogLevel() { return _logLevel; }

		//! Basic Log() method for logging single string
		virtual void Log(LogLevel logLevel, std::string_view message) = 0;

		//! Templated variadic Log() method for logging a variable number of params
		//! message params are positional: "Hi {0}, {1}. Repat: {0}"
		template <typename... Args>
		void Log(LogLevel logLevel, std::string_view message, const Args&... args)
		{
			if (logLevel < _logLevel && _logLevel != LogLevel::None)
				return;

			// Convert to safer array
			LogArgument argArray[] = {args...};
			_Log(logLevel, message, argArray, sizeof...(Args));
		}

		static std::unique_ptr<ILogger> UseNullLogger();
		static std::unique_ptr<ILogger> UseStdLogger(LogLevel logLevel = LogLevel::Warning);

	protected:
		virtual void _Log(LogLevel logLevel, const std::string_view &message, const LogArgument args [], size_t numberOfArgs) = 0;
	};

	//! No-Op logger - should get inlined to have minor performance impact
	class NullLogger: public ILogger
	{
	public:
		NullLogger() : ILogger(LogLevel::None) {}
		virtual ~NullLogger() {}
		virtual void Log(LogLevel logLevel, std::string_view message) {}
	
	protected:
		virtual void _Log(LogLevel logLevel, const std::string_view &message, const LogArgument args [], size_t numberOfArgs) {}
	};
}