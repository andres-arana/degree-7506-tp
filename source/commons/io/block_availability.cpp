/******************************************************************************
 * block_availability.cpp
 * 		Definiciones de la clase commons::block_availability
******************************************************************************/
#include "block_availability.h"
#include <utility>
#include "../assertions/assertions.h"

using namespace commons::io;

namespace {
	std::pair<int, int> calculate_coords(int position) {
		// Calculo el byte en el que está el flag de la posición que me pasaron
		int byte_position = static_cast<int>(position / 8);
		// Calculo el número de bit en el que está el flag
		int bit_position = position % 8;
		return std::make_pair(byte_position, bit_position);
	}
}

block_availability::block_availability(int block_size)
: b(block_size) {

}

void block_availability::clear() {
	for (int i = 0; i < b.get_size(); i++) {
		b[i] = 0;
	}
}

block &block_availability::get_block() {
	return b;
}

bool block_availability::is_available(int position) const {
	// Calculo las coordenadas de position
	std::pair<int, int> coords = calculate_coords(position);
	ASSERTION_WITH_MESSAGE(coords.first < b.get_size(), "El tamaño del bloque de control de disponibilidad es muy chico para la cantidad de bloques disponibles");
	// Me fijo si el bit bit_position del byte byte_position
	// es uno, en cuyo caso el bloque está disponible
	return b[coords.first] & (1 << coords.second);
}

void block_availability::make_unavailable(int position) {
	// Calculo las coordenadas de position
	std::pair<int, int> coords = calculate_coords(position);
	ASSERTION_WITH_MESSAGE(coords.first < b.get_size(), "El tamaño del bloque de control de disponibilidad es muy chico para la cantidad de bloques disponibles");
	// Apago el bit bit_position del byte byte_position
	b[coords.first] &= ( 0xFE << coords.second );
}

void block_availability::make_available(int position) {
	// Calculo las coordenadas de position
	std::pair<int, int> coords = calculate_coords(position);
	ASSERTION_WITH_MESSAGE(coords.first < b.get_size(), "El tamaño del bloque de control de disponibilidad es muy chico para la cantidad de bloques disponibles");
	// Apago el bit bit_position del byte byte_position
	b[coords.first] |= ( 1 << coords.second );
}

int block_availability::first_available() {
	// Recorro todas las posiciones, hasta
	// que alguna esté disponible
	for (int i = 0; i < 8 * b.get_size(); i++) {
		if (is_available(i))
			return i;
	}
	// No había ninguna disponible
	return -1;
}

int block_availability::first_occupied() {
	// Recorro todas las posiciones, hasta
	// que alguna esté disponible
	for (int i = 0; i < 8 * b.get_size(); i++) {
		if (!is_available(i))
			return i;
	}
	// No había ninguna disponible
	return -1;
}
