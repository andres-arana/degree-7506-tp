/******************************************************************************
 * compression_client.cpp
 * 		Definición de la clase commons::cmdline::compression_client
******************************************************************************/
#include "compression_client.h"
#include "../utils/stream_utils.h"
#include "../io/stream_char_source.h"
#include "../io/stream_char_destination.h"
#include "../io/stream_binary_source.h"
#include "../io/stream_binary_destination.h"
#include "../../ppmc/compressor.h"
#include "../../ppmc/decompressor.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

using namespace commons::cmdline;
using namespace std;

namespace {

const char *compress_description = "Su presencia indica que el cliente se está ejecutando en modo compresión. "
		"Indica además la cantidad máxima de contextos que deben utilizarse.";

const char *decompress_description = "Su presencia indica que el cliente se está ejecutando en modo descompresión. "
		"Indica además la cantidad máxima de contextos que deben utilizarse.";

const char *filename_description = "The file in which we will create the data structure";

const char *show_st_description = "Su presencia indica que el cliente emitirá estadísticas detalladas de la "
		"compresión / descompresión, como cantidad de matchs por contexto y tamaño antes y después de comprimir.";

const char *verbose_description = "Su presencia indica que el cliente emitirá información detallada de la configuración "
		"de la tabla de contextos luego de comprimir / descomprimir.";

struct statistics_accumulator : public container::element_inspector<std::string, arithmetic::symbol_distribution> {
	typedef std::map<unsigned int, unsigned int> match_container;
	match_container matches;

	virtual void inspect(const std::string &key, const arithmetic::symbol_distribution &value) {
		unsigned int context = key.size();

		typedef arithmetic::symbol_distribution::iterator iterator;
		for (iterator it = value.begin(); it != value.end(); it++) {
			arithmetic::symbol current_symbol = *it;
			if (current_symbol != arithmetic::symbol::ESC) {
				unsigned int times_matched_symbol = value.get_emissions(current_symbol) - 1;
				matches[context] += times_matched_symbol;
			}
		}
	}

};

};

compression_client::compression_client(context_container *cont)
: parser("PPMC compressor", ' ', "1.0"),
  compress_switch("c", "compress", compress_description, true, 0, "integer"),
  decompress_switch("d", "decompress", decompress_description, true, 0, "integer"),
  filename("f", "filename", filename_description, true, "", "filepath"),
  show_statistics_switch("e", "statistics", show_st_description),
  verbose_switch("v", "verbose", verbose_description),
  c(cont) {

}

void compression_client::run(int argc, char **argv) {
	try {
		// Agrego los argumentos directos
		parser.add(filename);
		parser.add(show_statistics_switch);
		parser.add(verbose_switch);
		// Agrego los argumentos mutuamente excluyentes
		vector<TCLAP::Arg *>args;
		args.push_back(&compress_switch);
		args.push_back(&decompress_switch);
		parser.xorAdd(args);

		// Parseo los argumentos
		parser.parse(argc, argv);

		if (compress_switch.isSet()) {
			do_compression();
		} else if (decompress_switch.isSet()) {
			do_decompression();
		}

	} catch (TCLAP::ArgParseException &e) {
		cout << e.error() << endl;
	}
}

void compression_client::do_compression() {
	int max_contexts = compress_switch.getValue();
	stringstream compressed_filename_builder;
	compressed_filename_builder << filename.getValue() << ".compressed";
	string compressed_filename = compressed_filename_builder.str();

	{
		ifstream input_stream(filename.getValue().c_str());
		commons::io::stream_char_source input(input_stream);

		ofstream output_stream(compressed_filename.c_str(), ios_base::out | ios_base::binary);
		commons::io::stream_binary_destination output(output_stream);

		ppmc::compressor compressor(output, max_contexts, c);

		compressor.compress(input);
	}

	if (show_statistics_switch.isSet()) {
		print_statistics(filename.getValue(), compressed_filename);
	}

	if (verbose_switch.isSet()) {
		print_container();
	}
}

void compression_client::do_decompression() {
	int max_contexts = decompress_switch.getValue();
	stringstream decompressed_filename_builder;
	decompressed_filename_builder << filename.getValue() << ".decompressed";
	string decompressed_filename = decompressed_filename_builder.str();

	{
		ifstream input_stream(filename.getValue().c_str(), ios_base::in | ios_base::binary);
		commons::io::stream_binary_source input(input_stream);

		ofstream output_stream(decompressed_filename.c_str());
		commons::io::stream_char_destination output(output_stream);

		ppmc::decompressor decompressor(input, max_contexts, c);

		decompressor.decompress(output);

	}

	if (show_statistics_switch.isSet()) {
		print_statistics(decompressed_filename, filename.getValue());
	}

	if (verbose_switch.isSet()) {
		print_container();
	}
}

void compression_client::print_statistics(const std::string &uncompressed_filename, const std::string &compressed_filename) {
	statistics_accumulator accumulator;
	c->inspect(accumulator);

	int uncompressed_size = commons::utils::streams::file_size(uncompressed_filename);
	int compressed_size = commons::utils::streams::file_size(compressed_filename);

	std::cout << "********************************" << std::endl;
	std::cout << "Estadísticas: " << std::endl;
	std::cout << "********************************" << std::endl;
	std::cout << "Tamaño sin comprimir (b): " << uncompressed_size << std::endl;
	std::cout << "Tamaño comprimido    (b): " << compressed_size << std::endl;
	std::cout << "Matches por contexto: " << std::endl;
	typedef statistics_accumulator::match_container::iterator iterator;
	for (iterator it = accumulator.matches.begin(); it != accumulator.matches.end(); it++) {
		unsigned int current_context = (*it).first;
		unsigned int match_count = (*it).second;
		if (match_count) {
			std::cout << "Contexto " << current_context << ": " << match_count << " matches" << std::endl;
		}
	}
	std::cout << std::endl << std::endl;
}

void compression_client::print_container() {
	std::cout << "********************************" << std::endl;
	std::cout << "Estado de la tabla de contextos" << std::endl;
	std::cout << "********************************" << std::endl;
	c->dump_to_stream(std::cout);
}
