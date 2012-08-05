/******************************************************************************
 * block_availability.h
 * 		Declaraciones de la clase commons::io::block_availability
******************************************************************************/
#ifndef __COMMONS_IO_BLOCK_AVAILABILITY_H_INCLUDED__
#define __COMMONS_IO_BLOCK_AVAILABILITY_H_INCLUDED__

#include "block.h"

namespace commons {
namespace io {

/**
 * Representa un bloque utilizado para mantener
 * las disponibilidad de un número de elementos
 */
class block_availability {
private:
	block b;
public:
	/**
	 * Crea una nueva instancia de block_availability
	 * con un bloque interno de tamaño block_size
	 */
	block_availability(int block_size);

	/**
	 * Limpia el bloque interno, seteando todos los bloques
	 * como no disponibles
	 */
	void clear();

	/**
	 * Accede al bloque interno que se utiliza
	 * para guardar el control de si un elemento
	 * dado está disponible o no.
	 */
	commons::io::block &get_block();

	/**
	 * Devuelve true si el elemento dado está disponible,
	 * o false en caso contrario
	 */
	bool is_available(int position) const;

	/**
	 * Marca un elemento como no disponible
	 */
	void make_unavailable(int position);

	/**
	 * Marca un elemento como disponible
	 */
	void make_available(int position);

	/**
	 * Devuelve el primer elemento disponible
	 */
	int first_available();

	/**
	 * Devuelve el primer elemento ocupado
	 */
	int first_occupied();
};

};
};

#endif
