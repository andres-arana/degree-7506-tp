#include "../../../dependencies/tut/tut.hpp"
#include "../../../commons/io/stream_char_source.h"

#include <sstream>

namespace {

struct test_data {

	void ensure_result_letter(const std::pair<bool, char> &result, char letter) {
		tut::ensure(result.first);
		tut::ensure_equals(letter, result.second);
	}

	void ensure_result_end(const std::pair<bool, char> &result) {
		tut::ensure(!result.first);
	}

};

tut::test_group<test_data> test_group("commons::io::stream_char_source class unit tests");

};

namespace tut {

template<>
template<>
void test_group<test_data>::object::test<1>() {
	set_test_name("Test getting chars from source");

	std::stringstream stream("PRUEBA DE get");
	stream.seekg(0);

	commons::io::stream_char_source source(stream);

	ensure_result_letter(source.get_next_char(), 'P');
	ensure_result_letter(source.get_next_char(), 'R');
	ensure_result_letter(source.get_next_char(), 'U');
	ensure_result_letter(source.get_next_char(), 'E');
	ensure_result_letter(source.get_next_char(), 'B');
	ensure_result_letter(source.get_next_char(), 'A');
	ensure_result_letter(source.get_next_char(), ' ');
	ensure_result_letter(source.get_next_char(), 'D');
	ensure_result_letter(source.get_next_char(), 'E');
	ensure_result_letter(source.get_next_char(), ' ');
	ensure_result_letter(source.get_next_char(), 'g');
	ensure_result_letter(source.get_next_char(), 'e');
	ensure_result_letter(source.get_next_char(), 't');
	ensure_result_end(source.get_next_char());
}

};
