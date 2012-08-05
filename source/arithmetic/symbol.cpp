/******************************************************************************
 * symbol.cpp
 * 		Definiciones de la clase arithmetic::symbol
******************************************************************************/
#include "symbol.h"

using namespace arithmetic;

symbol::symbol(unsigned int code)
: code(code) {

}

const symbol symbol::SEOF(256u);
const symbol symbol::ESC(257u);

symbol symbol::for_char(unsigned char c) {
	return symbol(static_cast<unsigned int>(c));
}

bool symbol::operator==(const symbol &other) const {
	return code == other.code;
}

bool symbol::operator!=(const symbol &other) const {
	return code != other.code;
}

bool symbol::operator<(const symbol &other) const {
	return code < other.code;
}

bool symbol::operator<=(const symbol &other) const {
	return code <= other.code;
}

bool symbol::operator>(const symbol &other) const {
	return code > other.code;
}

bool symbol::operator>=(const symbol &other) const {
	return code >= other.code;
}

unsigned char symbol::get_char_code() const {
	return static_cast<char>(code);
}

unsigned int symbol::get_sequential_code() const {
	return code;
}

std::ostream &arithmetic::operator<<(std::ostream &output, const symbol &s) {
	if (s == symbol::SEOF) {
		return output << "EOF";
	} else if (s == symbol::ESC) {
		return output << "ESC";
	}
	return output << s.get_char_code();
}
