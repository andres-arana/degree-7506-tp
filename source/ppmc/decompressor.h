/*
 * decompressor.h
 *
 */

#ifndef DECOMPRESSOR_H_
#define DECOMPRESSOR_H_

#include "../commons/io/binary_source.h"
#include "../commons/io/char_destination.h"
#include "../arithmetic/decompresor.h"
#include "../config/config.h"
#include "context_buffer.h"
#include "../associative_container.h"

namespace ppmc {

/**
 * Compresor PPMC
 */
class decompressor {
private:
	typedef container::associative_container<std::string, arithmetic::symbol_distribution> context_container;
	arithmetic::decompressor<ARITHMETIC_COMPRESSOR_PRECISION> arithmetic_decompressor;
	arithmetic::symbol_distribution context_zero;
	arithmetic::symbol_distribution context_default;
	arithmetic::symbol_distribution *distribution_cache;
	context_buffer buffer;
	context_container *container;

	void update_esc_emissions(int matching_context);
	void update_char_emissions(unsigned char c);
public:
	/**
	 * Crea una nueva instancia de source
	 * que emite chars al destination.
	 */
	decompressor(commons::io::binary_source &source, int max_contexts, context_container *container);

	/**
	 * Descomprime los bits que se obtienen desde source
	 * y emite el texto comprimido en destination.
	 */
	void decompress(commons::io::char_destination &destination);
};

};


#endif /* DECOMPRESSOR_H_ */
