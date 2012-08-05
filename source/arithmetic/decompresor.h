/******************************************************************************
 * decompressor.h
 * 		Declaraciones y definiciones de la clase arithmetic::decompressor
******************************************************************************/
#ifndef __ARITHMETIC_DECOMPRESSOR_H_INCLUDED__
#define __ARITHMETIC_DECOMPRESSOR_H_INCLUDED__

#include "../commons/io/binary_source.h"
#include "../commons/utils/bit_utils.h"
#include "../commons/assertions/assertions.h"
#include "interval_normalizer.h"
#include "symbol_distribution.h"
#include <vector>
#include <bitset>
#include <cmath>
#include "../commons/log/log.h"

namespace arithmetic {

/**
 * Decompresor aritmético
 */
template<int PRECISION>
class decompressor {
private:
	std::bitset<PRECISION> _floor;

	std::bitset<PRECISION> _roof;

	commons::io::binary_source &binary_source;

	std::vector<bool> current;

	bool get_bit_from_source();
public:
	/**
	* Crea una nueva instancia de decompresor que tomará bits
	* desde el binary_source dado.
	*/
	decompressor(commons::io::binary_source &source);

	/**
	* Descomprime el siguiente símbolo desde el binary_source
	* usando la distribución de probabilidades dadas.
	*/
	symbol decompress(const symbol_distribution &distribution);
};

template<int PRECISION>
decompressor<PRECISION>::decompressor(commons::io::binary_source &source)
: _floor(0), _roof(static_cast<unsigned long>(std::pow(2, PRECISION) - 1)), binary_source(source) {
	for (int i = 0; i < PRECISION; i++) {
		current.push_back(get_bit_from_source());
	}
	LOG_DEBUG(commons::utils::bit::to_bit_string(current));
}

template<int PRECISION>
symbol decompressor<PRECISION>::decompress(const symbol_distribution &distribution) {
	unsigned long interval = _roof.to_ulong() - _floor.to_ulong() + 1;
	unsigned long compare_value = commons::utils::bit::to_unsigned_long<PRECISION>(current);
	LOG_DEBUG("Decompressing next distribution");
	LOG_DEBUG_VAR(distribution);

	for (symbol_distribution::iterator it = distribution.begin(); it != distribution.end(); it++) {
		symbol current_symbol = *it;
		double floor_probability = distribution.get_accumulated_probability(current_symbol);
		double roof_probability = floor_probability +  distribution.get_probability_of(current_symbol);
		unsigned long new_floor = _floor.to_ulong() + std::floor(interval * floor_probability);
		unsigned long new_roof = _floor.to_ulong() + std::floor(interval * roof_probability) - 1;
		LOG_DEBUG_VAR(floor_probability);
		LOG_DEBUG_VAR(roof_probability);
		LOG_DEBUG_VAR(new_floor);
		LOG_DEBUG_VAR(new_roof);
		if ((new_floor <= compare_value) && (compare_value <= new_roof)) {
			arithmetic::interval_normalizer<PRECISION> normalizer(new_floor, new_roof);
			typename arithmetic::interval_normalizer<PRECISION>::normalization_result result = normalizer.normalize();

			_floor = result.new_floor;
			_roof = result.new_roof;

			LOG_DEBUG_VAR(result.new_roof);
			LOG_DEBUG_VAR(result.new_floor);
			LOG_DEBUG_VAR(result.overflow.size());
			LOG_DEBUG_VAR(result.underflow);

			for (std::size_t i = 0; i < result.overflow.size(); i++) {
				current.erase(current.begin());
				current.push_back(get_bit_from_source());
			}

			for (int i = 0; i < result.underflow; i++) {
				current.erase(current.begin() + 1);
				current.push_back(get_bit_from_source());
			}

			LOG_DEBUG_VAR(current_symbol);
			return current_symbol;
		}
	}

	ASSERTION_FAILURE("No se pudo encontrar un intervalo válido que contenga el siguiente set de bits");
}

template<int PRECISION>
bool decompressor<PRECISION>::get_bit_from_source() {
	std::pair<bool, bool> result = binary_source.get_next_bit();
	ASSERTION_WITH_MESSAGE(result.first, "Unexpected end of binary stream");
	return result.second;
}

};

#endif
