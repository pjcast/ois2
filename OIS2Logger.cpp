#include "OIS2Logger.h"
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <assert.h>

using namespace OIS2;

// Borrowed from Boost
template<typename Target, typename Source>
Target lexical_cast(Source arg)
{
	std::stringstream interpreter;
	Target result;

	if(!(interpreter << arg) || !(interpreter >> result) || !(interpreter >> std::ws).eof())
		throw std::runtime_error("Bad Lexical Cast");

	return result;
}

class StdLogger: public ILogger
{
public:
	StdLogger(LogLevel logLevel) : ILogger(logLevel) {}

	virtual ~StdLogger() {}

	const char* GetLogLevelStr(LogLevel logLevel)
	{
		switch(logLevel)
		{
			case LogLevel::Debug: return "[Debug] ";
			case LogLevel::Trace: return "[Trace] ";
			case LogLevel::Warning: return "[Warn] ";
			case LogLevel::Error: return "[Error] ";
			default:
				return "[?] ";
		}
	}

	void GetArgStrings(const LogArgument args[], size_t numberOfArgs, std::vector<std::string> &outArgStrings)
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
					outArgStrings.push_back(lexical_cast<std::string>(arg.GetValueConstCharPtr()));
					break;
				case LogArgument::Type::STRING_VIEW:
					outArgStrings.push_back(lexical_cast<std::string>(arg.GetValueStringView()));
					break;
				default:
					std::cerr << "Unhandled LogArgument.GetType()";
					break;
			}
		}
	}

	virtual void Log(LogLevel logLevel, std::string_view message)
	{
		std::ostringstream outputStream;
		outputStream << GetLogLevelStr(logLevel) << " " << message << std::endl;

		if (logLevel < LogLevel::Error)
			std::cout << outputStream.str();
		else
			std::cerr << outputStream.str();
	}

	class Tokenizer
	{
	private:
		size_t _currentIndex;
		bool _isOpen;
		std::string_view _originalView;

	public:
		enum TokenState { String, Index, End };

		Tokenizer(const Tokenizer&) = delete;
		Tokenizer& operator=(Tokenizer const&) = delete;

		Tokenizer(const std::string_view &originalView)
		{
			_originalView = originalView;
			_isOpen = false;
			_currentIndex = 0;
			IndexPosition = 0;
		}

		std::string_view Slice;
		int IndexPosition;

		//! Read a string such as " something {0} {1}, {{, }" - and, tokenize it into [" something ", 0, " ", 1, ", {, }"] without modifying original
		TokenState GetNextToken()
		{
			auto sv = _originalView;
			auto str_length = _originalView.length();
			auto last_position = _currentIndex;
			auto position = _currentIndex;
			if (position >= str_length)
				return TokenState::End;

			// What are we looking for? 
			// 	_isOpen => looking for }
			//	!_isOpen => looking for {. In which case, we then return the string up to that point, and set _isOpen=true

			if (!_isOpen)
			{
				do
				{
					// Either no {'s found, or we are at the end - and no room for closing } + index #
					auto openTokenPosition = sv.find_first_of('{', position);
					if (openTokenPosition == std::string_view::npos || openTokenPosition > str_length - 3)
					{
						auto length = sv.length() - _currentIndex;
						_currentIndex = sv.length();
						Slice = std::string_view(sv.data() + last_position, length);
						return TokenState::String;
					}

					// See if this has another { after it: e.g. {{. If it does, look for next open brace
					if (sv.at(openTokenPosition + 1) == '{')
					{
						++position;
						continue;
					}

					_isOpen = true;
					auto length = openTokenPosition - _currentIndex;
					_currentIndex = openTokenPosition + 1;
					Slice = std::string_view(sv.data() + last_position, length);
					return TokenState::String;
				} while(true);
			}
			else
			{
				auto openTokenPosition = last_position;
				// Ok, so, find ending } (if we find "{}"" - with no index room, return string up to and including '}')
				auto closedTokenPosition = sv.find_first_of('}', openTokenPosition);
				if (closedTokenPosition == std::string_view::npos || closedTokenPosition - openTokenPosition == 0)
				{
					auto length = sv.length() - closedTokenPosition;
					_currentIndex = closedTokenPosition + 1;
					Slice = std::string_view(sv.data() + last_position, length);
					_isOpen = false;
					return TokenState::String;
				}

				std::string_view indexString(sv.data() + openTokenPosition, closedTokenPosition - openTokenPosition);
				IndexPosition = lexical_cast<int>(indexString);

				_currentIndex = closedTokenPosition + 1;

				_isOpen = false;
				return TokenState::Index;
			}
		}
	};

	virtual void _Log(LogLevel logLevel, const std::string_view &message, const LogArgument args[], size_t numberOfArgs)
	{
		if (args == nullptr || numberOfArgs == 0 || message.length() == 0)
		{
			Log(logLevel, message);
			return;
		}

		std::ostringstream outputStream;
		outputStream << GetLogLevelStr(logLevel) << " ";

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

		outputStream << std::endl;
		auto outputData = outputStream.str();
		
		if (logLevel < LogLevel::Error)
			std::cout << outputData;
		else
			std::cerr << outputData;
	}
};

// C++11 - no need for a body, mark it default
ILogger::~ILogger() = default;

std::unique_ptr<ILogger> ILogger::UseNullLogger()
{
	return std::make_unique<OIS2::NullLogger>();
}

std::unique_ptr<ILogger> ILogger::UseStdLogger(LogLevel logLevel)
{
	return std::make_unique<StdLogger>(logLevel);
}

