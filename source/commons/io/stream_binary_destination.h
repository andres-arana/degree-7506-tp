/******************************************************************************
 * stream_binary_destination.h
 * 		Declaraciones de la clase commons::io::stream_binary_destination
******************************************************************************/
#ifndef __COMMONS_IO_STREAM_BINARY_DESTINATION_H_INCLUDED__
#define __COMMONS_IO_STREAM_BINARY_DESTINATION_H_INCLUDED__

#include "binary_destination.h"
#include <iostream>

namespace commons {
namespace io {

/**
 * Implementación de un destino de bits a un stream
 */
class stream_binary_destination : public binary_destination {
private:
	std::ostream &o;
	char current_char;
	char current_char_position;
public:
	/**
	 * Crea una nueva instancia de stream_binary_destination
	 * que emite bits en el stream dado
	 */
	stream_binary_destination(std::ostream &o);

	/**
	 * Override de binary_destination::emit_bit
	 */
	virtual void emit_bit(bool bit);

	virtual ~stream_binary_destination();
};

};
};

#endif
