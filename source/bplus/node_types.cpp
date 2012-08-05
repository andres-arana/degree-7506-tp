/******************************************************************************
 * node_types.h
 * 		Definición de constantes necesarias para identificar el tipo de nodo
 * 		que es un cierto bloque
******************************************************************************/
#include "node_types.h"
#include "../commons/io/serializators.h"

int bplus::extract_node_type(const commons::io::block &b) {
	return commons::io::deserialize<int>(b, 0);
}
