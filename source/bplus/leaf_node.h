/******************************************************************************
 * leaf_node.h
 * 		Declaraciones y definiciones de la clase bplus::leaf_node
******************************************************************************/
#ifndef __BPLUS_LEAF_NODE_H_INCLUDED__
#define __BPLUS_LEAF_NODE_H_INCLUDED__

#include "../commons/io/element_container_block.h"
#include "../commons/io/serializators.h"
#include "node_types.h"
#include "subtree.h"
#include <utility>
#include <stdexcept>

namespace bplus {

/**
 * Representa un nodo hoja en el archivo organizado
 * con un B+.
 */
template<typename K, typename T>
class leaf_node : public commons::io::element_container_block<K, T>, public subtree<K, T> {
private:
	int get_optimal_splitting_position();

	void save_type();
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
public:
	/**
	 * Crea una nueva instancia de leaf_node que trabaja
	 * como una vista de leaf node sobre el bloque b.
	 * Realiza una copia de este bloque para operar
	 * internamente.
	 */
	explicit leaf_node(const commons::io::block &b);

	/**
	 * Limpia el contenido de un leaf_node, inicializando
	 * a su vez todas las estructuras de control del leaf_node
	 * en el bloque como el puntero la siguiente oja y limpia los
	 * elementos que existan en el bloque. Este método se debe
	 * llamar la primera vez que un bloque empieza a utilizarse
	 * como leaf_node.
	 */
	virtual void clear();

	/**
	 * Obtiene el número de bloque en el que está la
	 * siguiente hoja a la que apunta este nodo.
	 */
	int get_next_leaf_pointer() const;

