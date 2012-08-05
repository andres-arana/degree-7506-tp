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

tut::test_group<test_data> test_group("hash::hash_container class unit tests");

};

namespace tut {

template<>
template<>
void test_group<test_data>::object::test<1>() {
	set_test_name("Test adding elements");

	element_container expected_bucket_contents;
	expected_bucket_contents.push_back(std::make_pair(0, "ABC"));
	expected_bucket_contents.push_back(std::make_pair(1, "DEF"));
	expected_bucket_contents.push_back(std::make_pair(2, "GHI"));

	add_all_elements(expected_bucket_contents);

	inspection i(close_for_inspection());
	ensure_equals(i.table->get_size(), 1);
	ensure_equals(i.table->get_key_position(0), 0);
	ensure_equals(i.table->get_key_position(1), 0);
	ensure_equals(i.table->get_key_position(2), 0);

	bucket_type b = i.buckets->get_bucket(0);
	ensure_equals(b.get_hash_factor(), 1);
	ensure_bucket_contents(b, expected_bucket_contents);
}

template<>
template<>
void test_group<test_data>::object::test<2>() {
	set_test_name("Test adding elements with overflow");

	element_container expected_first_bucket_contents;
	expected_first_bucket_contents.push_back(std::make_pair(0, std::string(200, 'A')));
	expected_first_bucket_contents.push_back(std::make_pair(2, std::string(200, 'C')));
	add_all_elements(expected_first_bucket_contents);

	element_container expected_second_bucket_contents;
	expected_second_bucket_contents.push_back(std::make_pair(1, std::string(200, 'B')));
	add_all_elements(expected_second_bucket_contents);

	inspection i(close_for_inspection());
	ensure_equals(i.table->get_size(), 2);
	ensure_equals(i.table->get_key_position(0), 1);
	ensure_equals(i.table->get_key_position(1), 0);

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

	container->add_element(0, std::string(300, 'A'));
	container->add_element(1, std::string(300, 'A'));
	container->add_element(2, std::string(300, 'A'));
	container->add_element(3, std::string(300, 'A'));
	container->add_element(4, std::string(300, 'A'));
	container->add_element(5, std::string(300, 'A'));
	container->add_element(6, std::string(300, 'A'));
	container->add_element(7, std::string(300, 'A'));
	container->add_element(8, std::string(300, 'A'));

	try {
		container->add_element(6, "B");
		fail("Duplicate exception not caught");
	} catch (duplicate &n) {

	}
}

template<>
template<>
void test_group<test_data>::object::test<4>() {
	set_test_name("Test adding complex elements with multiple overflows");

	std::string sample_data(100, 'A');

	for (int i = 0; i <= 16; i++)
		container->add_element(i, sample_data);

	inspection i(close_for_inspection());
	ensure_equals(i.table->get_size(), 8);
	ensure_equals(i.table->get_key_position(0), 4);
	ensure_equals(i.table->get_key_position(1), 3);
	ensure_equals(i.table->get_key_position(2), 1);
	ensure_equals(i.table->get_key_position(3), 0);
	ensure_equals(i.table->get_key_position(4), 2);
	ensure_equals(i.table->get_key_position(5), 3);
	ensure_equals(i.table->get_key_position(6), 1);
	ensure_equals(i.table->get_key_position(7), 0);

	element_container first_bucket_expected_contents;
	first_bucket_expected_contents.push_back(std::make_pair(3, sample_data));
	first_bucket_expected_contents.push_back(std::make_pair(7, sample_data));
	first_bucket_expected_contents.push_back(std::make_pair(11, sample_data));
	first_bucket_expected_contents.push_back(std::make_pair(15, sample_data));

	bucket_type b = i.buckets->get_bucket(0);
	ensure_equals(b.get_hash_factor(), 4);
	ensure_bucket_contents(b, first_bucket_expected_contents);

	element_container second_bucket_expected_contents;
	second_bucket_expected_contents.push_back(std::make_pair(2, sample_data));
	second_bucket_expected_contents.push_back(std::make_pair(6, sample_data));
	second_bucket_expected_contents.push_back(std::make_pair(10, sample_data));
	second_bucket_expected_contents.push_back(std::make_pair(14, sample_data));

	b = i.buckets->get_bucket(1);
	ensure_equals(b.get_hash_factor(), 4);
	ensure_bucket_contents(b, second_bucket_expected_contents);

	element_container third_bucket_expected_contents;
	third_bucket_expected_contents.push_back(std::make_pair(4, sample_data));
	third_bucket_expected_contents.push_back(std::make_pair(12, sample_data));

	b = i.buckets->get_bucket(2);
	ensure_equals(b.get_hash_factor(), 8);
	ensure_bucket_contents(b, third_bucket_expected_contents);

	element_container fourth_bucket_expected_contents;
	fourth_bucket_expected_contents.push_back(std::make_pair(1, sample_data));
	fourth_bucket_expected_contents.push_back(std::make_pair(5, sample_data));
	fourth_bucket_expected_contents.push_back(std::make_pair(9, sample_data));
	fourth_bucket_expected_contents.push_back(std::make_pair(13, sample_data));

	b = i.buckets->get_bucket(3);
	ensure_equals(b.get_hash_factor(), 4);
	ensure_bucket_contents(b, fourth_bucket_expected_contents);

	element_container fifth_bucket_expected_contents;
	fifth_bucket_expected_contents.push_back(std::make_pair(0, sample_data));
	fifth_bucket_expected_contents.push_back(std::make_pair(8, sample_data));
	fifth_bucket_expected_contents.push_back(std::make_pair(16, sample_data));

	b = i.buckets->get_bucket(4);
	ensure_equals(b.get_hash_factor(), 8);
	ensure_bucket_contents(b, fifth_bucket_expected_contents);
}

template<>
template<>
void test_group<test_data>::object::test<5>() {
	set_test_name("Test updating without overflow");

	container->add_element(0, "A");
	container->add_element(1, "B");
	container->add_element(2, "C");
	container->update_element(1, "BCDE");

	inspection i(close_for_inspection());
	ensure_equals(i.table->get_size(), 1);
	ensure_equals(i.table->get_key_position(0), 0);

	element_container first_expected_bucket_contents;
	first_expected_bucket_contents.push_back(std::make_pair(0, "A"));
	first_expected_bucket_contents.push_back(std::make_pair(1, "BCDE"));
	first_expected_bucket_contents.push_back(std::make_pair(2, "C"));

	bucket_type b = i.buckets->get_bucket(0);
	ensure_equals(b.get_hash_factor(), 1);
	ensure_bucket_contents(b, first_expected_bucket_contents);
}

template<>
template<>
void test_group<test_data>::object::test<6>() {
	set_test_name("Test updating with overflow");

	container->add_element(0, std::string(100, 'A'));
	container->add_element(1, std::string(100, 'B'));
	container->add_element(2, std::string(100, 'C'));
	container->add_element(3, std::string(100, 'D'));
	container->add_element(4, std::string(100, 'E'));
	container->add_element(5, std::string(100, 'F'));
	container->add_element(6, std::string(100, 'G'));
	container->update_element(2, std::string(200, 'C'));

	inspection i(close_for_inspection());
	ensure_equals(i.table->get_size(), 4);
	ensure_equals(i.table->get_key_position(0), 2);
	ensure_equals(i.table->get_key_position(1), 0);
	ensure_equals(i.table->get_key_position(2), 1);
	ensure_equals(i.table->get_key_position(3), 0);

	element_container first_expected_bucket_contents;
	first_expected_bucket_contents.push_back(std::make_pair(1, std::string(100, 'B')));
	first_expected_bucket_contents.push_back(std::make_pair(3, std::string(100, 'D')));
	first_expected_bucket_contents.push_back(std::make_pair(5, std::string(100, 'F')));

	bucket_type b = i.buckets->get_bucket(0);
	ensure_equals(b.get_hash_factor(), 2);
	ensure_bucket_contents(b, first_expected_bucket_contents);

	element_container second_expected_bucket_contents;
	second_expected_bucket_contents.push_back(std::make_pair(2, std::string(200, 'C')));
	second_expected_bucket_contents.push_back(std::make_pair(6, std::string(100, 'G')));

	b = i.buckets->get_bucket(1);
	ensure_equals(b.get_hash_factor(), 4);
	ensure_bucket_contents(b, second_expected_bucket_contents);

	element_container third_expected_bucket_contents;
	third_expected_bucket_contents.push_back(std::make_pair(0, std::string(100, 'A')));
	third_expected_bucket_contents.push_back(std::make_pair(4, std::string(100, 'E')));

	b = i.buckets->get_bucket(2);
	ensure_equals(b.get_hash_factor(), 4);
	ensure_bucket_contents(b, third_expected_bucket_contents);
}

template<>
template<>
void test_group<test_data>::object::test<7>() {
	set_test_name("Test updating with not found exception");

	container->add_element(0, "A");
	container->add_element(2, "A");
	container->add_element(4, "A");
	container->add_element(6, "A");
	container->add_element(8, "A");

	try {
		container->update_element(5, "B");
		fail("Not found exception not detected");
	} catch (not_found &n) {

	}
}

template<>
template<>
void test_group<test_data>::object::test<8>() {

		set_test_name("Test searching and deleting");

		container->add_element(1000, "zapato");
		container->add_element(2000, "pez");
		std::pair<bool,value_type> res= container->search_for_element(1000);
		ensure_equals(res.first, true);
		ensure_equals(res.second,"zapato");

		res =  container->search_for_element(2000);
		ensure_equals(res.first, true);
		ensure_equals(res.second,"pez");

		container->delete_element(1000);
		res= container->search_for_element(1000);
		ensure_equals(res.first, false);

		container->delete_element(2000);
		res =  container->search_for_element(2000);
		ensure_equals(res.first, false);

		container->add_element(0, std::string(200, 'A'));
		container->add_element(1, std::string(200, 'B'));

		/*Caso inicial:Pruebo borrar los dos primeros elementos*/
		container->delete_element(0);
		res= container->search_for_element(0);
		ensure_equals(res.first, false);

		container->delete_element(1);
		res= container->search_for_element(1);
		ensure_equals(res.first, false);

		/*Vuelvo a dar de alta los dos primeros elementos*/
		container->add_element(0, std::string(200, 'A'));
		container->add_element(1, std::string(200, 'B'));

		/*Agrego el 2: la tabla debe crecer*/
		container->add_element(2, std::string(200, 'C'));
		container->add_element(3, std::string(200, 'D'));

		/*Elimino los elementos pares:la tabla se reduce*/
		container->delete_element(2);
		container->delete_element(0);
		res= container->search_for_element(2);
		ensure_equals(res.first, false);
		res = container->search_for_element(2);
		ensure_equals(res.first, false);


		/*Vuelvo a dar el alta de los elementos borrados*/
		container->add_element(2, std::string(200, 'C'));
		container->add_element(0, std::string(200, 'A'));
		/*Se duplica la tabla*/
		container->add_element(4, std::string(200, 'E'));


		/*Elimino los elementos de la pos 0 la hash table no deberia reducirse*/
		container->delete_element(1);
		container->delete_element(3);
		res= container->search_for_element(1);
		ensure_equals(res.first, false);
		res = container->search_for_element(3);
		ensure_equals(res.first, false);

		/*Vuelvo a dar el alta de los elementos borrados*/
		container->add_element(1, std::string(200, 'B'));
		container->add_element(3, std::string(200, 'D'));

		/*Elimino los elementos de la pos 2 de la tabla de buckets, la  hash table se reduce*/
		container->delete_element(0);
		res = container->search_for_element(0);
		ensure_equals(res.first, false);

		container->delete_element(4);
		res = container->search_for_element(4);
		ensure_equals(res.first, false);

		/*Vuelvo a dar el alta de los elementos borrados*/
		container->add_element(0, std::string(200, 'A'));
		container->add_element(4, std::string(200, 'E'));

		/*Elimino los elementos de la pos 1 de la tabla de buckets,  la  hash table se reduce*/
		container->delete_element(2);
		res = container->search_for_element(2);
		ensure_equals(res.first, false);

		/*Vuelvo a dar el alta de los elementos borrados*/
		container->add_element(2, std::string(200, 'C'));
		/*Pruebo que el alta sea satisfactoria*/
		res = container->search_for_element(2);
		ensure_equals(res.first, true);
		ensure_equals(res.second, std::string(200, 'C'));

		/*Alta el 6 y el 10,la tabla se duplica*/
		container->add_element(6, std::string(200, 'F'));
		container->add_element(10, std::string(200, 'G'));

		/*Busco todos los elementos que deberian estar*/
		res = container->search_for_element(0);
		ensure_equals(res.first, true);
		ensure_equals(res.second, std::string(200, 'A'));
		res = container->search_for_element(1);
		ensure_equals(res.first, true);
		ensure_equals(res.second, std::string(200, 'B'));
		res = container->search_for_element(2);
		ensure_equals(res.first, true);
		ensure_equals(res.second, std::string(200, 'C'));
		res = container->search_for_element(3);
		ensure_equals(res.first, true);
		ensure_equals(res.second, std::string(200, 'D'));
		res = container->search_for_element(4);
		ensure_equals(res.first, true);
		ensure_equals(res.second, std::string(200, 'E'));
		res = container->search_for_element(6);
		ensure_equals(res.first, true);
		ensure_equals(res.second, std::string(200, 'F'));
		res = container->search_for_element(10);
		ensure_equals(res.first, true);
		ensure_equals(res.second, std::string(200, 'G'));

		/*Elimino el 6 y la tabla se reduce*/
		container->delete_element(6);
		res = container->search_for_element(6);
		ensure_equals(res.first, false);

		/*Vuelvo a dar de alta al 6*/
		container->add_element(6, std::string(200, 'F'));

		/*Elimino el 2 y el 10, que estan en la tercer posicion del bucket table, se reuce la hash table*/
		container->delete_element(2);
		res = container->search_for_element(2);
		ensure_equals(res.first, false);
		container->delete_element(10);
		res = container->search_for_element(10);
		ensure_equals(res.first, false);

		/*Vuelvo a dar de alta al 2 y al 10*/
		container->add_element(2, std::string(200, 'C'));
		container->add_element(10, std::string(200, 'G'));

		/*Ahora busco elementos que no deberian estar*/
		res = container->search_for_element(7);
		ensure_equals(res.first, false);

		res = container->search_for_element(5);
		ensure_equals(res.first, false);

		res = container->search_for_element(9);
		ensure_equals(res.first, false);

		/*Elimino el 1 y el 3, no deberia modificar la hash_table*/
		container->delete_element(3);
		res = container->search_for_element(3);
		ensure_equals(res.first, false);
		container->delete_element(1);
		res = container->search_for_element(1);
		ensure_equals(res.first, false);

		/*Vuelvo a dar de alta el 1 y el 3*/
		container->add_element(1, std::string(200, 'B'));
		container->add_element(3, std::string(200, 'D'));

		/*Elimino el 0 y el 4, no deberia modificar la hash_table*/
		container->delete_element(0);
		res = container->search_for_element(0);
		ensure_equals(res.first, false);
		container->delete_element(4);
		res = container->search_for_element(4);
		ensure_equals(res.first, false);

		/*Vuelvo a dar el alta de los elementos borrados*/
		container->add_element(0, std::string(200, 'A'));
		container->add_element(4, std::string(200, 'E'));

		/*Altas que hacen crecer la tabla de buckets */
		container->add_element(11, std::string(200, 'H'));
		container->add_element(7, std::string(200, 'I'));
		container->add_element(17, std::string(200, 'J'));
		container->add_element(13, std::string(200, 'K'));
		container->add_element(12, std::string(200, 'L'));

		/*Busco todos los elementos*/
		res = container->search_for_element(0);
		ensure_equals(res.first, true);
		ensure_equals(res.second, std::string(200, 'A'));
		res = container->search_for_element(1);
		ensure_equals(res.first, true);
		ensure_equals(res.second, std::string(200, 'B'));
		res = container->search_for_element(2);
		ensure_equals(res.first, true);
		ensure_equals(res.second, std::string(200, 'C'));
		res = container->search_for_element(3);
		ensure_equals(res.first, true);
		ensure_equals(res.second, std::string(200, 'D'));
		res = container->search_for_element(4);
		ensure_equals(res.first, true);
		ensure_equals(res.second, std::string(200, 'E'));
		res = container->search_for_element(6);
		ensure_equals(res.first, true);
		ensure_equals(res.second, std::string(200, 'F'));
		res = container->search_for_element(10);
		ensure_equals(res.first, true);
		ensure_equals(res.second, std::string(200, 'G'));
		res = container->search_for_element(11);
		ensure_equals(res.first, true);
		ensure_equals(res.second, std::string(200, 'H'));
		res = container->search_for_element(7);
		ensure_equals(res.first, true);
		ensure_equals(res.second, std::string(200, 'I'));
		res = container->search_for_element(17);
		ensure_equals(res.first, true);
		ensure_equals(res.second, std::string(200, 'J'));
		res = container->search_for_element(13);
		ensure_equals(res.first, true);
		ensure_equals(res.second, std::string(200, 'K'));
		res = container->search_for_element(12);
		ensure_equals(res.first, true);
		ensure_equals(res.second, std::string(200, 'L'));

		/*elimino todos los elementos*/
		container->delete_element(0);
		res = container->search_for_element(0);
		ensure_equals(res.first, false);
		container->delete_element(1);
		res = container->search_for_element(1);
		ensure_equals(res.first, false);
		container->delete_element(2);
		res = container->search_for_element(2);
		ensure_equals(res.first, false);
		container->delete_element(3);
		res = container->search_for_element(3);
		ensure_equals(res.first, false);
		container->delete_element(4);
		res = container->search_for_element(4);
		ensure_equals(res.first, false);
		container->delete_element(6);
		res = container->search_for_element(6);
		ensure_equals(res.first, false);
		container->delete_element(10);
		res = container->search_for_element(10);
		ensure_equals(res.first, false);
		container->delete_element(11);
		res = container->search_for_element(11);
		ensure_equals(res.first, false);
		container->delete_element(7);
		res = container->search_for_element(7);
		ensure_equals(res.first, false);
		container->delete_element(17);
		res = container->search_for_element(17);
		ensure_equals(res.first, false);
		container->delete_element(13);
		res = container->search_for_element(13);
		ensure_equals(res.first, false);
		container->delete_element(12);
		res = container->search_for_element(12);
		ensure_equals(res.first, false);
}

};

