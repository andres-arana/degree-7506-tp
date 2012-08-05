/******************************************************************************
 * recycling_block_file.h
 * 		Declaraciones de la clase commons::io::recycling_block_file
******************************************************************************/
#ifndef __COMMONS_IO_RECYCLING_BLOCK_FILE_H_INCLUDED__
#define __COMMONS_IO_RECYCLING_BLOCK_FILE_H_INCLUDED__

#include "block_file.h"
#include "block_availability.h"

namespace commons {
namespace io {

/**
 * Representa un archivo organizado por bloques que maneja
 * la disponibilidad de bloques internamente en un bloque de
 * control.
 */
class recycling_block_file {
private:
	block_availability availability;
	block_file file;

	/**
	 * Comando de inicialización del archivo. Utilizado para
	 * inicializar el archivo cuando este se crea por primera
	 * vez, en el caso de que el que usa a recycling_block_file
	 * no disponga de un inicializador propio.
	 */
	struct availability_initializer : public block_file::initializer {
		/**
		 * Inicializa el archivo cuando este se crea por
		 * primera vez.
		 */
		virtual void initialize(block_file *file) const;
	};

	void read_availability_block();
	void write_availability_block();
public:
	/**
	 * Comando de inicialización del archivo. Utilizado para
	 * inicializar el archivo cuando este se crea por primera
	 * vez.
	 */
	struct initializer {
		/**
		 * Inicializa el archivo cuando este se crea por
		 * primera vez.
		 */
		virtual void initialize(recycling_block_file *file) const = 0;
	};

	/**
	 * Crea una nueva instancia de recycling_block_file, abriendo
	 * el archivo dado por filename que contiene bloques de tamaño
	 * block_size.
	 */
	recycling_block_file(const char *filename, int block_size);

	/**
	 * Crea una nueva instancia de recycling_block_file, abriendo el
	 * archivo dado por filename que contiene bloques de tamaño
	 * block_size. Si el archivo no existe, lo crea y llama al
	 * initializer para que este lo inicialice.
	 */
	recycling_block_file(const char *filename, int block_size, const initializer &initializer);

	/**
	 * Cierra el archivo.
	 */
	virtual void close();

	/**
	 * Lee el bloque en la posición position y lo devuelve
	 */
	virtual block read_block(int position);

	/**
	 * Lee el bloque en la posición position y lo carga en
	 * el bloque b
	 */
	virtual void read_block_into(int position, block *b);

	/**
	 * Sobreescribe lo que haya en el bloque en la posición
	 * position con los contenidos del bloque b
	 */
	virtual void write_block(int position, const block &b);

	/**
	 * Agrega un bloque en una posición libre del archivo.
	 * Devuelve la posición en donde se agregó el bloque,
	 * dado que puede haberse utilizado una posición reciclada
	 * que anteriormente se había liberado
	 */
	virtual int append_block(const block &b);

	/**
	 * Reserva un bloque para su posterior uso. Este método es similar
	 * a append_block, pero no guarda ningún bloque, sólamente devuelve
	 * la posición que se ha reservado.
	 */
	virtual int reserve_block();

	/**
	 * Marca un bloque como disponible, haciendolo candidato
	 * a reciclado en próximas llamadas a append_block.
	 */
	void release_block(int position);

	/**
	 * Devuelve la cantidad de bloques que hay en el archivo
	 */
	virtual int get_block_count();

	/**
	 * Devuelve el primer bloque utilizado
	 */
	virtual int get_first_occupied();

	/**
	 * Devuelve el siguiente bloque utilizado a
	 * partir de un bloque dado, o -1 si no hay más
	 * bloques utilizados
	 */
	virtual int get_next_occupied(int current);

	/**
	 * Devuelve el tamaño de bloque del archivo
	 */
	virtual int get_block_size() const;

	virtual ~recycling_block_file();
};

};
};

#endif
