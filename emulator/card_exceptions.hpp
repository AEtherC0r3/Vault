#ifndef CARDEXCEPTIONS_HPP
#define CARDEXCEPTIONS_HPP

#include <exception>

class KeyExists : public std::exception {
public:
	const char *what();
};

class KeyNotExists : public std::exception {
public:
	const char *what();
};

#endif

