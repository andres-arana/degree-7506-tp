/******************************************************************************
 * bit_utils.h
 * 		Declaraciones de funciones de ayuda para manejo de bits
******************************************************************************/
#ifndef __COMMONS_UTILS_BIT_BIT_UTILS_H_INCLUDED__
#define __COMMONS_UTILS_BIT_BIT_UTILS_H_INCLUDED__

#include <string>
#include <bitset>
#include <vector>

namespace commons {
namespace utils {
namespace bit {

/**
 * Convierte un vector de bools a un string de chars '0' o '1'
 */
std::string to_bit_string(const std::vector<bool> &v);

/**
 * Crea un strings de chars '0' o '1' a partir de un vector
 * de bools
 */
std::vector<bool> to_bool_vector(const std::string &s);

/**
 * Crea un unsigned long a partir de una cadena de chars '1' o '0'
 * utilizando precisión P
 */
template<int P>
unsigned long to_unsigned_long(const std::string &value) {
	return std::bitset<P>(value).to_ulong();
}

/**
 * Crea un strings de chars '0' o '1' a partir de un unsigned
 * long con precisión P
 */
template<int P>
std::string to_bit_string(unsigned long value) {
	return std::bitset<P>(value).to_string();
}

/**
 * Crea un unsigned long a partir de un vector<bool>
 */
template<int P>
unsigned long to_unsigned_long(const std::vector<bool> &v) {
	return std::bitset<P>(to_bit_string(v)).to_ulong();
}

};
};
};

#endif
