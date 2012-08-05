/******************************************************************************
 * compressor.h
 * 		Declaraciones y definiciones de la clase arithmetic::compressor
******************************************************************************/
#ifndef __ARITHMETIC_COMPRESSOR_H_INCLUDED__
#define __ARITHMETIC_COMPRESSOR_H_INCLUDED__

#include "../commons/io/binary_destination.h"
#include "interval_normalizer.h"
#include "symbol_distribution.h"
#include <bitset>
#include <cmath>
#include "../commons/log/log.h"
#include "../commons/utils/bit_utils.h"

namespace arithmetic {

/**
 * Compresor aritmético genérico
 */
template<int PRECISION>
class compressor {
private:
	std::bitset<PRECISION> _floor;

	std::bitset<PRECISION> _roof;

	commons::io::binary_destination &binary_destination;

	int underflow;
public:

	/**
	 * Crea una nueva instancia de compressor que emite bits a
	 * un binary_destination dado
	 */
	compressor(commons::io::binary_destination &destination);

	/**
	 * Comprime un simbolo dada una distribución de probabilidades de
	 * símbolos. Dicha distribución DEBE contener al símbolo que se quiere
	 * comprimir. Emite bits en el binary_destination asociado.
	 */
	void compress(const symbol &s, const symbol_distribution &distribution);

	/**
	 * Termina la compresion.
	 */
	void finish_compression();
};

template<int PRECISION>
arithmetic::compressor<PRECISION>::compressor(commons::io::binary_destination &destination)
: _floor(0), _roof(static_cast<unsigned long>(pow(2, PRECISION) - 1)), binary_destination(destination), underflow(0) {
}

template<int PRECISION>
void arithmetic::compressor<PRECISION>::compress(const symbol &s, const symbol_distribution &distribution)  {
	LOG_DEBUG("Compressing next symbol");
	LOG_DEBUG_VAR(s.get_sequential_code());
	LOG_DEBUG("with this distribution");
	LOG_DEBUG_VAR(distribution);

	unsigned long interval = _roof.to_ulong() - _floor.to_ulong() + 1;

	double floor_probability = distribution.get_accumulated_probability(s);
	double roof_probability = floor_probability +  distribution.get_probability_of(s);
	unsigned long new_floor = _floor.to_ulong() + std::floor(interval * floor_probability);
	unsigned long new_roof = _floor.to_ulong() + std::floor(interval * roof_probability) - 1;
	LOG_DEBUG_VAR(floor_probability);
	LOG_DEBUG_VAR(roof_probability);
	LOG_DEBUG_VAR(interval);
	LOG_DEBUG_VAR(new_floor);
	LOG_DEBUG_VAR(new_roof);

	interval_normalizer<PRECISION> normalizer(new_floor, new_roof);
	typename interval_normalizer<PRECISION>::normalization_result r = normalizer.normalize();

	_floor = r.new_floor;
	_roof = r.new_roof;

	std::string emission;
	LOG_DEBUG("Normalized");
	LOG_DEBUG_VAR(r.new_floor);
	LOG_DEBUG_VAR(r.new_roof);
	LOG_DEBUG_VAR(commons::utils::bit::to_bit_string(r.overflow));
	LOG_DEBUG_VAR(r.underflow);

	if (!r.overflow.empty()) {
		binary_destination.emit_bit(r.overflow[0]);
		emission += r.overflow[0] ? "1" : "0";

		for (int i = 0; i < underflow; i++) {
			binary_destination.emit_bit(!r.overflow[0]);
			emission += !r.overflow[0] ? "1" : "0";
		}

		underflow = 0;

		for (size_t i = 1; i < r.overflow.size(); i++) {
			binary_destination.emit_bit(r.overflow[i]);
			emission += r.overflow[i] ? "1" : "0";
		}
	}

	underflow += r.underflow;
}

template<int PRECISION>
void arithmetic::compressor<PRECISION>::finish_compression(){
	LOG_DEBUG("Finish compression");
	LOG_DEBUG_VAR(_floor.to_string());
	binary_destination.emit_bit(_floor[PRECISION - 1]);

	for (int i = 0; i < underflow; i++) {
		binary_destination.emit_bit( !_floor[PRECISION - 1] );
	}
	underflow = 0;
	for (int i = PRECISION - 2; i >= 0; i--) {
		binary_destination.emit_bit(_floor[i]);
	}
}

}

#endif
