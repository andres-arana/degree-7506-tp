/******************************************************************************
 * main.cpp
 * 		Punto de entrada al programa cuando se compila el ejecutable que
 * 		permite interfacear con el compresor PPMC con btree
******************************************************************************/
#include "arithmetic/symbol_distribution.h"
#include "bplus/bplus_container.h"
#include "commons/cmdline/compression_client.h"
#include "config/config.h"
#include <cstdio>
#include <cstdlib>

int main(int argc, char **argv) {
	container::associative_container<std::string, arithmetic::symbol_distribution> *container
		= new bplus::bplus_container<std::string, arithmetic::symbol_distribution>("contexts.data", BPLUS_BLOCK_SIZE);
	{
		commons::cmdline::compression_client client(container);
		client.run(argc, argv);
	}
	delete container;
	std::remove("contexts.data");
	return EXIT_SUCCESS;
}
