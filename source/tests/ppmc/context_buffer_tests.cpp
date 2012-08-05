#include "../../dependencies/tut/tut.hpp"
#include "../../ppmc/context_buffer.h"

using namespace ppmc;

namespace {

struct test_data {
	context_buffer buffer;

	test_data() : buffer(5) {

	}
};

tut::test_group<test_data> test_group("ppmc::context_buffer class unit tests");

};

namespace tut {

template<>
template<>
void test_group<test_data>::object::test<1>() {
	set_test_name("Test incomplete buffer operations");

	buffer.push_char('a');
	buffer.push_char('b');
	buffer.push_char('c');

	ensure(buffer.has_context(1));
	ensure(buffer.has_context(2));
	ensure(buffer.has_context(3));
	ensure(!buffer.has_context(4));

	ensure_equals(buffer.get_context(1), "c");
	ensure_equals(buffer.get_context(2), "bc");
	ensure_equals(buffer.get_context(3), "abc");
}

template<>
template<>
void test_group<test_data>::object::test<2>() {
	set_test_name("Test complete buffer operations");

	buffer.push_char('a');
	buffer.push_char('b');
	buffer.push_char('c');
	buffer.push_char('d');
	buffer.push_char('e');

	ensure(buffer.has_context(1));
	ensure(buffer.has_context(2));
	ensure(buffer.has_context(3));
	ensure(buffer.has_context(4));
	ensure(buffer.has_context(5));
	ensure(!buffer.has_context(6));

	ensure_equals(buffer.get_context(1), "e");
	ensure_equals(buffer.get_context(2), "de");
	ensure_equals(buffer.get_context(3), "cde");
	ensure_equals(buffer.get_context(4), "bcde");
	ensure_equals(buffer.get_context(5), "abcde");
}

template<>
template<>
void test_group<test_data>::object::test<3>() {
	set_test_name("Test cyclic buffer operations");

	buffer.push_char('a');
	buffer.push_char('b');
	buffer.push_char('c');
	buffer.push_char('d');
	buffer.push_char('e');
	buffer.push_char('f');
	buffer.push_char('g');

	ensure(buffer.has_context(1));
	ensure(buffer.has_context(2));
	ensure(buffer.has_context(3));
	ensure(buffer.has_context(4));
	ensure(buffer.has_context(5));
	ensure(!buffer.has_context(6));

	ensure_equals(buffer.get_context(1), "g");
	ensure_equals(buffer.get_context(2), "fg");
	ensure_equals(buffer.get_context(3), "efg");
	ensure_equals(buffer.get_context(4), "defg");
	ensure_equals(buffer.get_context(5), "cdefg");
}

};
