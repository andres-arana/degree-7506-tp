#include "../../dependencies/tut/tut.hpp"
#include "../../bplus/bplus_container.h"
#include "../../commons/log/log.h"
#include <fstream>
#include <cstdio>
#include <utility>

using namespace std;

namespace {

typedef std::string value_type;
typedef int key_type;
typedef bplus::bplus_container<key_type, value_type> container_type;
typedef bplus::subtree<key_type, value_type> subtree_type;
typedef bplus::inner_node<key_type, value_type> inner_node_type;
typedef bplus::leaf_node<key_type, value_type> leaf_node_type;
typedef std::vector<std::pair<int, int> > inner_contents_type;
typedef std::pair<key_type, value_type> pair_type;
typedef std::vector<pair_type> leaf_contents_type;
typedef std::pair<bool, value_type> search_results;

struct test_data {
	container_type *container;
	int block_size;

	test_data() : container(new container_type("./btree_container_test.data", 4096)) {
		block_size = 4096;
	}

	~test_data() {
		if (container != NULL)
			delete container;
		std::remove("./btree_container_test.data");
	}

};

tut::test_group<test_data> test_group("integration::bplus tests");

};

namespace tut {

template<>
template<>
void test_group<test_data>::object::test<1>() {
	set_test_name("Test insertion very large amount of elements");

	// Inserto varios elementos
	for (int i = 0; i < 50000; i++) {
		container->add_element(i, std::string(40, (i % 10) + 65));
	}


	// Chequeo que los elementos esten
	for (int i = 0; i < 50000; i++) {
		search_results results = container->search_for_element(i);
		ensure(results.first);
		ensure_equals(results.second, std::string(40, (i % 10) + 65));
	}

	// Chequeo algunos elementos que no esten
	for (int i = 50000; i < 51000; i+= 100) {
		search_results results = container->search_for_element(i);
		ensure(!results.first);
	}

	// Elimino algunos elementos intermedios
	for (int i = 0; i < 50000; i+=1000) {
		container->delete_element(i);
	}

	// Chequeo que los elementos estén
	for (int i = 0; i < 50000; i++) {
		search_results results = container->search_for_element(i);
		if ((i % 1000) == 0) {
			ensure(!results.first);
		} else {
			ensure(results.first);
			ensure_equals(results.second, std::string(40, (i % 10) + 65));
		}
	}
}

};
