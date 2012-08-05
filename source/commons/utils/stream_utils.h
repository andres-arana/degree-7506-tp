/******************************************************************************
 * stream_utils.h
 * 		Declaraciones de las funciones de commons::utils::streams
******************************************************************************/
#ifndef __COMMONS_UTILS_STREAMUTILS_H_INCLUDED__
#define __COMMONS_UTILS_STREAMUTILS_H_INCLUDED__

#include <fstream>

namespace commons {
namespace utils {
namespace streams {

/**
 * Devuelve true si el tamaño del archivo file es menos que
 * byte_size
 */
bool is_size_less_than(std::fstream &file, int byte_size);

/**
 * Devuelve el tamaño de un archivo existente
 */
int file_size(const std::string &filename);

/**
 * Renombra un archivo con path current_filename a new_filename
 */
void rename(const std::string &current_filename, const std::string &new_filename);

/**
 * Extrae el directorio de un nombre de archivo
 */
std::string extract_path(const std::string &filename);

/**
 * Extrae el archivo de un nombre de archivo
 */
std::string extract_filename(const std::string &filename);

};
};
};

#endif
