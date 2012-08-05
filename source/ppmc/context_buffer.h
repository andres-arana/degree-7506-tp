/******************************************************************************
 * context_buffer.h
 * 		Declaraciones de la clase ppmc::context_buffer
******************************************************************************/
#ifndef __PPMC_CONTEXT_BUFFER_H_INCLUDED__
#define __PPMC_CONTEXT_BUFFER_H_INCLUDED__

#include <string>

namespace ppmc {

/**
 * Representa un buffer de los últimos n chars
 */
class context_buffer {
private:
	std::string buffer;
	unsigned int max_size;
	unsigned int buffer_start_position;
public:
	/**
	 * Crea una nueva instancia de context_buffer
	 * que almacenará hasta el contexto de tamaño
	 * max_size.
	 */
	context_buffer(unsigned int max_size);

	/**
	 * Mete un char en la lista de chars, posiblemente causando
	 * un descarte del char más viejo pusheado si es que se supero
	 * el tamaño máximo del buffer.
	 */
	void push_char(unsigned char c);

	/**
	 * Devuelve true si hay disponible un contexto de tamaño
	 * context_size en el buffer, o false en caso contrario
	 */
	bool has_context(unsigned int context_size) const;

	/**
	 * Devuelve el tamaño del mayor contexto disponible
	 * en el buffer.
	 */
	unsigned int max_context() const;

	/**
	 * Obtiene el contexto de orden context_size del buffer
	 */
	std::string get_context(unsigned int context_size) const;

};

};

#endif
