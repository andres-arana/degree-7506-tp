#include "../../dependencies/tut/tut.hpp"
#include "../../arithmetic/compressor.h"
#include "../../commons/io/stream_binary_destination.h"
#include "../../commons/io/stream_binary_source.h"
#include "../../commons/utils/bit_utils.h"
#include <sstream>

using namespace arithmetic;

namespace {

struct test_data {
	symbol_distribution distribution;
	std::stringstream stream;

	void ensure_emission(const std::string &emission) {
		stream.seekg(0);
		commons::io::stream_binary_source source(stream);
		std::vector<bool> bits = commons::utils::bit::to_bool_vector(emission);
		typedef std::vector<bool>::iterator iterator;
		for (iterator i = bits.begin(); i != bits.end(); i++) {
			std::pair<bool, bool> r = source.get_next_bit();
			tut::ensure(r.first);
			tut::ensure_equals(r.second, *i);
		}

	}
};

tut::test_group<test_data> test_group("arithmetic::compressor class unit tests");

};

namespace tut {

template<>
template<>
void test_group<test_data>::object::test<1>() {
	set_test_name("Test arithmetic compressor simple emission");

	distribution.register_symbol_emision(symbol::for_char('a'));
	distribution.register_symbol_emision(symbol::for_char('a'));
	distribution.register_symbol_emision(symbol::for_char('b'));

	{
		commons::io::stream_binary_destination destination(stream);
		arithmetic::compressor<8> compressor(destination);
		compressor.compress(symbol::for_char('b'), distribution);
	}

	ensure_emission("10");
}

template<>
template<>
void test_group<test_data>::object::test<2>() {
	set_test_name("Test aritmetic compressor 'SOLO(ESC)'");

	distribution.register_symbol_emision(symbol::for_char('s'));
	distribution.register_symbol_emision(symbol::for_char('l'));
	distribution.register_symbol_emision(symbol::for_char('o'));

	{
		commons::io::stream_binary_destination destination(stream);
		arithmetic::compressor<9> compressor(destination);

		compressor.compress(symbol::for_char('s'), distribution);
		distribution.register_symbol_emision(symbol::for_char('s'));

		compressor.compress(symbol::for_char('o'), distribution);
		distribution.register_symbol_emision(symbol::for_char('o'));

		compressor.compress(symbol::for_char('l'), distribution);
		distribution.register_symbol_emision(symbol::for_char('l'));

		compressor.compress(symbol::for_char('o'), distribution);
		distribution.register_symbol_emision(symbol::for_char('o'));

		compressor.compress(symbol::ESC, distribution);
		distribution.register_symbol_emision(symbol::ESC);

		compressor.finish_compression();
	}

	ensure_emission("10001101111000101000");
}

};
