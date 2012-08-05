/******************************************************************************
 * binary_source.h
 * 		Declaraciones de la clase commons::io::binary_source
******************************************************************************/
#ifndef __COMMONS_IO_BINARY_SOURCE_H_INCLUDED__
#define __COMMONS_IO_BINARY_SOURCE_H_INCLUDED__

#include <utility>

namespace commons {
namespace io {

/**
 * Interface de un origen de bits individuales
 */
class binary_source {
public:
	/**
	 * Obtiene el siguiente bit del origen, si está disponible.
	 * En caso de estarlo, pair.first es true y pair.second
	 * es el siguiente bit. En caso contrario pair.first es false.
	 */
	virtual std::pair<bool, bool> get_next_bit() = 0;

	virtual ~binary_source() {}
};

};
};

#endif
