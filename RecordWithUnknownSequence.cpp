#include "RecordWithUnknownSequence.h"

RecordWithUnknownSequence::RecordWithUnknownSequence(const std::string& unknown_word,
	const size_t line,
	const size_t elem_number) :
	msg_("Error, unknown rule sequence: " 
		+ unknown_word + "\n" 
		+ "Line: " + std::to_string(line) + " Element: " + std::to_string(elem_number) + "\n")
{}

char const* RecordWithUnknownSequence::what() const {
	return msg_.c_str();
}