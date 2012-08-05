#include "../../dependencies/tut/tut.hpp"
#include "../../arithmetic/interval_normalizer.h"
#include "../../commons/utils/bit_utils.h"

using namespace arithmetic;
using namespace commons::utils::bit;

namespace {

struct test_data {
};

tut::test_group<test_data> test_group("arithmetic::interval_normalizer class unit tests");

};

namespace tut {

template<>
template<>
void test_group<test_data>::object::test<1>() {
	set_test_name("Test bit input constructor and normalization case 1");

	unsigned long roof =  to_unsigned_long<8>(std::string("11001000"));
	unsigned long floor = to_unsigned_long<8>(std::string("11000011"));
	interval_normalizer<8> input(floor, roof);

	interval_normalizer<8>::normalization_result r = input.normalize();
	ensure_equals(to_bit_string(r.overflow), "1100");
	ensure_equals(r.underflow, 0);
	ensure_equals(r.new_roof,  to_unsigned_long<8>("10001111"));
	ensure_equals(r.new_floor, to_unsigned_long<8>("00110000"));
}

template<>
template<>
void test_group<test_data>::object::test<2>() {
	set_test_name("Test bit input constructor and normalization case 2");

	unsigned long roof =  to_unsigned_long<8>(std::string("11100101"));
	unsigned long floor = to_unsigned_long<8>(std::string("11001000"));
	interval_normalizer<8> input(floor, roof);

	interval_normalizer<8>::normalization_result r = input.normalize();
	ensure_equals(to_bit_string(r.overflow), "11");
	ensure_equals(r.underflow, 0);
	ensure_equals(r.new_roof,  to_unsigned_long<8>("10010111"));
	ensure_equals(r.new_floor, to_unsigned_long<8>("00100000"));
}

template<>
template<>
void test_group<test_data>::object::test<3>() {
	set_test_name("Test bit input constructor and normalization case 3");

	unsigned long roof =  to_unsigned_long<8>(std::string("10001111"));
	unsigned long floor = to_unsigned_long<8>(std::string("01111111"));
	interval_normalizer<8> input(floor, roof);

	interval_normalizer<8>::normalization_result r = input.normalize();
	ensure_equals(to_bit_string(r.overflow), "");
	ensure_equals(r.underflow, 3);
	ensure_equals(r.new_roof,  to_unsigned_long<8>("11111111"));
	ensure_equals(r.new_floor, to_unsigned_long<8>("01111000"));
}

template<>
template<>
void test_group<test_data>::object::test<4>() {
	set_test_name("Test bit input constructor and normalization case 4");

	unsigned long roof =  to_unsigned_long<8>(std::string("11100001"));
	unsigned long floor = to_unsigned_long<8>(std::string("11011111"));
	interval_normalizer<8> input(floor, roof);

	interval_normalizer<8>::normalization_result r = input.normalize();
	ensure_equals(to_bit_string(r.overflow), "11");
	ensure_equals(r.underflow, 4);
	ensure_equals(r.new_roof,  to_unsigned_long<8>("11111111"));
	ensure_equals(r.new_floor, to_unsigned_long<8>("01000000"));
}

template<>
template<>
void test_group<test_data>::object::test<5>() {
	set_test_name("Test bit input constructor and normalization case 5");

	unsigned long roof =  to_unsigned_long<8>(std::string("00010000"));
	unsigned long floor = to_unsigned_long<8>(std::string("00001111"));
	interval_normalizer<8> input(floor, roof);

	interval_normalizer<8>::normalization_result r = input.normalize();
	ensure_equals(to_bit_string(r.overflow), "000");
	ensure_equals(r.underflow, 4);
	ensure_equals(r.new_roof,  to_unsigned_long<8>("11111111"));
	ensure_equals(r.new_floor, to_unsigned_long<8>("00000000"));
}

template<>
template<>
void test_group<test_data>::object::test<6>() {
	set_test_name("Test bit input constructor and normalization case 6");

	unsigned long roof =  to_unsigned_long<8>(std::string("10110000"));
	unsigned long floor = to_unsigned_long<8>(std::string("01001111"));
	interval_normalizer<8> input(floor, roof);

	interval_normalizer<8>::normalization_result r = input.normalize();
	ensure_equals(to_bit_string(r.overflow), "");
	ensure_equals(r.underflow, 1);
	ensure_equals(r.new_roof,  to_unsigned_long<8>("11100001"));
	ensure_equals(r.new_floor, to_unsigned_long<8>("00011110"));
}

template<>
template<>
void test_group<test_data>::object::test<7>() {
	set_test_name("Test bit input constructor and normalization case 7");

	unsigned long roof =  to_unsigned_long<8>(std::string("11000111"));
	unsigned long floor = to_unsigned_long<8>(std::string("10111010"));
	interval_normalizer<8> input(floor, roof);

	interval_normalizer<8>::normalization_result r = input.normalize();
	ensure_equals(to_bit_string(r.overflow), "1");
	ensure_equals(r.underflow, 3);
	ensure_equals(r.new_roof,  to_unsigned_long<8>("11111111"));
	ensure_equals(r.new_floor, to_unsigned_long<8>("00100000"));
}

template<>
template<>
void test_group<test_data>::object::test<8>() {
	set_test_name("Test bit input constructor and normalization case 9");

	unsigned long roof =  to_unsigned_long<8>(std::string("01010011"));
	unsigned long floor = to_unsigned_long<8>(std::string("01001100"));
	interval_normalizer<8> input(floor, roof);

	interval_normalizer<8>::normalization_result r = input.normalize();
	ensure_equals(to_bit_string(r.overflow), "010");
	ensure_equals(r.underflow, 2);
	ensure_equals(r.new_roof,  to_unsigned_long<8>("11111111"));
	ensure_equals(r.new_floor, to_unsigned_long<8>("00000000"));
}

};

