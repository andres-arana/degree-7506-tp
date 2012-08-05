/******************************************************************************
 * ioexception.h
 * 		Declaraciones de la clase de commons::io::ioexception
******************************************************************************/
#ifndef __COMMONS_IO_IOEXCEPTION_H_INCLUDED__
#define __COMMONS_IO_IOEXCEPTION_H_INCLUDED__

#include <stdexcept>

namespace commons {
namespace io {

/**
 * Clase que representa un error grave de input-output
 */
class ioexception : public std::runtime_error {
public:
	/**
	 * Crea una nueva instancia de ioexception
	 */
	ioexception(const std::string &what);

	virtual ~ioexception() throw() {}
};

};
};


#endif
