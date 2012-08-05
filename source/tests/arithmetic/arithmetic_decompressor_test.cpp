#include "../../dependencies/tut/tut.hpp"
#include "../../arithmetic/decompresor.h"
#include "../../commons/io/stream_binary_destination.h"
#include "../../commons/io/stream_binary_source.h"
#include <sstream>

using namespace arithmetic;

namespace {

struct test_data {
	symbol_distribution distribution;
	std::stringstream stream;

	void set_source_bits(const std::string &emission) {
		commons::io::stream_binary_destination destination(stream);
		std::vector<bool> bits = commons::utils::bit::to_bool_vector(emission);
		typedef std::vector<bool>::iterator iterator;
		for (iterator i = bits.begin(); i != bits.end(); i++) {
			destination.emit_bit(*i);
		}
		stream.seekg(0);
	}



};

tut::test_group<test_data> test_group("arithmetic::decompressor class unit tests");

};

namespace tut {

template<>
template<>
void test_group<test_data>::object::test<1>() {
	set_test_name("Test arithmetic decompressor");

	set_source_bits("10001101111000101000");

	distribution.register_symbol_emision(symbol::for_char('s'));
	distribution.register_symbol_emision(symbol::for_char('l'));
	distribution.register_symbol_emision(symbol::for_char('o'));

	commons::io::stream_binary_source source(stream);
	decompressor<9> d(source);

	ensure_equals(d.decompress(distribution).get_sequential_code(), symbol::for_char('s').get_sequential_code());
	distribution.register_symbol_emision(symbol::for_char('s'));

	ensure_equals(d.decompress(distribution).get_sequential_code(), symbol::for_char('o').get_sequential_code());
	distribution.register_symbol_emision(symbol::for_char('o'));

	ensure_equals(d.decompress(distribution).get_sequential_code(), symbol::for_char('l').get_sequential_code());
	distribution.register_symbol_emision(symbol::for_char('l'));

	ensure_equals(d.decompress(distribution).get_sequential_code(), symbol::for_char('o').get_sequential_code());
	distribution.register_symbol_emision(symbol::for_char('o'));

	ensure_equals(d.decompress(distribution).get_sequential_code(), symbol::ESC.get_sequential_code());
}

}
