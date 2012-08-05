/******************************************************************************
 * pair_parser.h
 * 		Declaración de los operadores de parseo de entrada
******************************************************************************/
#ifndef __COMMONS_INPUT_PAIR_PARSER_H_INCLUDED__
#define __COMMONS_INPUT_PAIR_PARSER_H_INCLUDED__

#include <sstream>
#include <utility>
#include <string>
#include <stdexcept>

namespace commons {
namespace input {

/**
 * Parse de streams que puede interpretar instancias de std::pair
 * a partir de un feed de chars de la forma "(key;value)". Ignora
 * todos los espacios antes y después de los paréntesis, y eleva
 * excepciones adecuadas si el formato no es respetado.
 */
class pair_parser {
private:
	bool (pair_parser::*current_state_processor)(char);

	bool initial_state(char c);
	bool reading_key_state(char c);
	bool reading_value_state(char c);
	bool invalid_pair_read_state(char c);

	std::stringstream current_state_buffer;

	int current_key;
	std::string current_value;
public:
	/**
	 * Representa una excepción de parseo
	 */
	class parse_exception : public std::runtime_error {
	public:
		/**
		 * Crea una nueva instancia de parse_exception, cuyo
		 * error representado es descripto por what
		 */
		parse_exception(const std::string &what);

		virtual ~parse_exception() throw();
	};

	/**
	 * Crea una nueva instancia de pair_parser
	 */
	pair_parser();

	/**
	 * Tipo de los pairs que se parsean
	 */
	typedef std::pair<int, std::string> pair_type;

	/**
	 * Alimenta el parser con un char. Devuelve true si con
	 * lo que se alimento hasta el momento se puede crear un
	 * std::pair válido.
	 */
	bool feed_char(char c);

	/**
	 * Devuelve un par construido con lo que se alimentó
	 * hasta el momento.
	 */
	pair_type get_pair();

	/**
	 * Avisa al parser que no hay más chars disponibles
	 */
	void feed_ended();

};

};
};

#endif
