/******************************************************************************
 * main.cpp
 * 		Punto de entrada al programa cuando se compila el ejecutable que
 * 		permite interfacear con el compresor PPMC con hash
******************************************************************************/
#include "arithmetic/symbol_distribution.h"
#include "hash/hash_container.h"
#include "commons/cmdline/compression_client.h"
#include "config/config.h"
#include <cstdio>
#include <cstdlib>

int main(int argc, char **argv) {
	container::associative_container<std::string, arithmetic::symbol_distribution> *container
		= new hash::hash_container<std::string, arithmetic::symbol_distribution>("contexts.data", "contexts.index", HASH_BLOCK_SIZE);
	{
		commons::cmdline::compression_client client(container);
		client.run(argc, argv);
	}
	delete container;
	std::remove("contexts.data");
	std::remove("contexts.index");
	return EXIT_SUCCESS;
}
