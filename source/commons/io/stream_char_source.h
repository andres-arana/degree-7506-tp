/******************************************************************************
 * stream_char_source.h
 * 		Declaraciones de la clase commons::io::stream_char_source
******************************************************************************/
#ifndef __COMMONS_IO_STREAM_CHAR_SOURCE_H_INCLUDED__
#define __COMMONS_IO_STREAM_CHAR_SOURCE_H_INCLUDED__

#include "char_source.h"
#include <iostream>

namespace commons {
namespace io {

/**
 * Implementación de un origen de chars desde streams
 */
class stream_char_source : public char_source {
private:
	std::istream &s;
public:
	/**
	 * Crea una nueva instancia de stream_char_source
	 * que trabaja sobre un istream dado
	 */
	stream_char_source(std::istream &s);

	/**
	 * Override char_source::get_next_char
	 */
	virtual std::pair<bool, unsigned char> get_next_char();
};

};
};

#endif
