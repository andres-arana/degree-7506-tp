/******************************************************************************
 * assertion_failure.h
 * 		Declaraciones de la clase commons::assertions::assertion_failure
******************************************************************************/
#ifndef __COMMONS_ASSERTIONS_ASSERTION_FAILURE_H_INCLUDED__
#define __COMMONS_ASSERTIONS_ASSERTION_FAILURE_H_INCLUDED__

#include <stdexcept>
#include <string>

namespace commons {
namespace assertions {

/**
 * Excepción elevada cuando las assertions se compilan y
 * una condición de assertion no se cumple. Ver la macro
 * ASSERTION en assertions.h
 */
class assertion_failure : public std::exception {
public:
	/**
	 * Crea una nueva instancia de assertion_failure, sobre
	 * una condición fallada descripta por description, en
	 * el archivo file linea line.
	 */
	assertion_failure(const std::string &description, const std::string &file, int line);

	/**
	 * Sobreescribe std::exception::what
	 */
	virtual const char *what() const throw();

	/**
	 * Sobreescribe std::exception::~exception
	 */
	virtual ~assertion_failure() throw();
private:
	std::string description;
};

};
};

#endif
