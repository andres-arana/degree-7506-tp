#include "../../dependencies/tut/tut.hpp"
#include "../../hash/hash_container.h"
#include "../../commons/log/log.h"
#include <cstdio>
#include <vector>
#include <sstream>

namespace {

typedef std::string value_type;
typedef std::string key_type;
typedef std::pair<key_type, value_type> pair_type;
typedef hash::bucket<key_type, value_type> bucket_type;
typedef hash::hash_container<key_type, value_type> container_type;
typedef container_type::duplicate_exception duplicate;
typedef container_type::not_found_exception not_found;
typedef std::vector<pair_type> element_container;

struct test_data {
	container_type *container;
	int block_size;

	test_data() : container(new container_type("container_test.data", "container_test.index", 512)) {
		block_size = 512;
	}

	void require_container_size(int size) {
		block_size = size;
		if (container != 0)
			delete container;
		std::remove("container_test.data");
		std::remove("container_test.index");
		container = new container_type("container_test.data", "container_test.index", block_size);
	}

	void reopen() {
		if (container != 0)
			delete container;
		container = new container_type("container_test.data", "container_test.index", block_size);
	}

	void add_all_elements(const element_container &elements) {
		typedef element_container::const_iterator iterator;
		for (iterator it = elements.begin(); it != elements.end(); it++) {
			container->add_element(it->first, it->second);
		}
	}

	void ensure_bucket_contents(const bucket_type &b, const element_container &elements) {
		typedef element_container::const_iterator iterator;
		size_t element_count = 0;
		for (bucket_type::iterator it = b.begin(); it != b.end(); it++) {
			element_count++;
			std::pair<key_type, value_type> e = *it;
			bool found_key = false;
			for (iterator itv = elements.begin(); itv != elements.end(); itv++) {
				if (itv->first == e.first) {
					found_key = true;
					tut::ensure_equals(e.second, itv->second);
					break;
				}
			}

			if (!found_key)
				tut::fail("Invalid element key");
		}

		tut::ensure_equals(element_count, elements.size());
	}

	std::pair<hash::bucket_table<key_type, value_type> *, hash::hash_table<key_type> *> close_for_inspection() {
		if (container != 0) {
			delete container;
			container = 0;
		}

		return std::make_pair(
				new hash::bucket_table<key_type, value_type>("container_test.data", block_size),
				new hash::hash_table<key_type>("container_test.index"));
	}

	~test_data() {
		if (container != 0)
			delete container;
		std::remove("container_test.data");
		std::remove("container_test.index");
	}
};

struct inspection {
	hash::bucket_table<key_type, value_type> *buckets;
	hash::hash_table<key_type> *table;

	inspection(std::pair<hash::bucket_table<key_type, value_type> *, hash::hash_table<key_type> *> pair)
	: buckets(pair.first),
	  table(pair.second) {

	}

	~inspection() {
		delete buckets;
		delete table;
	}
};

tut::test_group<test_data> test_group("hash::hash_container_variable_key class unit tests");

};

