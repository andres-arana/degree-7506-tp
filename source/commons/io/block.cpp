/******************************************************************************
 * block.cpp
 * 		Definiciones de la clase commons::io::block
******************************************************************************/
#include "block.h"
#include "../assertions/assertions.h"
#include <algorithm>

using namespace commons::io;

block::block(int size, const char *data)
: size(size) {
	ASSERTION(size > 0);

	this->data = new char[size];

	if (data != 0) {
		std::copy(data, data + size, this->data);
	}
}

block::block(const block &other) {
	size = other.size;
	data = new char[size];

	std::copy(other.data, other.data + size, data);
}

block &block::operator=(const block &other) {
	delete[] data;

	size = other.size;
	data = new char[size];

	std::copy(other.data, other.data + size, data);

	return *this;
}

char block::operator[](int index) const {
	ASSERTION(index < size);
	ASSERTION(index >= 0);
	return data[index];
}

char &block::operator[](int index) {
	ASSERTION(index < size);
	ASSERTION(index >= 0);
	return data[index];
}

const char *block::raw_char_pointer() const {
	return data;
}

char *block::raw_char_pointer() {
	return data;
}

int block::get_size() const {
	return size;
}

void block::erase_and_compact(int index_from, int index_to) {
	ASSERTION(index_to >= index_from);
	ASSERTION(index_from >= 0);
	ASSERTION(index_to < size);
	std::copy(data + index_to + 1, data + size, data + index_from);
}

void block::insert_and_expand(int index_at, const char *new_data, int data_size) {
	// Hago espacio para los bytes que hay que meter
	make_gap(index_at, data_size);

	// Ahora si piso los bytes que quedaron copiados con los nuevos
	std::copy(new_data, new_data + data_size, data + index_at);
}

void block::make_gap(int index_at, int gap_size) {
	ASSERTION(index_at >= 0);
	ASSERTION(index_at < size);
	ASSERTION(index_at + gap_size <= size);

	// Tengo que copiar en orden reverso todos los elementos
	// desde el final del bloque (size - 1) hasta que el origen desde
	// donde copio (i - data_size) llegue al indice donde estoy
	// insertando
	for (int i = size - 1; i - gap_size >= index_at; i--) {
		data[i] = data[i - gap_size];
	}
}

void block::dump_to_stream(std::ostream &output) const {
	output << "[[";
	for (int i = 0; i < size; i++) {
		output << static_cast<int>(data[i]) << " ";
	}
	output << "]]";
}

void block::copy_to_block(block &other) const {
	std::copy(data, data + size, other.data);
}

block::~block() {
	delete[] data;
}


