/******************************************************************************
 * logger.h
 * 		Declaraciones de la clase commons::log::logger
******************************************************************************/
#ifndef __COMMONS_LOG_LOGGER_H_INCLUDED__
#define __COMMONS_LOG_LOGGER_H_INCLUDED__

#include <string>
#include <vector>
#include <fstream>

namespace commons {
namespace log {

/**
 * Clase que permite loguear eventos en un contexto determinado, como por ejemplo
 * eventos asociados a una clase en particular.
 */
class logger {
private:
	std::string full_filename;
	int max_file_size;
	std::fstream log_file;
	int file_number;

	// Deshabilito copia y asignación
	logger(const logger &);
	void operator=(const logger &);

	void do_log(const std::string &severity, const std::string &what, const std::string &file, int line);
public:
	/**
	 * Permite crear un logger que utilice un directorio de
	 * logs particular
	 */
	logger(const std::string &log_filename, int max_file_size);

	/**
	 * Proporciona acceso al logger global de la aplicación
	 */
	static logger instance;

	/**
	 * Loguea what con nivel de error critico, identificando con file y
	 * line el archivo y la linea de codigo que genero el log
	 */
	void critical(const std::string &what, const std::string &file, int line);

	/*
	 * Loguea what con nivel de error, identificando con file y
	 * line el archivo y la linea de codigo que genero el log
	 */
	void error(const std::string &what, const std::string &file, int line);

	/*
	 * Loguea what con nivel de warning, identificando con file y
	 * line el archivo y la linea de codigo que genero el log
	 */
	void warn(const std::string &what, const std::string &file, int line);

	/*
	 * Loguea what con nivel de informacion, identificando con file y
	 * line el archivo y la linea de codigo que genero el log
	 */
	void info(const std::string &what, const std::string &file, int line);

	/*
	 * Loguea what con nivel de trace, identificando con file y
	 * line el archivo y la linea de codigo que genero el log
	 */
	void trace(const std::string &what, const std::string &file, int line);

	/*
	 * Loguea what con nivel de debug, identificando con file y
	 * line el archivo y la linea de codigo que genero el log
	 */
	void debug(const std::string &what, const std::string &file, int line);

	/*
	 * Loguea el valor de una variable con nivel de debug,
	 * identificando con file y line el archivo y la linea
	 * de codigo que genero el log.
	 */
	void debug_var(const std::string &variable_name, const std::string &value, const std::string &file, int line);

	/**
	 * Busca las lineas de log que contengan las palabras key_words
	 */
	std::vector<std::string> search(const std::string &key_words);

	/**
	 * Hace output de todos los contenidos de los archivos de log
	 * a un stream dado
	 */
	void dump_to_stream(std::ostream &output);
};

};
};

#endif
