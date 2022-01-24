#pragma once

#include <exception>
#include <string>

class RecordWithUnknownSequence final :
	public std::exception {
public:
	explicit RecordWithUnknownSequence(const std::string& unknown_word, size_t line, size_t elem_number);

	[[nodiscard]] char const* what() const override;

private:
	std::string msg_;
};
