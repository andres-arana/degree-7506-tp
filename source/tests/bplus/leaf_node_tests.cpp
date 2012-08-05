#include "../../dependencies/tut/tut.hpp"
#include "../../bplus/leaf_node.h"
#include "../../bplus/node_types.h"
#include "../../bplus/subtree.h"
#include <memory>

namespace {

struct test_data {
	typedef std::string value_type;
	typedef int key_type;
	typedef bplus::leaf_node<key_type, value_type> node_type;

	typedef node_type::root_overflow_exception overflow;
	typedef node_type::iterator iterator;
	typedef node_type::duplicate_exception duplicate;
	typedef node_type::not_found_exception not_found;
	typedef node_type::merge_result merge_result;
	typedef node_type::balance_result balance_result;

	typedef std::pair<key_type, value_type> pair_type;
	typedef std::vector<pair_type> element_container;

	typedef bplus::subtree<key_type, value_type>::split_result split_result;
	typedef bplus::subtree<key_type, value_type> split_result_pointer;

	node_type node;
	node_type left_node;
	node_type right_node;
	int block_size;

	test_data() : node(commons::io::block(256)), left_node(commons::io::block(256)), right_node(commons::io::block(256)) {
		node.clear();
		left_node.clear();
		right_node.clear();
		block_size = 256;
	}

	void add_all_elements_in(const element_container &container, node_type *n) {
		for (element_container::const_iterator it = container.begin(); it != container.end(); it++) {
			n->add_element(it->first, it->second);
		}
	}

	void ensure_node_has(const node_type &n, const element_container &container) {
		unsigned int index = 0;
		for (iterator it = n.begin(); it != n.end(); it++) {
			pair_type p = *it;
			tut::ensure_equals(p.first, container[index].first);
			tut::ensure_equals(p.second, container[index].second);
			index++;
		}
		tut::ensure_equals(index, container.size());
	}
};

tut::test_group<test_data> test_group("bplus::leaf_node class unit tests");

};

