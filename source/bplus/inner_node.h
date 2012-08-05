/******************************************************************************
 * inner_node.h
 * 		Declaraciones de la clase bplus::inner_node
******************************************************************************/
#ifndef __BPLUS_INNER_NODE_H_INCLUDED__
#define __BPLUS_INNER_NODE_H_INCLUDED__

#include "../commons/io/serializators.h"
#include "node_types.h"
#include "subtree.h"
#include <utility>
#include <stdexcept>
#include <vector>
#include <memory>

namespace bplus {

/**
 * Representa un nodo hoja en el archivo organizado
 * con un B+.
 */
template<typename K, typename T>
class inner_node : public subtree<K, T> {
private:
	commons::io::block inner_block;

	void set_free_index(int free_index);
	void increase_free_index(int amount);
	int get_free_index() const;

	void save_type();

	int get_element_start_index() const;

	int get_key_position(const K &key) const;
	void erase_key_and_pointer(const K &key);

	int get_subtree_pointer_for(const K &key) const;
	std::pair<K, int> get_left_brother_of(int pointer) const;
	std::pair<K, int> get_right_brother_of(int pointer) const;

	int get_rightmost_pointer() const;

	void handle_insertion_overflow(const K &key, int right_pointer, int total_required_size);

	void handle_children_overflow(subtree<K, T> *overflowded_node, commons::io::recycling_block_file *file, int subtree_pointer);
	bool handle_children_underflow(subtree<K, T> * node, int subtree_pointer, commons::io::recycling_block_file *file);

	void copy_elements(int from, int to, inner_node<K, T> *node);

	int get_optimal_splitting_position();
public:
	/**
	 * Representa un error elevado cuando se intenta trabajar
	 * con una clave inexistente
	 */
	class not_found_exception {};

	/**
	 * Representa un error elevado cuando se intenta agregar
	 * una clave que ya existía.
	 */
	class duplicate_exception {};

	/**
	 * Crea una nueva instancia de inner_node que trabaja
	 * como una vista de inner node sobre el bloque b.
	 * Realiza una copia de este bloque para operar
	 * internamente.
	 */
	explicit inner_node(const commons::io::block &b);

	/**
	 * Limpia el contenido de un inner_node, inicializando
	 * a su vez todas las estructuras de control del inner_node
	 * en el bloque y limpia los elementos que existan en el
	 * bloque. Este método se debe llamar la primera vez que
	 * un bloque empieza a utilizarse como inner_node.
	 */
	virtual void clear();

	/**
	 * Obtiene el puntero a izquierda de una clave dada
	 */
	virtual int get_left_pointer_of(const K &key) const;

	/**
	 * Obtiene el puntero a derecha de una clave dada
	 */
	virtual int get_right_pointer_of(const K &key) const;

	/**
	 * Obtiene una lista de todas las claves de este nodo
	 */
	virtual std::vector<K>get_keys() const;

	/**
	 * Inserta una clave con su puntero a derecha de manera
	 * ordenada dentro del nodo, dejando intactos el resto de
	 * los pares de clave y valor.
	 */
	virtual void insert_key_in_order(const K &key, int right_pointer);

	/**
	 * Establece el puntero más a la izquierda de todos los elementos
	 * Este puntero es el "first"
	 */
	virtual void set_leftmost_pointer(int pointer);

	/**
	 * Obtiene el primer puntero
	 */
	virtual int get_leftmost_pointer() const;

	/**
	 * Devuelve el bloque interno sobre el que opera este nodo
	 */
	virtual const commons::io::block &get_block() const;

	/**
	 * Devuelve un puntero al bloque interno sobre el que opera
	 * este nodo
	 */
	virtual commons::io::block *get_block_pointer();

	/**
	 * Override subtree
	 */
	virtual bool recursive_add_element(const K &key, const T &value, commons::io::recycling_block_file *file);

	/**
	 * Override subtree
	 */
	virtual bool recursive_update_element(const K &key, const T &value, commons::io::recycling_block_file *file);

	/**
	 * Override subtree
	 */
	virtual bool recursive_delete_element(const K &key, commons::io::recycling_block_file *file);

	/**
	 * Override subtree
	 */
	virtual std::pair<bool, T> recursive_search(const K &key, commons::io::recycling_block_file *file);

