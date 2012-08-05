/******************************************************************************
 * element_container_block.h
 * 		Declaraciones y definiciones de la clase
 * 		commons::io::element_container_block
******************************************************************************/
#ifndef __COMMONS_IO_ELEMENT_CONTAINER_BLOCK_H_INCLUDED__
#define __COMMONS_IO_ELEMENT_CONTAINER_BLOCK_H_INCLUDED__

#include "block.h"
#include "serializators.h"
#include <utility>
#include <stdexcept>

namespace commons {
namespace io {

/**
 * Representa un bloque que contiene elementos  de
 * tamaño variable de tipo T.
 */
template<typename K, typename T>
class element_container_block {
private:
	block inner_block;
protected:
	/**
	 * Obtiene el bloque interno sobre el que trabaja
	 * el contenedor
	 */
	const block &get_inner_block() const;

	/**
	 * Obtiene un puntero al bloque interno sobre el que
	 * trabaja el contenedor
	 */
	block *get_inner_block_pointer();


	/**
	 * Establece el índice que apunta al primer
	 * espacio disponible para escribir nuevos
	 * elementos
	 */
	void set_free_index(int index);

	/**
	 * Aumenta el índice que apunta al primer
	 * espacio disponible para escribir nuevos
	 * elementos.
	 */
	void increase_free_index(int size);

	/**
	 * Obtiene el índice que apunta al primer
	 * espacio disponible para escribir nuevos
	 * elementos.
	 */
	int get_free_index() const;

	/**
	 * Obtiene un índice que apunta al lugar
	 * en donde empieza el elemento de clave
	 * key.
	 */
	int get_element_index(const K &key) const;

	/**
	 * Obtiene el índice que apunta a la
	 * posición en donde empiezan los elementos.
	 */
	int get_element_start_index() const;

	/**
	 * Obtiene la posición en donde inicia el
	 * elemento siguiente al elemento que empieza
	 * en current_position
	 */
	int get_next_element_index(int current_position) const;

	/**
	 * Obtiene la longitud de los metadatos.
	 */
	virtual int get_metadata_length() const = 0;

	/**
	 * Hace espacio para un elemento element y devuelve
	 * la posición en la que debe escribirse el elemento.
	 */
	virtual int make_room_for_element(const K &key, const T &value) = 0;

	/**
	 * Maneja un overflow al agregar un elemento.
	 */
	virtual void handle_insertion_overflow(const K &key, const T &value, int total_required_size) = 0;

	/**
	 * Maneja un overflow al modificar un elemento.
	 */
	virtual void handle_modification_overflow(const K &key, const T &value, int total_required_size) = 0;
public:
	/**
	 * Iterador a los elementos del contenedor
	 */
	class iterator;
	friend class iterator;
	class iterator {
	private:
		const element_container_block &container;
		int current_position;
	public:
		/**
		 * Crea una nueva instancia de iterator apuntando
		 * a un bloque block
		 */
		iterator(const element_container_block &container, int start_position);

		/**
		 * Mueve el iterador al siguiente elemento de la secuencia
		 */
		void operator++(int);

		/**
		 * Obtiene la referencia al elemento apuntado
		 */
		std::pair<K, T> operator*() const;

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
	 * Excepción elevada cuando se intenta insertar dos veces
	 * el mismo elemento
	 */
	class duplicate_exception {};

	/**
	 * Excepción elevada cuando se intenta trabajar con un
	 * elemento que no existe
	 */
	class not_found_exception {};

	/**
	 * Crea una nueva instancia de element_container_block que
	 * trabaja como una vista de un contenedor de elementos sobre
	 * el bloque b. Realiza una copia de este bloque para operar
	 * internamente.
	 */
	explicit element_container_block(const commons::io::block &b);

	/**
	 * Limpia el contenido de un element_container_block, eliminando
	 * todos los elementos que contiene, pero a diferencia de clear
	 * no inicializa otras estructuras de control.
	 */
	virtual void clear_elements();

	/**
	 * Obtiene un iterador al primer elemento del contenedor
	 */
	iterator begin() const { return iterator(*this, get_element_start_index()); }

	/**
	 * Obtiene un iterador al último elemento
	 */
	iterator end() const { return iterator(*this, get_free_index()); }

	/**
	 * Agrega un elemento a este contenedor.
	 */
	virtual void add_element(const K &key, const T &value);

