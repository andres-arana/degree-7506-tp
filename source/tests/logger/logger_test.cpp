#include "../../dependencies/tut/tut.hpp"
#include "../../commons/log/logger.h"
#include "../../commons/log/log.h"
#include "../../commons/io/directory.h"
#include "../../commons/utils/stream_utils.h"
#include <fstream>
#include <vector>
#include <cstdio>
#include <string>

namespace {

struct test_data {
	commons::io::directory dir;
	commons::log::logger logger;


	test_data()
	: dir(commons::utils::streams::extract_path("./test_logs/runtime.log")),
	  logger("./test_logs/runtime.log", 4096) {

	}

	~test_data(){
		dir.remove();
	}
};

tut::test_group<test_data> test_group("commons::log::logger class unit tests");

};

using namespace std;

namespace tut {

template<>
template<>
void test_group<test_data>::object::test<1>() {
	set_test_name("Test writing and log splitting");
	logger.info(string(1025, 'A'), __FILE__, __LINE__);
	logger.critical(string(1025, 'B'), __FILE__, __LINE__);
	logger.error(string(1025, 'C'), __FILE__, __LINE__);
	logger.warn(string(1025, 'D'), __FILE__, __LINE__);
	logger.trace(string(1025, 'E'), __FILE__, __LINE__);
	logger.debug(string(1025, 'F'), __FILE__, __LINE__);

	string line;
	vector<string> all_files = dir.get_list_filenames();
	ensure_equals(all_files.size(), 2u);
	ensure_equals(all_files[0], "./test_logs/runtime.log");
	ensure_equals(all_files[1], "./test_logs/runtime.log.0");

	ifstream first_file(all_files[0].c_str());
	getline(first_file, line);
	ensure("TRACE entry invalid, missing severity", line.find("TRACE") != string::npos);
	ensure("TRACE entry invalid, missing file", line.find("logger_test.cpp") != string::npos);
	ensure("TRACE entry invalid, missing description", line.find(string(1025, 'E')) != string::npos);

	getline(first_file, line);
	ensure("DEBUG entry invalid, missing severity", line.find("DEBUG") != string::npos);
	ensure("DEBUG entry invalid, missing file", line.find("logger_test.cpp") != string::npos);
	ensure("DEBUG entry invalid, missing description", line.find(string(1025, 'F')) != string::npos);

	ifstream second_file(all_files[1].c_str());
	getline(second_file, line);
	ensure("INFO entry invalid, missing severity", line.find("INFO") != string::npos);
	ensure("INFO entry invalid, missing file", line.find("logger_test.cpp") != string::npos);
	ensure("INFO entry invalid, missing description", line.find(string(1025, 'A')) != string::npos);

	getline(second_file, line);
	ensure("CRITICAL entry invalid, missing severity", line.find("CRITICAL") != string::npos);
	ensure("CRITICAL entry invalid, missing file", line.find("logger_test.cpp") != string::npos);
	ensure("CRITICAL entry invalid, missing description", line.find(string(1025, 'B')) != string::npos);

	getline(second_file, line);
	ensure("ERROR entry invalid, missing severity", line.find("ERROR") != string::npos);
	ensure("ERROR entry invalid, missing file", line.find("logger_test.cpp") != string::npos);
	ensure("ERROR entry invalid, missing description", line.find(string(1025, 'C')) != string::npos);

	getline(second_file, line);
	ensure("WARN entry invalid, missing severity", line.find("WARN") != string::npos);
	ensure("WARN entry invalid, missing file", line.find("logger_test.cpp") != string::npos);
	ensure("WARN entry invalid, missing description", line.find(string(1025, 'D')) != string::npos);
}

template<>
template<>
void test_group<test_data>::object::test<2>() {
	set_test_name("Test searching one keyword");

	logger.info("hello world", __FILE__, __LINE__);
	logger.critical("world", __FILE__, __LINE__);
	logger.error("Sunday morning", __FILE__, __LINE__);
	logger.trace("say what you need to say", __FILE__, __LINE__);
	logger.warn("hello", __FILE__, __LINE__);
	logger.debug("another", __FILE__, __LINE__);

	vector<string> result = logger.search("hello");

	ensure_equals(result.size(), 2u);
	ensure("First item should be the info", result[0].find("INFO") != string::npos);
	ensure("Last item should be the warn", result[1].find("WARN") != string::npos);
}

template<>
template<>
void test_group<test_data>::object::test<3>() {
	set_test_name("Test searching multiple keywords");

	logger.info("hello world", __FILE__, __LINE__);
	logger.critical("nice world", __FILE__, __LINE__);
	logger.error("Sunday morning", __FILE__, __LINE__);
	logger.warn("say what you need to say", __FILE__, __LINE__);
	logger.trace("hello dude!", __FILE__, __LINE__);
	logger.debug("another world", __FILE__, __LINE__);

	vector<string> result = logger.search("hello world");

	ensure_equals(result.size(), 4u);
	ensure("First item should be the info", result[0].find("INFO") != string::npos);
	ensure("Second item should be the critical", result[1].find("CRITICAL") != string::npos);
	ensure("Third item should be the trace", result[2].find("TRACE") != string::npos);
	ensure("Last item should be the debug", result[3].find("DEBUG") != string::npos);
}


};
