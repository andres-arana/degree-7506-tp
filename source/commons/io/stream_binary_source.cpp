/******************************************************************************
 * stream_binary_source.cpp
 * 		Definiciones de la clase commons::io::stream_binary_source
******************************************************************************/
#include "stream_binary_source.h"

using namespace commons::io;

stream_binary_source::stream_binary_source(std::istream &s)
: s(s), current_char(0), current_char_position(8), eof(false) {

}

std::pair<bool, bool> stream_binary_source::get_next_bit() {
	if (eof)
		return std::make_pair(false, false);

	std::pair<bool, bool> result;
	// Si la posición que tengo que devolver ahora está
	// más adelante del char actual, tengo que cargar el
	// siguiente char desde el stream
	if (current_char_position >= 8) {
		current_char_position = 0;
		result.first = s.get(current_char);
		eof = !result.first;
	} else {
		result.first = true;
	}
	result.second = current_char & (1 << (7 - current_char_position));
	current_char_position++;
	return result;
}
