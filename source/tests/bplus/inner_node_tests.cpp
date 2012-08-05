#include "../../dependencies/tut/tut.hpp"
#include "../../bplus/inner_node.h"
#include "../../bplus/node_types.h"
#include "../../bplus/subtree.h"
#include <memory>

namespace {

struct test_data {
	typedef std::string value_type;
	typedef int key_type;

	typedef bplus::inner_node<key_type, value_type> node_type;

	typedef node_type::root_overflow_exception overflow;
	typedef node_type::duplicate_exception duplicate;
	typedef std::vector< std::pair<key_type, int> > element_container;

	typedef bplus::subtree<key_type, value_type>::split_result split_result;

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

	void ensure_node_has(const node_type &n, const element_container &container) {
		unsigned int index = 0;
		int expected_right_pointer = n.get_leftmost_pointer();
		std::vector<int>  v = n.get_keys();
		for (std::vector<int>::iterator it = v.begin(); it < v.end(); it++) {
			int key = *it;
			int left_pointer = n.get_left_pointer_of(key);
			int right_pointer = n.get_right_pointer_of(key);
			tut::ensure_equals(key, container[index].first);
			tut::ensure_equals(expected_right_pointer, left_pointer);
			expected_right_pointer = container[index].second;
			tut::ensure_equals(expected_right_pointer, right_pointer);
			index++;
		}
		tut::ensure_equals(index, container.size());
	}
};

tut::test_group<test_data> test_group("bplus::inner_node class unit tests");

};

