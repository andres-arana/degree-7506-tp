/******************************************************************************
 * pair_parser.cpp
 * 		Definición de los operadores de parseo de entrada
******************************************************************************/
#include "pair_parser.h"
#include <cctype>

using namespace commons::input;
using namespace std;

pair_parser::parse_exception::parse_exception(const string &what)
: runtime_error(what) {

}

pair_parser::parse_exception::~parse_exception() throw() {

}

bool pair_parser::initial_state(char c) {
	// Ignoro los espacios
	if (std::isspace(c)) {
	// Si es una apertura de paréntesis, empiezo a leer la clave
	} else if (c == '(') {
		current_state_processor = &pair_parser::reading_key_state;
	// Cualquier otro caso es un error
	} else {
		stringstream error_description;
		error_description << "Invalid char '" << c <<"' while expecting '(' or whitespace";
		throw parse_exception(error_description.str());
	}

	return true;
}

bool pair_parser::reading_key_state(char c) {
	// Si es un número, acumulo
	if (std::isdigit(c)) {
		current_state_buffer << c;
	// Si es un punto y coma, terminé de leer la clave
	} else if (c == ';') {
		current_state_buffer >> current_key;
		current_state_buffer.str(string());
		current_state_buffer.clear();
		current_state_processor = &pair_parser::reading_value_state;
	// Cualquier otro caso es un error
	} else {
		stringstream error_description;
		error_description << "Invalid char '" << c <<"' while expecting ';' or digit";
		throw parse_exception(error_description.str());
	}

	return true;
}

bool pair_parser::reading_value_state(char c) {
	// Si es un paréntesis de cierre, terminé de leer un par
	if (c == ')') {
		current_value = current_state_buffer.str();
		current_state_buffer.str(string());
		current_state_buffer.clear();
		current_state_processor = &pair_parser::initial_state;
		return false;
	// Si es un paréntesis de apertura, alguien la pifió
	} else if (c == '(') {
		stringstream error_description;
		error_description << "Invalid char '" << c <<"' while expecting any char but '('";
		throw parse_exception(error_description.str());
	// Cualquier otro caso, es un caracter del value
	} else {
		current_state_buffer << c;
		return true;
	}
}

pair_parser::pair_parser() {
	current_state_processor = &pair_parser::initial_state;
}

bool pair_parser::feed_char(char c) {
	return (this->*current_state_processor)(c);
}

pair_parser::pair_type pair_parser::get_pair() {
	return make_pair(current_key, current_value);
}

void pair_parser::feed_ended() {
	// Si termina el feed y no estamos en initial, hubo problemas
	if (current_state_processor != &pair_parser::initial_state) {
		throw parse_exception("End of feed reached before finishing reading a pair");
	}
}
