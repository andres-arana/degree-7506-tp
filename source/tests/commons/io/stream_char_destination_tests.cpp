#include "../../../dependencies/tut/tut.hpp"
#include "../../../commons/io/stream_char_destination.h"

#include <sstream>

namespace {

struct test_data {


};

tut::test_group<test_data> test_group("commons::io::stream_char_destination class unit tests");

};

namespace tut {

template<>
template<>
void test_group<test_data>::object::test<1>() {
	set_test_name("Test emitting chars to source");

	std::stringstream stream;

	{
		commons::io::stream_char_destination destination(stream);

		destination.emit_char('p');
		destination.emit_char('r');
		destination.emit_char('u');
		destination.emit_char('e');
		destination.emit_char('b');
		destination.emit_char('a');
	}

	ensure_equals("prueba", stream.str());
}

};
