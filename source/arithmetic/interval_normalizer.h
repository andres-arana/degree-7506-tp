/******************************************************************************
 * interval_normalizer.h
 * 		Declaraciones de la clase arithmetic::interval_normalizer
******************************************************************************/
#ifndef __ARITHMETIC_INTERVAL_NORMALIZER_H_INCLUDED__
#define __ARITHMETIC_INTERVAL_NORMALIZER_H_INCLUDED__

#include <cmath>
#include <bitset>
#include <vector>

namespace arithmetic {

/**
 * Clase que contiene la lógica para normalizar un intervalo
 * piso-techo de un compresor aritmético. El parámetro de plantilla
 * es la precisión (cantidad de bits) con la que están representados
 * tanto el piso como el techo.
 */
template<int P>
class interval_normalizer {
private:

	std::bitset<P> floor;

	std::bitset<P> roof;

	bool one_zero_match_once;

	bool overflow(int index);

	bool underflow(int index);

	std::bitset<P> complete_with_ones(std::bitset<P> roof, int underflow, int overflow);

	std::bitset<P> complete_with_zeros(std::bitset<P> floor, int underflow, int overflow);
public:
	/**
	 * Crea una nueva instancia de interval_normalizer que
	 * normaliza un piso-techo dados.
	 */
	interval_normalizer(unsigned long floor, unsigned long roof);

	/**
	 * Estructura utilizada para representar el resultado de una
	 * normalización
	 */
	struct normalization_result {
		/**
		 * Lista de bits eliminados por overflow
		 */
		std::vector<bool> overflow;
		/**
		 * Cantidad de bits eliminados por underflow
		 */
		int underflow;
		/**
		 * Nuevo piso, luego de normalizar
		 */
		unsigned long new_floor;
		/**
		 * Nuevo techo, luego de normalizar
		 */
		unsigned long new_roof;

		normalization_result()
		: underflow(0), new_floor(0), new_roof(0) {}
	};

	/**
	 * Calcula el resultado de normalizar el intervalo dado.
	 */
	normalization_result normalize();
};

template<int P>
interval_normalizer<P>::interval_normalizer(unsigned long floor, unsigned long roof)
: floor(floor), roof(roof), one_zero_match_once(false) {

}

template<int P>
typename interval_normalizer<P>::normalization_result interval_normalizer<P>::normalize(){
	normalization_result result;

	for (int i = P - 1; i >= 0; i--) {
		if(overflow(i)){
			result.overflow.push_back(roof.test(i));
		} else if (underflow(i)) {
			result.underflow++;
		} else {
			break;
		}
	}

	result.new_floor = complete_with_zeros(floor, result.underflow, result.overflow.size()).to_ulong();
	result.new_roof = complete_with_ones(roof, result.underflow, result.overflow.size()).to_ulong();
	return result;
}

template<int P>
bool interval_normalizer<P>::overflow(int index) {
	bool one_match = floor.test(index) && roof.test(index);
	bool zero_match = !(floor.test(index) || roof.test(index));

	//No coinciden entre los valores
	if ( !one_match && !zero_match ){
		return false;
	}

	return true;
}

template<int P>
bool interval_normalizer<P>::underflow(int index) {
	if (index <= 0)
		return false;
	bool one_zero_match = roof.test(index) && !floor.test(index);
	index--;
	bool zero_one_match = !roof.test(index) && floor.test(index);

	if (one_zero_match_once && zero_one_match) {
		return true;
	}

	if (one_zero_match && zero_one_match){
		one_zero_match_once = true;
		return true;
	}
	return false;
}

template<int P>
std::bitset<P> interval_normalizer<P>::complete_with_ones(std::bitset<P> roof, int underflow, int overflow) {
	if (underflow == 0){
		std::bitset<P> temp_bitset = roof.flip() << overflow;
		return temp_bitset.flip();
	} else {
		std::bitset<P> temp_bitset = roof.flip() << (underflow + overflow);
		temp_bitset.flip();
		temp_bitset.set(temp_bitset.size() - 1, 1);
		return temp_bitset;
	}
}

template<int P>
std::bitset<P> interval_normalizer<P>::complete_with_zeros(std::bitset<P> floor, int underflow, int overflow) {
	if (underflow == 0){
		return floor <<  overflow;
	} else {
		std::bitset<P> temp_bitset = floor << (underflow + overflow);
		temp_bitset.set(temp_bitset.size() - 1, 0);
		return temp_bitset;
	}
}

};

#endif
