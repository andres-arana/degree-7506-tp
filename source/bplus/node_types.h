/******************************************************************************
 * node_types.h
 * 		Declaraciónde constantes necesarias para identificar el tipo de nodo
 * 		que es un cierto bloque.
******************************************************************************/
#ifndef __BPLUS_NODE_TYPES_H_INCLUDED__
#define __BPLUS_NODE_TYPES_H_INCLUDED__

#include "../commons/io/block.h"

namespace bplus {

/**
 * Enumeración utilizada para identificar
 * los tipos de nodos que puede ser un bloque.
 */
const int leaf_node_type = 1;
const int inner_node_type = 2;

/**
 * Extrae el tipo de nodo que es un bloque
 */
int extract_node_type(const commons::io::block &b);

};

#endif
