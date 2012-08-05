/******************************************************************************
 * stream_char_destination.h
 * 		Declaraciones de la clase commons::io::stream_char_destination
******************************************************************************/
#ifndef __COMMONS_IO_STREAM_CHAR_DESTINATION_H_INCLUDED__
#define __COMMONS_IO_STREAM_CHAR_DESTINATION_H_INCLUDED__

#include "char_destination.h"
#include <iostream>

namespace commons {
namespace io {

/**
 * Implementación de un destino de chars a un stream
 */
class stream_char_destination : public char_destination {
private:
	std::ostream &o;
public:
	/**
	 * Crea una nueva instancia de stream_char_destination
	 * que emite chars en el stream dado
	 */
	stream_char_destination(std::ostream &o);

	/**
	 * Override de char_destination::emit_char
	 */
	virtual void emit_char(unsigned char c);
};

};
};

#endif
