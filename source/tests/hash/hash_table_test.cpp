#include "../../dependencies/tut/tut.hpp"
#include "../../hash/hash_table.h"

namespace {

struct test_data {
	typedef hash::hash_table<int> table_type;
	table_type *table;

	test_data() : table(new table_type("hash_table_test")) {

	}

	void close_and_reopen() {
		delete table;
		table = new table_type("hash_table_test");
	}

	~test_data() {
		delete table;
		std::remove("hash_table_test");
	}
};

tut::test_group<test_data> test_group("hash::hash_table class unit tests");

};

namespace tut {

template<>
template<>
void test_group<test_data>::object::test<1>() {
	set_test_name("Test hash table initialization");

	ensure_equals(table->get_size(), 1);
	for (int i = 0; i < 100; i++)
		ensure_equals(table->get_key_position(i), 0);
}

template<>
template<>
void test_group<test_data>::object::test<2>() {
	set_test_name("Test duplication and remapping after a bucket overflow with closure");

	close_and_reopen();

	table->grow();
	table->remap_last_used_entry(1, 1);

	ensure_equals(table->get_size(), 2);
	for (int i = 0; i < 100; i++) {
		ensure_equals(table->get_key_position(i), (i % 2) == 0 ? 1 : 0);
	}
}

template<>
template<>
void test_group<test_data>::object::test<3>() {
	set_test_name("Test data is persistent after closure");

	table->grow();
	table->remap_last_used_entry(1, 1);

	close_and_reopen();

	ensure_equals(table->get_size(), 2);
	for (int i = 0; i < 100; i++) {
		ensure_equals(table->get_key_position(i), (i % 2) == 0 ? 1 : 0);
	}
}

template<>
template<>
void test_group<test_data>::object::test<4>() {
	set_test_name("Test remapping complex scenario");

	table->grow();
	table->remap_last_used_entry(1, 1);

	ensure_equals(table->get_key_position(0), 1);
	ensure_equals(table->get_key_position(1), 0);

	table->get_key_position(1);
	table->grow();
	table->remap_last_used_entry(2, 2);

	ensure_equals(table->get_key_position(0), 1);
	ensure_equals(table->get_key_position(1), 2);
	ensure_equals(table->get_key_position(2), 1);
	ensure_equals(table->get_key_position(3), 0);

	table->get_key_position(3);
	table->grow();
	table->remap_last_used_entry(3, 4);

	ensure_equals(table->get_key_position(0), 1);
	ensure_equals(table->get_key_position(1), 2);
	ensure_equals(table->get_key_position(2), 1);
	ensure_equals(table->get_key_position(3), 3);
	ensure_equals(table->get_key_position(4), 1);
	ensure_equals(table->get_key_position(5), 2);
	ensure_equals(table->get_key_position(6), 1);
	ensure_equals(table->get_key_position(7), 0);

	table->get_key_position(0);
	table->remap_last_used_entry(4, 2);

	ensure_equals(table->get_key_position(0), 4);
	ensure_equals(table->get_key_position(1), 2);
	ensure_equals(table->get_key_position(2), 1);
	ensure_equals(table->get_key_position(3), 3);
	ensure_equals(table->get_key_position(4), 4);
	ensure_equals(table->get_key_position(5), 2);
	ensure_equals(table->get_key_position(6), 1);
	ensure_equals(table->get_key_position(7), 0);
}

template<>
template<>
void test_group<test_data>::object::test<5>() {
	set_test_name("Test shrinking empty table");

	table->try_position_erasure(1);

	ensure_equals(1, table->get_size());
	ensure_equals(table->get_key_position(0), 0);
}

template<>
template<>
void test_group<test_data>::object::test<6>() {
	set_test_name("Test shrink not successful");

	table->grow();
	table->remap_last_used_entry(1, 1);
	table->get_key_position(1);
	table->grow();
	table->remap_last_used_entry(2, 2);

	ensure_equals(table->get_key_position(0), 1);
	ensure_equals(table->get_key_position(1), 2);
	ensure_equals(table->get_key_position(2), 1);
	ensure_equals(table->get_key_position(3), 0);

	table->get_key_position(2);
	std::pair<bool, int> result = table->try_position_erasure(2);

	ensure(!result.first);
	ensure_equals(table->get_size(), 4);
}

template<>
template<>
void test_group<test_data>::object::test<7>() {
	set_test_name("Test erasure with shrinking");

	table->grow();
	table->remap_last_used_entry(1, 1);

	table->get_key_position(1);
	table->grow();
	table->remap_last_used_entry(2, 2);

	table->get_key_position(3);
	table->grow();
	table->remap_last_used_entry(3, 4);

	ensure_equals(table->get_key_position(0), 1);
	ensure_equals(table->get_key_position(1), 2);
	ensure_equals(table->get_key_position(2), 1);
	ensure_equals(table->get_key_position(3), 3);
	ensure_equals(table->get_key_position(4), 1);
	ensure_equals(table->get_key_position(5), 2);
	ensure_equals(table->get_key_position(6), 1);
	ensure_equals(table->get_key_position(7), 0);

	table->get_key_position(3);
	std::pair<bool, int> result = table->try_position_erasure(8);

	ensure(result.first);
	ensure_equals(result.second, 0);
	ensure_equals(table->get_size(), 4);
	ensure_equals(table->get_key_position(0), 1);
	ensure_equals(table->get_key_position(1), 2);
	ensure_equals(table->get_key_position(2), 1);
	ensure_equals(table->get_key_position(3), 0);
}

template<>
template<>
void test_group<test_data>::object::test<8>() {
	set_test_name("Test erasure without shrinking");

	table->grow();
	table->remap_last_used_entry(1, 1);

	table->get_key_position(1);
	table->grow();
	table->remap_last_used_entry(2, 2);

	table->get_key_position(3);
	table->grow();
	table->remap_last_used_entry(3, 4);

	table->get_key_position(1);
	table->remap_last_used_entry(4, 4);

	ensure_equals(table->get_key_position(0), 1);
	ensure_equals(table->get_key_position(1), 4);
	ensure_equals(table->get_key_position(2), 1);
	ensure_equals(table->get_key_position(3), 3);
	ensure_equals(table->get_key_position(4), 1);
	ensure_equals(table->get_key_position(5), 2);
	ensure_equals(table->get_key_position(6), 1);
	ensure_equals(table->get_key_position(7), 0);

	table->get_key_position(1);
	std::pair<bool, int> result = table->try_position_erasure(8);

	ensure(result.first);
	ensure_equals(result.second, 2);
	ensure_equals(table->get_size(), 8);

	ensure_equals(table->get_key_position(0), 1);
	ensure_equals(table->get_key_position(1), 2);
	ensure_equals(table->get_key_position(2), 1);
	ensure_equals(table->get_key_position(3), 3);
	ensure_equals(table->get_key_position(4), 1);
	ensure_equals(table->get_key_position(5), 2);
	ensure_equals(table->get_key_position(6), 1);
	ensure_equals(table->get_key_position(7), 0);
}

};
