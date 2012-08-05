/******************************************************************************
 * context_buffer.cpp
 * 		Definiciones de la clase ppmc::context_buffer
******************************************************************************/
#include "context_buffer.h"

using namespace ppmc;

context_buffer::context_buffer(unsigned int max_size)
: max_size(max_size), buffer_start_position(0) {

}

void context_buffer::push_char(unsigned char c) {
	if (buffer.size() < max_size) {
		buffer += c;
	} else {
		buffer[buffer_start_position] = c;
		buffer_start_position++;
		buffer_start_position %= max_size;
	}
}

bool context_buffer::has_context(unsigned int context_size) const {
	return buffer.size() >= context_size;
}

unsigned int context_buffer::max_context() const {
	return buffer.size();
}

std::string context_buffer::get_context(unsigned int context_size) const {
	std::string result;

	for (std::string::size_type i = buffer_start_position + buffer.size() - context_size; i < buffer.size(); i++) {
		result += buffer[i];
	}

	int actual_start_position = ((int)buffer_start_position - (int)context_size < 0) ? 0 : buffer_start_position - context_size;
	for (std::string::size_type i = actual_start_position; i < buffer_start_position; i++) {
		result += buffer[i];
	}

	return result;
}
