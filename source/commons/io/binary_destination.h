/******************************************************************************
 * binary_destination.h
 * 		Declaraciones de la clase commons::io::binary_destination
******************************************************************************/
#ifndef __COMMONS_IO_BINARY_DESTINATION_H_INCLUDED__
#define __COMMONS_IO_BINARY_DESTINATION_H_INCLUDED__

namespace commons {
namespace io {

/**
 * Interface de un destino de bits individuales
 */
class binary_destination {
public:
	/**
	 * Emite un char al destino
	 */
	virtual void emit_bit(bool bit) = 0;

	virtual ~binary_destination() {}
};

};
};

#endif