	/**
	 * Establece el número de bloque en el que está la
	 * siguiente hoja a la que apunta este nodo.
	 */
	void set_next_leaf_pointer(int leaf_pointer);

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

template<typename K, typename T>
leaf_node<K, T>::leaf_node(const commons::io::block &b)
: parent(b) {

}

template<typename K, typename T>
void leaf_node<K, T>::clear() {
	// Inicializo el factor de dispersión
	set_next_leaf_pointer(-1);

	// Inicializo el identificador de tipo
	save_type();

	// Inicializo los elementos
	parent::clear_elements();
}

template<typename K, typename T>
int leaf_node<K, T>::get_next_leaf_pointer() const {
	return commons::io::deserialize<int>(parent::get_inner_block(), sizeof(int));
}

template<typename K, typename T>
void leaf_node<K, T>::set_next_leaf_pointer(int leaf_pointer) {
	commons::io::serialize(leaf_pointer, parent::get_inner_block_pointer(), static_cast<int>(sizeof(int)));
}

template<typename K, typename T>
bool leaf_node<K, T>::recursive_add_element(const K &key, const T &value, commons::io::recycling_block_file *file) {
	// Cuando se está agregando recursivamente un elemento,
	// y la llamada recursiva llega a una hoja, entonces
	// hay que agregar el elemento a la hoja
	parent::add_element(key, value);
	return true;
}

template<typename K, typename T>
bool leaf_node<K, T>::recursive_update_element(const K &key, const T &value, commons::io::recycling_block_file *file) {
	// Cuando se está modificando recursivamente un elemento
	// y la llamada recursiva llega a una hoja, entonces
	// el elemento a modifica está en esa hoja y debo
	// modificarlo
	parent::update_element(key, value);
	return true;
}

template<typename K, typename T>
bool leaf_node<K, T>::recursive_delete_element(const K &key, commons::io::recycling_block_file *file) {
	// Cuando se está eliminadno recursivamente un elemento,
	// y la llamada recursiva llega a una hoja, entonces
	// hay que eliminar el elemento de la hoja
	parent::remove_element(key);
	return true;
}

template<typename K, typename T>
std::pair<bool, T> leaf_node<K, T>::recursive_search(const K &key, commons::io::recycling_block_file *file) {
	// Cuando se está buscando recursivamente un elemento,
	// y la llamada recursiva llega a una hoja, entonces hay
	// que buscar el elemento en la hoja
	return parent::get_element(key);
}

template<typename K, typename T>
void leaf_node<K, T>::recursive_dump(std::ostream &output, commons::io::recycling_block_file *file, int space_count) {
	// Recorremos todos los elementos y los imprimimos
	output << "LEAF. (" << get_load_factor() << "%) NP: " << get_next_leaf_pointer() << std::endl;
	for (typename parent::iterator it = parent::begin(); it != parent::end(); it++) {
		std::pair<K, T> e = *it;
		output << std::string(space_count + 2, ' ') << "[" << e.first << "] : [" << e.second << "]" << std::endl;
	}
}

template<typename K, typename T>
void leaf_node<K, T>::recursive_inspect(container::element_inspector<K, T> &inspector, commons::io::recycling_block_file *file) {
	for (typename parent::iterator it = parent::begin(); it != parent::end(); it++) {
		std::pair<K, T> e = *it;
		inspector.inspect(e.first, e.second);
	}
}

template<typename K, typename T>
typename subtree<K, T>::split_result leaf_node<K, T>::split(int left_node_pointer, int right_node_pointer, int block_size) {
	// Busco la posición "más mejor" para partir el nodo
	int optimal_position = get_optimal_splitting_position();

	// Creo los resultados
	leaf_node<K, T> *left_node = new leaf_node<K, T>(commons::io::block(block_size));
	leaf_node<K, T> *right_node = new leaf_node<K, T>(commons::io::block(block_size));

	// Inicializo los nodos
	left_node->clear();
	right_node->clear();

	// Inicializo los punteros de siguiente
	left_node->set_next_leaf_pointer(right_node_pointer);
	right_node->set_next_leaf_pointer(get_next_leaf_pointer());

	// Copio todos los elementos a los nodos
	int i = 0;
	K splitting_key;
	for (typename parent::iterator it = parent::begin(); it != parent::end(); it++) {
		std::pair<K, T> e = *it;
		if (i < optimal_position) {
			left_node->add_element(e.first, e.second);
		} else if (i == optimal_position) {
			splitting_key = e.first;
			right_node->add_element(e.first, e.second);
		} else {
			right_node->add_element(e.first, e.second);
		}
		i++;
	}

	return typename subtree<K, T>::split_result(left_node, right_node, splitting_key);
}

template<typename K, typename T>
typename subtree<K, T>::merge_result leaf_node<K, T>::try_merge(subtree<K, T> *left, subtree<K, T> *right, const K &middle_key) {
	// Se que estoy mergeando leafs
	leaf_node<K, T> *left_node = dynamic_cast<leaf_node<K, T> *>(left);
	leaf_node<K, T> *right_node = dynamic_cast<leaf_node<K, T> *>(right);

	// Primero chequeo que cuando mergee no haya overflow
	if (left_node->get_load_factor() + right_node->get_load_factor() > 100)
		return typename subtree<K, T>::merge_result(false, NULL);

	// Si no va a haber overflow, creo el nodo resultante
	leaf_node<K, T> *merged = new leaf_node<K, T>(commons::io::block(parent::get_inner_block().get_size()));
	merged->clear();

	// El siguiente puntero es el siguiente del nodo derecho
	merged->set_next_leaf_pointer(right_node->get_next_leaf_pointer());

	// Agrego todos los elementos del nodo izquierdo
	for (typename parent::iterator it = left_node->begin(); it != left_node->end(); it++) {
		std::pair<K, T> e = *it;
		merged->add_element(e.first, e.second);
	}

	// Agrego todos los elementos del nodo derecho
	for (typename parent::iterator it = right_node->begin(); it != right_node->end(); it++) {
		std::pair<K, T> e = *it;
		merged->add_element(e.first, e.second);
	}

	return typename subtree<K, T>::merge_result(true, merged);
}

template<typename K, typename T>
std::pair<int, subtree<K, T> *> leaf_node<K, T>::try_replace_empty_element(commons::io::recycling_block_file *file) {
	// Un leaf nunca puede reemplazarse
	return std::make_pair(0, (subtree<K, T> *)NULL);
}

template<typename K, typename T>
commons::io::block leaf_node<K, T>::get_root_block() const {
	return parent::get_inner_block();
}

template<typename K, typename T>
double leaf_node<K, T>::get_load_factor(){
	int used = parent::get_free_index() - parent::get_element_start_index();
	int total = parent::get_inner_block().get_size() - parent::get_element_start_index();
	return static_cast<double>(used) * 100 / static_cast<double>(total);
}

template<typename K, typename T>
typename subtree<K, T>::balance_result leaf_node<K, T>::try_balance(subtree<K, T> *left, subtree<K, T> *right, const K &key){
	//No tengo en cuenta la key
	//Creo el nuevo node, copia del que tengo teniendo en cuenta lo usado en ambos.
	int size = parent::get_inner_block().get_size();
	int estimated_size = ((left->get_load_factor() + right->get_load_factor()) * size / 100)+parent::get_element_start_index();

	leaf_node<K, T> *leaf_balanced = new leaf_node<K, T>(commons::io::block(estimated_size));

	///Inicializo el nuevo nodo
	leaf_balanced->clear();

	//Se que es un hermano asi que puedo
	//castearlo al mismo tipo que tengo
	leaf_node<K, T> *left_leaf_brother = dynamic_cast<leaf_node<K, T> *>(left);
	leaf_node<K, T> *right_leaf_brother = dynamic_cast<leaf_node<K, T> *>(right);

	//Le seteo la estructura del izquierdo
	leaf_balanced->set_next_leaf_pointer(right_leaf_brother->get_next_leaf_pointer());

	//Copio los que estan en el leaf y right subtree
	for (typename parent::iterator it = left_leaf_brother->begin(); it != left_leaf_brother->end(); it++) {
		std::pair<K, T> e = *it;
		leaf_balanced->add_element(e.first, e.second);
	}
	for (typename parent::iterator it = right_leaf_brother->begin(); it != right_leaf_brother->end(); it++) {
		std::pair<K, T> e = *it;
		leaf_balanced->add_element(e.first, e.second);
	}

	//Split de lo que tengo en el leaf balanceado
	typename subtree<K, T>::split_result result = leaf_balanced->split(0,left_leaf_brother->get_next_leaf_pointer(), size);

	double left_factor = result.left_node->get_load_factor();
	double right_factor = result.right_node->get_load_factor();
	//Evaluo si sus hijos quedan en underflow
	if(right_factor < 50)
		return typename subtree<K, T>::balance_result();

	if(left_factor < 50)
		return typename subtree<K, T>::balance_result();

	//Devuelvo el resultado balanceado
	return typename subtree<K, T>::balance_result(true, result);
}

template<typename K, typename T>
int leaf_node<K, T>::get_metadata_length() const {
	// El espacio de metadata incluye
	return
		sizeof(int) + // Identificador de tipo
		sizeof(int);  // Puntero al siguiente elemento
}

template<typename K, typename T>
int leaf_node<K, T>::make_room_for_element(const K &key, const T &value) {
	// Arranco con el primer elemento
	int current_record_position = parent::get_element_start_index();

	while (current_record_position < parent::get_free_index()) {
		// Levanto la clave actual
		K current_key = commons::io::deserialize<K>(parent::get_inner_block(), current_record_position);
		// Si estoy en la primer clave que es mayor a la que tengo que
		// insertar, paro acá
		if (current_key > key) {
			// Hago espacio para el elemento
			int key_length = commons::io::serialization_length(key);
			int value_length = commons::io::serialization_length(value);
			parent::get_inner_block_pointer()->make_gap(current_record_position, key_length + value_length);
			// Devuelvo la posición donde hay que insertar
			return current_record_position;
		} else {
			// Sigo para adelante
			int key_length = commons::io::serialization_length(current_key);
			int value_length = commons::io::serialization_length<T>(parent::get_inner_block(), current_record_position + key_length);
			current_record_position += key_length + value_length;
		}
	}

	// Si llegamos acá, no había ninguna clave mayor en la hoja.
	// La posición donde insertar el elemento es la última.
	return parent::get_free_index();
}

template<typename K, typename T>
void leaf_node<K, T>::handle_insertion_overflow(const K &key, const T &value, int total_required_size) {
	// Creo un nuevo leaf_node en el que entre el elemento
	leaf_node<K, T> *overflowded_node = new leaf_node<K, T>(commons::io::block(total_required_size));

	// Copio el estado del leaf_node actual
	parent::get_inner_block().copy_to_block(*overflowded_node->get_block_pointer());

	// Agrego el elemento en este nodo, que si tiene capacidad
	overflowded_node->add_element(key, value);

	// Disparo la excepción de overflow
	throw typename subtree<K, T>::root_overflow_exception(overflowded_node);
}

template<typename K, typename T>
void leaf_node<K, T>::handle_modification_overflow(const K &key, const T &value, int total_required_size) {
	// Creo un nuevo leaf_node en el que entre el elemento
	leaf_node<K, T> *overflowded_node = new leaf_node<K, T>(commons::io::block(total_required_size));

	// Copio el estado del leaf_node actual
	parent::get_inner_block().copy_to_block(*overflowded_node->get_block_pointer());

	// Agrego el elemento en este nodo, que si tiene capacidad
	overflowded_node->update_element(key, value);

	// Disparo la excepción de overflow
	throw typename subtree<K, T>::root_overflow_exception(overflowded_node);
}

template<typename K, typename T>
int leaf_node<K, T>::get_optimal_splitting_position() {
	// Busco la posición ideal en donde partir, que dejaría
	// el 50% cargado cada nodo
	int halfway_position = (parent::get_free_index() - parent::get_element_start_index()) / 2 + parent::get_element_start_index();

	// Ahora recorro los elementos buscando el elemento en el que recae
	// la posición óptima.
	int previous_position = 0;
	int previous_element = 0;
	int current_position = parent::get_element_start_index();
	int current_element = 0;
	do
	{
		previous_position = current_position;
		previous_element = current_element;
		current_position = parent::get_next_element_index(previous_position);
		current_element++;

		if (previous_position <= halfway_position && current_position >= halfway_position)
			break;
	} while (current_position < parent::get_free_index());

	// Calculo la posición más cercana a la óptima
	return
		halfway_position - previous_position < current_position - halfway_position ?
			previous_element :
			current_element;
}

template<typename K, typename T>
void leaf_node<K, T>::save_type() {
	commons::io::serialize(leaf_node_type, parent::get_inner_block_pointer(), 0);
}

};

#endif
