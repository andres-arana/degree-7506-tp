/******************************************************************************
 * bucket_table.h
 * 		Declaraciones de la clase hash::bucket_table
******************************************************************************/
#ifndef __HASH_BUCKET_TABLE_H_INCLUDED__
#define __HASH_BUCKET_TABLE_H_INCLUDED__

#include "bucket.h"
#include "../commons/io/block_availability.h"
#include "../commons/io/recycling_block_file.h"
#include "../commons/assertions/assertions.h"
#include <utility>
#include <string>

namespace hash {

/**
 * Contenedor persistente de buckets a un archivo, que maneja
 * además una lista de buckets disponibles para poder reutilizarlos
 * cuando se eliminan y luego se agregan nuevos buckets.
 */
template<typename K, typename T>
class bucket_table {
private:
	commons::io::recycling_block_file file;

	struct block_file_initializer : public commons::io::recycling_block_file::initializer {
		void initialize(commons::io::recycling_block_file *file) const;
	};
public:

	/**
	 * Iterador a los buckets de la tabla
	 */
	class iterator;
	friend class iterator;
	class iterator {
	private:
		bucket_table<K, T> &table;
		int current_block;
	public:
		/**
		 * Crea una nueva instancia de un iterador, apuntando
		 * al bucket en la posición current_block de una tabla table,
		 */
		iterator(bucket_table<K, T> &table, int current_block);

		/**
		 * Mueve el iterador al siguiente elemento de la secuencia
		 */
		void operator++(int);

		/**
		 * Obtiene la referencia al elemento apuntado
		 */
		std::pair<bucket<K, T>,int> operator*();

		/**
		 * Compara si dos iteradores son iguales
		 */
		bool operator==(const iterator &other) const;

		/**
		 * Compara si dos iteradores son diferentes
		 */
		bool operator!=(const iterator &other) const;
	};

	/**
	 * Crea una nueva instancia de bucket_table, cuyo almacén
	 * persistente es el archivo indicador por path, con un tamaño
	 * de bloque block_size. Crea este archivo y lo inicializa,
	 * si este no existe, o abre el archivo si este ya existía.
	 */
	explicit bucket_table(const std::string &path, int block_size);

	/**
	 * Agrega un bucket en alguna posición disponible del archivo.
	 * Devuelve la posición en la que se creo el bucket.
	 */
	int append_bucket(const bucket<K, T> &b);

	/**
	 * Similar a append_bucket, pero no guarda un bucket nuevo
	 * sino que reserva la posición para poderse guardar más adelante.
	 */
	int reserve_bucket();

	/**
	 * Persiste los contendios del bucket b en la posición position
	 * del archivo.
	 * Precondición: Hay un bucket existente en la posición position.
	 */
	void save_bucket(int position, const bucket<K, T> &b);

	/**
	 * Agrega un bucket a la lista de bucktes disponibles, haciendolo
	 * inaccesible y candidato a ser reutilizado por get_next_free_bucket.
	 * Precondición: Hay un bucket existente en la posición position.
	 */
	void remove_bucket(int position);

	/**
	 * Devuelve un bucket en la posición position.
	 * Precondición: Hay un bucket existente en la posición position.
	 */
	bucket<K, T> get_bucket(int position);

	/**
	 * Idéntico a get_bucket, pero no realiza copias de bloques debido
	 * a que utiliza un bucket previamente creado
	 */
	void get_bucket_into(int position, bucket<K, T> *b);

	/**
	 * Obtiene un iterador al primer bucket de la tabla
	 */
	iterator begin() { return iterator(*this, file.get_first_occupied()); }

	/**
	 * Obtiene un iterador al último bucket de la tabla
	 */
	iterator end() { return iterator(*this, -1); }
};


template<typename K, typename T>
bucket_table<K, T>::iterator::iterator(bucket_table<K, T> &table, int current_block)
: table(table), current_block(current_block) {

}

template<typename K, typename T>
void bucket_table<K, T>::iterator::operator++(int) {
	// Busco el siguiente bloque que no esté disponible
	current_block = table.file.get_next_occupied(current_block);

}

template<typename K, typename T>
std::pair<bucket<K, T>,int> bucket_table<K, T>::iterator::operator*() {
	return std::make_pair(table.get_bucket(current_block), current_block);
}

template<typename K, typename T>
bool bucket_table<K, T>::iterator::operator==(const iterator &other) const {
	return current_block == other.current_block;
}

template<typename K, typename T>
bool bucket_table<K, T>::iterator::operator!=(const iterator &other) const {
	return current_block != other.current_block;
}


template<typename K, typename T>
bucket_table<K, T>::bucket_table(const std::string &path, int block_size)
: file(path.c_str(), block_size, block_file_initializer()) {

}

template<typename K, typename T>
int bucket_table<K, T>::append_bucket(const bucket<K, T> &b) {
	return file.append_block(b.get_block());
}

template<typename K, typename T>
int bucket_table<K, T>::reserve_bucket() {
	return file.reserve_block();
}

template<typename K, typename T>
void bucket_table<K, T>::save_bucket(int position, const bucket<K, T> &b) {
	file.write_block(position, b.get_block());
}

template<typename K, typename T>
void bucket_table<K, T>::remove_bucket(int position) {
	file.release_block(position);
}

template<typename K, typename T>
bucket<K, T> bucket_table<K, T>::get_bucket(int position) {
	return bucket<K, T>(file.read_block(position));
}

template<typename K, typename T>
void bucket_table<K, T>::get_bucket_into(int position, bucket<K, T> *b) {
	file.read_block_into(position, b->get_block_pointer());
}

template<typename K, typename T>
void bucket_table<K, T>::block_file_initializer::initialize(commons::io::recycling_block_file *file) const {
	// Inicializo el primer bucket
	bucket<K, T> initial(commons::io::block(file->get_block_size()));
	initial.clear();
	initial.set_hash_factor(1);
	file->append_block(initial.get_block());
}

};

#endif // __HASH_BUCKET_TABLE_H_INCLUDED__
