/******************************************************************************
 * block.h
 * 		Declaraciones de la clase commons::io::block
******************************************************************************/
#ifndef __COMMONS_IO_BLOCK_H_INCLUDED__
#define __COMMONS_IO_BLOCK_H_INCLUDED__

#include <iostream>

namespace commons {
namespace io {

/**
 * Representa un area de tamaño variable de bytes
 */
class block {
private:
	int size;
	char *data;
public:
	/**
	 * Crea una nueva instancia de block, de tamaño
	 * size, copiando size bytes de data
	 */
	explicit block(int size, const char *data = 0);

	/**
	 * Crea una nueva instancia de block, copiando
	 * el contenido de otro block
	 */
	block(const block &other);

	/**
	 * Reemplaza el contenido y tamaño de un block
	 * por el contenido y tamaño de otro block
	 */
	block &operator=(const block &other);

	/**
	 * Accede al byte en la posición index del block
	 */
	char operator[](int index) const;
	/**
	 * Accede al byte en la posición index del block
	 * y permitiendo modificaciones
	 */
	char &operator[](int index);

	/**
	 * Obtiene un puntero a los datos del block
	 */
	const char *raw_char_pointer() const;

	/**
	 * Obtiene un puntero a los datos del block,
	 * permitiendo su modificación
	 */
	char *raw_char_pointer();

	/**
	 * Obtiene el tamaño del block
	 */
	int get_size() const;

	/**
	 * Elimina los bytes entre las posiciones index_from
	 * e index_to inclusive, y compacta los bytes restantes
	 * copiando todos los bytes que se encuentran después de
	 * index_to a index_from.
	 */
	void erase_and_compact(int index_from, int index_to);

	/**
	 * Mueve los bytes siguientes a la posición index_at a la posición
	 * index_at + data_size, y luego copia data_size bytes de data a la
	 * posición index_at
	 */
	void insert_and_expand(int index_at, const char *new_data, int data_size);

	/**
	 * Mueve los bytes siguientes a la posición index_at a las posición
	 * index_at + gap_size, dejando gap_size bytes disponibles después
	 * de index_at
	 */
	void make_gap(int index_at, int gap_size);

	/**
	 * Método conveniente para hacer dump de los contenidos del bloque
	 * a un stream, para poder debuguear
	 */
	void dump_to_stream(std::ostream &output) const;

	/**
	 * Copia los contenidos de este bloque a otro bloque
	 */
	void copy_to_block(block &other) const;

	~block();
};

};
};

#endif
