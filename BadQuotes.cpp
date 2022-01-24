#include "BadQuotes.h"

BadQuotes::BadQuotes(const size_t line_number, const size_t element_number) :
	error_message_("Error, one of screening symbols does not have pair.\nLine: "
		+ std::to_string(line_number) + " Element: " + std::to_string(element_number))
{}

char const* BadQuotes::what() const {
	return error_message_.c_str();
}