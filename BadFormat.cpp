#include <string>

#include "BadFormat.h"

BadFormat::BadFormat(const size_t line_number, const size_t element_number) :
	msg_("Wrong csv format, line: " + std::to_string(line_number) + ", element: " + std::to_string(element_number) + ".")
{}

char const* BadFormat::what() const {
	return msg_.c_str();
}