namespace tut {

template<>
template<>
void test_group<test_data>::object::test<1>() {
	set_test_name("Test node initialization");

	ensure_equals(bplus::extract_node_type(node.get_block()), bplus::inner_node_type);
}

template<>
template<>
void test_group<test_data>::object::test<2>() {
	set_test_name("Test adding valid key-pointers");

	node.set_leftmost_pointer(0);
	node.insert_key_in_order(100, 1);
	node.insert_key_in_order(200, 2);
	node.insert_key_in_order(50, 3);

	ensure_equals(node.get_left_pointer_of(50), 0);
	ensure_equals(node.get_right_pointer_of(50), 3);
	ensure_equals(node.get_left_pointer_of(100), 3);
	ensure_equals(node.get_right_pointer_of(100), 1);
	ensure_equals(node.get_left_pointer_of(200), 1);
	ensure_equals(node.get_right_pointer_of(200), 2);
}

template<>
template<>
void test_group<test_data>::object::test<3>() {
	set_test_name("Test adding duplicate key-pointers");

	node.insert_key_in_order(50, 1);
	node.insert_key_in_order(100, 2);
	node.insert_key_in_order(150, 3);
	node.insert_key_in_order(250, 4);

	try {
		node.insert_key_in_order(150, 5);
		fail("Failed to detect duplicate key-pointer");
	} catch (duplicate &o) {

	}
}

template<>
template<>
void test_group<test_data>::object::test<4>() {
	set_test_name("Test adding key-pointer in overflow");

	node.set_leftmost_pointer(0);

	// Cantidad máxima de pares clave-valor que entran en el nodo
	int number_of_pairs =
			(block_size - 3 * sizeof(int)) / (sizeof(int) + sizeof(int));

	// Inserto al revés para probar la inserción ordenada
	for (int i = number_of_pairs + 1; i > 0; i--) {
		// Salto el 5 elemento, que es el que voy a meter
		// después con overflow
		if (i == 5) break;

		node.insert_key_in_order(i * 10, i);
	}

	// Ahora meto el 5 que saltie antes, causa overflow
	try {
		node.insert_key_in_order(50, 5);
	} catch (overflow &o) {
		std::auto_ptr<node_type> n((node_type *)o.get_overflowded_subtree());

		for (int i = 1; i <= number_of_pairs + 1; i++) {
			ensure_equals(n->get_left_pointer_of(10 * i), i - 1);
			ensure_equals(n->get_right_pointer_of(10 * i), i);
		}
	}
}

template<>
template<>
void test_group<test_data>::object::test<6>() {
	set_test_name("Test merging normal");

	left_node.set_leftmost_pointer(1);
	left_node.insert_key_in_order(100, 2);
	left_node.insert_key_in_order(200, 3);
	left_node.insert_key_in_order(300, 4);

	right_node.set_leftmost_pointer(5);
	right_node.insert_key_in_order(400, 6);
	right_node.insert_key_in_order(500, 7);
	right_node.insert_key_in_order(600, 8);

	node_type::merge_result r = left_node.try_merge(&left_node, &right_node, 350);
	node_type &n = *((node_type *)r.merged_node);

	ensure(r.success);
	ensure_equals(n.get_left_pointer_of(100), 1);
	ensure_equals(n.get_right_pointer_of(100), 2);
	ensure_equals(n.get_left_pointer_of(200), 2);
	ensure_equals(n.get_right_pointer_of(200), 3);
	ensure_equals(n.get_left_pointer_of(300), 3);
	ensure_equals(n.get_right_pointer_of(300), 4);
	ensure_equals(n.get_left_pointer_of(350), 4);
	ensure_equals(n.get_right_pointer_of(350), 5);
	ensure_equals(n.get_left_pointer_of(400), 5);
	ensure_equals(n.get_right_pointer_of(400), 6);
	ensure_equals(n.get_left_pointer_of(500), 6);
	ensure_equals(n.get_right_pointer_of(500), 7);
	ensure_equals(n.get_left_pointer_of(600), 7);

	delete r.merged_node;
}

template<>
template<>
void test_group<test_data>::object::test<7>() {
	set_test_name("Test merging overflow");

	// Cantidad máxima de pares clave-valor que entran en el nodo
	int number_of_pairs =
			(block_size - 3 * sizeof(int)) / (sizeof(int) + sizeof(int));

	left_node.set_leftmost_pointer(0);
	for (int i = 1; i < number_of_pairs; i++) {
		left_node.insert_key_in_order(i * 10, i);
	}

	right_node.set_leftmost_pointer(0);
	for (int i = 1; i < number_of_pairs; i++) {
		right_node.insert_key_in_order(i * 10, i);
	}

	node_type::merge_result r = left_node.try_merge(&left_node, &right_node, 350);
	ensure(!r.success);
}

template<>
template<>
void test_group<test_data>::object::test<8>() {
	set_test_name("Test split method (middle_key)");

	for (int i = 0; i < 4; i++){
		node.insert_key_in_order(i,0);
	}

	split_result result = node.split(0,0,node.get_block().get_size());
	ensure_equals(result.middle_key,1);
}

/**
 * El nodo original es así: 0 1 2 3
 * Se promueve el 1, y el nodo queda dividido así:
 *
 *            (5) 1 (15)
 *   (-1) 0 | 5          (15) 2 | 25 || 3 | 35
 */
template<>
template<>
void test_group<test_data>::object::test<9>(){
	set_test_name("Test split method (left and right pointers)");

	for (int i = 0; i < 4 ; i++){
		node.insert_key_in_order(i,i * 10 + 5);
	}

	split_result result = node.split(0,0,node.get_block().get_size());

	int left_element = node.get_left_pointer_of(1);
	ensure_equals(left_element, 5);

	int right_element = node.get_right_pointer_of(1);
	ensure_equals(right_element, 15);

	left_element = node.get_left_pointer_of(2);
	ensure_equals(left_element, 15);

	right_element = node.get_right_pointer_of(2);
	ensure_equals(right_element, 25);

	left_element = node.get_left_pointer_of(0);
	ensure_equals(left_element, -1);

	right_element = node.get_right_pointer_of(0);
	ensure_equals(right_element, 5);

	left_element = node.get_left_pointer_of(3);
	ensure_equals(left_element, 25);

	right_element = node.get_right_pointer_of(3);
	ensure_equals(right_element, 35);
}

template<>
template<>
void test_group<test_data>::object::test<10>(){
	set_test_name("Test split method (middle_key) with overflow");

	int node_size = node.get_block().get_size() - 3 * sizeof(int);
	int size_of_element = sizeof(int) + sizeof(int);
	int number_of_elements = node_size/size_of_element;

	// Con un bloque de 256, se pueden insertar 30 elementos sin tener overflow
	int i = 1;
	for (i = 1; i < number_of_elements + 1; i++){
		node.insert_key_in_order(i, i*10);
	}

	// Insertamos 31 para causar overflow
	i++;
	try{
		node.insert_key_in_order(i,0);
	} catch (overflow e){
		split_result result = e.get_overflowded_subtree()->split(0, 0 , node.get_block().get_size());
		int promoted_key = number_of_elements/ 2 + 1;
		ensure_equals(promoted_key, result.middle_key);

		// Verificamos que todos los punteros a derecha sean de la forma j*10
		for (int j = 1; j < number_of_elements + 1; j++){
			ensure_equals(node.get_right_pointer_of(j), j*10);
		}

		// Verificamos que el puntero a izquierda del elemento 0 sea -1
		ensure_equals(node.get_left_pointer_of(1), -1);

		// Verificamos que el puntero a izquierda del 15 sea 140
		ensure_equals(node.get_left_pointer_of(promoted_key), (promoted_key-1)*10);

		// Verificamos que el puntero a izquierda del 16, que es la clave que subió, sea 140
		ensure_equals(node.get_left_pointer_of(result.middle_key), (result.middle_key - 1) * 10);

	}
}

template<>
template<>
void test_group<test_data>::object::test<11>() {
	set_test_name("Test split method (middle_key) an odd quantity of elements");

	for (int i = 0; i < 5; i++){
		node.insert_key_in_order(i,0);
	}

	split_result result = node.split(0,0,node.get_block().get_size());
	ensure_equals(result.middle_key,2);
}


template<>
template<>
void test_group<test_data>::object::test<12>() {
	set_test_name("Test balance normal");

	//Seteo los maximos para cargar los nodos derecho e izquierdo
	int number_of_pairs =
				(block_size - 3 * sizeof(int)) / (sizeof(int) + sizeof(int));
	int max_l = number_of_pairs;
	int max_r = number_of_pairs - 10;

	//Cargo el nodo izquierdo
	left_node.set_leftmost_pointer(1);
	for(int i = 1; i < max_l; i++){
		left_node.insert_key_in_order(i*100, i+1);
	}

	//Cargo el nodo derecho
	int right_leftmost_pointer = 222;
	right_node.set_leftmost_pointer(right_leftmost_pointer);
	for(int i = 1; i < max_r; i++){
		right_node.insert_key_in_order((max_l-1+i)*100, (max_l-1+i)+1);
	}

	//Preparo los expected
	//Verifico que la inserccion de 350 se pueda realizar
	ensure(max_l > 4);
	//Cargo el vector contenedor de los esperados por izquierda
	std::vector< std::pair<int,int> > container_left;
	container_left.push_back(std::make_pair(100, 2));
	container_left.push_back(std::make_pair(200, 3));
	container_left.push_back(std::make_pair(300, 4));
	//Inserto la middle key con su puntero a derecha
	int middle_key = 350;
	container_left.push_back(std::make_pair(middle_key, right_leftmost_pointer));
	int expected_max = max_l + max_r;
	int expected_split = expected_max/2;
	for (int j = 4; j < (expected_split-1); j++) {
		container_left.push_back(std::make_pair(j*100, j+1));
	}
	//Cargo el vector de los esperados por derecha
	std::vector< std::pair<int,int> > container_right;
	for (int j = expected_split; j < (expected_max-1); j++) {
		container_right.push_back(std::make_pair(j*100, j+1));
	}


	//Trata de balancear
	node_type::balance_result r = left_node.try_balance(&left_node, &right_node, middle_key);
	node_type &n_leaf = *((node_type *)r.result.left_node);
	node_type &n_right = *((node_type *)r.result.right_node);

	ensure(r.success);
	ensure_equals(r.result.middle_key, (expected_split-1)*100);

	ensure_node_has(n_leaf,container_left);
	ensure_node_has(n_right,container_right);
}

};
