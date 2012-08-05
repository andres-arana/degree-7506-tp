/******************************************************************************
 * bplus_container.h
 * 		Declaraciones y definiciones de la clase bplus::bplus_container
******************************************************************************/
#ifndef __BPLUS_BPLUS_CONTAINER_H_INCLUDED__
#define __BPLUS_BPLUS_CONTAINER_H_INCLUDED__

#include "../commons/io/recycling_block_file.h"
#include "../associative_container.h"
#include "subtree.h"
#include "inner_node.h"
#include "leaf_node.h"
#include "node_factory.h"

namespace bplus{

/**
 * Contenedor de elementos en una estructura
 * de árbol B+
 */
template<typename K, typename T>
class bplus_container : public container::associative_container<K, T> {
private:
	typedef container::associative_container<K, T> parent;

	commons::io::recycling_block_file file;

	struct block_file_initializer : public commons::io::recycling_block_file::initializer {
		void initialize(commons::io::recycling_block_file *file) const;
	};

	typedef subtree<K, T> subtree_type;
	typedef leaf_node<K, T> leaf_node_type;
	typedef inner_node<K, T> inner_node_type;

	subtree_type *root_node;

	void handle_children_overflow(subtree_type *overflowded_node);
	void try_replace_root();
	subtree_type *interpret_block(const commons::io::block &b) const;
public:

	/**
	 * Crea una nueva instancia de bplus_container
	 * en el archivo de datos filename, con un tamaño
	 * de bloque block_size
	 */
	explicit bplus_container(const char *filename, int block_size);

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
	typedef std::pair<bool, T>search_result_type;
	search_result_type search_for_element(const K &key);

	/**
	 * Vuelca el contenido de la estructura de datos en
	 * un stream dado.
	 */
	void dump_to_stream(std::ostream &output);

	/**
	 * Inspecciona todos los pares clave-valor almacenados en
	 * el contenedor asociativo utilizando la interface de inspección
	 * dada
	 */
	virtual void inspect(container::element_inspector<K, T> &inspector);

	~bplus_container();
};


template<typename K, typename T>
void bplus_container<K, T>::block_file_initializer::initialize(commons::io::recycling_block_file *file) const {
	leaf_node_type starting_node(commons::io::block(file->get_block_size()));
	starting_node.clear();
	file->append_block(starting_node.get_root_block());
}

template<typename K, typename T>
bplus_container<K, T>::bplus_container(const char *filename, int block_size)
: file(filename, block_size, block_file_initializer()) {
	commons::io::block root_node_block = file.read_block(0);
	root_node = interpret_block(root_node_block);
}

template<typename K, typename T>
void bplus_container<K, T>::add_element(const K &key, const T &value) {
	try {
		if (root_node->recursive_add_element(key, value, &file)) {
			file.write_block(0, root_node->get_root_block());
		}
	} catch (typename subtree_type::root_overflow_exception &o) {
		handle_children_overflow(o.get_overflowded_subtree());
	} catch (typename leaf_node_type::duplicate_exception &d) {
		throw typename parent::duplicate_exception();
	}
}

template<typename K, typename T>
void bplus_container<K, T>::update_element(const K &key, const T &value) {
	try {
		if (root_node->recursive_update_element(key, value, &file)) {
			// Intento reemplazar la raiz si está en overflow
			try_replace_root();

			// Guardo la raiz
			file.write_block(0, root_node->get_root_block());
		}
	} catch (typename subtree_type::root_overflow_exception &o) {
		handle_children_overflow(o.get_overflowded_subtree());
	} catch (typename leaf_node_type::not_found_exception &d) {
		throw typename parent::not_found_exception();
	}
}

template<typename K, typename T>
void bplus_container<K, T>::delete_element(const K &key) {
	try {
		if (root_node->recursive_delete_element(key, &file)) {
			// Intento reemplazar la raiz si está en overflow
			try_replace_root();

			// Guardo la raiz
			file.write_block(0, root_node->get_root_block());
		}
	} catch (typename leaf_node_type::not_found_exception &e) {
		throw typename parent::not_found_exception();
	}
}

template<typename K, typename T>
typename bplus_container<K, T>::search_result_type bplus_container<K, T>::search_for_element(const K &key) {
	return root_node->recursive_search(key, &file);
}

template<typename K, typename T>
void bplus_container<K, T>::dump_to_stream(std::ostream &output) {
	output << "0: ";
	root_node->recursive_dump(output, &file, 0);
}

template<typename K, typename T>
void bplus_container<K, T>::inspect(container::element_inspector<K, T> &inspector) {
	root_node->recursive_inspect(inspector, &file);
}

template<typename K, typename T>
bplus_container<K, T>::~bplus_container() {
	delete root_node;
}

template<typename K, typename T>
void bplus_container<K, T>::handle_children_overflow(subtree_type *overflowded_node) {
	// Obtengo el nodo en overflow
	std::auto_ptr<subtree_type> overflowded_node_deleter(overflowded_node);

	// Hay que dividir el nodo en tres. Dos de los nuevos
	// nodos va a ir a parar a nuevas posiciones en el
	// archivo, así que reservo las posiciones
	int left_subtree_pointer = file.reserve_block();
	int right_subtree_pointer = file.reserve_block();

	// Divido el nodo a la mitad
	typename subtree_type::split_result s = overflowded_node->split(left_subtree_pointer, right_subtree_pointer, file.get_block_size());
	std::auto_ptr<subtree_type> left_split(s.left_node);
	std::auto_ptr<subtree_type> right_split(s.right_node);

	// Grabo cada mitad en la posición que corresponda
	file.write_block(left_subtree_pointer, left_split->get_root_block());
	file.write_block(right_subtree_pointer, right_split->get_root_block());

	// Cuando la raiz se divide, siempre hay que reemplazarla
	// con un nuevo inner node
	inner_node_type *new_root = new inner_node_type(commons::io::block(file.get_block_size()));
	new_root->clear();
	new_root->set_leftmost_pointer(left_subtree_pointer);
	new_root->insert_key_in_order(s.middle_key, right_subtree_pointer);
	file.write_block(0, new_root->get_root_block());

	// Reemplazo la raiz actual
	delete root_node;
	root_node = new_root;
}

template<typename K, typename T>
void bplus_container<K, T>::try_replace_root() {
	// Veo si puedo reemplazar la raiz por alguno de sus hijos
	// (por ejemplo, porque la raiz quedó vacía)
	std::pair<int, subtree_type*> r = root_node->try_replace_empty_element(&file);

	// Si se puede reemplazar
	if (r.second != NULL) {
		// Reemplazo mi raiz
		delete root_node;
		root_node = r.second;

		// Limpio el bloque en el que estaba el nodo que
		// reemplazó la raiz
		file.release_block(r.first);
	}
}

template<typename K, typename T>
typename bplus_container<K, T>::subtree_type *bplus_container<K, T>::interpret_block(const commons::io::block &b) const {
	return node_factory::create_subtree_from_block<K, T>(b);
}

};

#endif