namespace tut {

template<>
template<>
void test_group<test_data>::object::test<1>() {
	set_test_name("Test leaf node initialization");

	ensure_equals(node.get_next_leaf_pointer(), -1);
	for (iterator it = node.begin(); it != node.end(); it++) {
		fail("Se encontró un elemento");
	}

	ensure_equals(bplus::extract_node_type(node.get_block()), bplus::leaf_node_type);
}

template<>
template<>
void test_group<test_data>::object::test<2>() {
	set_test_name("Test reading / writing next leaf pointer");

	node.set_next_leaf_pointer(2);

	ensure_equals(node.get_next_leaf_pointer(), 2);
}

template<>
template<>
void test_group<test_data>::object::test<3>() {
	set_test_name("Test add element to node with no overflow");

	element_container input_elements;
	input_elements.push_back(pair_type(200, "Segundo elemento"));
	input_elements.push_back(pair_type(100, "Primer elemento"));
	input_elements.push_back(pair_type(300, "Tercer elemento"));

	element_container expected_elements;
	expected_elements.push_back(pair_type(100, "Primer elemento"));
	expected_elements.push_back(pair_type(200, "Segundo elemento"));
	expected_elements.push_back(pair_type(300, "Tercer elemento"));

	add_all_elements_in(input_elements, &node);
	ensure_node_has(node, expected_elements);
}

template<>
template<>
void test_group<test_data>::object::test<4>() {
	set_test_name("Test find valid object in set");

	node.add_element(100, "A");
	node.add_element(200, "AB");
	node.add_element(300, "ABC");
	node.add_element(400, "ABCD");
	node.add_element(500, "ABCDE");
	node.add_element(600, "ABCDEF");

	std::pair<bool, value_type> result = node.get_element(400);

	ensure(result.first);
	ensure_equals(result.second, std::string("ABCD"));
}

template<>
template<>
void test_group<test_data>::object::test<5>() {
	set_test_name("Test find invalid object in set");

	node.add_element(100, "A");
	node.add_element(200, "AB");
	node.add_element(300, "ABC");
	node.add_element(400, "ABCD");
	node.add_element(500, "ABCDE");
	node.add_element(600, "ABCDEF");

	std::pair<bool, value_type> result = node.get_element(350);

	ensure(!result.first);
}

template<>
template<>
void test_group<test_data>::object::test<6>() {
	set_test_name("Test find invalid object in empty set");

	std::pair<bool, value_type> result = node.get_element(350);

	ensure(!result.first);
}

template<>
template<>
void test_group<test_data>::object::test<9>() {
	set_test_name("Test inserting duplicate key");

	node.add_element(100, "A");
	node.add_element(200, "AB");
	node.add_element(300, "ABC");

	try {
		node.add_element(200, "ABCDE");
		fail("Insertion successful, duplicate key not found");
	} catch (duplicate &d) {

	}
}

template<>
template<>
void test_group<test_data>::object::test<10>() {
	set_test_name("Test updating valid element");

	element_container input;
	input.push_back(pair_type(100, "A"));
	input.push_back(pair_type(200, "B"));
	input.push_back(pair_type(300, "C"));

	element_container expected;
	expected.push_back(pair_type(100, "A"));
	expected.push_back(pair_type(200, "ABC"));
	expected.push_back(pair_type(300, "C"));

	add_all_elements_in(input, &node);
	node.update_element(200, "ABC");
	ensure_node_has(node, expected);
}

template<>
template<>
void test_group<test_data>::object::test<11>() {
	set_test_name("Test updating invalid element");

	node.add_element(100, "A");
	node.add_element(200, "B");
	node.add_element(300, "C");

	try {
		node.update_element(250, "ABC");
		fail("Failed to throw exception when updating invalid element");
	} catch (not_found &f) {

	}
}

template<>
template<>
void test_group<test_data>::object::test<12>() {
	set_test_name("Test removing a valid element");

	element_container input;
	input.push_back(pair_type(100, "AAA"));
	input.push_back(pair_type(200, "BBBBBB"));
	input.push_back(pair_type(300, "CCCCCCCCC"));

	element_container expected;
	expected.push_back(pair_type(100, "AAA"));
	expected.push_back(pair_type(300, "CCCCCCCCC"));

	add_all_elements_in(input, &node);
	node.remove_element(200);
	ensure_node_has(node, expected);
}

template<>
template<>
void test_group<test_data>::object::test<13>() {
	set_test_name("Test removing an invalid element");

	node.add_element(100, "AAA");
	node.add_element(200, "BBB");
	node.add_element(300, "CCC");

	try {
		node.remove_element(250);
		fail("Failed to throw an exception after removing an invalid element");
	} catch (not_found &n) {

	}
}

template<>
template<>
void test_group<test_data>::object::test<14>() {
	set_test_name("Test empty element at startup");

	ensure(node.is_empty());
}

template<>
template<>
void test_group<test_data>::object::test<15>() {
	set_test_name("Test empty element after insertion and removal");

	node.add_element(100, "AAA");
	node.add_element(200, "AAAAAA");
	node.add_element(300, "AAAAAAAAA");

	node.remove_element(200);
	node.remove_element(300);
	node.remove_element(100);

	ensure(node.is_empty());
}

template<>
template<>
void test_group<test_data>::object::test<16>() {
	set_test_name("Test updating valid element causing compacting");

	element_container input;
	input.push_back(pair_type(100, "A"));
	input.push_back(pair_type(200, "BBBB"));
	input.push_back(pair_type(300, "C"));

	element_container expected;
	expected.push_back(pair_type(100, "A"));
	expected.push_back(pair_type(200, "B"));
	expected.push_back(pair_type(300, "C"));

	add_all_elements_in(input, &node);
	node.update_element(200, "B");
	ensure_node_has(node, expected);
}

template<>
template<>
void test_group<test_data>::object::test<17>() {
	set_test_name("Test adding an element which causes an overflow");

	std::string a_data(50, 'A');
	std::string b_data(50, 'B');
	std::string c_data(50, 'B');
	std::string d_data(200, 'B');

	node.add_element(300, a_data);
	node.add_element(250, b_data);
	node.add_element(100, c_data);

	try {
		node.add_element(150, d_data);
		fail("No overflow detected");
	} catch (overflow &o) {
		element_container expected_elements;
		expected_elements.push_back(pair_type(100, c_data));
		expected_elements.push_back(pair_type(150, d_data));
		expected_elements.push_back(pair_type(250, b_data));
		expected_elements.push_back(pair_type(300, a_data));

		std::auto_ptr<node_type> n((node_type *)o.get_overflowded_subtree());
		ensure_node_has(*n, expected_elements);
	}
}

template<>
template<>
void test_group<test_data>::object::test<19>() {
	set_test_name("Test merging normal");

	left_node.add_element(200, "BBB");
	left_node.add_element(100, "AAA");
	left_node.add_element(300, "CCC");

	right_node.add_element(600, "FFF");
	right_node.add_element(400, "DDD");
	right_node.add_element(500, "EEE");

	element_container expected;
	expected.push_back(pair_type(100, "AAA"));
	expected.push_back(pair_type(200, "BBB"));
	expected.push_back(pair_type(300, "CCC"));
	expected.push_back(pair_type(400, "DDD"));
	expected.push_back(pair_type(500, "EEE"));
	expected.push_back(pair_type(600, "FFF"));

	merge_result r = left_node.try_merge(&left_node, &right_node, 350);

	ensure(r.success);
	ensure_node_has(*((node_type *)r.merged_node), expected);

	delete r.merged_node;
}

template<>
template<>
void test_group<test_data>::object::test<20>() {
	set_test_name("Test merging overflow");

	left_node.add_element(200, std::string(100, 'B'));
	left_node.add_element(100, std::string(100, 'A'));

	right_node.add_element(600, std::string(100, 'F'));
	right_node.add_element(400, std::string(100, 'D'));

	merge_result r = left_node.try_merge(&left_node, &right_node, 350);

	ensure(!r.success);
}

template<>
template<>
void test_group<test_data>::object::test<21>() {
	set_test_name("Test balance normal");
	int tam = 18;
	left_node.add_element(200, std::string(tam,'A'));
	left_node.add_element(100, std::string(tam,'B'));
	left_node.add_element(300, std::string(tam,'C'));
	left_node.add_element(800, std::string(tam,'D'));
	left_node.add_element(900, std::string(tam,'E'));
	left_node.add_element(1000, std::string(tam,'F'));
	left_node.add_element(1400, std::string(tam,'G'));

	right_node.add_element(600, std::string(tam,'H'));
	right_node.add_element(400, std::string(tam,'I'));
	right_node.add_element(500, std::string(tam,'J'));
	right_node.add_element(2400, std::string(tam,'K'));
//	right_node.add_element(2500, std::string(tam,'L'));
//	right_node.add_element(2600, std::string(tam,'M'));
//	right_node.add_element(2800, std::string(tam,'O'));
//	right_node.add_element(3000, std::string(tam,'P'));

	element_container expected_left;
	element_container expected_right;
	expected_left.push_back(pair_type(100, std::string(tam,'B')));
	expected_left.push_back(pair_type(200, std::string(tam,'A')));
	expected_left.push_back(pair_type(300, std::string(tam,'C')));
	expected_left.push_back(pair_type(400, std::string(tam,'I')));
	expected_left.push_back(pair_type(500, std::string(tam,'J')));
	expected_left.push_back(pair_type(600, std::string(tam,'H')));

	expected_right.push_back(pair_type(800, std::string(tam,'D')));
	expected_right.push_back(pair_type(900, std::string(tam,'E')));
	expected_right.push_back(pair_type(1000, std::string(tam,'F')));
	expected_right.push_back(pair_type(1400, std::string(tam,'G')));
	expected_right.push_back(pair_type(2400, std::string(tam,'K')));

	balance_result r = left_node.try_balance(&left_node, &right_node, 350);

	ensure(r.success);
	ensure_node_has(*((node_type *)r.result.left_node), expected_left);
	ensure_node_has(*((node_type *)r.result.right_node), expected_right);

//	delete r.merged_node;
}

/**
 * El nodo original es así: 0 1 2 3
 * Se promueve el 2, (siempre se promueve al primero de la derecha)
 * entonces se copia arriba y el nodo queda dividido así:
 *              2
 *     0 | 1         2 | 3
 */
template<>
template<>
void test_group<test_data>::object::test<22>() {
	set_test_name("Test split method (middle_key)");

	for (int i = 0; i < 4; i++){
		node.add_element(i, "0");
	}

	split_result result = node.split(0,0,node.get_block().get_size());

	ensure_equals(result.middle_key, 2);
}

/**
 * El nodo original es así: 0 1 2 3 4
 * Se promueve el 2, (siempre se promueve al primero de la derecha)
 * entonces se copia arriba y el nodo queda dividido así:
 *            2
 *     0 | 1     2 | 3 | 4
 */
template<>
template<>
void test_group<test_data>::object::test<23>() {
	set_test_name("Test split method (middle_key) with an odd quantity of elements");

	for (int i = 0; i < 5; i++){
		node.add_element(i, "0");
	}

	split_result result = node.split(0, 0, node.get_block().get_size());

	// Verifico que se promueva el 2
	ensure_equals(result.middle_key, 2);
}

/**
 * El nodo original es así: 0 1 2 3 4 5
 * Se promueve el 3, (siempre se promueve al primero de la derecha)
 * entonces se copia arriba y el nodo queda dividido así:
 *                 3
 *     0 | 1 | 2        3 | 4 | 5
 */
template<>
template<>
void test_group<test_data>::object::test<24>() {
	set_test_name("Test split method (middle_key) with an odd quantity of elements");

	for (int i = 0; i < 6; i++){
		node.add_element(i, "0");
	}

	split_result result = node.split(0,0,node.get_block().get_size());

	// Verifico que se promueva el 3
	ensure_equals(result.middle_key, 3);
}

};
