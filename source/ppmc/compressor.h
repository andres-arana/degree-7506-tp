/******************************************************************************
 * compressor.h
 * 		Declaraciones de la clase ppmc::compressor
******************************************************************************/
#ifndef __PPMC_COMPRESSOR_H_INCLUDED__
#define __PPMC_COMPRESSOR_H_INCLUDED__


#include "../commons/io/char_source.h"
#include "../commons/io/binary_destination.h"
#include "../arithmetic/compressor.h"
#include "../config/config.h"
#include "context_buffer.h"
#include "../associative_container.h"

namespace ppmc {

/**
 * Compresor PPMC
 */
class compressor {
private:
	typedef container::associative_container<std::string, arithmetic::symbol_distribution> context_container;
	arithmetic::compressor<ARITHMETIC_COMPRESSOR_PRECISION> arithmetic_compressor;
	arithmetic::symbol_distribution context_zero;
	arithmetic::symbol_distribution context_default;
	arithmetic::symbol_distribution *distribution_cache;
	context_buffer buffer;
	context_container *container;

	void process_char(const arithmetic::symbol &c);

	int search_for_match_in_contexts(const arithmetic::symbol &c, arithmetic::symbol_distribution::exclusion_set &exclusion);
	int search_for_match_in_context_zero(const arithmetic::symbol &c, arithmetic::symbol_distribution::exclusion_set &exclusion);

	void update_esc_emissions(int matching_context);
	void update_char_emissions(unsigned char c);
public:
	/**
	 * Crea una nueva instancia de compressor
	 * que emite bits al destination.
	 */
	compressor(commons::io::binary_destination &destination, int max_contexts, context_container *container);

	/**
	 * Comprime los chars que se obtienen desde source
	 * y emite el texto comprimido en destination.
	 */
	void compress(commons::io::char_source &source);
};

};

#endif
