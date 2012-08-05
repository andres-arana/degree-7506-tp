#include "../../../dependencies/tut/tut.hpp"
#include "../../../commons/io/block.h"

namespace {

struct test_data {
	commons::io::block test_block;

	test_data() : test_block(4, "1234") {

	}
};

tut::test_group<test_data> test_group("commons::io::block class unit tests");

};

namespace tut {

template<>
template<>
void test_group<test_data>::object::test<1>() {
	set_test_name("Test accessor methods");

	ensure_equals(test_block.get_size(), 4);
	ensure_equals(test_block[0], '1');
	ensure_equals(test_block[1], '2');
	ensure_equals(test_block[2], '3');
	ensure_equals(test_block[3], '4');

	test_block[0] = '5';
	test_block[1] = '6';
	test_block[2] = '7';
	test_block[3] = '8';

	ensure_equals(test_block[0], '5');
	ensure_equals(test_block[1], '6');
	ensure_equals(test_block[2], '7');
	ensure_equals(test_block[3], '8');
}

template<>
template<>
void test_group<test_data>::object::test<2>() {
	set_test_name("Test copy construction");

	commons::io::block copy_block(test_block);

	ensure_equals(copy_block.get_size(), 4);
	ensure_equals(copy_block[0], '1');
	ensure_equals(copy_block[1], '2');
	ensure_equals(copy_block[2], '3');
	ensure_equals(copy_block[3], '4');
}

template<>
template<>
void test_group<test_data>::object::test<3>() {
	set_test_name("Test assignment operator");

	commons::io::block copy_block(1, "a");
	ensure_equals(copy_block[0],'a');

	copy_block = test_block;

	ensure_equals(copy_block.get_size(), 4);
	ensure_equals(copy_block[0], '1');
	ensure_equals(copy_block[1], '2');
	ensure_equals(copy_block[2], '3');
	ensure_equals(copy_block[3], '4');
}

template<>
template<>
void test_group<test_data>::object::test<4>() {
	set_test_name("Test raw accessors");

	ensure_equals(test_block.raw_char_pointer()[0], '1');
	ensure_equals(test_block.raw_char_pointer()[1], '2');
	ensure_equals(test_block.raw_char_pointer()[2], '3');
	ensure_equals(test_block.raw_char_pointer()[3], '4');
}

template<>
template<>
void test_group<test_data>::object::test<5>() {
	set_test_name("Test raw accessors for writing");

	test_block.raw_char_pointer()[0] = '5';
	test_block.raw_char_pointer()[1] = '6';
	test_block.raw_char_pointer()[2] = '7';
	test_block.raw_char_pointer()[3] = '8';

	ensure_equals(test_block[0], '5');
	ensure_equals(test_block[1], '6');
	ensure_equals(test_block[2], '7');
	ensure_equals(test_block[3], '8');
}

template<>
template<>
void test_group<test_data>::object::test<6>() {
	set_test_name("Test erasure and compact");

	test_block.erase_and_compact(1, 2);

	ensure_equals(test_block[0], '1');
	ensure_equals(test_block[1], '4');
	ensure_equals(test_block[2], '3');
	ensure_equals(test_block[3], '4');
};

template<>
template<>
void test_group<test_data>::object::test<7>() {
	set_test_name("Test insertion and expansion");

	test_block.insert_and_expand(1, "78", 2);

	ensure_equals(test_block[0], '1');
	ensure_equals(test_block[1], '7');
	ensure_equals(test_block[2], '8');
	ensure_equals(test_block[3], '2');
}

template<>
template<>
void test_group<test_data>::object::test<8>() {
	set_test_name("Test insertion and expansion at index zero");

	test_block = commons::io::block(5, "12345");
	test_block.insert_and_expand(0, "987", 3);

	ensure_equals(test_block[0], '9');
	ensure_equals(test_block[1], '8');
	ensure_equals(test_block[2], '7');
	ensure_equals(test_block[3], '1');
	ensure_equals(test_block[4], '2');
}

template<>
template<>
void test_group<test_data>::object::test<9>() {
	set_test_name("Test insertion and expansion at the middle");

	test_block = commons::io::block(5, "12345");
	test_block.insert_and_expand(1, "98", 2);

	ensure_equals(test_block[0], '1');
	ensure_equals(test_block[1], '9');
	ensure_equals(test_block[2], '8');
	ensure_equals(test_block[3], '2');
	ensure_equals(test_block[4], '3');
}

template<>
template<>
void test_group<test_data>::object::test<10>() {
	set_test_name("Test insertion and expansion at end");

	test_block = commons::io::block(5, "12345");
	test_block.insert_and_expand(2, "987", 3);

	ensure_equals(test_block[0], '1');
	ensure_equals(test_block[1], '2');
	ensure_equals(test_block[2], '9');
	ensure_equals(test_block[3], '8');
	ensure_equals(test_block[4], '7');
}

template<>
template<>
void test_group<test_data>::object::test<11>() {
	set_test_name("Test copying");

	commons::io::block other_block(10);

	test_block.copy_to_block(other_block);

	ensure_equals(other_block[0], '1');
	ensure_equals(other_block[1], '2');
	ensure_equals(other_block[2], '3');
	ensure_equals(other_block[3], '4');
}

template<>
template<>
void test_group<test_data>::object::test<12>() {
	set_test_name("Test copying");

	test_block = commons::io::block(5, "12345");

	test_block.make_gap(1, 2);

	ensure_equals(test_block[0], '1');
	ensure_equals(test_block[3], '2');
	ensure_equals(test_block[4], '3');
}

};
