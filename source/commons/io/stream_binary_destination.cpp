/******************************************************************************
 * stream_binary_destination.cpp
 * 		Definiciones de la clase commons::io::stream_binary_destination
******************************************************************************/
#include "stream_binary_destination.h"

using namespace commons::io;

stream_binary_destination::stream_binary_destination(std::ostream &o)
: o(o), current_char(0), current_char_position(0) {

}

void stream_binary_destination::emit_bit(bool bit) {
	if (current_char_position >= 8) {
		o.put(current_char);
		current_char_position = 0;
		current_char = 0;
	}

	if (bit)
		current_char |= (1 << (7 - current_char_position));
	current_char_position++;
}

stream_binary_destination::~stream_binary_destination() {
	if (current_char_position > 0)
		o.put(current_char);
}
