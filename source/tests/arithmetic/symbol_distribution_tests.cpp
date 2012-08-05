#include "../../dependencies/tut/tut.hpp"
#include "../../arithmetic/symbol_distribution.h"
#include "../../commons/io/serializators.h"
#include <iostream>

using namespace arithmetic;

namespace {

struct test_data {
	symbol_distribution distribution;

	test_data() : distribution(symbol::ESC) {

	}
};

tut::test_group<test_data> test_group("arithmetic::symbol_distribution class unit tests");

};

namespace tut {


template<>
template<>
void test_group<test_data>::object::test<1>() {
	set_test_name("Test default distribution");

	ensure(distribution.has_symbol(symbol::ESC));
	ensure(!distribution.has_symbol(symbol::SEOF));
	for(int c = 0; c < 256; c++) {
		ensure(!distribution.has_symbol(symbol::for_char(c)));
	}
}

template<>
template<>
void test_group<test_data>::object::test<2>() {
	set_test_name("Test registering symbol emissions and calculating probabilities");

	distribution.register_symbol_emision(symbol::for_char('a'));
	distribution.register_symbol_emision(symbol::for_char('a'));
	distribution.register_symbol_emision(symbol::for_char('b'));

	ensure(distribution.has_symbol(symbol::for_char('a')));
	ensure(distribution.has_symbol(symbol::for_char('b')));
	ensure(distribution.has_symbol(symbol::ESC));

	ensure_equals(distribution.get_probability_of(symbol::for_char('a')), 0.5);
	ensure_equals(distribution.get_probability_of(symbol::for_char('b')), 0.25);
	ensure_equals(distribution.get_probability_of(symbol::ESC),0.25);

	ensure_equals(distribution.get_accumulated_probability(symbol::for_char('a')), 0.0);
	ensure_equals(distribution.get_accumulated_probability(symbol::for_char('b')), 0.5);
	ensure_equals(distribution.get_accumulated_probability(symbol::ESC), 0.75);
}

template<>
template<>
void test_group<test_data>::object::test<3>() {
	set_test_name("Test serialization mechanisms");

	distribution.register_symbol_emision(symbol::for_char('a'));
	distribution.register_symbol_emision(symbol::for_char('a'));
	distribution.register_symbol_emision(symbol::for_char('b'));

	commons::io::block b(8192);
	int current_position = 0;

	commons::io::serialize(100, &b, current_position);
	current_position += commons::io::serialization_length(100);

	commons::io::serialize(distribution, &b, current_position);
	current_position += commons::io::serialization_length(distribution);

	commons::io::serialize(200, &b, current_position);
	current_position += commons::io::serialization_length(200);

	current_position = 0;
	int first = commons::io::deserialize<int>(b, current_position);
	ensure_equals(first, 100);
	current_position += commons::io::serialization_length<int>(b, current_position);

	symbol_distribution serialized = commons::io::deserialize<symbol_distribution>(b, current_position);
	ensure_equals(commons::io::serialization_length<symbol_distribution>(b, current_position), commons::io::serialization_length(serialized));
	ensure_equals(commons::io::serialization_length<symbol_distribution>(b, current_position), commons::io::serialization_length(distribution));
	ensure_equals(distribution.get_probability_of(symbol::for_char('a')), 0.5);
	ensure_equals(distribution.get_probability_of(symbol::for_char('b')), 0.25);
	ensure_equals(distribution.get_probability_of(symbol::ESC),0.25);
	ensure_equals(distribution.get_accumulated_probability(symbol::for_char('a')), 0.0);
	ensure_equals(distribution.get_accumulated_probability(symbol::for_char('b')), 0.5);
	ensure_equals(distribution.get_accumulated_probability(symbol::ESC), 0.75);
	current_position += commons::io::serialization_length<symbol_distribution>(b, current_position);

	int last = commons::io::deserialize<int>(b, current_position);
	ensure_equals(last, 200);
}

template<>
template<>
void test_group<test_data>::object::test<4>() {
	set_test_name("Test iterator");

	distribution.register_symbol_emision(symbol::for_char('a'));
	distribution.register_symbol_emision(symbol::for_char('b'));
	distribution.register_symbol_emision(symbol::for_char('c'));

	symbol_distribution::iterator it = distribution.begin();

	symbol current = *it;
	ensure_equals(current.get_sequential_code(), symbol::for_char('a').get_sequential_code());
	it++;

	current = *it;
	ensure_equals(current.get_sequential_code(), symbol::for_char('b').get_sequential_code());
	it++;

	current = *it;
	ensure_equals(current.get_sequential_code(), symbol::for_char('c').get_sequential_code());
	it++;

	current = *it;
	ensure_equals(current.get_sequential_code(), symbol::ESC.get_sequential_code());
	it++;

	ensure(it == distribution.end());
}

template<>
template<>
void test_group<test_data>::object::test<5>() {
	set_test_name("Test exclusion builder");

	distribution.register_symbol_emision(symbol::for_char('v'));
	distribution.register_symbol_emision(symbol::for_char('o'));
	distribution.register_symbol_emision(symbol::for_char('y'));

	symbol_distribution::exclusion_set exclusion;
	exclusion.insert(symbol::for_char('v'));
	distribution.append_to_exclusion_set(exclusion);

	ensure_equals(exclusion.size(), 3u);
	ensure(exclusion.end() != exclusion.find(symbol::for_char('v')));
	ensure(exclusion.end() != exclusion.find(symbol::for_char('o')));
	ensure(exclusion.end() != exclusion.find(symbol::for_char('y')));
}

template<>
template<>
void test_group<test_data>::object::test<6>() {
	set_test_name("Test exclude");

	distribution.register_symbol_emision(symbol::for_char('c'));
	distribution.register_symbol_emision(symbol::for_char('c'));
	distribution.register_symbol_emision(symbol::for_char('c'));
	distribution.register_symbol_emision(symbol::for_char('j'));
	distribution.register_symbol_emision(symbol::for_char('j'));
	distribution.register_symbol_emision(symbol::for_char('h'));

	symbol_distribution::exclusion_set exclusion;
	exclusion.insert(symbol::for_char('c'));
	exclusion.insert(symbol::for_char('h'));

	symbol_distribution excluded = distribution.exclude(exclusion);

	ensure_equals(excluded.get_emissions(symbol::for_char('j')), 2u);
	ensure_equals(excluded.get_emissions(symbol::ESC), 1u);

	ensure_equals(excluded.get_probability_of(symbol::for_char('j')), 2.0 / 3.0);
	ensure_equals(excluded.get_probability_of(symbol::ESC), 1.0 / 3.0);

	ensure_equals(excluded.get_emissions(symbol::for_char('c')), 0u);
	ensure_equals(excluded.get_emissions(symbol::for_char('h')), 0u);
}
};

