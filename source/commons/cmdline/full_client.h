/******************************************************************************
 * full_client.h
 * 		Declaración de la clase commons::cmdline::full_client
******************************************************************************/
#ifndef __COMMONS_CMDLINE_FULL_CLIENT_H_INCLUDED__
#define __COMMONS_CMDLINE_FULL_CLIENT_H_INCLUDED__

#include "read_client.h"
#include <string>
#include <utility>

namespace commons {
namespace cmdline {

/**
 * Cliente de la linea de comandos para interactuar
 * con una estructura de datos que permite todas las
 * operaciones de un read client y además puede
 * modificar y eliminar elementos
 */
class full_client : public read_client{
private:
	TCLAP::SwitchArg update_switch;
	TCLAP::SwitchArg delete_switch;
public:
	/**
	 * Crea una nueva instancia de full_client con titulo
	 * title
	 */
	full_client(const std::string &title);

	virtual ~full_client() {}
protected:
	/**
	 * Callback a ejecutar cuando se detecta una modificación
	 * de un elemento
	 */
	virtual void do_update(const std::pair<int, std::string> &element) = 0;

	/**
	 * Callback a ejecutar cuando se detecta una eliminación
	 * de un elemento
	 */
	virtual void do_delete(int key) = 0;

	/**
	 * Sobreescribe de read_client
	 */
	virtual void register_argument_switchs(std::vector<TCLAP::Arg *> &args);

	/**
	 * Sobreescribe de read_client
	 */
	virtual void do_operation_on_element(const std::pair<int, std::string> &element);
};

};
};

#endif
