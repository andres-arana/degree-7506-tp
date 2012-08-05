#include "../../dependencies/tut/tut.hpp"
#include "../../hash/bucket.h"

namespace {

struct test_data {
	typedef hash::bucket<int, std::string> container_type;
	typedef container_type::iterator bucket_iterator;
	typedef container_type::overflow_exception overflow;
	typedef container_type::duplicate_exception duplicate;
	typedef container_type::not_found_exception not_found;
	typedef std::pair<int, std::string> element_type;

	container_type bucket;
	int bucket_block_size;

	test_data() : bucket(commons::io::block(256)) {
		bucket.clear();
		bucket_block_size = 256;
	}
};

tut::test_group<test_data> test_group("hash::bucket class unit tests");

};

namespace tut {

template<>
template<>
void test_group<test_data>::object::test<1>() {
	set_test_name("Test bucket block initialization");

	ensure_equals(bucket.get_hash_factor(), 0);
	for (bucket_iterator it = bucket.begin(); it != bucket.end(); it++) {
		fail("Se encontró un elemento");
	}
}

template<>
template<>
void test_group<test_data>::object::test<2>() {
	set_test_name("Test reading / writing hash factor");

	bucket.set_hash_factor(2);

	ensure_equals(bucket.get_hash_factor(), 2);
}

template<>
template<>
void test_group<test_data>::object::test<3>() {
	set_test_name("Test add element to bucket with no overflow");

	bucket.add_element(100, "Primer elemento");
	bucket.add_element(200, "Segundo elemento");
	bucket.add_element(300, "Tercer elemento");

	int element_count = 0;
	for (bucket_iterator it = bucket.begin(); it != bucket.end(); it++) {
		element_count++;
		element_type e = *it;
		switch(e.first) {
		case 100:
			ensure_equals(e.second, "Primer elemento");
			break;
		case 200:
			ensure_equals(e.second, "Segundo elemento");
			break;
		case 300:
			ensure_equals(e.second, "Tercer elemento");
			break;
		default:
			fail("Read an unexpected key");
		}
	}
	ensure_equals(element_count, 3);
}

template<>
template<>
void test_group<test_data>::object::test<4>() {
	set_test_name("Test add element with overflow");

	std::string big_data(200, 'A');
	std::string other_big_data(200, 'A');
	try {
		bucket.add_element(100, big_data);
		bucket.add_element(200, other_big_data);
		fail("No overflow detected");
	} catch (overflow &o) {
		int element_count = 0;
		for (bucket_iterator it = bucket.begin(); it != bucket.end(); it++) {
			element_count++;
			element_type e = *it;
			switch (e.first) {
			case 100:
				ensure_equals(e.second, big_data);
				break;
			default:
				fail("Read an unexpected key");
			}
		}
		ensure_equals(element_count, 1);
	}
}

template<>
template<>
void test_group<test_data>::object::test<7>() {
	set_test_name("Test find valid object in set");

	bucket.add_element(100, "A");
	bucket.add_element(200, "AB");
	bucket.add_element(300, "ABC");
	bucket.add_element(400, "ABCD");
	bucket.add_element(500, "ABCDE");
	bucket.add_element(600, "ABCDEF");

	std::pair<bool, std::string> result = bucket.get_element(400);

	ensure(result.first);
	ensure_equals(result.second, "ABCD");
}

template<>
template<>
void test_group<test_data>::object::test<8>() {
	set_test_name("Test find invalid object in set");

	bucket.add_element(100, "A");
	bucket.add_element(200, "AB");
	bucket.add_element(300, "ABC");
	bucket.add_element(400, "ABCD");
	bucket.add_element(500, "ABCDE");
	bucket.add_element(600, "ABCDEF");

	std::pair<bool, std::string> result = bucket.get_element(350);

	ensure(!result.first);
}

template<>
template<>
void test_group<test_data>::object::test<9>() {
	set_test_name("Test find invalid object in empty set");

	std::pair<bool, std::string> result = bucket.get_element(350);

	ensure(!result.first);
}

template<>
template<>
void test_group<test_data>::object::test<12>() {
	set_test_name("Test inserting duplicate key");

	bucket.add_element(100, "A");
	bucket.add_element(200, "AB");
	bucket.add_element(300, "ABC");

	try {
		bucket.add_element(200, "ABCDE");
		fail("Insertion successful, duplicate key not found");
	} catch (duplicate &d) {

	}
}

template<>
template<>
void test_group<test_data>::object::test<13>() {
	set_test_name("Test updating valid element");

	bucket.add_element(100, "A");
	bucket.add_element(200, "B");
	bucket.add_element(300, "C");

	bucket.update_element(200, "ABC");

	int element_count = 0;
	for (bucket_iterator it = bucket.begin(); it != bucket.end(); it++) {
		element_count++;
		element_type e = *it;

		switch(e.first) {
		case 100:
			ensure_equals(e.second, "A");
			break;
		case 200:
			ensure_equals(e.second, "ABC");
			break;
		case 300:
			ensure_equals(e.second, "C");
			break;
		default:
			fail("Invalid element key");
		}
	}

	ensure_equals(element_count, 3);
}

template<>
template<>
void test_group<test_data>::object::test<14>() {
	set_test_name("Test updating valid element");

	bucket.add_element(100, "A");
	bucket.add_element(200, "B");
	bucket.add_element(300, "C");

	try {
		bucket.update_element(250, "ABC");
		fail("Failed to throw exception when updating invalid element");
	} catch (not_found &f) {

	}
}

template<>
template<>
void test_group<test_data>::object::test<15>() {
	set_test_name("Test updating an element which causes an overflow");

	bucket.add_element(100, "A");
	bucket.add_element(200, "B");
	bucket.add_element(300, "C");

	try {
		bucket.update_element(200, std::string(400, 'A'));
		fail("Failed to throw exception when updating a valid element which causes overflow");
	} catch (overflow &o) {

	}
}

template<>
template<>
void test_group<test_data>::object::test<16>() {
	set_test_name("Test removing a valid element");

	bucket.add_element(100, "AAA");
	bucket.add_element(200, "BBBBBB");
	bucket.add_element(300, "CCCCCCCCC");

	bucket.remove_element(200);

	int element_count = 0;
	for (bucket_iterator it = bucket.begin(); it != bucket.end(); it++) {
		element_count++;
		element_type e = *it;

		switch(e.first) {
		case 100:
			ensure_equals(e.second, "AAA");
			break;
		case 300:
			ensure_equals(e.second, "CCCCCCCCC");
			break;
		default:
			fail("Invalid element key");
		}
	}

	ensure_equals(element_count, 2);
}

template<>
template<>
void test_group<test_data>::object::test<17>() {
	set_test_name("Test removing an invalid element");

	bucket.add_element(100, "AAA");
	bucket.add_element(200, "BBB");
	bucket.add_element(300, "CCC");

	try {
		bucket.remove_element(250);
		fail("Failed to throw an exception after removing an invalid element");
	} catch (not_found &n) {

	}
}

template<>
template<>
void test_group<test_data>::object::test<18>() {
	set_test_name("Test empty element at startup");

	ensure(bucket.is_empty());
}

template<>
template<>
void test_group<test_data>::object::test<19>() {
	set_test_name("Test empty element after insertion and removal");

	bucket.add_element(100, "AAA");
	bucket.add_element(200, "AAAAAA");
	bucket.add_element(300, "AAAAAAAAA");

	bucket.remove_element(200);
	bucket.remove_element(300);
	bucket.remove_element(100);

	ensure(bucket.is_empty());
}

template<>
template<>
void test_group<test_data>::object::test<20>() {
	set_test_name("Test updating valid element causing compacting");

	bucket.add_element(100, "A");
	bucket.add_element(200, "BBBB");
	bucket.add_element(300, "C");

	bucket.update_element(200, "B");

	int element_count = 0;
	for (bucket_iterator it = bucket.begin(); it != bucket.end(); it++) {
		element_count++;
		element_type e = *it;

		switch(e.first) {
		case 100:
			ensure_equals(e.second, "A");
			break;
		case 200:
			ensure_equals(e.second, "B");
			break;
		case 300:
			ensure_equals(e.second, "C");
			break;
		default:
			fail("Invalid element key");
		}
	}

	ensure_equals(element_count, 3);
}

};
