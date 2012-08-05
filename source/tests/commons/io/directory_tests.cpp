#include "../../../dependencies/tut/tut.hpp"
#include "../../../commons/io/directory.h"
#include "../../../commons/io/ioexception.h"

#include <cstdio>

struct test_data {
	commons::io::directory dir;

	test_data() : dir("./directory_test") {

	}

	~test_data() {
		dir.remove();
	}

};

tut::test_group<test_data> test_group("commons::io::directory class unit tests");

namespace tut {

template<>
template<>
void test_group<test_data>::object::test<1>() {
	set_test_name("Test getting filenames simple");

	std::fstream f;
	std::string filename;

	filename = dir.get_path() + "/a_first_file.file";
	f.open(filename.c_str(), std::ios_base::in | std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
	f.close();

	filename = dir.get_path() + "/b_first_file.dat";
	f.open(filename.c_str(), std::ios_base::in | std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
	f.close();

	filename = dir.get_path() + "/c_first_file1.dat";
	f.open(filename.c_str(), std::ios_base::in | std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
	f.close();

	filename = dir.get_path() + "/c_first_file2.file";
	f.open(filename.c_str(), std::ios_base::in | std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
	f.close();

	std::vector<std::string> all_files;
	std::vector<std::string> ending_with_dat;
	std::vector<std::string> starting_with_c;
	std::vector<std::string> starting_with_c_end_with_dat;

	try {
		 all_files = dir.get_list_filenames();
		 ending_with_dat = dir.get_list_filenames("", "dat");
		 starting_with_c = dir.get_list_filenames("c_");
		 starting_with_c_end_with_dat = dir.get_list_filenames("c_", "dat");
	} catch (commons::io::ioexception &e) {

	}

	ensure_equals(all_files.size(), 4u);
	ensure_equals(all_files[0], dir.get_path() + "/a_first_file.file");
	ensure_equals(all_files[1], dir.get_path() + "/b_first_file.dat");
	ensure_equals(all_files[2], dir.get_path() + "/c_first_file1.dat");
	ensure_equals(all_files[3], dir.get_path() + "/c_first_file2.file");

	ensure_equals(ending_with_dat.size(), 2u);
	ensure_equals(ending_with_dat[0], dir.get_path() + "/b_first_file.dat");
	ensure_equals(ending_with_dat[1], dir.get_path() + "/c_first_file1.dat");

	ensure_equals(starting_with_c.size(), 2u);
	ensure_equals(starting_with_c[0], dir.get_path() + "/c_first_file1.dat");
	ensure_equals(starting_with_c[1], dir.get_path() + "/c_first_file2.file");

	ensure_equals(starting_with_c_end_with_dat.size(), 1u);
	ensure_equals(starting_with_c_end_with_dat[0], dir.get_path() + "/c_first_file1.dat");
}

};
