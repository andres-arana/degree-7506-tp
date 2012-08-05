/*
 * decompressor.cpp
 *
 */

#include "decompressor.h"
#include <memory>

using namespace ppmc;

decompressor::decompressor(commons::io::binary_source &source, int max_contexts, context_container *container)
: arithmetic_decompressor(source), context_default(arithmetic::symbol::SEOF), buffer(max_contexts), container(container) {
	// Inicializamos el contexto equiprobable default con todos los chars posibles
	for (unsigned int c = 0; c < 256; c++) {
		context_default.register_symbol_emision(arithmetic::symbol::for_char(c));
	}
}

void decompressor::decompress(commons::io::char_destination &destination) {

	while (true) {
		distribution_cache = new arithmetic::symbol_distribution[buffer.max_context()];
		arithmetic::symbol_distribution::exclusion_set exclusion;

		unsigned char matching_char = 0;
		int matching_context = -1;
		for (int i = static_cast<int>(buffer.max_context()) - 1; i >= 0; i--) {
			std::pair<bool, arithmetic::symbol_distribution> result = container->search_for_element(buffer.get_context(i+1));

			if (result.first) {
				//Utilizo la distribución del contenedor
				distribution_cache[i] = result.second;
			} else {
				//Creo una nueva distribución para ese contexto
				distribution_cache[i] = arithmetic::symbol_distribution();
			}

			if (matching_context == -1) {
				arithmetic::symbol_distribution excluded_distribution = distribution_cache[i].exclude(exclusion);
				arithmetic::symbol s = arithmetic_decompressor.decompress(excluded_distribution);
				if (s != arithmetic::symbol::ESC) {
					matching_char = s.get_char_code();
					matching_context = i + 1;
				} else {
					distribution_cache[i].append_to_exclusion_set(exclusion);
				}
			}

		}

		if (matching_context == -1) {
			arithmetic::symbol_distribution excluded_distribution = context_zero.exclude(exclusion);
			arithmetic::symbol s = arithmetic_decompressor.decompress(excluded_distribution);
			if (s != arithmetic::symbol::ESC) {
				matching_char = s.get_char_code();
				matching_context = 0;
			} else {
				context_zero.append_to_exclusion_set(exclusion);
			}
		}

		if (matching_context == -1) {
			arithmetic::symbol s = arithmetic_decompressor.decompress(context_default);
			if (s == arithmetic::symbol::SEOF) {
				break;
			}
			matching_char = s.get_char_code();
		}

		update_esc_emissions(matching_context);
		update_char_emissions(matching_char);

		destination.emit_char(matching_char);
		buffer.push_char(matching_char);
		delete[] distribution_cache;
	}

}

void decompressor::update_esc_emissions(int matching_context) {
	if (matching_context < 0) {
		if (!context_zero.has_only_the_symbol(arithmetic::symbol::ESC))
			context_zero.register_symbol_emision(arithmetic::symbol::ESC);
	}
	for (unsigned int i = (matching_context >= 0 ? matching_context : 0); i < buffer.max_context(); i++) {
		if (!distribution_cache[i].has_only_the_symbol(arithmetic::symbol::ESC))
			distribution_cache[i].register_symbol_emision(arithmetic::symbol::ESC);
	}
}

void decompressor::update_char_emissions(unsigned char c) {
	context_zero.register_symbol_emision(arithmetic::symbol::for_char(c));
	for (unsigned int i = 0; i < buffer.max_context(); i++) {
		distribution_cache[i].register_symbol_emision(arithmetic::symbol::for_char(c));
		try {
			container->update_element(buffer.get_context(i+1), distribution_cache[i]);
		} catch (context_container::not_found_exception &not_found_exception) {
			container->add_element(buffer.get_context(i+1),distribution_cache[i]);
		}
	}
}
