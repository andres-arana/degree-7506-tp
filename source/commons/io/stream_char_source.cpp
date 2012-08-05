/******************************************************************************
 * stream_char_source.cpp
 * 		Definiciones de la clase commons::io::stream_char_source
******************************************************************************/
#include "stream_char_source.h"

using namespace commons::io;

stream_char_source::stream_char_source(std::istream &s)
: s(s) {

}

std::pair<bool, unsigned char> stream_char_source::get_next_char() {
	std::pair<bool, unsigned char> result;
	char data;
	result.first = s.get(data);
	result.second = data;
	return result;
}
