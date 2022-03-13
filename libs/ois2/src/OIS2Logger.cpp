#include "OIS2Logger.h"
#include "OIS2Tokenizer.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <assert.h>

using namespace OIS2;

class StdLogger: public ILogger
{
public:
	StdLogger(LogLevel logLevel) : ILogger(logLevel) {}

	virtual ~StdLogger() {}

	virtual void Log(LogLevel logLevel, std::string_view message)
	{
		if (!ShouldLog(logLevel))
			return;

		std::ostringstream prefix;
		prefix << "[" << GetLogLevelString(logLevel) << "] ";

		if (logLevel < LogLevel::Error)
			std::cout << prefix.str() << message << std::endl;
		else
			std::cerr << prefix.str() << message << std::endl;
	}
};

// C++11 - no need for a body, mark it default
ILogger::~ILogger() = default;

void ILogger::GetArgStrings(const LogArgument args[], size_t numberOfArgs, std::vector<std::string> &outArgStrings)
{
	for(size_t i = 0; i < numberOfArgs; ++i)
	{
		auto arg = args[i];
		switch(arg.GetType())
		{
			case LogArgument::Type::INT: 
				outArgStrings.push_back(lexical_cast<std::string>(arg.GetValueInt()));
				break;
			case LogArgument::Type::INT64:
				outArgStrings.push_back(lexical_cast<std::string>(arg.GetValueInt64()));
				break;
			case LogArgument::Type::DOUBLE:
				outArgStrings.push_back(lexical_cast<std::string>(arg.GetValueDouble()));
				break;
			case LogArgument::Type::CONST_CHAR_PTR:
				outArgStrings.push_back(std::string(arg.GetValueConstCharPtr()));
				break;
			case LogArgument::Type::STRING_VIEW:
				outArgStrings.push_back(std::string(arg.GetValueStringView()));
				break;
			default:
				std::cerr << "Unhandled LogArgument.GetType()";
				break;
		}
	}
}

void ILogger::LogFormat(LogLevel logLevel, const std::string_view &message, const LogArgument args [], size_t numberOfArgs)
{
	if (args == nullptr || numberOfArgs == 0 || message.length() == 0)
	{
		Log(logLevel, message);
		return;
	}

	std::ostringstream outputStream;

	std::vector<std::string> argumentStrings;
	GetArgStrings(args, numberOfArgs, argumentStrings);

	// Actually replace {Index} with proper arg value
	size_t position = 0;
	Tokenizer tokenizer(message);
	do
	{
		auto state = tokenizer.GetNextToken();
		if (state == Tokenizer::TokenState::String)
		{
			if (tokenizer.Slice.length() > 0)
				outputStream << tokenizer.Slice;
		}
		else if (state == Tokenizer::TokenState::Index)
		{
			if (tokenizer.IndexPosition >= 0 && tokenizer.IndexPosition < argumentStrings.size())
				outputStream <<  argumentStrings[tokenizer.IndexPosition];
			else
				outputStream << "<INVALID INDEXER:" << tokenizer.IndexPosition << ">";
		}
		else
			break;
	} while (true);

	Log(logLevel, outputStream.str());
}

std::unique_ptr<ILogger> ILogger::UseStdLogger(LogLevel logLevel)
{
	return std::make_unique<StdLogger>(logLevel);
}

