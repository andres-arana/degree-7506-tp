#include "../../dependencies/tut/tut.hpp"
#include "../../hash/hash_container.h"
#include "../../commons/log/log.h"
#include <cstdio>
#include <vector>

namespace {

typedef std::string value_type;
typedef int key_type;
typedef std::pair<key_type, value_type> pair_type;
typedef hash::bucket<key_type, value_type> bucket_type;
typedef hash::hash_container<key_type, value_type> container_type;
typedef container_type::duplicate_exception duplicate;
typedef container_type::not_found_exception not_found;
typedef std::vector<pair_type> element_container;


struct test_data {
	container_type *container;
	int block_size;

	test_data() : container(new container_type("container_test.data", "container_test.index", 4096)) {
		block_size = 4096;
	}

	~test_data() {
		if (container != 0)
			delete container;
		std::remove("container_test.data");
		std::remove("container_test.index");
	}
};

tut::test_group<test_data> test_group("integration::hash tests");

};

namespace tut {

template<>
template<>
void test_group<test_data>::object::test<1>() {
	set_test_name("Test insertion very large amount of elements");

	// Inserto varios elementos
	for (int i = 0; i < 50000; i++)
		container->add_element(i, std::string(40, (i % 10) + 65));

	// Chequeo que los elementos esten
	for (int i = 0; i < 50000; i++) {
		std::pair<bool, value_type> results = container->search_for_element(i);
		ensure(results.first);
		ensure_equals(results.second, std::string(40, (i % 10) + 65));
	}

	// Chequeo algunos elementos que no esten
	for (int i = 50000; i < 51000; i+= 100) {
		std::pair<bool, value_type> results = container->search_for_element(i);
		ensure(!results.first);
	}

	// Elimino algunos elementos intermedios
	for (int i = 0; i < 50000; i+=1000) {
		container->delete_element(i);
	}

	// Chequeo que los elementos estén
	for (int i = 0; i < 50000; i++) {
		std::pair<bool, value_type> results = container->search_for_element(i);
		if ((i % 1000) == 0) {
			ensure(!results.first);
		} else {
			ensure(results.first);
			ensure_equals(results.second, std::string(40, (i % 10) + 65));
		}
	}
}







};

