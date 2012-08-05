/******************************************************************************
 * read_client.cpp
 * 		Definición de la clase commons::cmdline::read_client
******************************************************************************/
#include "read_client.h"
#include "../input/pair_parser.h"
#include <iostream>

using namespace commons::cmdline;
using namespace commons::input;
using namespace std;

namespace {

const char *search_description = "Reads a key-value pair from standard input, and then searches the data "
		"structure for an element with that key. Outputs the contents of the element to standard output "
		"if the element exists, otherwise outputs [NOT FOUND]";

const char *insert_description = "Reads a key-value pair from standard input, and then attempts to insert "
		"elements with those key-value pairs in the data structure. Outputs [DUPLICATE KEY __key__] for "
		"each element which already existed on the structure to the standard output, otherwise outputs [OK]";

const char *dump_description = "Outputs the contents of the entire data structure to the standard output.";

const char *filename_description = "The file in which we will create the data structure";

};

read_client::read_client(const string &title)
: parser(title, ' ', "1.0"),
  filename("filename", filename_description, true, "", "path to file"),
  search_switch("B", "search", search_description),
  insert_switch("I", "insert", insert_description),
  dump_switch("S", "dump", dump_description) {

}

void read_client::run(int argc, char **argv) {
	try {
		// Agrego el argumento de nombre de archivo
		parser.add(filename);
		// Genero los argumentos y los registro
		// en el parser
		vector<TCLAP::Arg *>args;
		register_argument_switchs(args);
		parser.xorAdd(args);

		// Parseo los argumentos
		parser.parse(argc, argv);

		// Inicializo las estructuras de datos
		// de los clientes hijos de acuerdo
		// a los valores parseados de la linea
		// de comandos
		initialize_data_structure(filename.getValue());

		// Ejecuto operaciones generales que no
		// dependan de los elementos que vienen
		// por stdin
		if (do_operation()) {

			// Ahora si, parseo stdin y llamo a las
			// operaciones que correspondan
			pair_parser input;
			while (true) {
				char input_char = cin.get();
				if (cin.eof() || cin.fail())
					break;

				if (!input.feed_char(input_char))
					do_operation_on_element(input.get_pair());
			}
			input.feed_ended();
		}
	} catch (TCLAP::ArgParseException &e) {
		cout << e.error() << endl;
	} catch (pair_parser::parse_exception &e) {
		cout << "Input parse error: " << e.what() << endl;
	}
}

void read_client::register_argument_switchs(vector<TCLAP::Arg *> &args) {
	args.push_back(&search_switch);
	args.push_back(&insert_switch);
	args.push_back(&dump_switch);
}

bool read_client::do_operation() {
	if (dump_switch.getValue()) {
		do_dump();
		return false;
	}
	return true;
}

void read_client::do_operation_on_element(const pair<int, string> &element) {
	if (search_switch.getValue()) {
		do_search(element.first);
	} else if (insert_switch.getValue()) {
		do_insert(element);
	}
}
