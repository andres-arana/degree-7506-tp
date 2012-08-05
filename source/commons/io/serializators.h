/******************************************************************************
 * serializators.h
 * 		Declaraciones y definiciones de funciones de interpretación de datos
 * 		del bloque
******************************************************************************/
#ifndef __COMMONS_IO_SERIALIZATORS_H_INCLUDED__
#define __COMMONS_IO_SERIALIZATORS_H_INCLUDED__

#include "block.h"
#include <string>

namespace commons {
namespace io {

/**
 * Calcula el tamaño que necesita instance para
 * ser serializada a un bloque.
 */
template<typename T>
int serialization_length(const T &instance);

/**
 * Calcula el tamaño de una instancia serializada
 * en el bloque b a partir del índice index.
 */
template<typename T>
int serialization_length(const block &b, int index);

/**
 * Serializa una instancia instance en el bloque b
 * a partir del índice index. Sobreescribe los siguientes
 * bytes a partir de index en el bloque; cuantos bytes
 * se sobreescriben esta dado por serialization_length(instance)
 */
template<typename T>
void serialize(const T&instance, block *b, int index);

/**
 * Deserializa una instancia escrita en el bloque
 * b a partir del índice index. Lee los siguientes bytes
 * a partir de index desde el bloque; cuantos bytes se
 * leen esta dado por serializaton_length<T>(b, index);
 */
template<typename T>
T deserialize(const block &b, int index);


// Implementación genérica para tipos shallow
template<typename T>
int serialization_length(const T &instance) {
	return static_cast<int>(sizeof(T));
}

template<typename T>
int serialization_length(const block &b, int index) {
	return static_cast<int>(sizeof(T));
}

template<typename T>
void serialize(const T& instance, block *b, int index) {
	const char *serialized = reinterpret_cast<const char *>(&instance);
	for (size_t i = 0; i != sizeof(T); i++) {
		(*b)[i + index] = serialized[i];
	}
}

template<typename T>
T deserialize(const block &b, int index) {
	const char *raw_char = &(b.raw_char_pointer())[index];
	const T *serialized = reinterpret_cast<const T *>(raw_char);
	return *serialized;
}

// Implementación especial para chars, por cuestiones de eficiencia
template<>
int serialization_length<char>(const char &instance);

template<>
int serialization_length<char>(const block &b, int index);

template<>
void serialize<char>(const char &instance, block *b, int index);

template<>
char deserialize<char>(const block &b, int index);

// Implementación especial para strings. Los strings se serializan como
// un string::size_type que guarda el tamaño más los chars del string
template<>
int serialization_length<std::string>(const std::string &instance);

template<>
int serialization_length<std::string>(const block &b, int index);

template<>
void serialize<std::string>(const std::string &instance, block *b, int index);

template<>
std::string deserialize<std::string>(const block &b, int index);

};
};

#endif
