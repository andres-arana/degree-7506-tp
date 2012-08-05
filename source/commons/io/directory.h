/******************************************************************************
 * directory.h
 * 		Declaraciones de la clase commons::io::directory
******************************************************************************/
#ifndef __COMMONS_IO_DIRECTORY_H_INCLUDED__
#define __COMMONS_IO_DIRECTORY_H_INCLUDED__

#include <string>
#include <vector>
#include <fstream>

namespace commons {
namespace io {

/**
 * Clase que maneja el acceso a la información de un
 * directorio en disco.
 */
class directory {
private:
	std::string path;

	// Previene copias de cualquier tipo
	directory(const directory &other);
	void operator=(const directory &other);
public:
	/**
	 * Crea una nueva instancia de directory, cuyo
	 * path es path. Crea el directorio en path, si
	 * este no existe.
	 */
	directory(const std::string &path);

	/**
	 * Devuelve el path de este directorio
	 */
	const std::string &get_path() const;

	/**
	 * Devuelve una lista de los nombres de archivos
	 * en el directorio que empiecen como begin y terminen
	 * con end
	 */
	std::vector<std::string> get_list_filenames(const std::string &begin = "", const std::string &end = "") const;

	/**
	 * Elimina el directorio y todos sus archivos
	 */
	void remove();
};

};
};

#endif
