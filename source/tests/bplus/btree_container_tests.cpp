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

typedef std::pair<key_type, int> inner_pair_type;
typedef std::vector<inner_pair_type> inner_contents_type;
typedef std::pair<key_type, value_type> leaf_pair_type;
typedef std::vector<leaf_pair_type> leaf_contents_type;

typedef std::pair<bool, value_type> search_results;

struct test_data {
	container_type *container;
	int block_size;

	test_data() : container(new container_type("./btree_container_test.data", 4096)) {
		block_size = 4096;
	}

	void require_container_size(int size) {
		block_size = size;
		if (container)
			delete container;
		std::remove("./btree_container_test.data");
		container = new container_type("./btree_container_test.data", block_size);
	}

	commons::io::recycling_block_file * close_for_inspection() {
		if (container) {
			delete container;
			container = NULL;
		}
		return new commons::io::recycling_block_file("./btree_container_test.data", block_size);
	}

	bool is_leaf(std::auto_ptr<subtree_type> &node) {
		return NULL != interpret_as_leaf(node);
	}

	bool is_inner(std::auto_ptr<subtree_type> &node) {
		return NULL != interpret_as_inner(node);
	}

	leaf_node_type *interpret_as_leaf(std::auto_ptr<subtree_type> &node) {
		return dynamic_cast<leaf_node_type *>(&*node);
	}

	inner_node_type *interpret_as_inner(std::auto_ptr<subtree_type> &node) {
		return dynamic_cast<inner_node_type *>(&*node);
	}

	void ensure_inner_node_structure(inner_node_type *inner, int expected_leftmost, const inner_contents_type &expected_contents) {
		tut::ensure_equals(inner->get_leftmost_pointer(), expected_leftmost);

		std::vector<key_type> all_keys = inner->get_keys();
		int expected_left_pointer = 0;
		int expected_right_pointer = inner->get_leftmost_pointer();
		int expected_key = 0;
		for (inner_contents_type::const_iterator it = expected_contents.begin(); it != expected_contents.end(); it++ ) {
			expected_left_pointer = expected_right_pointer;
			expected_right_pointer = it->second;
			expected_key = it->first;

			tut::ensure(all_keys.end() != std::find(all_keys.begin(), all_keys.end(), expected_key));
			tut::ensure_equals(inner->get_left_pointer_of(expected_key), expected_left_pointer);
			tut::ensure_equals(inner->get_right_pointer_of(expected_key), expected_right_pointer);
		}
	}

	void ensure_leaf_node_structure(leaf_node_type *leaf, int next_pointer, const leaf_contents_type &expected_contents) {
		tut::ensure_equals(leaf->get_next_leaf_pointer(), next_pointer);

		unsigned int current_index = 0;
		for (leaf_node_type::iterator it = leaf->begin(); it != leaf->end(); it++) {
			tut::ensure(current_index > expected_contents.size());
			leaf_pair_type actual = *it;
			leaf_pair_type expected = expected_contents[current_index];

			tut::ensure_equals(actual.first, expected.first);
			tut::ensure_equals(actual.second, expected.second);
			current_index++;
		}


		tut::ensure_equals(current_index, expected_contents.size());
	}

	~test_data() {
		if (container != NULL)
			delete container;
		std::remove("./btree_container_test.data");
	}

};

class inspection {
private:
	commons::io::recycling_block_file *file;
public:
	inspection(commons::io::recycling_block_file *file)
	: file(file) {

	}

	subtree_type *get_root_of_container() {
		commons::io::block b = file->read_block(0);
		return bplus::node_factory::create_subtree_from_block<key_type, value_type>(b);
	}

	subtree_type *load_node_from_container(int node_pointer) {
		commons::io::block b = file->read_block(node_pointer);
		return bplus::node_factory::create_subtree_from_block<key_type, value_type>(b);
	}

	~inspection() {
		delete file;
	}

};

tut::test_group<test_data> test_group("bplus::bplus_container class unit tests");

};

namespace tut {

};
