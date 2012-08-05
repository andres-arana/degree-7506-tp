/******************************************************************************
 * serializators.cpp
 * 		Definiciones de funciones especializadas de interpretación de datos
 * 		del bloque
******************************************************************************/
#include "serializators.h"
#include <sstream>

// Implementación especial para chars, por cuestiones de eficiencia
template<>
int commons::io::serialization_length<char>(const char &instance) {
	return 1;
}

template<>
int commons::io::serialization_length<char>(const block &b, int index) {
	return 1;
}

template<>
void commons::io::serialize<char>(const char &instance, block *b, int index) {
	(*b)[index] = instance;
}

template<>
char commons::io::deserialize<char>(const block &b, int index) {
	return b[index];
}

// Implementación especial para strings. Los strings se serializan como
// un string::size_type que guarda el tamaño más los chars del string
template<>
int commons::io::serialization_length<std::string>(const std::string &instance) {
	return
		serialization_length(instance.size()) +
		instance.size() * serialization_length(std::string::value_type());
}

template<>
int commons::io::serialization_length<std::string>(const block &b, int index) {
	return
		deserialize<std::string::size_type>(b, index) +
		serialization_length<std::string::size_type>(b, index);
}

template<>
void commons::io::serialize<std::string>(const std::string &instance, block *b, int index) {
	serialize(instance.size(), b, index);
	for (std::string::size_type position = 0; position < instance.size(); position++) {
		serialize(instance[position], b, serialization_length(instance.size()) + position + index);
	}
}

template<>
std::string commons::io::deserialize<std::string>(const block &b, int index) {
	unsigned int position = index;
	std::string::size_type length = deserialize<std::string::size_type>(b, position);
	position += serialization_length<std::string::size_type>(b, position);

	std::stringstream result_builder;

	for (std::string::size_type i = 0; i < length; i++) {
		result_builder << deserialize<std::string::value_type>(b, position + i);
	}

	return result_builder.str();
}
