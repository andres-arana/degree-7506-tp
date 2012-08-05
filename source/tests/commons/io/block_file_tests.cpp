#include "../../../dependencies/tut/tut.hpp"
#include "../../../commons/io/block_file.h"
#include <cstdio>

namespace {

struct free_block_initializer : public commons::io::block_file::initializer {
	virtual void initialize(commons::io::block_file *file) const {
		file->append_block(commons::io::block(512));
	}
};

struct test_data {
	commons::io::block_file file;

	test_data() : file("test_block_file", 512, free_block_initializer()) {

	}

	~test_data() {
		file.close();
		std::remove("test_block_file");
	}
};

tut::test_group<test_data> test_group("commons::io::block_file class unit tests");

};

namespace tut {

template<>
template<>
void test_group<test_data>::object::test<1>() {
	set_test_name("Test reading and writing blocks");

	commons::io::block b(file.get_block_size());
	for (int i = 0; i < b.get_size(); i++)
		b[i] = (i % 100) + 1;

	file.write_block(0, b);

	commons::io::block written_block = file.read_block(0);

	for (int i = 0; i < b.get_size(); i++) {
		ensure_equals(written_block[i], b[i]);
	}
}

template<>
template<>
void test_group<test_data>::object::test<2>() {
	set_test_name("Test inplace reading blocks");

	commons::io::block b(file.get_block_size());
	for (int i = 0; i < b.get_size(); i++)
		b[i] = (i % 100) + 1;

	file.write_block(0, b);

	commons::io::block written_block(file.get_block_size());
	file.read_block_into(0, &written_block);


	for (int i = 0; i < b.get_size(); i++) {
		ensure_equals(written_block[i], b[i]);
	}
}

template<>
template<>
void test_group<test_data>::object::test<3>() {
	set_test_name("Test appending blocks");

	commons::io::block b(file.get_block_size());
	for (int i = 0; i < b.get_size(); i++)
		b[i] = (i % 100) + 1;

	file.append_block(b);
	commons::io::block written_block = file.read_block(1);

	for (int i = 0; i < b.get_size(); i++) {
		ensure_equals(written_block[i], b[i]);
	}
}

template<>
template<>
void test_group<test_data>::object::test<4>() {
	set_test_name("Test getting block count");

	ensure_equals(file.get_block_count(), 1);

	file.append_block(commons::io::block(file.get_block_size()));

	ensure_equals(file.get_block_count(), 2);
}

template<>
template<>
void test_group<test_data>::object::test<5>() {
	set_test_name("Test opening existing file");

	commons::io::block b(file.get_block_size());
	for (int i = 0; i < b.get_size(); i++)
		b[i] = (i % 100) + 1;

	file.write_block(0, b);
	file.close();

	commons::io::block_file other_file("test_block_file", 512, free_block_initializer());
	ensure_equals(other_file.get_block_count(), 1);

	commons::io::block written_block = other_file.read_block(0);

	for (int i = 0; i < b.get_size(); i++)
		ensure_equals(written_block[i], b[i]);
}

};

