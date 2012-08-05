/******************************************************************************
 * symbol.h
 * 		Declaraciones de la clase arithmetic::symbol
******************************************************************************/
#ifndef __ARITHMETIC_SYMBOL_H_INCLUDED__
#define __ARITHMETIC_SYMBOL_H_INCLUDED__

#include <iostream>

/**
 * Constante que representa la cantidad de simbolos
 * distintos posibles
 */
#define ARITHMETIC_SYMBOL_COUNT 258

namespace arithmetic {

/**
 * Representa un símbolo que se puede comprimir con el compresor
 * aritmético.
 */
class symbol {
private:
	unsigned int code;
public:
	/**
	 * Constante que representa el símbolo no-char EOF
	 */
	static const symbol SEOF;

	/**
	 * Constante que representa el símbolo no-char ESC
	 */
	static const symbol ESC;

	/**
	 * Devuelve el símbolo correspondiente a un char dado
	 */
	static symbol for_char(unsigned char c);

	/**
	 * Crea un nuevo símbolo con un código sequencial code
	 */
	symbol(unsigned int code);

	/**
	 * Comparación entre símbolos
	 */
	bool operator==(const symbol &other) const;
	bool operator!=(const symbol &other) const;
	bool operator<(const symbol &other) const;
	bool operator<=(const symbol &other) const;
	bool operator>(const symbol &other) const;
	bool operator>=(const symbol &other) const;

	/**
	 * Obtiene el char representado por el símbolo
	 * si este símbolo no es EOF o ESC
	 */
	unsigned char get_char_code() const;

	/**
	 * Obtiene el código secuencial numérico del símbolo
	 */
	unsigned int get_sequential_code() const;
};

std::ostream &operator<<(std::ostream &output, const symbol &s);

};

#endif
