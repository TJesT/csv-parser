#pragma once

#include <exception>
#include <string>

class BadFormat final : public std::exception {
public:
	explicit BadFormat(size_t line_number, size_t element_number);
	[[nodiscard]] char const* what() const override;

private:
	std::string msg_;
};
