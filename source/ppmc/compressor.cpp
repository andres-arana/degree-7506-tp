/******************************************************************************
 * compressor.cpp
 * 		Definiciones de la clase ppmc::compressor
******************************************************************************/
#include "compressor.h"

using namespace ppmc;
using namespace std;

compressor::compressor(commons::io::binary_destination &destination, int max_contexts, context_container *container)
: arithmetic_compressor(destination), context_default(arithmetic::symbol::SEOF), buffer(max_contexts), container(container) {
	// Inicializamos el contexto equiprobable default con todos los chars posibles
	for (unsigned int c = 0; c < 256; c++) {
		context_default.register_symbol_emision(arithmetic::symbol::for_char(c));
	}
}

void compressor::compress(commons::io::char_source &source) {
	LOG_DEBUG("Start Compression with ppmc");

	std::pair<bool, unsigned char> char_extraction = source.get_next_char();

	// Procesar cada char individualmente
	while (char_extraction.first) {
		LOG_DEBUG_VAR(arithmetic::symbol::for_char(char_extraction.second));
		process_char(arithmetic::symbol::for_char(char_extraction.second));
		char_extraction = source.get_next_char();
	}

	// Por último, emitir el eof y terminar
	process_char(arithmetic::symbol::SEOF);
	arithmetic_compressor.finish_compression();
}

void compressor::process_char(const arithmetic::symbol &c) {
	LOG_DEBUG("Process char");
	LOG_DEBUG_VAR(c.get_sequential_code());
	LOG_DEBUG_VAR(static_cast<int>(buffer.max_context()));
	distribution_cache = new arithmetic::symbol_distribution[buffer.max_context()];
	arithmetic::symbol_distribution::exclusion_set exclusion;

	// Indica en que contexto se emitió el char
	int matching_context = search_for_match_in_contexts(c, exclusion);

	// Si no se paro el ciclo antes de llegar al contexto-0
	// entonces tenemos que intentar con el contexto-0
	if (matching_context < 0) {
		matching_context = search_for_match_in_context_zero(c, exclusion);
	}

	// Si llegamos acá y todavía nadie matcheo, entonces tenemos que emitir con el contexto equiprobable
	if (matching_context < 0) {
		arithmetic_compressor.compress(c, context_default);
		LOG_DEBUG("Matching in context -1");
	}

	// Ahora hay que actualizar todos los contextos en los que no hubo match para
	// que registren que hubo una emisión de escape
	update_esc_emissions(matching_context);

	// Actualizamos todos los contextos (independientemente de si hubo match o no)
	// para que se tenga en cuenta esta emisión
	if (c != arithmetic::symbol::SEOF && c != arithmetic::symbol::ESC) {
		update_char_emissions(c.get_char_code());

		buffer.push_char(c.get_char_code());
	}

	delete[] distribution_cache;
}

int compressor::search_for_match_in_contexts(const arithmetic::symbol &c, arithmetic::symbol_distribution::exclusion_set &exclusion) {
	LOG_DEBUG("Search for match in contexts");
	int matching_context = -1;
	// Busco todos los contextos, empezando por el mayor posible,
	// en el contenedor asociativo
	for (int i = static_cast<int>(buffer.max_context()) - 1; i >= 0; i--) {
		LOG_DEBUG("Context:");
		LOG_DEBUG_VAR(buffer.get_context(i+1));
		// Busco el contexto en el contenedor persistente de contextos
		std::pair<bool, arithmetic::symbol_distribution> search_result = container->search_for_element(buffer.get_context(i+1));
		// Si está, la copio en la lista de distribuciones a actualizar
		// sino genero una distribución nueva
		if (search_result.first) {
			distribution_cache[i] = search_result.second;
		} else {
			distribution_cache[i] = arithmetic::symbol_distribution();
		}

		LOG_DEBUG("Distribution before compressing");
		LOG_DEBUG_VAR(distribution_cache[i]);

		// Si ya hubo match, lo único que necesitabamos era cargar la distribución para
		// poder actualizarla después, así que paramos acá. Pero si no, hay que ver si
		// podemos emitir el char con esta distribución.
		if (matching_context == -1) {
			if (distribution_cache[i].has_symbol(c)) {
				// Encontramos una distribución que contiene el símbolo!
				arithmetic::symbol_distribution excluded_distribution = distribution_cache[i].exclude(exclusion);
				arithmetic_compressor.compress(c, excluded_distribution);
				matching_context = i + 1;
				LOG_DEBUG("Matcheo en el contexto!!");
				LOG_DEBUG_VAR(matching_context);
			} else {
				// Emitimos un escape en este contexto
				arithmetic::symbol_distribution excluded_distribution = distribution_cache[i].exclude(exclusion);
				arithmetic_compressor.compress(arithmetic::symbol::ESC, excluded_distribution);
				distribution_cache[i].append_to_exclusion_set(exclusion);
				LOG_DEBUG("Emito ESC");
			}
		}
	}

	return matching_context;
}

int compressor::search_for_match_in_context_zero(const arithmetic::symbol &c, arithmetic::symbol_distribution::exclusion_set &exclusion) {
	LOG_DEBUG("Search for match in context Zero");
	int matching_context = -1;
	LOG_DEBUG_VAR(context_zero);
	if (context_zero.has_symbol(c)) {
		arithmetic::symbol_distribution excluded_distribution = context_zero.exclude(exclusion);
		arithmetic_compressor.compress(c, excluded_distribution);
		matching_context = 0;
		LOG_DEBUG("Matching in context Zero!!!");
	} else {
		arithmetic::symbol_distribution excluded_distribution = context_zero.exclude(exclusion);
		arithmetic_compressor.compress(arithmetic::symbol::ESC, excluded_distribution);
		context_zero.append_to_exclusion_set(exclusion);
		LOG_DEBUG("Emito ESC");
	}
	return matching_context;
}

void compressor::update_esc_emissions(int matching_context) {
	LOG_DEBUG("Update ESC emissions");
	if (matching_context < 0) {
		if (!context_zero.has_only_the_symbol(arithmetic::symbol::ESC)) {
 			context_zero.register_symbol_emision(arithmetic::symbol::ESC);
			LOG_DEBUG("Update ESC emission in context Zero");
		}
	}
	for (unsigned int i = (matching_context >= 0 ? matching_context : 0); i < buffer.max_context(); i++) {
		if (!distribution_cache[i].has_only_the_symbol(arithmetic::symbol::ESC)) {
			LOG_DEBUG("Update ESC emission in context...");
			LOG_DEBUG_VAR(i);
			distribution_cache[i].register_symbol_emision(arithmetic::symbol::ESC);
		}
	}
}

void compressor::update_char_emissions(unsigned char c) {
	LOG_DEBUG("Update Char emission");
	LOG_DEBUG_VAR(c);
	context_zero.register_symbol_emision(arithmetic::symbol::for_char(c));
	for (unsigned int i = 0; i < buffer.max_context(); i++) {
		LOG_DEBUG_VAR(i);
		distribution_cache[i].register_symbol_emision(arithmetic::symbol::for_char(c));
		try {
			container->update_element(buffer.get_context(i+1), distribution_cache[i]);
		} catch (context_container::not_found_exception &not_found_exception) {
			container->add_element(buffer.get_context(i+1),distribution_cache[i]);
		}
	}
}
