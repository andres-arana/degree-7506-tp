/******************************************************************************
 * subtree.h
 * 		Declaraciones y definiciones de la clase bplus::subtree
******************************************************************************/
#ifndef __BPLUS_SUBTREE_H_INCLUDED__
#define __BPLUS_SUBTREE_H_INCLUDED__

#include "../commons/io/recycling_block_file.h"
#include "../element_inspector.h"

namespace bplus {

/**
 * Representa un subárbol dentro de un árbol
 */
template<typename K, typename T>
class subtree {
public:
	/**
	 * Representa una excepción genérica a manejar
	 * cuando un la raiz de este sub-arbol tuvo un
	 * overflow.
	 */
	class root_overflow_exception {
	private:
		subtree<K, T> *overflowded_subtree;
	public:
		/**
		 * Crea una nueva root_overflow_exception
		 */
		root_overflow_exception(subtree<K, T> *overflowded_subtree);

		/**
		 * Obtiene el subárbol como hubiese quedado después
		 * de la operación que causó el overflow.
		 */
		subtree<K, T> *get_overflowded_subtree();
	};

	/**
	 * Estructura utilizada para los resultados del
	 * método split.
	 */
	struct split_result {
		subtree<K, T> *left_node;
		subtree<K, T> *right_node;
		K middle_key;

		split_result(subtree<K, T> *left_node, subtree<K, T> *right_node, const K &middle_key);
	};

	/**
	 * Estructura utilizada para los resultados del
	 * método try_balance.
	 */
	struct balance_result {
		bool success;
		split_result result;

		balance_result();

		balance_result(bool success, split_result result);
	};

	/**
	 * Estructura utilizada para los resultados del
	 * try_merge
	 */
	struct merge_result {
		bool success;
		subtree<K, T> *merged_node;

		merge_result(bool success, subtree<K, T> *merged_node);
	};

	/**
	 * Recursivamente agrega un elemento al nodo
	 * o a sus descendientes, según corresponda.
	 * Devuelve true si el nodo fue modificado
	 * en el proceso, lo que requiere que dicho
	 * nodo se baje al archivo.
	 */
	virtual bool recursive_add_element(const K &key, const T &value, commons::io::recycling_block_file *file) = 0;

	/**
	 * Recursivamente modifica un elemento en el
	 * nodo o en sus descendientes, según corresponda.
	 * Devuelve true si el noodo fue modificado
	 * en el proceso, lo que requiere que dicho nodo se
	 * baje al archivo.
	 */
	virtual bool recursive_update_element(const K &key, const T &value, commons::io::recycling_block_file *file) = 0;

	/**
	 * Recursivamente elimina un elemento del
	 * nodo o de sus descendentes, según corresponda.
	 * Devuelve true si el nodo fue modificado en el proceso,
	 * lo que requiere que dicho nodo se baje al archivo
	 */
	virtual bool recursive_delete_element(const K &key, commons::io::recycling_block_file *file) = 0;

	/**
	 * Busca recursivamente un elemento de clave key.
	 * Devuelve result.first en true si lo encuentra
	 * y en result.second el elemento buscado. En caso
	 * contrario, result.first es false.
	 */
	virtual std::pair<bool, T> recursive_search(const K &key, commons::io::recycling_block_file *file) = 0;

	/**
	 * Recursivamente imprime los contenidos del
	 * subárbol en output.
	 */
	virtual void recursive_dump(std::ostream &output, commons::io::recycling_block_file *file, int space_count = 0) = 0;

	/**
	 * Recursivamente inspecciona los contenidos del subárbol
	 */
	virtual void recursive_inspect(container::element_inspector<K, T> &inspector, commons::io::recycling_block_file *file) = 0;

	/**
	 * Divide un nodo a la mitad (o lo más cercano a la
	 * mitad posible), y eleva la clave que queda en el medio
	 * (mayor a todos los elementos de la izquierda y menor
	 * que todos los elementos de la derecha).
	 */
	virtual split_result split(int left_node_pointer, int right_node_pointer, int block_size) = 0;

	/**
	 * Intenta mergear un nodo con otro contiguo. Asume que el tipo de
	 * other es el mismo que el tipo de este nodo. Middle key
	 * es la clave que separa a los punteros a los nodos a mergear.
	 * result.success es true si se pueden mergear los nodos, false
	 * en caso contrario. Si se pueden mergear los nodos, result.merged_node
	 * es un nodo que contiene al merge de este nodo con other.
	 */
	virtual merge_result try_merge(subtree<K, T> *left, subtree<K, T> *right, const K &middle_key) = 0;

	/**
	 * Intenta reemplazar a la raiz de este subárbol por alguno de los hijos,
	 * si la raiz misma está vacia (y por lo tanto, el nodo de la raiz está
	 * de más).
	 */
	virtual std::pair<int, subtree<K, T> *> try_replace_empty_element(commons::io::recycling_block_file *file) = 0;

	/**
	 * Devuelve el bloque interno que representa el estado
	 * de este nodo.
	 */
	virtual commons::io::block get_root_block() const = 0;

	/**
	 * Devuelve el factor de carga del nodo
	 */
	virtual double get_load_factor() = 0;

	/**
	 * Intenta balancear
	 */
	virtual balance_result try_balance(subtree<K, T> *left, subtree<K, T> *right, const K &key) = 0;

	virtual ~subtree() {}
};

template<typename K, typename T>
subtree<K, T>::root_overflow_exception::root_overflow_exception(subtree<K, T> *overflowded_subtree)
: overflowded_subtree(overflowded_subtree) {

}

template<typename K, typename T>
subtree<K, T> *subtree<K, T>::root_overflow_exception::get_overflowded_subtree() {
	return overflowded_subtree;
}

template<typename K, typename T>
subtree<K, T>::split_result::split_result(subtree<K, T> *left_node, subtree<K, T> *right_node, const K &middle_key)
: left_node(left_node), right_node(right_node), middle_key(middle_key) {

}

template<typename K, typename T>
subtree<K, T>::balance_result::balance_result()
: success(false), result(NULL,NULL, K()) {

}

template<typename K, typename T>
subtree<K, T>::balance_result::balance_result(bool success, split_result result)
: success(success), result(result) {

}

template<typename K, typename T>
subtree<K, T>::merge_result::merge_result(bool success, subtree<K, T> *merged_node)
: success(success), merged_node(merged_node) {

}

};

#endif
