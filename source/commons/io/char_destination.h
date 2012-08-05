/******************************************************************************
 * char_destination.h
 * 		Declaraciones de la clase commons::io::char_destination
******************************************************************************/
#ifndef __COMMONS_IO_CHAR_DESTINATION_H_INCLUDED__
#define __COMMONS_IO_CHAR_DESTINATION_H_INCLUDED__

namespace commons {
namespace io {

/**
 * Interface de un destino de chars
 */
class char_destination {
public:
	/**
	 * Emite un char al destino
	 */
	virtual void emit_char(unsigned char c) = 0;
};

};
};

#endif
