#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "OIS2Logger.h"

using namespace OIS2;

TEST_CASE("ILogger - UseStdLogger") {
	auto logger = ILogger::UseStdLogger();
	CHECK(logger);
}

class NeverCalledTestLogger: public ILogger
{
public:
	NeverCalledTestLogger(LogLevel logLevel) : ILogger(logLevel) {}
	virtual ~NeverCalledTestLogger() {}
	virtual void Log(LogLevel logLevel, std::string_view message)
	{
	}

protected:
	inline virtual void _Log(LogLevel logLevel, const std::string_view &message, const LogArgument args [], size_t numberOfArgs)
	{
	}
};
