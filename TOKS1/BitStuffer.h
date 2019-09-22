#pragma once
#include <string_view>
#include <string>
#include <optional>
#include <algorithm>
#include <vector>

enum BitStufferWorkStrategy
{
	CODE,
	DECODE
};

class StringContentException : public std::exception {
public:
	StringContentException(const std::string& description =
		"Wrong string passed")
		:exception(description.c_str()) {}

	const char* what()const override { return this->what(); }
};

class BitStuffer
{
private:
	std::string pattern;

	bool consistsOf(const std::string_view& data, const std::string_view& ofWhat) const {
		return std::all_of(data.cbegin(), data.cend(), [&ofWhat](auto letter) {
			return std::any_of(ofWhat.cbegin(), ofWhat.cend(), [letter](auto ofWhatSymbol) {
				return letter == ofWhatSymbol;
			});
		});
	}

public:

	BitStuffer(const std::string_view& pattern = "01111110") :pattern(pattern.data()) {};

	std::string CodeBytes(const std::string_view& bytes,
		const BitStufferWorkStrategy strategy,
		const std::optional<std::string_view>& pattern = std::nullopt) const 
	{
		const std::string selected_pattern = 
			pattern.has_value() ? pattern.value().data() : this->pattern;
		std::string data = bytes.data();
		std::string pattern_symbols = selected_pattern;
		sort(pattern_symbols.begin(), pattern_symbols.end());
		pattern_symbols.erase(std::unique(pattern_symbols.begin(), pattern_symbols.end()), pattern_symbols.end());

		if (!consistsOf(data, pattern_symbols)) {
			throw StringContentException
			("Exception in CodeBytrs method. Only ones and zeroes containing strings can be passed.");
		}

		size_t pattern_begin_pos = 0;
		size_t pattern_end_pos = 0;

		while(true){
			pattern_begin_pos = data.find(selected_pattern, pattern_end_pos);
			if (pattern_begin_pos == std::string::npos) {
				break;
			}
			pattern_end_pos = pattern_begin_pos + selected_pattern.length();
			if (strategy == BitStufferWorkStrategy::CODE) {
				data.insert(pattern_end_pos-1, "1");
			}
			else {	//strategy == DECODE
				data.erase(pattern_end_pos-1,1);
			}
		} 
		return data;
	}

	~BitStuffer() = default;
};

