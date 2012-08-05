#include "../../../dependencies/tut/tut.hpp"
#include "../../../commons/utils/stream_utils.h"
#include "../../../commons/io/ioexception.h"
#include <fstream>
#include <cstdio>

namespace {

struct test_data {
	std::fstream file;

	test_data() {
		file.open("stream_utils_test.file", std::ios_base::in | std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
		file << "123456789012345";
	}

	~test_data() {
		file.close();
		std::remove("stream_utils_test.file");
	}

};

tut::test_group<test_data> test_group("commons::utils::streams functions unit tests");

};

namespace tut {

template<>
template<>
void test_group<test_data>::object::test<1>() {
	set_test_name("Test calculation of stream sizes");

	ensure("Not shorter size case", !commons::utils::streams::is_size_less_than(file, 11));
	ensure("Equal size case", commons::utils::streams::is_size_less_than(file, 15));
	ensure("Shorter case", commons::utils::streams::is_size_less_than(file, 16));
}

template<>
template<>
void test_group<test_data>::object::test<2>() {
	set_test_name("Test renaming files");

	std::fstream f("some_file.file", std::ios_base::in | std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
	f.close();

	commons::utils::streams::rename("some_file.file", "some_other_file.file");

	f.open("some_file.file", std::ios_base::in | std::ios_base::out | std::ios_base::binary | std::ios_base::ate);
	ensure("Previous File still exists", !f.is_open());
	f.clear();

	f.open("some_other_file.file", std::ios_base::in | std::ios_base::out | std::ios_base::binary | std::ios_base::ate);
	ensure("New file does not exist", f.is_open());

	std::remove("some_other_file.file");
}

template<>
template<>
void test_group<test_data>::object::test<3>() {
	set_test_name("Test renaming invalid files");

	try {
		commons::utils::streams::rename("unexistent", "other");
		fail("Failed to detect invalid filename");
	} catch (commons::io::ioexception &e) {

	}

	std::remove("some_other_file.file");
}

template<>
template<>
void test_group<test_data>::object::test<4>() {
	set_test_name("Test renaming file invalid destination");

	std::fstream f("some_file.file", std::ios_base::in | std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
	f.close();

	try {
		commons::utils::streams::rename("some_file.file", "");
		fail("Failed to detect invalid destination filename");
	} catch (commons::io::ioexception &e) {

	}

	std::remove("some_file.file");
}

template<>
template<>
void test_group<test_data>::object::test<5>() {
	set_test_name("Test extracting path");

	ensure_equals(commons::utils::streams::extract_path("./dir/somefile.file"), std::string("./dir"));
	ensure_equals(commons::utils::streams::extract_path("./somefile.file"), std::string("."));
	ensure_equals(commons::utils::streams::extract_path("somefile.file"), std::string("."));
}

template<>
template<>
void test_group<test_data>::object::test<6>() {
	set_test_name("Test extracting filename");

	ensure_equals(commons::utils::streams::extract_filename("./dir/somefile.file"), std::string("somefile.file"));
	ensure_equals(commons::utils::streams::extract_filename("./somefile.file"), std::string("somefile.file"));
	ensure_equals(commons::utils::streams::extract_filename("somefile.file"), std::string("somefile.file"));
}

};
