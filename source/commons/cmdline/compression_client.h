/******************************************************************************
 * compression_client.h
 * 		Declaración de la clase commons::cmdline::compression_client
******************************************************************************/
#ifndef __COMMONS_CMDLINE_COMPRESSION_CLIENT_H_INCLUDED__
#define __COMMONS_CMDLINE_COMPRESSION_CLIENT_H_INCLUDED__

#include "../../dependencies/tclap/CmdLine.h"
#include "../../associative_container.h"
#include "../../arithmetic/symbol_distribution.h"

namespace commons {
namespace cmdline {

/**
 * Cliente de la linea de comandos para interactuar
 * con un compresor ppmc.
 */
class compression_client {
	TCLAP::CmdLine parser;

	TCLAP::ValueArg<int> compress_switch;
	TCLAP::ValueArg<int> decompress_switch;
	TCLAP::ValueArg<std::string> filename;
	TCLAP::SwitchArg show_statistics_switch;
	TCLAP::SwitchArg verbose_switch;

	typedef container::associative_container<std::string, arithmetic::symbol_distribution> context_container;
	context_container *c;

	void do_compression();

	void do_decompression();

	void print_statistics(const std::string &uncompressed_filename, const std::string &compressed_filename);

	void print_container();
public:
	/**
	 * Crea un nuevo cliente de compresión que utiliza el
	 * contenedor asociativo dado.
	 */
	compression_client(context_container *cont);

	/**
	 * Ejecuta el cliente, interactuando con el usuario
	 * a través de la linea de comandos y llamando a los
	 * callbacks necesarios para procesar la entrada
	 */
	void run(int argc, char **argv);
};

};
};

#endif