namespace tut {

template<>
template<>
void test_group<test_data>::object::test<1>() {
	set_test_name("Test adding elements");

	element_container expected_bucket_contents;
	expected_bucket_contents.push_back(std::make_pair("0", "ABC"));
	expected_bucket_contents.push_back(std::make_pair("1", "DEF"));
	expected_bucket_contents.push_back(std::make_pair("2", "GHI"));

	add_all_elements(expected_bucket_contents);

	inspection i(close_for_inspection());
	ensure_equals(i.table->get_size(), 1);
	ensure_equals(i.table->get_key_position("0"), 0);
	ensure_equals(i.table->get_key_position("1"), 0);
	ensure_equals(i.table->get_key_position("2"), 0);

	bucket_type b = i.buckets->get_bucket(0);
	ensure_equals(b.get_hash_factor(), 1);
	ensure_bucket_contents(b, expected_bucket_contents);
}

template<>
template<>
void test_group<test_data>::object::test<2>() {
	set_test_name("Test adding elements with overflow");

	element_container expected_first_bucket_contents;
	expected_first_bucket_contents.push_back(std::make_pair("0", std::string(200, 'A')));
	expected_first_bucket_contents.push_back(std::make_pair("2", std::string(200, 'C')));
	add_all_elements(expected_first_bucket_contents);

	element_container expected_second_bucket_contents;
	expected_second_bucket_contents.push_back(std::make_pair("1", std::string(200, 'B')));
	add_all_elements(expected_second_bucket_contents);

	inspection i(close_for_inspection());
	ensure_equals(i.table->get_size(), 2);
	ensure_equals(i.table->get_key_position("0"), 1);
	ensure_equals(i.table->get_key_position("1"), 0);

	bucket_type b = i.buckets->get_bucket(0);
	ensure_equals(b.get_hash_factor(), 2);
	ensure_bucket_contents(b, expected_second_bucket_contents);

	b = i.buckets->get_bucket(1);
	ensure_equals(b.get_hash_factor(), 2);
	ensure_bucket_contents(b, expected_first_bucket_contents);
}

template<>
template<>
void test_group<test_data>::object::test<3>() {
	set_test_name("Test adding duplicate elements");

	container->add_element("0", std::string(300, 'A'));
	container->add_element("1", std::string(300, 'A'));
	container->add_element("2", std::string(300, 'A'));
	container->add_element("3", std::string(300, 'A'));
	container->add_element("4", std::string(300, 'A'));
	container->add_element("5", std::string(300, 'A'));
	container->add_element("6", std::string(300, 'A'));
	container->add_element("7", std::string(300, 'A'));
	container->add_element("8", std::string(300, 'A'));

	try {
		container->add_element("6", "B");
		fail("Duplicate exception not caught");
	} catch (duplicate &n) {

	}
}

template<>
template<>
void test_group<test_data>::object::test<5>() {
	set_test_name("Test updating without overflow");

	container->add_element("0", "A");
	container->add_element("1", "B");
	container->add_element("2", "C");
	container->update_element("1", "BCDE");

	inspection i(close_for_inspection());
	ensure_equals(i.table->get_size(), 1);
	ensure_equals(i.table->get_key_position("0"), 0);

	element_container first_expected_bucket_contents;
	first_expected_bucket_contents.push_back(std::make_pair("0", "A"));
	first_expected_bucket_contents.push_back(std::make_pair("1", "BCDE"));
	first_expected_bucket_contents.push_back(std::make_pair("2", "C"));

	bucket_type b = i.buckets->get_bucket(0);
	ensure_equals(b.get_hash_factor(), 1);
	ensure_bucket_contents(b, first_expected_bucket_contents);
}

template<>
template<>
void test_group<test_data>::object::test<6>() {
	set_test_name("Test updating with overflow");

	container->add_element("0", std::string(100, 'A'));
	container->add_element("1", std::string(100, 'B'));
	container->add_element("2", std::string(100, 'C'));
	container->add_element("3", std::string(100, 'D'));
	container->add_element("4", std::string(100, 'E'));
	container->add_element("5", std::string(100, 'F'));
	container->add_element("6", std::string(100, 'G'));
	container->update_element("2", std::string(200, 'C'));

	inspection i(close_for_inspection());
	ensure_equals(i.table->get_size(), 4);
	ensure_equals(i.table->get_key_position("0"), 2);
	ensure_equals(i.table->get_key_position("1"), 0);
	ensure_equals(i.table->get_key_position("2"), 1);
	ensure_equals(i.table->get_key_position("3"), 0);
	ensure_equals(i.table->get_key_position("4"), 2);
	ensure_equals(i.table->get_key_position("5"), 0);
	ensure_equals(i.table->get_key_position("6"), 1);


	element_container first_expected_bucket_contents;
	first_expected_bucket_contents.push_back(std::make_pair("1", std::string(100, 'B')));
	first_expected_bucket_contents.push_back(std::make_pair("3", std::string(100, 'D')));
	first_expected_bucket_contents.push_back(std::make_pair("5", std::string(100, 'F')));

	bucket_type b = i.buckets->get_bucket(0);
	ensure_equals(b.get_hash_factor(), 2);
	ensure_bucket_contents(b, first_expected_bucket_contents);

	element_container second_expected_bucket_contents;
	second_expected_bucket_contents.push_back(std::make_pair("2", std::string(200, 'C')));
	second_expected_bucket_contents.push_back(std::make_pair("6", std::string(100, 'G')));

	b = i.buckets->get_bucket(1);
	ensure_equals(b.get_hash_factor(), 4);
	ensure_bucket_contents(b, second_expected_bucket_contents);

	element_container third_expected_bucket_contents;
	third_expected_bucket_contents.push_back(std::make_pair("0", std::string(100, 'A')));
	third_expected_bucket_contents.push_back(std::make_pair("4", std::string(100, 'E')));

	b = i.buckets->get_bucket(2);
	ensure_equals(b.get_hash_factor(), 4);
	ensure_bucket_contents(b, third_expected_bucket_contents);
}

template<>
template<>
void test_group<test_data>::object::test<7>() {
	set_test_name("Test updating with not found exception");

	container->add_element("0", "A");
	container->add_element("2", "A");
	container->add_element("4", "A");
	container->add_element("6", "A");
	container->add_element("8", "A");

	try {
		container->update_element("5", "B");
		fail("Not found exception not detected");
	} catch (not_found &n) {

	}
}

};