	/**
	 * Obtiene el elemento de clave key del contenedor. Si este existe,
	 * resultado.first será true, y en resultado.second estará el
	 * elemento. En caso contrario, resultado.first será false.
	 */
	virtual std::pair<bool, T> get_element(const K &key) const;

	/**
	 * Actualiza los datos de un elemento dado.
	 */
	virtual void update_element(const K &key, const T &value);

	/**
	 * Elimina un elemento dada su clave key.
	 */
	virtual void remove_element(const K &key);

	/**
	 * Devuelve true si el nodo está vacio. En caso contrario
	 * devuelve false.
	 */
	virtual bool is_empty() const;

	/**
	 * Devuelve el bloque interno sobre el que opera este contenedor
	 */
	virtual const commons::io::block &get_block() const;

	/**
	 * Devuelve un puntero al bloque interno sobre el que opera
	 * este contenedor
	 */
	virtual commons::io::block *get_block_pointer();
};



template<typename K, typename T>
element_container_block<K, T>::iterator::iterator(const element_container_block &container, int start_position)
: container(container), current_position(start_position) {

}

template<typename K, typename T>
void element_container_block<K, T>::iterator::operator++(int) {
	current_position = container.get_next_element_index(current_position);
}

template<typename K, typename T>
std::pair<K, T> element_container_block<K, T>::iterator::operator*() const {
	// Levanto la clave saltando la longitud del registro
	K key = commons::io::deserialize<K>(container.inner_block, current_position);

	// Calculo la posición en la que empieza el registro
	int record_position = current_position + commons::io::serialization_length(key);

	// Deserializo el registro
	T value = commons::io::deserialize<T>(container.inner_block, record_position);

	return std::make_pair(key, value);
}

template<typename K, typename T>
bool element_container_block<K, T>::iterator::operator==(const element_container_block<K, T>::iterator &other) const {
	return current_position == other.current_position;
}

template<typename K, typename T>
bool element_container_block<K, T>::iterator::operator!=(const element_container_block<K, T>::iterator &other) const {
	return current_position != other.current_position;
}

template<typename K, typename T>
element_container_block<K, T>::element_container_block(const commons::io::block &b)
: inner_block(b) {

}

template<typename K, typename T>
void element_container_block<K, T>::clear_elements() {
	// Inicializo el control de espacio libre
	set_free_index(get_element_start_index());
}

template<typename K, typename T>
void element_container_block<K, T>::add_element(const K &key, const T &value) {
	// Chequeamos si agregar el elemento causaría un overflow
	int totalRecordLength =
			commons::io::serialization_length(key) +
			commons::io::serialization_length(value);

	if (get_free_index() + totalRecordLength > inner_block.get_size()) {
		handle_insertion_overflow(key, value, get_free_index() + totalRecordLength);
	}

	// Chequeamos si el elemento ya existe
	if (0 <= get_element_index(key)) {
		throw duplicate_exception();
	}

	// Obtengo la posición en donde tengo que escribir el elemento
	int current_field_position = make_room_for_element(key, value);

	// Primero escribo la clave del registro
	commons::io::serialize(key, &inner_block, current_field_position);
	current_field_position += commons::io::serialization_length(key);

	// Por último escribo los datos del registro
	commons::io::serialize(value, &inner_block, current_field_position);

	// Calculo la nueva última posición libre y actualizo
	increase_free_index(totalRecordLength);
}

template<typename K, typename T>
std::pair<bool, T> element_container_block<K, T>::get_element(const K &key) const {
	// Obtengo la posición del elemento que estoy buscando
	int element_position = get_element_index(key);

	// Si no lo encontré, paramos acá
	if (element_position >= 0) {
		T value = commons::io::deserialize<T>(inner_block, element_position + commons::io::serialization_length<K>(inner_block, element_position));
		return std::make_pair(true, value);
	} else {
		return std::make_pair(false, T());
	}
}

template<typename K, typename T>
void element_container_block<K, T>::update_element(const K &key, const T &value) {
	// Obtengo la posición del elemento que estoy buscando
	int element_position = get_element_index(key);

	// Si no existe... estamos en el horno
	if (element_position < 0) {
		throw not_found_exception();
	}

	// Calculo la posición de los datos del registro
	int data_position =
			element_position + commons::io::serialization_length(key);

	// Ahora comparo con la nueva longitud del registro.
	int length_delta =
			commons::io::serialization_length(value) -
			commons::io::serialization_length<T>(inner_block, data_position);

	if (length_delta > 0) {
		// Tenemos que hacer espacio para más datos. Puede ser
		// que haya overflow
		if (length_delta + get_free_index() > inner_block.get_size()) {
			handle_modification_overflow(key, value, length_delta + get_free_index());
		}
		// Si es menor tengo que hacer espacio para los nuevos datos
		inner_block.make_gap(data_position, length_delta);
		// Actualizo la posición libre que ahora está más lejos
		set_free_index(get_free_index() + length_delta);
	} else if (length_delta < 0) {
		// Si es mayor tengo que quitar lo que sobra
		inner_block.erase_and_compact(data_position, data_position - length_delta - 1);
		// Actualizo la posición libre que ahora está más cerca (length_delta es negativo)
		set_free_index(get_free_index() + length_delta);
	}

	// Ahora que hay espacio serializo los datos
	commons::io::serialize(value, &inner_block, data_position);
}

template<typename K, typename T>
void element_container_block<K, T>::remove_element(const K &key) {
	// Obtengo la posición del elemento que estoy buscando
	int element_position = get_element_index(key);

	// Si no existe... estamos en el horno
	if (element_position < 0) {
		throw not_found_exception();
	}

	// El tamaño total de bytes que hay que volar es
	// el tamaño de la clave mas el tamaño del elemento
	int key_length = commons::io::serialization_length<K>(inner_block, element_position);
	int value_length = commons::io::serialization_length<T>(inner_block, element_position + key_length);
	int total_length = key_length + value_length;

	// Y ahora comprimo los datos eliminando lo que
	// está en la posición del elemento
	inner_block.erase_and_compact(element_position, element_position + total_length - 1);

	// Por último actualizo la posición disponible
	set_free_index(get_free_index() - total_length);
}

template<typename K, typename T>
bool element_container_block<K, T>::is_empty() const {
	return get_free_index() == get_element_start_index();
}

template<typename K, typename T>
const block &element_container_block<K, T>::get_block() const {
	return inner_block;
}

template<typename K, typename T>
block *element_container_block<K, T>::get_block_pointer() {
	return &inner_block;
}

template<typename K, typename T>
const block &element_container_block<K, T>::get_inner_block() const {
	return inner_block;
}

template<typename K, typename T>
block *element_container_block<K, T>::get_inner_block_pointer() {
	return &inner_block;
}

template<typename K, typename T>
void element_container_block<K, T>::set_free_index(int index) {
	commons::io::serialize(index, &inner_block, get_metadata_length());
}

template<typename K, typename T>
void element_container_block<K, T>::increase_free_index(int size) {
	set_free_index(get_free_index() + size);
}

template<typename K, typename T>
int element_container_block<K, T>::get_free_index() const {
	return commons::io::deserialize<int>(inner_block, get_metadata_length());
}

template<typename K, typename T>
int element_container_block<K, T>::get_element_index(const K &key) const {
	// Me paro en la primer posición que puede tener un elemento
	int current_position = get_element_start_index();

	// Mientras la posición actual no sea la primer posición disponible
	while (current_position != get_free_index()) {
		// Me paro en la posición actual, que es la posición del primer campo
		// del registro
		int current_field = current_position;

		// Obtengo la clave del registro y avanzo al siguiente campo
		K record_key = commons::io::deserialize<K>(inner_block, current_field);
		current_field += commons::io::serialization_length(record_key);

		if (record_key == key) {
			return current_position;
		} else {
			current_position =
					current_field +
					commons::io::serialization_length<T>(inner_block, current_field);
		}
	}
	// El registro no se encontró.
	return -1;
}

template<typename K, typename T>
int element_container_block<K, T>::get_next_element_index(int current_position) const {
	// Obtengo la longitud de la clave
	int key_length = commons::io::serialization_length<K>(inner_block, current_position);

	// Obtengo la longitud del valor
	int value_length = commons::io::serialization_length<T>(inner_block, current_position + key_length);

	// Salto
	return current_position + key_length + value_length;
}

template<typename K, typename T>
int element_container_block<K, T>::get_element_start_index() const {
	return sizeof(int) + get_metadata_length();
}

};
};

#endif
