/******************************************************************************
 * char_source.h
 * 		Declaraciones de la clase commons::io::char_source
******************************************************************************/
#ifndef __COMMONS_IO_CHAR_SOURCE_H_INCLUDED__
#define __COMMONS_IO_CHAR_SOURCE_H_INCLUDED__

#include <utility>

namespace commons {
namespace io {

/**
 * Interface de un origen de chars
 */
class char_source {
public:
	/**
	 * Obtiene el siguiente char del origen, si está disponible.
	 * En caso de estarlo, pair.first es true y pair.second
	 * es el siguiente char. En caso contrario pair.first es false.
	 */
	virtual std::pair<bool, unsigned char> get_next_char() = 0;
};

};
};

#endif
