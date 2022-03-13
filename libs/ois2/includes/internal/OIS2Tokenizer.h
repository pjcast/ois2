#pragma once
#include <sstream>
#include <algorithm>

namespace OIS2
{
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

	//! Internal Class utilized for log format parsing
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

		Tokenizer(const std::string_view &originalView);
		~Tokenizer();

		std::string_view Slice;
		int IndexPosition;

		//! Read a string such as " something {0} {1}, {{, }" - and, tokenize it into [" something ", 0, " ", 1, ", {, }"] without modifying original
		TokenState GetNextToken();
	};
}