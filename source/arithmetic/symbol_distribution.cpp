/******************************************************************************
 * symbol_distribution.cpp
 * 		Definiciones de la clase arithmetic::symbol_distribution
******************************************************************************/
#include "symbol_distribution.h"
#include "../commons/io/serializators.h"

using namespace arithmetic;

symbol_distribution::symbol_distribution(bool default_esc)
: total_frequencies(0) {
	for (int i = 0; i < ARITHMETIC_SYMBOL_COUNT; i++) {
		frequencies[i] = 0u;
	}

	if (default_esc) {
		frequencies[symbol::ESC.get_sequential_code()] = 1u;
		total_frequencies = 1;
	}
}

symbol_distribution::symbol_distribution(const symbol &s)
: total_frequencies(1) {
	for (int i = 0; i < ARITHMETIC_SYMBOL_COUNT; i++) {
		frequencies[i] = 0u;
	}
	frequencies[s.get_sequential_code()] = 1u;
}

void symbol_distribution::register_symbol_emision(const symbol &s) {
	frequencies[s.get_sequential_code()]++;
	total_frequencies++;
}

bool symbol_distribution::has_symbol(const symbol &s) const {
	return 0 != frequencies[s.get_sequential_code()];
}

double symbol_distribution::get_accumulated_probability(const symbol &s) const {
	double result = 0.0;
	for (unsigned int i = 0; i < s.get_sequential_code(); i++) {
		result +=
				static_cast<double>(frequencies[i]) /
				static_cast<double>(total_frequencies);
	}
	return result;
}

double symbol_distribution::get_probability_of(const symbol &s) const {
	int index = s.get_sequential_code();
	return
		static_cast<double>(frequencies[index]) /
		static_cast<double>(total_frequencies);
}

bool symbol_distribution::has_only_the_symbol(const symbol &s) const {
	return frequencies[s.get_sequential_code()] == 1 && total_frequencies == 1;
}

unsigned int symbol_distribution::get_emissions(const symbol &s) const {
	return frequencies[s.get_sequential_code()];
}

void symbol_distribution::append_to_exclusion_set(std::set<symbol> &exclusion) const {
	for (iterator it = begin(); it != end(); it++) {
		symbol current_symbol = *it;
		if (current_symbol != symbol::ESC)
			exclusion.insert(current_symbol);
	}
}

symbol_distribution symbol_distribution::exclude(const std::set<symbol> &exclusion) const {
	symbol_distribution copy = *this;

	typedef std::set<symbol>::const_iterator iterator;
	for (iterator it = exclusion.begin(); it != exclusion.end(); it++) {
		symbol current_symbol = *it;

		copy.total_frequencies -= copy.frequencies[current_symbol.get_sequential_code()];
		copy.frequencies[current_symbol.get_sequential_code()] = 0;
	}
	return copy;
}

symbol_distribution::iterator::iterator(unsigned int current_position, const unsigned int *frequencies)
: current_position(current_position), frequencies(frequencies) {
	if (current_position < ARITHMETIC_SYMBOL_COUNT && frequencies[current_position] == 0) {
		this->operator++(0);
	}
}

symbol symbol_distribution::iterator::operator*() const {
	return symbol(current_position);
}

bool symbol_distribution::iterator::operator==(const iterator &other) const {
	return current_position == other.current_position;
}

bool symbol_distribution::iterator::operator!=(const iterator &other) const {
	return current_position != other.current_position;
}

void symbol_distribution::iterator::operator++(int) {
	do {
		current_position++;
	} while (current_position < ARITHMETIC_SYMBOL_COUNT && frequencies[current_position] == 0);
}

symbol_distribution::iterator symbol_distribution::begin() const {
	return iterator(0, frequencies);
}

symbol_distribution::iterator symbol_distribution::end() const {
	return iterator(ARITHMETIC_SYMBOL_COUNT, frequencies);
}

std::ostream &arithmetic::operator<<(std::ostream &output, const symbol_distribution &distribution) {
	output << "Symbol distribution: ";
	for (symbol_distribution::iterator it = distribution.begin(); it != distribution.end(); it++) {
		symbol current_symbol = *it;
		output << "[" << current_symbol << ":" << distribution.get_probability_of(current_symbol) << "] ";
	}
	return output;
}

// Los serializadores de symbol distribution funcionan con el siguiente esquema de serialización:
// cantidad_de_simbolos_distintos | simbolo_n | frequencia_de_simbolo_n | simbolo_n+1 | frequencia_de_simbolo_n+1 | ...

template<>
int commons::io::serialization_length<symbol_distribution>(const symbol_distribution &instance) {
	unsigned short distinct_symbols = 0;
	for (unsigned short i = 0; i < ARITHMETIC_SYMBOL_COUNT; i++) {
		distinct_symbols += (instance.frequencies[i] > 0 ? 1 : 0);
	}

	return
		sizeof(unsigned short)  + 						// 1 unsigned short para la cantidad de simbolos distintos
		distinct_symbols * sizeof(unsigned short) + 	// N unsigned shorts para el código de símbolo
		distinct_symbols * sizeof(unsigned int);		// N unsigned ints para las frequencias
}

template<>
int commons::io::serialization_length<symbol_distribution>(const commons::io::block &b, int index) {
	// Levantamos la cantidad de símbolos
	unsigned short distinct_symbols = commons::io::deserialize<unsigned short>(b, index);

	return
		sizeof(unsigned short)  + 						// 1 unsigned short para la cantidad de simbolos distintos
		distinct_symbols * sizeof(unsigned short) + 	// N unsigned shorts para el código de símbolo
		distinct_symbols * sizeof(unsigned int);		// N unsigned ints para las frequencias
}

template<>
void commons::io::serialize<symbol_distribution>(const symbol_distribution &instance, commons::io::block *b, int index) {
	unsigned short distinct_symbols = 0;
	for (unsigned short i = 0; i < ARITHMETIC_SYMBOL_COUNT; i++) {
		distinct_symbols += (instance.frequencies[i] > 0 ? 1 : 0);
	}

	// Serializamos la cantidad de símbolos
	int current_field_position = index;
	commons::io::serialize(distinct_symbols, b, current_field_position);
	current_field_position += commons::io::serialization_length(distinct_symbols);

	// Por cada símbolo que tenga frequencia
	for (unsigned short i = 0; i < ARITHMETIC_SYMBOL_COUNT; i++) {
		if (instance.frequencies[i] > 0) {
			commons::io::serialize(i, b, current_field_position);
			current_field_position += commons::io::serialization_length(i);

			commons::io::serialize(instance.frequencies[i], b, current_field_position);
			current_field_position += commons::io::serialization_length(instance.frequencies[i]);
		}
	}
}

template<>
symbol_distribution commons::io::deserialize<symbol_distribution>(const commons::io::block &b, int index) {
	symbol_distribution result(false);

	int current_field_position = index;
	// Levantamos la cantidad de símbolos
	unsigned short distinct_symbols = commons::io::deserialize<unsigned short>(b, current_field_position);
	current_field_position += commons::io::serialization_length(distinct_symbols);

	for (unsigned short i = 0; i < distinct_symbols; i++) {
		unsigned short symbol_sequential_code = commons::io::deserialize<unsigned short>(b, current_field_position);
		current_field_position += commons::io::serialization_length(symbol_sequential_code);

		unsigned int frequency = commons::io::deserialize<unsigned int>(b, current_field_position);
		current_field_position += commons::io::serialization_length(frequency);

		result.frequencies[symbol_sequential_code] = frequency;
		result.total_frequencies += frequency;
	}

	return result;
}
