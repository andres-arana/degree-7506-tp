/******************************************************************************
 * hash_container.h
 * 		Declaraciones y definiciones de la clase hash::hash_container
******************************************************************************/
#ifndef __HASH_HASH_CONTAINER_H_INCLUDED__
#define __HASH_HASH_CONTAINER_H_INCLUDED__

#include "bucket_table.h"
#include "../associative_container.h"
#include "hash_table.h"
#include <utility>
#include <iostream>

namespace hash {

/**
 * Contenedor de elementos en una estructura de
 * hash extensible.
 */
template<typename K, typename T>
class hash_container : public container::associative_container<K, T> {

private:
	typedef container::associative_container<K, T> parent;
	bucket_table<K, T> buckets;
	hash_table<K> table;

	void handle_overflow(const bucket<K, T> overflow_bucket, int overflow_position);
public:

	/**
	 * Crea una nueva instancia de hash_container, cuyo
	 * archivo de datos es data_file, archivo de indexado
	 * es index_file y el tamaño de bloque del archivo de
	 * datos es block_size
	 */
	hash_container(const char *data_filename, const char *index_filename, int block_size);

	/**
	 * Agrega un elemento al contenedor. Valida si el
	 * elemento está en el contenedor, en cuyo caso eleva
	 * duplicate_exception.
	 */
	void add_element(const K &key, const T &value);

	/**
	 * Modifica un elemento existente en el contenedor.
	 * Valida que el elemento exista, en caso contrario
	 * eleva not_found_exception
	 */
	void update_element(const K &key, const T &value);

	/**
	 * Elimina un elemento existente en el contenedor.
	 * Valida que el elemento exista, en caso contrario
	 * eleva not_found_exception
	 */
	void delete_element(const K &key);

	/**
	 * Busca un elemento en el contenedor. Si el elemento
	 * existe, result.first será true, y result.second será
	 * una copia del elemento almacenado. En caso contrario,
	 * result.first será false.
	 */
	std::pair<bool, T> search_for_element(const K &key);

	/**
	 * Vuelca el contenido de la estructura de datos en
	 * un stream dado.
	 */
	virtual void dump_to_stream(std::ostream &output);

