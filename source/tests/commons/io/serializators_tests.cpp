#include "../../../dependencies/tut/tut.hpp"
#include "../../../commons/io/serializators.h"
#include <string>

using namespace commons::io;
using namespace std;

namespace {

struct test_data {
	block b;

	test_data() : b(256) {

	}
};

tut::test_group<test_data> test_group("commons::io::serializators unit tests");

};

namespace tut {

template<>
template<>
void test_group<test_data>::object::test<1>() {
	set_test_name("Test serializing, deserializing and serialization_length of various concurrent types");

	// Probamos la longitud de serialización de instancias en memoria
	ensure_equals(serialization_length(10), (int)sizeof(int));
	ensure_equals(serialization_length(10L), (int)sizeof(long));
	ensure_equals(serialization_length(string("TEST")), 4 + (int)sizeof(string::size_type));
	ensure_equals(serialization_length('c'), (int)sizeof(char));

	// Serializamos las instancias de manera contigua
	int position = 0;
	serialize(150, &b, position);
	position += serialization_length(150);
	serialize(1500L, &b, position);
	position += serialization_length(150L);
	serialize(string("Test"), &b, position);
	position += serialization_length(string("Test"));
	serialize('u', &b, position);

	// Deserializamos las instancias
	position = 0;
	ensure_equals(deserialize<int>(b, position), 150);
	position += serialization_length(150);
	ensure_equals(deserialize<long>(b, position), 1500L);
	position += serialization_length(150L);
	ensure_equals(deserialize<string>(b, position), string("Test"));
	position += serialization_length(string("Test"));
	ensure_equals(deserialize<char>(b, position), 'u');

	// Probamos la longitud de serialización de instancias serializadas
	position = 0;
	ensure_equals(serialization_length<int>(b, position), serialization_length(150));
	position += serialization_length(150);
	ensure_equals(serialization_length<long>(b, position), serialization_length(1500L));
	position += serialization_length(150L);
	ensure_equals(serialization_length<string>(b, position), serialization_length(string("Test")));
	position += serialization_length(string("Test"));
	ensure_equals(serialization_length<char>(b, position), serialization_length('u'));
}

};

