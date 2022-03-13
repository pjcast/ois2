
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "OIS2Logger.h"
#include <string>

using namespace OIS2;

TEST_CASE("ILogger - UseStdLogger")
{
	auto logger = ILogger::UseStdLogger();
	CHECK(logger);
}

// Would be great to use a mocking library - something like Fakeit looks like a possibility. 
class TestLogger : public ILogger
{
public:
	int LogCalled = 0;
	LogLevel LogLevelPassed = None;
	std::string LogMessagePassed;
	
	TestLogger() {}
	virtual ~TestLogger() {}
	virtual void Log(LogLevel logLevel, std::string_view message)
	{
		++LogCalled;
		LogLevelPassed = logLevel;
		LogMessagePassed = message;
	}
};

TEST_CASE("ILogger - Log Behaviors")
{
	std::unique_ptr<TestLogger> logger = std::make_unique<TestLogger>();
	CHECK(logger);

	SUBCASE("Error")
	{
		auto p2 = static_cast<ILogger*>(logger.get());
		p2->Log(ILogger::LogLevel::Error, "{0}-{0}-{1}::f{10}", 34, "34234", " dsf dsf sdf", 1, 2, 3, 4, 5, 6, 7, 8, 9, 10.23);
		CHECK(logger->LogCalled == 1);
		CHECK(logger->LogLevelPassed == ILogger::LogLevel::Error);
		CHECK(logger->LogMessagePassed == "34-34-34234::f8");
	}
	SUBCASE("Warning")
	{
		auto p2 = static_cast<ILogger*>(logger.get());
		p2->Log(ILogger::LogLevel::Warning, "{3};{2};{1};{0}:{10}", 34, "344", 10.23, 23.8f);
		CHECK(logger->LogCalled == 1);
		CHECK(logger->LogLevelPassed == ILogger::LogLevel::Warning);
		CHECK(logger->LogMessagePassed == "23.8;10.23;344;34:<INVALID INDEXER:10>");
	}
	SUBCASE("None")
	{
		auto p2 = static_cast<ILogger*>(logger.get());
		p2->Log(ILogger::LogLevel::None, "{3};{2};{1};{0}:{10}", 34, "344", 10.23, 23.8f);
		CHECK(logger->LogCalled == 0);
	}
}

TEST_CASE("Tokenizer - Parsing") 
{
	// TODO
}