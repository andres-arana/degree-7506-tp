/******************************************************************************
 * bucket.h
 * 		Declaraciones y definiciones de la clase hash::bucket
******************************************************************************/
#ifndef __HASH_BUCKET_H_INCLUDED__
#define __HASH_BUCKET_H_INCLUDED__

#include "../commons/io/element_container_block.h"
#include "../commons/io/serializators.h"
#include <utility>
#include <stdexcept>

namespace hash {

/**
 * Representa un nodo del archivo de datos del hash extensible.
 */
template<typename K, typename T>
class bucket : public commons::io::element_container_block<K, T> {
protected:
	typedef commons::io::element_container_block<K, T> parent;

	/**
	 * Override element_container_block
	 */
	virtual int get_metadata_length() const;

	/**
	 * Override element_container_block
	 */
	virtual int make_room_for_element(const K &key, const T &value);

	/**
	 * Override element_container_block
	 */
	virtual void handle_insertion_overflow(const K &key, const T &value, int total_required_size);

	/**
	 * Override element_container_block
	 */
	virtual void handle_modification_overflow(const K &key, const T &value, int total_required_size);

	/**
	 * Generic overflow handling logic
	 */
	virtual void handle_overflow();

public:
	/**
	 * Excepción elevada cuando un método de modificación
	 * causa que los elementos dentro del bucket sobrepasen
	 * el tamaño del bloque
	 */
	class overflow_exception {};

	/**
	 * Crea una nueva instancia de bucket que trabaja
	 * como una vista de hash bucket sobre el bloque b.
	 * Realiza una copia de este bloque para operar
	 * internamente
	 */
	explicit bucket(const commons::io::block &b);

	/**
	 * Limpia el contenido de un bucket, inicializando
	 * a su vez todas las estructuras de control del bucket
	 * en el bloque como el factor de dispersión y limpia los
	 * elementos que existan en el bloque. Este método se debe
	 * llamar la primera vez que un bloque empieza a utilizarse
	 * como bucket.
	 */
	virtual void clear();

	/**
	 * Obtiene el factor de dispersión de este bucket
	 */
	int get_hash_factor() const;

	/**
	 * Establece el factor de dispersión de este bucket
	 */
	void set_hash_factor(int hash_factor);
};

template<typename K, typename T>
bucket<K, T>::bucket(const commons::io::block &b)
: parent(b) {

}

template<typename K, typename T>
void bucket<K, T>::clear() {
	// Inicializo el factor de dispersión
	set_hash_factor(0);

	// Inicializo los elementos
	parent::clear_elements();
}

template<typename K, typename T>
int bucket<K, T>::get_hash_factor() const {
	return commons::io::deserialize<int>(parent::get_inner_block(), 0);
}

template<typename K, typename T>
void bucket<K, T>::set_hash_factor(int hash_factor) {
	commons::io::serialize(hash_factor, parent::get_inner_block_pointer(), 0);
}

template<typename K, typename T>
int bucket<K, T>::get_metadata_length() const {
	return sizeof(int);
}

template<typename K, typename T>
int bucket<K, T>::make_room_for_element(const K &key, const T &value) {
	return parent::get_free_index();
}

template<typename K, typename T>
void bucket<K, T>::handle_insertion_overflow(const K &key, const T &value, int total_required_size) {
	handle_overflow();
}

template<typename K, typename T>
void bucket<K, T>::handle_modification_overflow(const K &key, const T &value, int total_required_size) {
	handle_overflow();
}

template<typename K, typename T>
void bucket<K, T>::handle_overflow() {
	throw overflow_exception();
}

};

#endif