	/**
	 * Inspecciona todos los pares clave-valor almacenados en
	 * el contenedor asociativo utilizando la interface de inspección
	 * dada
	 */
	virtual void inspect(container::element_inspector<K, T> &inspector);

};

template<typename K, typename T>
hash_container<K, T>::hash_container(const char *data_filename, const char *index_filename, int block_size)
: buckets(data_filename, block_size),
  table(index_filename) {

}

template<typename K, typename T>
void hash_container<K, T>::add_element(const K &key, const T &value) {
	bool add_successful = false;

	while (!add_successful) {
		// Obtengo la posición en la que va el elemento
		int bucket_position = table.get_key_position(key);
		// Obtengo el bucket que está en esa posición
		bucket<K, T> actual_bucket = buckets.get_bucket(bucket_position);
		try {
			// Agrego el elemento en el bucket
			actual_bucket.add_element(key, value);
			// Guardo el bucket modificado
			buckets.save_bucket(bucket_position, actual_bucket);
			// Finalmente pudimos agregar el elemento sin
			// que haya overflow. Podemos salir del bucle
			add_successful = true;
		} catch (typename bucket<K, T>::overflow_exception &overflow) {
			handle_overflow(actual_bucket, bucket_position);
		} catch (typename bucket<K, T>::duplicate_exception &duplicate) {
			throw typename parent::duplicate_exception();
		}
	}
}

template<typename K, typename T>
void hash_container<K, T>::update_element(const K &key, const T &value) {
	bool update_successful = false;

	while(!update_successful) {
		// Obtengo la posición en la que tendría que estar
		// el elemento
		int bucket_position = table.get_key_position(key);
		// Obtengo el bucket de esa posición
		bucket<K, T> actual_bucket = buckets.get_bucket(bucket_position);
		try {
			// Updateo el bucket
			actual_bucket.update_element(key, value);
			// Guardo el bucket
			buckets.save_bucket(bucket_position, actual_bucket);
			update_successful = true;
		} catch (typename bucket<K, T>::overflow_exception &overflow) {
			handle_overflow(actual_bucket, bucket_position);
		} catch (typename bucket<K, T>::not_found_exception &not_found) {
			throw typename parent::not_found_exception();
		}
	}
}

template<typename K, typename T>
void hash_container<K, T>::delete_element(const K &key) {
	// Obtengo la posición en la que tendría que estar el
	// elemento
	int bucket_position = table.get_key_position(key);
	// Obtengo el bucket de esa posición
	bucket<K, T> actual_bucket = buckets.get_bucket(bucket_position);

	try {
		// Elimino el elemento
		actual_bucket.remove_element(key);
		// Si el bucket quedó vacio puede ser que pueda
		// eliminarlo, siempre que la tabla de dispersión
		// pueda reacomodarse para que ninguna entrada
		// apunte a él
		if (actual_bucket.is_empty()) {
			std::pair<bool,int> erasure_result = table.try_position_erasure(actual_bucket.get_hash_factor());
			// Si la tabla se reacomodó para que ninguna entrada
			// apunte al bucket
			if (erasure_result.first) {
				// Podemos borrar el bucket!
				buckets.remove_bucket(bucket_position);
				// Hay que actualizar el bucket al que reapuntaron
				// las entradas de la tabla porque ahora tiene el doble
				// de entradas apuntando a él
				bucket<K, T> remapped_bucket = buckets.get_bucket(erasure_result.second);
				remapped_bucket.set_hash_factor(remapped_bucket.get_hash_factor() / 2);
				buckets.save_bucket(erasure_result.second, remapped_bucket);
				return;
			}
		}

		// Si llegamos acá, no se puedo borrar el bucket,
		// así que lo guardamos así como está.
		buckets.save_bucket(bucket_position, actual_bucket);

	} catch (typename bucket<K, T>::not_found_exception &not_found) {
		throw typename parent::not_found_exception();
	}
}

template<typename K, typename T>
std::pair<bool, T> hash_container<K, T>::search_for_element(const K &key) {
	// Obtengo la posición en la que tendría que estar el
	// elemento
	int bucket_position = table.get_key_position(key);
	// Obtengo el bucket de esa posición
	bucket<K, T> actual_bucket = buckets.get_bucket(bucket_position);
	// Busco si el elemento está en el bucket
	// y lo devuelvo
	return actual_bucket.get_element(key);
}

template<typename K, typename T>
void hash_container<K, T>::dump_to_stream(std::ostream &output) {

	int number_of_elements = table.get_size();
	int i;
	output << "TABLA DE HASH " << std::endl;
	output << "----- -- ---- " << std::endl;
	output << std::endl;
	output << "Pos Tabla Hash TAB Pos Tabla de buckets a la que apunta " << std::endl;
	for ( i=0; i<number_of_elements; i++){
		int pos = table.get_table_position(i);

		output << "(" << i << "\t" << pos << ")" << std::endl;
	}
	output << std::endl;
	output << std::endl;
	output << "<Aclaracion: Los elementos que no se muestran en la tabla de buckets es porque han sido borrados> " << std::endl;
	output << "TABLA DE BUCKETS " << std::endl;
	output << "----- -- ------- " << std::endl;
	typedef typename bucket_table<K, T>::iterator bucket_table_iterator;
	typedef typename bucket<K, T>::iterator bucket_iterator;

	 for (bucket_table_iterator it = buckets.begin(); it != buckets.end(); it++) {
		std::pair<bucket<K, T>,int> b = *it;
		output << "Posicion:" << b.second << std::endl;
		output << "Factor de Hash:" << b.first.get_hash_factor() << std::endl;
		output << "---Elementos del bucket---: " << std::endl;
		for ( bucket_iterator it2 = b.first.begin(); it2!= b.first.end(); it2++) {
			std::pair<K, T> element = *it2;
			output << "(" << element.first << "," << element.second << ")" << std::endl;

		}

	}
}

template<typename K, typename T>
void hash_container<K, T>::inspect(container::element_inspector<K, T> &inspector) {
	typedef typename bucket_table<K, T>::iterator bucket_table_iterator;
	typedef typename bucket<K, T>::iterator bucket_iterator;

	for (bucket_table_iterator it = buckets.begin(); it != buckets.end(); it++) {
		std::pair<bucket<K, T>,int> b = *it;
		for ( bucket_iterator it2 = b.first.begin(); it2!= b.first.end(); it2++) {
				std::pair<K, T> element = *it2;
				inspector.inspect(element.first, element.second);
		}
	}
}

template<typename K, typename T>
void hash_container<K, T>::handle_overflow(const bucket<K, T> overflow_bucket, int overflow_position) {
	// Si hubo un overflow, tenemos que agregar un nuevo bucket
	std::pair<int, bucket<K, T> > new_bucket = std::make_pair(buckets.reserve_bucket(), overflow_bucket);
	new_bucket.second.clear();
	// Y reemplazar el bucket original dado que los contenidos van
	// a cambiar
	bucket<K, T> overflow_bucket_replacement = overflow_bucket;
	overflow_bucket_replacement.clear();
	// Si el factor de dispersión del bucket que tuvo overflow
	// es igual al tamaño de la tabla, hay que duplicar la tabla
	if (overflow_bucket.get_hash_factor() == table.get_size())
		table.grow();
	// Duplicamos los tamaños de dispersión
	overflow_bucket_replacement.set_hash_factor(overflow_bucket.get_hash_factor() * 2);
	new_bucket.second.set_hash_factor(overflow_bucket.get_hash_factor() * 2);
	// Actualizamos la entrada que pincho y sus hermanas para que
	// apunten al nuevo bucket
	table.remap_last_used_entry(new_bucket.first, overflow_bucket.get_hash_factor());
	// Y ahora rehasheo los contenidos, repartiendo entre
	// el reemplazo del bucket original y el nuevo bucket
	for (typename bucket<K, T>::iterator it = overflow_bucket.begin(); it != overflow_bucket.end(); it++) {
		std::pair<K, T> element = *it;
		if (table.get_key_position(element.first) == overflow_position) {
			overflow_bucket_replacement.add_element(element.first, element.second);
		} else {
			new_bucket.second.add_element(element.first, element.second);
		}
	}
	// Una vez fueron rehasheados los contenidos, guardo
	// los buckets
	buckets.save_bucket(overflow_position, overflow_bucket_replacement);
	buckets.save_bucket(new_bucket.first, new_bucket.second);
}

};

#endif // __HASH_HASH_CONTAINER_H_INCLUDED__
