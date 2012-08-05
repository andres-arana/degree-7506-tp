#include "../../dependencies/tut/tut.hpp"
#include "../../hash/bucket_table.h"
#include <cstdio>

namespace {

struct test_data {
	typedef int key_type;
	typedef std::string value_type;
	typedef hash::bucket<key_type, value_type> bucket_type;
	typedef hash::bucket_table<key_type, value_type> table_type;

	table_type *table;
	int block_size;

	test_data() : table(new table_type("bucket_table_test", 512)) {
		block_size = 512;
	}

	void close_and_reopen() {
		delete table;
		table = new table_type("bucket_table_test", 512);
	}

	~test_data() {
		delete table;
		std::remove("bucket_table_test");
	}
};

tut::test_group<test_data> test_group("hash::bucket_table class unit tests");

};

namespace tut {

template<>
template<>
void test_group<test_data>::object::test<1>() {
	set_test_name("Test appending bucket at an empty file");

	bucket_type b = bucket_type(commons::io::block(block_size));
	ensure_equals(table->append_bucket(b), 1);
}

template<>
template<>
void test_group<test_data>::object::test<2>() {
	set_test_name("Test appending bucket after closing an empty file");

	close_and_reopen();

	bucket_type b = bucket_type(commons::io::block(block_size));
	ensure_equals(table->append_bucket(b), 1);
}

template<>
template<>
void test_group<test_data>::object::test<3>() {
	set_test_name("Test writing and reading a bucket after closing a file");

	bucket_type b = table->get_bucket(0);

	b.add_element(100, "FIRST");
	b.add_element(200, "SECOND");
	b.add_element(300, "THIRD");

	table->save_bucket(0, b);

	close_and_reopen();

	bucket_type bucket = table->get_bucket(0);

	ensure_equals(bucket.get_hash_factor(), 1);

	int element_count = 0;
	for (bucket_type::iterator it = bucket.begin(); it != bucket.end(); it++) {
		element_count++;
		std::pair<key_type, value_type> e = *it;

		switch(e.first) {
		case 100:
			ensure_equals(e.second, "FIRST");
			break;
		case 200:
			ensure_equals(e.second, "SECOND");
			break;
		case 300:
			ensure_equals(e.second, "THIRD");
			break;
		default:
			fail("Invalid bucket element key");
		}
	}

	ensure_equals(element_count, 3);
}

template<>
template<>
void test_group<test_data>::object::test<4>() {
	set_test_name("Test writing and reading a bucket after closing a file using into syntax");

	bucket_type b = table->get_bucket(0);

	b.add_element(100, "FIRST");
	b.add_element(200, "SECOND");
	b.add_element(300, "THIRD");

	table->save_bucket(0, b);

	close_and_reopen();

	bucket_type bucket = bucket_type(commons::io::block(block_size));
	table->get_bucket_into(0, &bucket);

	ensure_equals(bucket.get_hash_factor(), 1);

	int element_count = 0;
	for (bucket_type::iterator it = bucket.begin(); it != bucket.end(); it++) {
		element_count++;
		std::pair<key_type, value_type> e = *it;

		switch(e.first) {
		case 100:
			ensure_equals(e.second, "FIRST");
			break;
		case 200:
			ensure_equals(e.second, "SECOND");
			break;
		case 300:
			ensure_equals(e.second, "THIRD");
			break;
		default:
			fail("Invalid bucket element key");
		}
	}

	ensure_equals(element_count, 3);
}

template<>
template<>
void test_group<test_data>::object::test<5>() {
	set_test_name("Test writing and reading does not mess up with the file");

	bucket_type b = bucket_type(commons::io::block(block_size));
	b.set_hash_factor(2);
	ensure_equals(table->append_bucket(b), 1);

	b = bucket_type(commons::io::block(block_size));
	b.set_hash_factor(3);
	ensure_equals(table->append_bucket(b), 2);

	close_and_reopen();

	bucket_type bucket = table->get_bucket(0);
	ensure_equals(bucket.get_hash_factor(), 1);

	bucket = table->get_bucket(1);
	ensure_equals(bucket.get_hash_factor(), 2);

	bucket = table->get_bucket(2);
	ensure_equals(bucket.get_hash_factor(), 3);
}

template<>
template<>
void test_group<test_data>::object::test<6>() {
	set_test_name("Test removing a bucket");

	bucket_type b = bucket_type(commons::io::block(block_size));
	b.set_hash_factor(2);
	table->append_bucket(b);

	b = bucket_type(commons::io::block(block_size));
	b.set_hash_factor(3);
	table->append_bucket(b);

	b = bucket_type(commons::io::block(block_size));
	b.set_hash_factor(4);
	table->append_bucket(b);

	table->remove_bucket(2);

	b = bucket_type(commons::io::block(block_size));
	ensure_equals(table->append_bucket(b), 2);

	ensure_equals(table->append_bucket(b), 4);
}

template<>
template<>
void test_group<test_data>::object::test<9>() {
	set_test_name("Test iterating over a bucket_table");

	bucket_type zero = table->get_bucket(0);
	zero.set_hash_factor(1);
	table->save_bucket(0, zero);

	bucket_type b = bucket_type(commons::io::block(block_size));
	b.set_hash_factor(2);
	table->append_bucket(b);

	b = bucket_type(commons::io::block(block_size));
	b.set_hash_factor(3);
	table->append_bucket(b);

	b = bucket_type(commons::io::block(block_size));
	b.set_hash_factor(4);
	table->append_bucket(b);

	table->remove_bucket(2);

	table_type::iterator it = table->begin();
	ensure(it != table->end());
	std::pair<bucket_type, int>  result = *it;
	ensure_equals(result.first.get_hash_factor(), 1);
	it++;
	ensure(it != table->end());
	result = *it;
	ensure_equals(result.first.get_hash_factor(), 2);
	it++;
	ensure(it != table->end());
	result = *it;
	ensure_equals(result.first.get_hash_factor(), 4);
	it++;
	ensure(it == table->end());
}

};

