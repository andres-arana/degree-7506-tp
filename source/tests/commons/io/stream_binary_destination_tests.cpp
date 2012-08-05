#include "../../../dependencies/tut/tut.hpp"
#include "../../../commons/io/stream_binary_destination.h"
#include "../../../commons/io/stream_binary_source.h"


#include <sstream>

namespace {

struct test_data {
	void ensure_result(const std::pair<bool, bool> &result, bool bit) {
		tut::ensure(result.first);
		tut::ensure_equals(result.second, bit);
	}

	void ensure_result_end(const std::pair<bool, char> &result) {
		tut::ensure(!result.first);
	}
};

tut::test_group<test_data> test_group("commons::io::stream_binary_destination class unit tests");

};

namespace tut {

template<>
template<>
void test_group<test_data>::object::test<1>() {
	set_test_name("Test emitting bits to source");

	std::stringstream stream;

	{
		commons::io::stream_binary_destination destination(stream);
		destination.emit_bit(0);
		destination.emit_bit(0);
		destination.emit_bit(1);
		destination.emit_bit(0);
		destination.emit_bit(1);
		destination.emit_bit(1);
		destination.emit_bit(0);
		destination.emit_bit(0);

		destination.emit_bit(0);
		destination.emit_bit(0);
		destination.emit_bit(1);
		destination.emit_bit(1);
		destination.emit_bit(0);
		destination.emit_bit(1);
		destination.emit_bit(1);
		destination.emit_bit(1);
	}

	std::string result = stream.str();
	ensure_equals(2u, result.size());
	ensure_equals(result[0], 44);
	ensure_equals(result[1], 55);
}

template<>
template<>
void test_group<test_data>::object::test<2>() {
	set_test_name("Test integration between source and destination");

	std::stringstream stream;

	{
		commons::io::stream_binary_destination destination(stream);
		destination.emit_bit(1);
		destination.emit_bit(0);
		destination.emit_bit(1);
		destination.emit_bit(1);
		destination.emit_bit(1);
		destination.emit_bit(0);
		destination.emit_bit(0);
		destination.emit_bit(1);

		destination.emit_bit(0);
		destination.emit_bit(1);
		destination.emit_bit(1);
		destination.emit_bit(0);
	}

	stream.seekg(0);

	{
		commons::io::stream_binary_source source(stream);
		ensure_result(source.get_next_bit(), 1);
		ensure_result(source.get_next_bit(), 0);
		ensure_result(source.get_next_bit(), 1);
		ensure_result(source.get_next_bit(), 1);
		ensure_result(source.get_next_bit(), 1);
		ensure_result(source.get_next_bit(), 0);
		ensure_result(source.get_next_bit(), 0);
		ensure_result(source.get_next_bit(), 1);

		ensure_result(source.get_next_bit(), 0);
		ensure_result(source.get_next_bit(), 1);
		ensure_result(source.get_next_bit(), 1);
		ensure_result(source.get_next_bit(), 0);
	}
}



};
