/******************************************************************************
 * stream_binary_source.h
 * 		Declaraciones de la clase commons::io::stream_binary_source
******************************************************************************/
#ifndef __COMMONS_IO_STREAM_BINARY_SOURCE_H_INCLUDED__
#define __COMMONS_IO_STREAM_BINARY_SOURCE_H_INCLUDED__

#include "binary_source.h"
#include <iostream>

namespace commons {
namespace io {

/**
 * Implementación de un origen de bits individuales
 * desde un stream.
 */
class stream_binary_source : public binary_source {
private:
	std::istream &s;
	char current_char;
	char current_char_position;
	bool eof;
public:
	/**
	 * Crea una nueva instancia de stream_binary_source
	 * que toma bits individuales desde un stream dado
	 */
	stream_binary_source(std::istream &s);

	/**
	 * Override de binary_source::get_next_bit
	 */
	virtual std::pair<bool, bool> get_next_bit();
};

};
};

#endif
