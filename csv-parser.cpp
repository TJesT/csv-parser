#include "CSVParser.h"

#include <iostream>
#include <tuple>

int main() {
	Parser<int, double, int> parser("in.csv", 0);
	try {
		parser.CheckFile();
		
		for (auto& record : parser) {
			std::cout << record << std::endl;
		}
	}
	
	catch (std::exception& e) 	{
		std::cout << e.what();
	}
	
	return 0;
}
