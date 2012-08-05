/******************************************************************************
 * assertion_failure.cpp
 * 		Definiciones de la clase commons::assertions::assertion_failure
******************************************************************************/
#include "assertion_failure.h"
#include <sstream>

using namespace commons::assertions;
using namespace std;

assertion_failure::assertion_failure(const string &description, const string &file, int line)
{
	stringstream builder;
	builder << "(" << file << ":" << line << ") " << description;

	this->description = builder.str();
}

const char *assertion_failure::what() const throw() {
	return this->description.c_str();
}

assertion_failure::~assertion_failure() throw() {

}
