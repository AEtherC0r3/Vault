#include <card_exceptions.hpp>

const char* KeyExists::what()
{
	return "A key pair with the same name already exists";
}

const char* KeyNotExists::what()
{
	return "No key pair with that name exists";
}

