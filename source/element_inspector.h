/******************************************************************************
 * element_inspector.h
 * 		Declaraciones de la clase hash::element_inspector
******************************************************************************/
#ifndef __CONTAINER_ELEMENT_INSPECTOR_H_INCLUDED__
#define __CONTAINER_ELEMENT_INSPECTOR_H_INCLUDED__

namespace container {

/**
 * Interface utilizada cuando se quieren inspeccionar uno por uno
 * todos los elementos del contenedor
 */
template<typename K, typename T>
struct element_inspector {
	/**
	 * Llamado una vez por cada par de clave-valor
	 */
	virtual void inspect(const K &key, const T &value) = 0;
};

};

#endif
