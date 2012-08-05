/******************************************************************************
 * block_file.h
 * 		Declaraciones de la clase commons::io::block_file
******************************************************************************/
#ifndef __COMMONS_IO_BLOCK_FILE_H_INCLUDED__
#define __COMMONS_IO_BLOCK_FILE_H_INCLUDED__

#include "block.h"
#include <fstream>

namespace commons {
namespace io {

/**
 * Representa un archivo organizado por bloques
 */
class block_file {
private:
	int block_size;
	std::fstream file;
	bool just_created;

	void initialize_file(const char *filename, int block_size);
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
		virtual void initialize(block_file *file) const = 0;
	};

	/**
	 * Crea una nueva instancia de block_file, abriendo el archivo
	 * dado por filename que contiene bloques de tamaño block_size.
	 */
	block_file(const char *filename, int block_size);

	/**
	 * Crea una nueva instancia de block_file, abriendo el archivo
	 * dado por filename que contiene bloques de tamaño block_size.
	 * Si el archivo no existe, lo crea y llama al initializer para
	 * que este lo inicialice.
	 */
	block_file(const char *filename, int block_size, const initializer &initializer);

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
	 * Agrega un bloque en una posición libre del archivo
	 */
	virtual void append_block(const block &b);

	/**
	 * Devuelve la cantidad de bloques que hay en el archivo
	 */
	virtual int get_block_count();

	/**
	 * Devuelve el tamaño de bloque del archivo
	 */
	virtual int get_block_size() const;

	/**
	 * Devuelve true si el block_file se creo porque
	 * no existía el archivo.
	 */
	virtual bool is_just_created() const;

	virtual ~block_file();
};

};
};

#endif
