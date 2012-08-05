/******************************************************************************
 * bit_utils.cpp
 * 		Definiciones de funciones de ayuda para manejo de bits
******************************************************************************/
#include "bit_utils.h"

std::string commons::utils::bit::to_bit_string(const std::vector<bool> &v) {
	std::string result;

	typedef std::vector<bool>::const_iterator iterator;
	for (iterator i = v.begin(); i != v.end(); i++) {
		result += (*i ? '1' : '0');
	}

	return result;
}

std::vector<bool> commons::utils::bit::to_bool_vector(const std::string &s) {
	std::vector<bool> result;
	typedef std::string::const_iterator iterator;
	for (iterator i = s.begin(); i != s.end(); i++) {
		result.push_back(*i == '1');
	}
	return result;
}
