/******************************************************************************
 * node_factory.h
 * 		Declaración y definición de función que centraliza la creación de nodos
 * 		de tipos indeterminados
******************************************************************************/
#ifndef __BPLUS_NODE_FACTORY_H_INCLUDED__
#define __BPLUS_NODE_FACTORY_H_INCLUDED__

#include "../commons/io/block.h"
#include "inner_node.h"
#include "leaf_node.h"
#include "node_types.h"
#include "subtree.h"

namespace bplus {
namespace node_factory {

/**
 * Crea un nodo concreto dependiendo del tipo de un bloque
 * y devuelve una interfaz unificada para manejarlo. El
 * nodo concreto creado se crea en memoria dinámica, es
 * responsabilidad del que está llamando limpiar esa memoria
 * después de usarla.
 */
template<typename K, typename T>
subtree<K, T> *create_subtree_from_block(const commons::io::block &b) {
	switch (extract_node_type(b)) {
	case leaf_node_type:
		return new leaf_node<K, T>(b);
	case inner_node_type:
		return new inner_node<K, T>(b);
	}
	return NULL;
}

};
};

#endif
