#include "OIS2Tokenizer.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <assert.h>

using namespace OIS2;

Tokenizer::Tokenizer(const std::string_view &originalView)
{
	_originalView = originalView;
	_isOpen = false;
	_currentIndex = 0;
	IndexPosition = 0;
}

Tokenizer::~Tokenizer()
{
}

Tokenizer::TokenState Tokenizer::GetNextToken()
{
	auto sv = _originalView;
	auto str_length = _originalView.length();
	auto last_position = _currentIndex;
	auto position = _currentIndex;
	if (position >= str_length)
		return TokenState::End;

	// _isOpen => looking for }
	//!_isOpen => looking for {. In which case, we then return the string up to that point, and set _isOpen=true

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
