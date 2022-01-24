#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>

#include "AttemptToDereferenceEndIterator.h"
#include "BadFormat.h"
#include "BadQuotes.h"
#include "RecordWithUnknownSequence.h"

constexpr char LINE_DELIMITER	 = '\n';
constexpr char ELEMENT_DELIMITER = ',';
constexpr char SCREEN_GUARD		 = '"';

const std::unordered_map<std::string, char> RULE_SEQUENCE_MEANING = {
		{"comma", ELEMENT_DELIMITER},
		{"endl", LINE_DELIMITER},
		{"quote", SCREEN_GUARD}
};

inline void removeRuleSequences(std::string& string_record,
								const size_t line_number,
								const size_t number_from_end,
								const size_t elem_amount) {
	int last_changed_screen_guard_index = -1;
	
	while (string_record.find(SCREEN_GUARD, last_changed_screen_guard_index + 1) != std::string::npos) {
		size_t first_quote_occurrence = string_record.find(SCREEN_GUARD,
														   last_changed_screen_guard_index + 1);
		size_t second_quote_occurrence = string_record.find(SCREEN_GUARD,
															first_quote_occurrence + 1);
		std::string word_inside_guards = string_record.substr(first_quote_occurrence + 1,
															  second_quote_occurrence - first_quote_occurrence - 1);
		
		if (second_quote_occurrence == std::string::npos) {
			throw BadQuotes(line_number, elem_amount - number_from_end + 1);
		}
		try {
			string_record.replace(first_quote_occurrence,
							      second_quote_occurrence - first_quote_occurrence + 1,
								  std::string{ 
									  RULE_SEQUENCE_MEANING.at(word_inside_guards) 
								  });
			last_changed_screen_guard_index = first_quote_occurrence;
		}
		catch (std::out_of_range&) {
			throw RecordWithUnknownSequence(word_inside_guards, 
											line_number,
											elem_amount - number_from_end + 1);
		}
	}
}

template<size_t Number>
std::tuple<> readTuple(std::basic_istream<char>& str, const size_t record_number) {
	return std::make_tuple();
}

template<size_t Number, typename T, typename... Rest>
std::tuple<T, Rest...> readTuple(std::basic_istream<char>& str, const size_t record_number) {
	T first_element;
	std::string string_element;

	std::getline(str, string_element, ELEMENT_DELIMITER);
	removeRuleSequences(string_element, record_number, sizeof...(Rest) + 1, Number);

	std::stringstream char_stream(string_element);
	char_stream >> first_element;
	
	if (str.fail() || char_stream.fail() 
				   || !char_stream.eof()) {
		throw BadFormat(record_number, Number - sizeof...(Rest));
	}
	
	std::tuple<Rest...> tail = readTuple<Number, Rest...>(str, record_number);
	
	return std::tuple_cat(std::make_tuple(first_element), tail);
}

template<typename T, typename ... Args>
auto& operator<<(std::basic_ostream<char>& output_stream, const std::tuple<T, Args...>& tuple) {
	auto remove_first_element = [&](auto first, auto... args) -> auto
	{
		return std::make_tuple(args...);
	};
	
	return output_stream << std::get<0>(tuple) << "," << std::apply(remove_first_element, tuple);
}

template<typename T>
auto& operator<<(std::basic_ostream<char>& output_stream, const std::tuple<T>& tuple) {
	return output_stream << std::get<0>(tuple);
}

template <typename... Args>
class Parser {
public:
	class ParserIterator {
	public:
		explicit ParserIterator(const std::string& file_name, size_t line_number);

		bool operator==(const ParserIterator& other)const;
		bool operator!=(const ParserIterator& other)const;
		
		ParserIterator& operator++();

		std::tuple<Args...>& operator*();
		std::tuple<Args...>* operator->();

		using iterator_category = std::input_iterator_tag;
		using value_type = std::tuple<Args...>;

	private:
		void readRecord();
		
		std::tuple<Args...> _current_record;
		std::string _file_name;
		std::ifstream _file_stream;
		size_t _current_line_number;
		bool _read_record;
	};

	explicit Parser(std::string file_name, size_t skip_rows = 0);
	
	void CheckFile() const;
	ParserIterator begin();
	ParserIterator end();

private:
	std::string _csv_file_name;
	size_t _skip_records;
};

template <typename ... Args>
Parser<Args...>::ParserIterator::ParserIterator(const std::string& file_name, const size_t line_number) :
												_file_name(file_name),
												_file_stream(file_name),
												_current_line_number(line_number),
												_read_record(false) {
	
	for (size_t i = 0; i < _current_line_number - 1; ++i) {
		std::string line;
		std::getline(_file_stream, line, LINE_DELIMITER);
		
		if (_file_stream.eof()) {
			break;
		}
	}
}

template <typename ... Args>
bool Parser<Args...>::ParserIterator::operator==(const ParserIterator& other) const {
	return _file_name == other._file_name && _current_line_number == other._current_line_number;
}

template <typename ... Args>
bool Parser<Args...>::ParserIterator::operator!=(const ParserIterator& other) const {
	return !(*this == other);
}

template <typename ... Args>
typename Parser<Args...>::ParserIterator& Parser<Args...>::ParserIterator::operator++() {
	readRecord();

	_read_record = false;
	
	if (_file_stream.eof()) {
		_current_line_number = std::string::npos;
	}
	else {
		++_current_line_number;
	}

	return *this;
}

template <typename ... Args>
void Parser<Args...>::ParserIterator::readRecord() {
	if (!_read_record) {
		if (_current_line_number == std::string::npos) {
			throw AttemptToDereferenceEndIterator();
		}

		std::string string_record;
		std::getline(_file_stream, string_record, LINE_DELIMITER);
		
		std::stringstream string_stream(string_record);
		_current_record = readTuple<sizeof...(Args), Args...>(string_stream, _current_line_number);
		
		_read_record = true;
	}
}

template <typename ... Args>
std::tuple<Args...>& Parser<Args...>::ParserIterator::operator*() {
	readRecord();

	return _current_record;
}

template <typename ... Args>
std::tuple<Args...>* Parser<Args...>::ParserIterator::operator->() {
	readRecord();

	return &_current_record;
}

template <typename ... Args>
Parser<Args...>::Parser(std::string file_name, const size_t skip_rows) :
	_csv_file_name(std::move(file_name)),
	_skip_records(skip_rows) {}

template <typename ... Args>
void Parser<Args...>::CheckFile() const {
	Parser test_parser(_csv_file_name, _skip_records);
	std::tuple<Args...> buffer;

	for (auto& record : test_parser) {
		buffer = record;
	}
}

template <typename ... Args>
typename Parser<Args...>::ParserIterator Parser<Args...>::begin() {
	return ParserIterator(_csv_file_name, _skip_records + 1);
}

template <typename ... Args>
typename Parser<Args...>::ParserIterator Parser<Args...>::end() {
	return ParserIterator(_csv_file_name, std::string::npos);
}
