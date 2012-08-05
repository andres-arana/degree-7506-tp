#include "../../../dependencies/tut/tut.hpp"
#include "../../../commons/utils/string_utils.h"
#include <sstream>

namespace {

struct test_data {
	void test_uppercase(const std::string &str, const std::string &expected) {
		std::string result;
		commons::utils::strings::to_uppercase(str, result);
		tut::ensure_equals(result, expected);
	}

	void test_tokenize(const std::string &str, const std::vector<std::string> &expected) {
		std::stringstream error_message;
		error_message << "The tokenization of '" << str << "' was unsuccessful";
		std::vector<std::string> result;
		commons::utils::strings::split(str, " ", result);
		tut::ensure(error_message.str(), result == expected);
	}
};

tut::test_group<test_data> test_group("commons::utils::strings functions unit tests");

};

namespace tut {

template<>
template<>
void test_group<test_data>::object::test<1>() {
	set_test_name("Test valid string uppercasing");
	test_uppercase("hoLa", "HOLA");
	test_uppercase("qwerasdf!@#$%&/()=?¡\"", "QWERASDF!@#$%&/()=?¡\"");
	test_uppercase("", "");
}

template<>
template<>
void test_group<test_data>::object::test<2>() {
	set_test_name("Test tokenization");

	std::vector<std::string> expected_result;
	expected_result.push_back("Esto");
	expected_result.push_back("es");
	expected_result.push_back("una");
	expected_result.push_back("prueba");

	test_tokenize("Esto es una prueba", expected_result);
	test_tokenize("  Esto es una prueba", expected_result);
	test_tokenize("Esto    es   una prueba  ", expected_result);
	test_tokenize("    Esto  es     una  prueba    ", expected_result);
}

};
