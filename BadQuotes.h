#pragma once

#include <exception>
#include <string>

class BadQuotes final :
	public std::exception {
public:
	explicit BadQuotes(size_t line_number, size_t element_number);
	[[nodiscard]] char const* what() const override;

private:
	std::string error_message_;
};
