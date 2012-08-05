/******************************************************************************
 * string_utils.h
 * 		Declaraciones de las funciones de commons::utils::strings
******************************************************************************/
#ifndef __COMMONS_UTILS_STRINGUTILS_H_INCLUDED__
#define __COMMONS_UTILS_STRINGUTILS_H_INCLUDED__

#include <string>
#include <vector>
#include <sstream>

namespace commons {
namespace utils {
namespace strings {
/**
 * Devuelve tokens de un string de acuerdo a un delimitador pasado
 * como parámetro
 */
void split(const std::string &str, const std::string &delim, std::vector<std::string> &results);

/**
 * Recibe un string str y lo devuelve en mayusculas en la variable
 * uppercase
 */
void to_uppercase(const std::string &str, std::string &uppercase);

/**
 * Convierte cualquier cosa a string
 */
template<typename T>
std::string anything_to_string(const T &what) {
	std::stringstream result;
	result << what;
	return result.str();
}

};
};
};

#endif