	/**
	 * Override subtree
	 */
	virtual void recursive_dump(std::ostream &output, commons::io::recycling_block_file *file, int space_count = 0);

	/**
	 * Override subtree
	 */
	virtual void recursive_inspect(container::element_inspector<K, T> &inspector, commons::io::recycling_block_file *file);

	/**
	 * Override subtree
	 */
	virtual typename subtree<K, T>::split_result split(int left_node_pointer, int right_node_pointer, int block_size);

	/**
	 * Override subtree
	 */
	virtual typename subtree<K, T>::merge_result try_merge(subtree<K, T> *left, subtree<K, T> *right, const K &middle_key);

	/**
	 * Override subtree
	 */
	virtual std::pair<int, subtree<K, T> *> try_replace_empty_element(commons::io::recycling_block_file *file);

	/**
	 * Override subtree
	 */
	virtual commons::io::block get_root_block() const;

	/**
	 * Override subtree
	 */
	virtual double get_load_factor();

	/**
	 * Override subtree
	 */
	virtual typename subtree<K, T>::balance_result try_balance(subtree<K, T> *left, subtree<K, T> *right, const K &key);
};
};


#include "node_factory.h"

namespace bplus {

template<typename K, typename T>
inner_node<K, T>::inner_node(const commons::io::block &b)
: inner_block(b) {

}

template<typename K, typename T>
void inner_node<K, T>::clear() {
	// Inicializo el control de espacio libre
	set_free_index(get_element_start_index());

	// Inicializo el identificador de tipo
	save_type();

	// Inicializo el puntero de más de la izquierda
	set_leftmost_pointer(-1);
}

template<typename K, typename T>
int inner_node<K, T>::get_left_pointer_of(const K &key) const {
	// Busco la posición de la clave
	int position = get_key_position(key);

	// Si no existía... estamos en problemas
	if (position <= 0) {
		throw not_found_exception();
	}

	// Si existía... leo el primer puntero
	// guardado a la izquierda de la clave
	int pointer_position = position - sizeof(int);
	return commons::io::deserialize<int>(inner_block, pointer_position);
}

template<typename K, typename T>
int inner_node<K, T>::get_right_pointer_of(const K &key) const {
	// Busco la posición de la clave
	int position = get_key_position(key);

	// Si no existía... estamos en problemas
	if (position <= 0) {
		throw not_found_exception();
	}

	// Si existía... leo el primer puntero
	// guardado a la derecha de la clave
	int pointer_position = position + commons::io::serialization_length<K>(inner_block, position);
	return commons::io::deserialize<int>(inner_block, pointer_position);
}

template<typename K, typename T>
std::vector<K> inner_node<K, T>::get_keys() const {
	// Lleno el vector con todos las claves
	std::vector<K>result;
	for (
			int current_position = get_element_start_index();
			current_position < get_free_index();
			current_position += sizeof(int) + commons::io::serialization_length<K>(inner_block, current_position)) {
		// Leo la clave en la posición actual y la meto en el resultado
		result.push_back(commons::io::deserialize<K>(inner_block, current_position));
	}
	return result;
}

template<typename K, typename T>
void inner_node<K, T>::insert_key_in_order(const K &key, int right_pointer) {
	int new_pair_size = sizeof(int) + commons::io::serialization_length(key);

	// Primero me fijo si insertar este elemento no
	// causaría un overflow
	if (new_pair_size + get_free_index() > inner_block.get_size()) {
		handle_insertion_overflow(key, right_pointer, new_pair_size + get_free_index());
	}

	// Ahora recorro todas las claves, buscando la primera
	// que sea superior a la que estoy intentando agregar
	int current_position = 0;
	for (
			current_position = get_element_start_index();
			current_position < get_free_index();
			current_position += sizeof(int) + commons::io::serialization_length<K>(inner_block, current_position)) {
		// Deserializo la clave
		K element_key = commons::io::deserialize<K>(inner_block, current_position);

		// Si la clave es la misma que la que estoy intentando
		// usar... we are screwed!
		if (element_key == key)
			throw duplicate_exception();

		// Si la clave es mayor a la que estoy intentando agregar
		if (element_key > key) {
			// Tengo que meter mi clave justo antes de esta clave,
			// así que hago espacio para que entre y salgo del bucle.
			inner_block.make_gap(current_position, new_pair_size);
			break;
		}
	}

	// Ahora que tengo el lugar donde tengo que meter la clave y su
	// puntero, y que ya tengo espacio suficiente para hacerlo, escribo
	commons::io::serialize(key, &inner_block, current_position);

	current_position += commons::io::serialization_length(key);

	commons::io::serialize(right_pointer, &inner_block, current_position);

	// Actualizo el puntero a espacio libre
	increase_free_index(new_pair_size);
}

template<typename K, typename T>
void inner_node<K, T>::set_leftmost_pointer(int pointer) {
	commons::io::serialize(pointer, &inner_block, (2 * sizeof(int)));
}

template<typename K, typename T>
int inner_node<K, T>::get_leftmost_pointer() const {
	return commons::io::deserialize<int>(inner_block, 2 * sizeof(int));
}

template<typename K, typename T>
const commons::io::block &inner_node<K, T>::get_block() const {
	return inner_block;
}

template<typename K, typename T>
commons::io::block *inner_node<K, T>::get_block_pointer() {
	return &inner_block;
}

template<typename K, typename T>
bool inner_node<K, T>::recursive_add_element(const K &key, const T &value, commons::io::recycling_block_file *file) {
	// Busco el puntero en el que debería ir el elemento
	int subtree_pointer = get_subtree_pointer_for(key);

	try {
		// Levanto el bloque del archivo y lo interpreto
		commons::io::block b = file->read_block(subtree_pointer);
		std::auto_ptr<subtree<K, T> > node(node_factory::create_subtree_from_block<K, T>(b));

		// Recursivamente agrego el elemento a este nodo
		bool modified = node->recursive_add_element(key, value, file);

		// Y lo grabo, si es necesario
		if (modified)
			file->write_block(subtree_pointer, node->get_root_block());

		return false;
	} catch (typename subtree<K, T>::root_overflow_exception &o) {
		handle_children_overflow(o.get_overflowded_subtree(), file, subtree_pointer);
		return true;
	}
}

template<typename K, typename T>
bool inner_node<K, T>::recursive_update_element(const K &key, const T &value, commons::io::recycling_block_file *file) {
	// Busco el puntero en el que debería estar el elemento
	int subtree_pointer = get_subtree_pointer_for(key);

	try {
		// Levanto el bloque del archivo y lo interpreto
		commons::io::block b = file->read_block(subtree_pointer);
		std::auto_ptr<subtree<K, T> > node(node_factory::create_subtree_from_block<K, T>(b));

		// Lo modifico y lo grabo si es necesario
		if (node->recursive_update_element(key, value, file)) {

			if (node->get_load_factor() < 50.0) {
				if (handle_children_underflow(&*node, subtree_pointer, file))
					return true;
			}

			file->write_block(subtree_pointer, node->get_root_block());
		}

		return false;
	} catch (typename subtree<K, T>::root_overflow_exception &o) {
		handle_children_overflow(o.get_overflowded_subtree(), file, subtree_pointer);
		return true;
	}
}

template<typename K, typename T>
bool inner_node<K, T>::recursive_delete_element(const K &key, commons::io::recycling_block_file *file) {
	// Busco el puntero en el que debería ir el elemento
	int subtree_pointer = get_subtree_pointer_for(key);

	// Levanto el bloque del archivo y lo interpreto
	commons::io::block b = file->read_block(subtree_pointer);
	std::auto_ptr<subtree<K, T> > node(node_factory::create_subtree_from_block<K, T>(b));

	// Recursivamente elimino el elemento del nodo
	if (node->recursive_delete_element(key, file)) {

		// Si quedé en underflow, tengo que intentar arreglarlo
		if (node->get_load_factor() < 50.0) {
			if (handle_children_underflow(&*node, subtree_pointer, file))
				return true;
		}

		file->write_block(subtree_pointer, node->get_root_block());
	}

	return false;
}

template<typename K, typename T>
std::pair<bool, T> inner_node<K, T>::recursive_search(const K &key, commons::io::recycling_block_file *file) {
	// Busco el puntero en el que debería ir el elemento
	int subtree_pointer = get_subtree_pointer_for(key);

	// Levanto el bloque del archivo y lo interpreto
	commons::io::block b = file->read_block(subtree_pointer);
	std::auto_ptr<subtree<K, T> > node(node_factory::create_subtree_from_block<K, T>(b));

	// Busco el elemento en ese nodo
	return node->recursive_search(key, file);
}

template<typename K, typename T>
void inner_node<K, T>::recursive_dump(std::ostream &output, commons::io::recycling_block_file *file, int space_count) {
	output << "INNER (" << get_load_factor() << "%)" << std::endl;
	std::vector<K>keys = get_keys();

	for (typename std::vector<K>::iterator it = keys.begin(); it != keys.end(); it++) {
		output << std::string(space_count + 2, ' ') <<
				"[" << get_left_pointer_of(*it) << "] (" << *it << ") [" << get_right_pointer_of(*it) << "]" << std::endl;
	}

	output << std::endl;
	commons::io::block b = file->read_block(get_leftmost_pointer());
	std::auto_ptr<subtree<K, T> > leftmost_node(node_factory::create_subtree_from_block<K, T>(b));
	output << std::string(space_count + 4, ' ') << get_leftmost_pointer() << ": ";
	leftmost_node->recursive_dump(output, file, space_count + 4);

	for (typename std::vector<K>::iterator it = keys.begin(); it != keys.end(); it++) {
		b = file->read_block(get_right_pointer_of(*it));
		std::auto_ptr<subtree<K, T> > node(node_factory::create_subtree_from_block<K, T>(b));
		output << std::string(space_count + 4, ' ') << get_right_pointer_of(*it) << ": ";
		node->recursive_dump(output, file, space_count + 4);
	}
}

template<typename K, typename T>
void inner_node<K, T>::recursive_inspect(container::element_inspector<K, T> &inspector, commons::io::recycling_block_file *file) {
	std::vector<K>keys = get_keys();
	commons::io::block b = file->read_block(get_leftmost_pointer());
	std::auto_ptr<subtree<K, T> > leftmost_node(node_factory::create_subtree_from_block<K, T>(b));
	leftmost_node->recursive_inspect(inspector, file);
	for (typename std::vector<K>::iterator it = keys.begin(); it != keys.end(); it++) {
		b = file->read_block(get_right_pointer_of(*it));
		std::auto_ptr<subtree<K, T> > node(node_factory::create_subtree_from_block<K, T>(b));
		node->recursive_inspect(inspector, file);
	}
}

template<typename K, typename T>
typename subtree<K, T>::split_result inner_node<K, T>::split(int left_node_pointer, int right_node_pointer, int block_size) {
	// Busco la posición "más mejor" para partir el nodo
	int optimal_position = get_optimal_splitting_position();

	// Creo los resultados
	inner_node<K, T> *left_node = new inner_node<K, T>(commons::io::block(block_size));
	inner_node<K, T> *right_node = new inner_node<K, T>(commons::io::block(block_size));

	// Inicializo los nodos
	left_node->clear();
	right_node->clear();

	// Inicializo el left-most pointer del de la izquierda
	left_node->set_leftmost_pointer(get_leftmost_pointer());

	// Copiamos todos los elementos en los nodos, menos la clave que
	// está en el medio, que será promovida.
	copy_elements(get_element_start_index(), optimal_position, left_node);
	copy_elements(optimal_position + sizeof(int) + commons::io::serialization_length<K>(inner_block, optimal_position), get_free_index(), right_node);

	// Deserializo el par en la posición óptima
	K promoted_key = commons::io::deserialize<K>(inner_block, optimal_position);
	int promoted_pointer = commons::io::deserialize<int>(inner_block, optimal_position + commons::io::serialization_length(promoted_key));

	// El nodo derecho se queda con el puntero
	right_node->set_leftmost_pointer(promoted_pointer);

	return typename subtree<K, T>::split_result(left_node, right_node, promoted_key);
}

template<typename K, typename T>
typename subtree<K, T>::merge_result inner_node<K, T>::try_merge(subtree<K, T> *left, subtree<K, T> *right, const K &middle_key) {
	// Se que estoy mergeando inners
	inner_node<K, T> *left_node = dynamic_cast<inner_node<K, T> *>(left);
	inner_node<K, T> *right_node = dynamic_cast<inner_node<K, T> *>(right);

	// Primero chequeo que cuando mergee no haya overflow
	// Necesito poner todos los de la izquierda, todos los
	// de la derecha y además el middle key
	int bytes_required_left = left_node->get_free_index() - left_node->get_element_start_index();
	int bytes_required_right = right_node->get_free_index() - right_node->get_element_start_index();
	int bytes_required_middlekey = sizeof(int) + commons::io::serialization_length(middle_key);
	if ((bytes_required_left + bytes_required_right + bytes_required_middlekey) >= (inner_block.get_size() - left_node->get_element_start_index()))
		return typename subtree<K, T>::merge_result(false, NULL);

	// Si no hay overflow, creo el nodo resultante
	inner_node<K, T> *merged = new inner_node<K, T>(commons::io::block(inner_block.get_size()));
	merged->clear();

	// El leftmost pointer es el mismo que estaba en el nodo izquierdo
	merged->set_leftmost_pointer(left_node->get_leftmost_pointer());

	// Copio todos los elementos del nodo izquierdo
	left_node->copy_elements(left_node->get_element_start_index(), left_node->get_free_index(), merged);

	// Inserto la clave del medio
	merged->insert_key_in_order(middle_key, right_node->get_leftmost_pointer());

	// Copio todos los elementos del nodo derecho
	right_node->copy_elements(right_node->get_element_start_index(), right_node->get_free_index(), merged);

	return typename subtree<K, T>::merge_result(true, merged);
}

template<typename K, typename T>
std::pair<int, subtree<K, T> *> inner_node<K, T>::try_replace_empty_element(commons::io::recycling_block_file *file) {
	// Puedo reemplazar a este nodo si no hay elementos
	if (get_free_index() == get_element_start_index()) {
		// Obtengo el único nodo hijo
		int only_child_pointer = get_leftmost_pointer();
		commons::io::block b = file->read_block(only_child_pointer);
		subtree<K, T> *only_child = node_factory::create_subtree_from_block<K, T>(b);
		return std::make_pair(only_child_pointer, only_child);

	} else {
		return std::make_pair(0, (subtree<K, T> *)NULL);
	}
}

template<typename K, typename T>
commons::io::block inner_node<K, T>::get_root_block() const {
	return inner_block;
}

template<typename K, typename T>
double inner_node<K, T>::get_load_factor(){
	int used = get_free_index() - get_element_start_index();
	int total = inner_block.get_size() - get_element_start_index();
	double factor = (double) used * 100 / total;
	return factor;
}

template<typename K, typename T>
typename subtree<K, T>::balance_result inner_node<K, T>::try_balance(subtree<K, T> *left, subtree<K, T> *right, const K &key){
	//Creo el nuevo node, copia del que tengo teniendo en cuenta lo usado en ambos.
	int size = inner_block.get_size();
	int estimated_size =
			((left->get_load_factor() + right->get_load_factor()) * size / 100)
			+ get_element_start_index() + sizeof(int) + commons::io::serialization_length(key);

	inner_node<K, T> *inner_balanced = new inner_node<K, T>(commons::io::block(estimated_size));

	///Inicializo el nuevo nodo
	inner_balanced->clear();

	//Se que es un hermano asi que puedo
	//castearlo al mismo tipo que tengo
	inner_node<K, T> *left_inner_brother = dynamic_cast<inner_node<K, T> *>(left);
	inner_node<K, T> *right_inner_brother = dynamic_cast<inner_node<K, T> *>(right);

	//Le seteo el first del izquierdo
	inner_balanced->set_leftmost_pointer(left_inner_brother->get_leftmost_pointer());

	//Copio los que estan en el inner y right subtree
	for (
			int current_position = left_inner_brother->get_element_start_index();
			current_position < left_inner_brother->get_free_index();
			current_position += sizeof(int) + commons::io::serialization_length<K>(left_inner_brother->inner_block, current_position)) {

		K current_key = commons::io::deserialize<K>(left_inner_brother->get_block(), current_position);
		int current_pointer = commons::io::deserialize<int>(left_inner_brother->get_block(), current_position + commons::io::serialization_length(current_key));
		inner_balanced->insert_key_in_order(current_key, current_pointer);
	}
	inner_balanced->insert_key_in_order(key, right_inner_brother->get_leftmost_pointer());
	for (
			int current_position = right_inner_brother->get_element_start_index();
			current_position < right_inner_brother->get_free_index();
			current_position += sizeof(int) + commons::io::serialization_length<K>(right_inner_brother->inner_block, current_position)) {
		K current_key = commons::io::deserialize<K>(right_inner_brother->get_block(), current_position);
		int current_pointer = commons::io::deserialize<int>(right_inner_brother->get_block(), current_position + commons::io::serialization_length(current_key));
		inner_balanced->insert_key_in_order(current_key, current_pointer);
	}

	//Split de lo que tengo en el inner balanceado
	typename subtree<K, T>::split_result result = inner_balanced->split(0, 0, size);

	double left_factor = result.left_node->get_load_factor();
	double right_factor = result.right_node->get_load_factor();
	//Evaluo si sus hijos quedan en underflow
	if((right_factor < 50) || (left_factor < 50))
		return typename subtree<K, T>::balance_result();

	//Devuelvo el resultado balanceado
	return typename subtree<K, T>::balance_result(true,result);
}

template<typename K, typename T>
void inner_node<K, T>::set_free_index(int free_index) {
	commons::io::serialize<int>(free_index, &inner_block, sizeof(int));
}

template<typename K, typename T>
void inner_node<K, T>::increase_free_index(int amount) {
	set_free_index(get_free_index() + amount);
}

template<typename K, typename T>
int inner_node<K, T>::get_free_index() const {
	return commons::io::deserialize<int>(inner_block, sizeof(int));
}

template<typename K, typename T>
void inner_node<K, T>::save_type() {
	commons::io::serialize<int>(inner_node_type, &inner_block, 0);
}

template<typename K, typename T>
int inner_node<K, T>::get_element_start_index() const {
	return sizeof(int) * 3;
}

template<typename K, typename T>
int inner_node<K, T>::get_key_position(const K &key) const {
	// Recorro todas las claves
	for (
			int current_position = get_element_start_index();
			current_position < get_free_index();
			current_position += sizeof(int) + commons::io::serialization_length<K>(inner_block, current_position)) {
		// Levanto la clave en la que estoy parado
		K current_key = commons::io::deserialize<K>(inner_block, current_position);

		// Si es la que estoy buscando... ya estamos!
		if (current_key == key)
			return current_position;
	}

	// No encontramos la clave
	return -1;
}

template<typename K, typename T>
void inner_node<K, T>::erase_key_and_pointer(const K &key) {
	// Busco la posición de la clave que quiero borrar
	int key_pointer = get_key_position(key);

	// Borro y compacto el bloque
	int element_length = sizeof(int) + commons::io::serialization_length<K>(inner_block, key_pointer);
	inner_block.erase_and_compact(key_pointer, key_pointer + element_length - 1);

	// Actualizo el free index
	increase_free_index(-element_length);
}

template<typename K, typename T>
int inner_node<K, T>::get_subtree_pointer_for(const K &key) const {
	// Busco la primer clave que sea mayor a la que estoy
	// intentando agregar
	for (
			int current_position = get_element_start_index();
			current_position < get_free_index();
			current_position += sizeof(int) + commons::io::serialization_length<K>(inner_block, current_position)) {
		// Deserializo la clave
		K element_key = commons::io::deserialize<K>(inner_block, current_position);

		// Si es mayor... el elemento va en el nodo apuntado
		// por el puntero inmediatamente anterior
		if (element_key > key) {
			return commons::io::deserialize<int>(inner_block, current_position - sizeof(int));
		}
	}

	// Si ninguna clave es mayor... entonces tenemos que
	// agregarlo en el el nodo apuntado por el último puntero
	return commons::io::deserialize<int>(inner_block, get_free_index() - sizeof(int));
}

template<typename K, typename T>
std::pair<K, int> inner_node<K, T>::get_left_brother_of(int pointer) const {
	// Busco el puntero que me pasaron. Como precondición, se que no es el primer puntero
	int last_position = 0;
	int current_position = get_element_start_index();
	while (current_position < get_free_index()) {
		last_position = current_position;
		current_position += sizeof(int) + commons::io::serialization_length<K>(inner_block, current_position);

		// Obtengo el puntero actual
		int current_pointer = commons::io::deserialize<int>(inner_block, current_position - sizeof(int));

		// Si es el puntero actual, devuelvo los datos desde ahí
		if (current_pointer == pointer) {
			int left_pointer = commons::io::deserialize<int>(inner_block, last_position - sizeof(int));
			K left_key = commons::io::deserialize<K>(inner_block, last_position);

			return std::make_pair(left_key, left_pointer);
		}
	}

	return std::make_pair(K(), 0);
}

template<typename K, typename T>
std::pair<K, int> inner_node<K, T>::get_right_brother_of(int pointer) const {
	// Busco la ubicación del puntero que me pasaron, arrancando desde el leftmost
	for (
			int current_position = get_element_start_index() - sizeof(int);
			current_position < get_free_index();
			current_position += sizeof(int) + commons::io::serialization_length<K>(inner_block, current_position + sizeof(int))) {
		// Obtengo el puntero actual
		int current_pointer = commons::io::deserialize<int>(inner_block, current_position);

		// Si es el puntero que me pasaron, devuelvo el hermano derecho
		if (current_pointer == pointer) {
			int right_pointer_position =
					current_position +
					sizeof(int) +
					commons::io::serialization_length<K>(inner_block, current_position + sizeof(int));

			int right_pointer =  commons::io::deserialize<int>(inner_block, right_pointer_position);
			K right_key = commons::io::deserialize<K>(inner_block, current_position + sizeof(int));

			return std::make_pair(right_key, right_pointer);
		}
	}

	return std::make_pair(K(), 0);
}

template<typename K, typename T>
int inner_node<K, T>::get_rightmost_pointer() const {
	int last_pointer_position = get_free_index() - sizeof(int);

	return commons::io::deserialize<int>(inner_block, last_pointer_position);
}

template<typename K, typename T>
void inner_node<K, T>::handle_insertion_overflow(const K &key, int right_pointer, int total_required_size) {
	// Creo un nuevo inner_node en el que entre el par clave-rightpointer
	inner_node<K, T> *n = new inner_node<K, T>(commons::io::block(total_required_size));

	// Copio el estado del nodo actual
	inner_block.copy_to_block(*n->get_block_pointer());

	// Inserto el par clave-rightpointer en el nuevo nodo
	n->insert_key_in_order(key, right_pointer);

	// Disparo la excepción de overflow
	throw typename subtree<K, T>::root_overflow_exception(n);
}

template<typename K, typename T>
void inner_node<K, T>::handle_children_overflow(subtree<K, T> *overflowded_node, commons::io::recycling_block_file *file, int subtree_pointer) {
	// Obtengo el nodo en overflow
	std::auto_ptr<subtree<K, T> > auto_node_deleter(overflowded_node);

	// Hay que dividir el nodo en dos. Uno de los nuevos
	// nodos va a ir a parar a una nueva posición en el
	// archivo, así que reservo una posición
	int new_subtree_pointer = file->reserve_block();

	// Divido el nodo a la mitad
	typename subtree<K, T>::split_result s = overflowded_node->split(subtree_pointer, new_subtree_pointer, file->get_block_size());
	std::auto_ptr<subtree<K, T> > left_split(s.left_node);
	std::auto_ptr<subtree<K, T> > right_split(s.right_node);

	// Grabo cada mitad en la posición que corresponda
	file->write_block(subtree_pointer, left_split->get_root_block());
	file->write_block(new_subtree_pointer, right_split->get_root_block());

	// Agrego la clave del medio a este nodo
	insert_key_in_order(s.middle_key, new_subtree_pointer);
}

template<typename K, typename T>
bool inner_node<K, T>::handle_children_underflow(subtree<K, T> * node, int subtree_pointer, commons::io::recycling_block_file *file) {
	// Si no es el elemento de más de la izquierda
	// intento balancear a izquierda
	if (get_leftmost_pointer() != subtree_pointer) {
		std::pair<K, int> left_brother_info = get_left_brother_of(subtree_pointer);
		commons::io::block left_brother_block = file->read_block(left_brother_info.second);
		std::auto_ptr<subtree<K, T> > left_brother_node(node_factory::create_subtree_from_block<K, T>(left_brother_block));

		typename subtree<K, T>::balance_result b = node->try_balance(&*left_brother_node, node, left_brother_info.first);

		if (b.success) {
			file->write_block(left_brother_info.second, b.result.left_node->get_root_block());
			file->write_block(subtree_pointer, b.result.right_node->get_root_block());
			erase_key_and_pointer(left_brother_info.first);
			insert_key_in_order(b.result.middle_key, subtree_pointer);
			return true;
		}
	}

	// Si no es el elemento de más de la derecha
	// intento balancear a derecha
	if (get_rightmost_pointer() != subtree_pointer) {
		std::pair<K, int> right_brother_info = get_right_brother_of(subtree_pointer);
		commons::io::block right_brother_block = file->read_block(right_brother_info.second);
		std::auto_ptr<subtree<K, T> > right_brother_node(node_factory::create_subtree_from_block<K, T>(right_brother_block));

		typename subtree<K, T>::balance_result b = node->try_balance(node, &*right_brother_node, right_brother_info.first);

		if (b.success) {
			file->write_block(subtree_pointer, b.result.left_node->get_root_block());
			file->write_block(right_brother_info.second, b.result.right_node->get_root_block());
			erase_key_and_pointer(right_brother_info.first);
			insert_key_in_order(b.result.middle_key,right_brother_info.second);
			return true;
		}
	}

	// Si no es el elemento de más de la izquierda
	// intento mergear a izquierda
	if (get_leftmost_pointer() != subtree_pointer) {
		std::pair<K, int> left_brother_info = get_left_brother_of(subtree_pointer);
		commons::io::block left_brother_block = file->read_block(left_brother_info.second);
		std::auto_ptr<subtree<K, T> > left_brother_node(node_factory::create_subtree_from_block<K, T>(left_brother_block));

		typename subtree<K, T>::merge_result r = node->try_merge(&*left_brother_node, node, left_brother_info.first);

		if (r.success) {
			file->write_block(left_brother_info.second, r.merged_node->get_root_block());
			file->release_block(subtree_pointer);
			erase_key_and_pointer(left_brother_info.first);
			return true;
		}
	}

	// Si no es el elemento de más de la derecha
	// intento mergear a derecha
	if (get_rightmost_pointer() != subtree_pointer) {
		std::pair<K, int> right_brother_info = get_right_brother_of(subtree_pointer);
		commons::io::block right_brother_block = file->read_block(right_brother_info.second);
		std::auto_ptr<subtree<K, T> > right_brother_node(node_factory::create_subtree_from_block<K, T>(right_brother_block));

		typename subtree<K, T>::merge_result r = node->try_merge(node, &*right_brother_node, right_brother_info.first);

		if (r.success) {
			file->write_block(subtree_pointer, r.merged_node->get_root_block());
			file->release_block(right_brother_info.second);
			erase_key_and_pointer(right_brother_info.first);
			return true;
		}
	}

	return false;
}

template<typename K, typename T>
void inner_node<K, T>::copy_elements(int from, int to, inner_node<K, T> *node) {
	for (int i = from; i < to; i += sizeof(int) + commons::io::serialization_length<K>(inner_block, i)) {
		K key = commons::io::deserialize<K>(inner_block, i);
		int pointer = commons::io::deserialize<int>(inner_block, i + sizeof(int));

		node->insert_key_in_order(key, pointer);
	}
}

template<typename K, typename T>
int inner_node<K, T>::get_optimal_splitting_position() {
	// Busco la posición ideal en donde partir, que dejaría
	// el 50% cargado cada nodo
	int halfway_position = (get_free_index() - get_element_start_index()) / 2 + get_element_start_index();

	// Ahora recorro los elementos buscando el elemento en el que recae
	// la posición óptima.
	int previous_position = 0;
	int current_position = get_element_start_index();
	do
	{
		previous_position = current_position;
		current_position += sizeof(int) + commons::io::serialization_length<K>(inner_block, current_position);

		if (previous_position <= halfway_position && current_position >= halfway_position)
			break;
	} while (current_position < get_free_index());

	// Devuelvo la posición izquierda, porque la clave se promueve
	return previous_position;
}

};

#endif
