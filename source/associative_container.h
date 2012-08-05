/*
 * associative_container.h
 *
 */

#ifndef ASSOCIATIVE_CONTAINER_H_
#define ASSOCIATIVE_CONTAINER_H_

#include "element_inspector.h"
#include <utility>
#include <iostream>

namespace container {

template<typename K, typename T>
class associative_container {

public:

	/**
	 * Excepción elevada cuando se intenta agregar un elemento
	 * que ya existe al contenedor
	 */
	class duplicate_exception {};

	/**
	 * Excepción elevada cuando un elemento no se encuentra
	 * en el contenedor cuando debería ser así.
	 */
	class not_found_exception {};

	/**
	 * Agrega un elemento al contenedor. Valida si el
	 * elemento está en el contenedor, en cuyo caso eleva
	 * duplicate_exception.
	 */
	virtual void add_element(const K &key, const T &value) = 0;

	/**
	 * Modifica un elemento existente en el contenedor.
	 * Valida que el elemento exista, en caso contrario
	 * eleva not_found_exception
	 */
	virtual void update_element(const K &key, const T &value) = 0;

	/**
	 * Elimina un elemento existente en el contenedor.
	 * Valida que el elemento exista, en caso contrario
	 * eleva not_found_exception
	 */
	virtual void delete_element(const K &key) = 0;

	/**
	 * Busca un elemento en el contenedor. Si el elemento
	 * existe, result.first será true, y result.second será
	 * una copia del elemento almacenado. En caso contrario,
	 * result.first será false.
	 */
	typedef std::pair<bool, T> search_result_type;
	virtual search_result_type search_for_element(const K &key) = 0;

	/**
	 * Vuelca el contenido de la estructura de datos en
	 * un stream dado.
	 */
	virtual void dump_to_stream(std::ostream &output) = 0;

	/**
	 * Inspecciona todos los pares clave-valor almacenados en
	 * el contenedor asociativo utilizando la interface de inspección
	 * dada
	 */
	virtual void inspect(element_inspector<K, T> &inspector) = 0;

	virtual ~associative_container() {};

};

};

#endif /* ASSOCIATIVE_CONTAINER_H_ */
