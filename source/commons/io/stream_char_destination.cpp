/******************************************************************************
 * stream_char_destination.cpp
 * 		Definiciones de la clase commons::io::stream_char_destination
******************************************************************************/
#include "stream_char_destination.h"

using namespace commons::io;

stream_char_destination::stream_char_destination(std::ostream &o)
: o(o) {

}

void stream_char_destination::emit_char(unsigned char c) {
	o << c;
}
