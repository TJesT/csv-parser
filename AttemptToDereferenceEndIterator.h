#pragma once

#include <exception>

class AttemptToDereferenceEndIterator final :
	std::exception
{};
