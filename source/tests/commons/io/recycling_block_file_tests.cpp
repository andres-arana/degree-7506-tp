#include "../../../dependencies/tut/tut.hpp"
#include "../../../commons/io/recycling_block_file.h"

#include <cstdio>

namespace {

struct free_block_initializer : public commons::io::recycling_block_file::initializer {
	virtual void initialize(commons::io::recycling_block_file *file) const {
		file->append_block(commons::io::block(512));
	}
};

struct test_data {
	commons::io::recycling_block_file file;

	test_data() : file("recycling_test_block_file", 512, free_block_initializer()) {

	}

	commons::io::block create_test_block(int seed) {
		commons::io::block result(file.get_block_size());

		for (int i = 0; i < result.get_size(); i++)
			result[i] = (i % 100) + 1 + seed;

		return result;
	}

	void ensure_blocks_are_equals(const commons::io::block &actual, const commons::io::block &expected) {
		tut::ensure_equals(actual.get_size(), expected.get_size());
		for (int i = 0; i < actual.get_size(); i++) {
			tut::ensure_equals(actual[i], expected[i]);
		}
	}

	~test_data() {
		file.close();
		std::remove("recycling_test_block_file");
	}
};

tut::test_group<test_data> test_group("commons::io::recycling_block_file class unit tests");

};

namespace tut {

template<>
template<>
void test_group<test_data>::object::test<1>() {
	set_test_name("Test reading and writing blocks");

	file.write_block(0, create_test_block(0));
	commons::io::block written_block = file.read_block(0);

	ensure_blocks_are_equals(written_block, create_test_block(0));
	ensure_equals(file.get_block_count(), 1);

}

template<>
template<>
void test_group<test_data>::object::test<2>() {
	set_test_name("Test reading blocks with into syntax");

	file.write_block(0, create_test_block(0));

	commons::io::block written_block(file.get_block_size());
	file.read_block_into(0, &written_block);

	ensure_blocks_are_equals(written_block, create_test_block(0));
	ensure_equals(file.get_block_count(), 1);
}

template<>
template<>
void test_group<test_data>::object::test<3>() {
	set_test_name("Test append-release recycling");

	ensure_equals(file.get_block_count(), 1);

	file.release_block(0);

	ensure_equals(file.get_block_count(), 0);

	ensure_equals(file.append_block(create_test_block(0)), 0);
	ensure_equals(file.get_block_count(), 1);

	ensure_equals(file.append_block(create_test_block(100)), 1);
	ensure_equals(file.get_block_count(), 2);

	file.close();

	commons::io::recycling_block_file check_file("recycling_test_block_file", 512, free_block_initializer());
	ensure_equals(check_file.get_block_count(), 2);
	ensure_blocks_are_equals(check_file.read_block(0), create_test_block(0));
	ensure_blocks_are_equals(check_file.read_block(1), create_test_block(100));
}

};
