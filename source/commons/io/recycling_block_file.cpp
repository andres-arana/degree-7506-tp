/******************************************************************************
 * recycling_block_file.cpp
 * 		Definiciones de la clase commons::io::recycling_block_file
******************************************************************************/
#include "recycling_block_file.h"
#include "../assertions/assertions.h"

using namespace commons::io;
using namespace std;

void recycling_block_file::availability_initializer::initialize(block_file *file) const {
	block_availability availability(file->get_block_size());
	availability.clear();
	file->append_block(availability.get_block());
}

void recycling_block_file::read_availability_block() {
	file.read_block_into(0, &availability.get_block());
}

void recycling_block_file::write_availability_block() {
	file.write_block(0, availability.get_block());
}

recycling_block_file::recycling_block_file(const char *filename, int block_size)
: availability(block_size), file(filename, block_size, availability_initializer()) {
	read_availability_block();
}

recycling_block_file::recycling_block_file(const char *filename, int block_size, const recycling_block_file::initializer &initializer)
: availability(block_size), file(filename, block_size, availability_initializer()) {
	read_availability_block();
	if (file.is_just_created())
		initializer.initialize(this);

}

void recycling_block_file::close() {
	file.close();
}

block recycling_block_file::read_block(int position) {
	ASSERTION(!availability.is_available(position));

	return file.read_block(position + 1);
}

void recycling_block_file::read_block_into(int position, block *b) {
	ASSERTION(!availability.is_available(position));

	file.read_block_into(position + 1, b);
}

void recycling_block_file::write_block(int position, const block &b) {
	ASSERTION(!availability.is_available(position));

	file.write_block(position + 1, b);
}

int recycling_block_file::append_block(const block &b) {
	int available_position = availability.first_available();

	if (available_position >= 0) {
		file.write_block(available_position + 1, b);
	} else {
		file.append_block(b);
		available_position = file.get_block_count() - 2;
	}

	availability.make_unavailable(available_position);
	write_availability_block();
	return available_position;
}

int recycling_block_file::reserve_block() {
	int available_position = availability.first_available();

	if (available_position < 0) {
		file.append_block(block(get_block_size()));
		available_position = file.get_block_count() - 2;
	}

	availability.make_unavailable(available_position);
	write_availability_block();
	return available_position;
}

void recycling_block_file::release_block(int position) {
	availability.make_available(position);
	write_availability_block();
}

int recycling_block_file::get_block_count() {
	int amount = 0;
	for (int i = 0; i < file.get_block_count() - 1; i++) {
		if (!availability.is_available(i)) amount++;
	}

	return amount;
}

int recycling_block_file::get_first_occupied() {
	return availability.first_occupied();
}

int recycling_block_file::get_next_occupied(int current) {
	for (current++; current <= file.get_block_count() - 2; current++) {
		if (!availability.is_available(current))
			return current;
	}

	return -1;
}

int recycling_block_file::get_block_size() const {
	return file.get_block_size();
}

recycling_block_file::~recycling_block_file() {

}
