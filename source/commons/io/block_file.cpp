/******************************************************************************
 * block_file.cpp
 * 		Definiciones de la clase commons::io::block_file
******************************************************************************/
#include "block_file.h"
#include "../assertions/assertions.h"

using namespace commons::io;
using namespace std;

void block_file::initialize_file(const char *filename, int block_size) {
	ASSERTION(block_size > 0);

	this->block_size = block_size;

	file.open(filename, ios_base::in | ios_base::out | ios_base::binary | ios_base::ate);
	just_created = !file.is_open();
	if (just_created) {
		file.clear();
		file.open(filename, ios_base::in | ios_base::out | ios_base::binary | ios_base::trunc);
	}
}

block_file::block_file(const char *filename, int block_size, const initializer &initializer) {
	initialize_file(filename, block_size);
	if (just_created)
		initializer.initialize(this);
}

block_file::block_file(const char *filename, int block_size) {
	initialize_file(filename, block_size);
}

void block_file::close() {
	file.close();
}

block block_file::read_block(int position) {
	block result(block_size);
	read_block_into(position, &result);
	return result;
}

void block_file::read_block_into(int position, block *b) {
	ASSERTION(position < get_block_count());
	ASSERTION(b->get_size() == block_size);

	file.seekg(position * block_size);
	file.read(b->raw_char_pointer(), block_size);
}

void block_file::write_block(int position, const block &b) {
	ASSERTION(position < get_block_count());
	ASSERTION(b.get_size() == block_size);

	file.seekp(position * block_size);
	file.write(b.raw_char_pointer(), block_size);
}

void block_file::append_block(const block &b) {
	ASSERTION(b.get_size() == block_size);

	file.seekp(0, ios_base::end);
	file.write(b.raw_char_pointer(), block_size);
}

int block_file::get_block_count() {
	file.seekp(0, ios_base::end);
	return file.tellp() / block_size;
}

int block_file::get_block_size() const {
	return block_size;
}

bool block_file::is_just_created() const {
	return just_created;
}

block_file::~block_file() {

}
