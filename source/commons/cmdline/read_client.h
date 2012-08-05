/******************************************************************************
 * read_client.h
 * 		Declaración de la clase commons::cmdline::read_client
******************************************************************************/
#ifndef __COMMONS_CMDLINE_READ_CLIENT_H_INCLUDED__
#define __COMMONS_CMDLINE_READ_CLIENT_H_INCLUDED__

#include "../../dependencies/tclap/CmdLine.h"
#include <vector>
#include <string>
#include <utility>

namespace commons {
namespace cmdline {

/**
 * Cliente de la linea de comandos para interactuar
 * con una estructura de datos que permite inserciones,
 * búsquedas y dump sólamente.
 */
class read_client {
	TCLAP::CmdLine parser;

	TCLAP::UnlabeledValueArg<std::string> filename;
	TCLAP::SwitchArg search_switch;
	TCLAP::SwitchArg insert_switch;
	TCLAP::SwitchArg dump_switch;
public:
	/**
	 * Crea una nueva instancia de read_client con titulo
	 * title
	 */
	read_client(const std::string &title);

	/**
	 * Ejecuta el cliente, interactuando con el usuario
	 * a través de la linea de comandos y llamando a los
	 * callbacks necesarios para procesar la entrada
	 */
	void run(int argc, char **argv);

	virtual ~read_client() {}

protected:
	/**
	 * Callback a ejecutar cuando se detecta una búsqueda
	 * de un elemento
	 */
	virtual void do_search(int key) = 0;

	/**
	 * Callback a ejecutar cuando se detecta una inserción
	 * de un elemento
	 */
	virtual void do_insert(const std::pair<int, std::string> &element) = 0;

	/**
	 * Callback a ejecutar cuando hay que hacer un dump de la
	 * estructura a la salida estandar
	 */
	virtual void do_dump() = 0;

	/**
	 * Registra los switchs de argumentos a parsear desde la
	 * linea de comandos
	 */
	virtual void register_argument_switchs(std::vector<TCLAP::Arg *> &args);

	/**
	 * Realiza las operaciones que sean necesarias de acuerdo
	 * al estado de los argumentos. Devuelve true si hay que seguir
	 * procesando los elementos.
	 */
	virtual bool do_operation();

	/**
	 * Realiza las operaciones que sean necesarias de acuerdo al
	 * estado de los argumentos sobre el elemento parseado element
	 */
	virtual void do_operation_on_element(const std::pair<int, std::string> &element);

	/**
	 * Inicializa la estructura de datos de acuerdo al nombre de
	 * archivo especificado en la linea de comandos
	 */
	virtual void initialize_data_structure(std::string filename) = 0;
};

};
};

#endif
