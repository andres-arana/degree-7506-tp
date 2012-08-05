#include "../../../dependencies/tut/tut.hpp"
#include "../../../commons/io/stream_binary_source.h"

#include <sstream>
#include <bitset>

namespace {

struct test_data {

	char build_char(const std::bitset<8> &bits) {
		char result = 0;
		for (size_t i = 0; i < bits.size(); i++) {
			if (bits[i])
				result |= (1 << i);
		}
		return result;
	}

	void ensure_result(const std::pair<bool, bool> &result, bool bit) {
		tut::ensure(result.first);
		tut::ensure_equals(result.second, bit);
	}

	void ensure_result_end(const std::pair<bool, char> &result) {
		tut::ensure(!result.first);
	}

};

tut::test_group<test_data> test_group("commons::io::stream_binary_source class unit tests");

};

namespace tut {

template<>
template<>
void test_group<test_data>::object::test<1>() {
	set_test_name("Test getting bits from source");

	std::stringstream stream;
	stream.put(build_char(std::bitset<8>(std::string("00101100"))));
	stream.put(build_char(std::bitset<8>(std::string("00110111"))));
	stream.seekg(0);

	commons::io::stream_binary_source source(stream);
	ensure_result(source.get_next_bit(), 0);
	ensure_result(source.get_next_bit(), 0);
	ensure_result(source.get_next_bit(), 1);
	ensure_result(source.get_next_bit(), 0);
	ensure_result(source.get_next_bit(), 1);
	ensure_result(source.get_next_bit(), 1);
	ensure_result(source.get_next_bit(), 0);
	ensure_result(source.get_next_bit(), 0);

	ensure_result(source.get_next_bit(), 0);
	ensure_result(source.get_next_bit(), 0);
	ensure_result(source.get_next_bit(), 1);
	ensure_result(source.get_next_bit(), 1);
	ensure_result(source.get_next_bit(), 0);
	ensure_result(source.get_next_bit(), 1);
	ensure_result(source.get_next_bit(), 1);
	ensure_result(source.get_next_bit(), 1);
	ensure_result_end(source.get_next_bit());
	ensure_result_end(source.get_next_bit());
}

};
