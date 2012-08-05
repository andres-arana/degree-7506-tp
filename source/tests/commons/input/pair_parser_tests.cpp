#include "../../../dependencies/tut/tut.hpp"
#include "../../../commons/input/pair_parser.h"
#include <string>
#include <vector>

using namespace std;
using namespace commons::input;

namespace {

struct test_data {
	pair_parser parser;

	void ensure_parsing_success(const string &input_feed, const vector<pair_parser::pair_type> &expected) {
		vector<pair_parser::pair_type> actual;
		for (string::const_iterator it = input_feed.begin(); it != input_feed.end(); it++) {
			if (!parser.feed_char(*it)) {
				actual.push_back(parser.get_pair());
			}
		}
		parser.feed_ended();

		tut::ensure_equals(actual.size(), expected.size());
		for (size_t i = 0; i < actual.size(); i++) {
			tut::ensure_equals(actual[i].first, expected[i].first);
			tut::ensure_equals(actual[i].second, expected[i].second);
		}
	}

	void ensure_parsing_fails(const string &input_feed, const vector<pair_parser::pair_type> &expected) {
		vector<pair_parser::pair_type> actual;
		try {
			for (string::const_iterator it = input_feed.begin(); it != input_feed.end(); it++) {
				if (!parser.feed_char(*it)) {
					actual.push_back(parser.get_pair());
				}
			}
			parser.feed_ended();
			tut::fail("Unable to detect parse error");
		} catch (pair_parser::parse_exception &e) {

		}

		tut::ensure_equals(actual.size(), expected.size());
		for (size_t i = 0; i < actual.size(); i++) {
			tut::ensure_equals(actual[i].first, expected[i].first);
			tut::ensure_equals(actual[i].second, expected[i].second);
		}
	}

};

tut::test_group<test_data> test_group("commons::input::pair_parser class unit tests");

};

namespace tut {

template<>
template<>
void test_group<test_data>::object::test<1>() {
	set_test_name("Test parsing simple case");

	string input_feed = "(1234;sample)";
	vector<pair_parser::pair_type> expectations;
	expectations.push_back(make_pair(1234, string("sample")));

	ensure_parsing_success(input_feed, expectations);
}

template<>
template<>
void test_group<test_data>::object::test<2>() {
	set_test_name("Test parsing with white spaces");

	string input_feed = "  		(1234;sam p	le)  	  ";
	vector<pair_parser::pair_type> expectations;
	expectations.push_back(make_pair(1234, string("sam p	le")));

	ensure_parsing_success(input_feed, expectations);
}

template<>
template<>
void test_group<test_data>::object::test<3>() {
	set_test_name("Test parsing multiple without spaces");

	string input_feed = "(1234;sample)(1999;another sample)";
	vector<pair_parser::pair_type> expectations;
	expectations.push_back(make_pair(1234, string("sample")));
	expectations.push_back(make_pair(1999, string("another sample")));

	ensure_parsing_success(input_feed, expectations);
}

template<>
template<>
void test_group<test_data>::object::test<4>() {
	set_test_name("Test parsing multiple with spaces");

	string input_feed = "  	(1234;sample) 		 (1999;another sample)   ";
	vector<pair_parser::pair_type> expectations;
	expectations.push_back(make_pair(1234, string("sample")));
	expectations.push_back(make_pair(1999, string("another sample")));

	ensure_parsing_success(input_feed, expectations);
}

template<>
template<>
void test_group<test_data>::object::test<5>() {
	set_test_name("Test parsing invalid missing start bracket");

	string input_feed = "1234;sample)";
	vector<pair_parser::pair_type> expectations;

	ensure_parsing_fails(input_feed, expectations);
}

template<>
template<>
void test_group<test_data>::object::test<6>() {
	set_test_name("Test parsing invalid missing start bracket multiple elements");

	string input_feed = "(1999;another sample)1234;sample)";
	vector<pair_parser::pair_type> expectations;
	expectations.push_back(make_pair(1999, string("another sample")));

	ensure_parsing_fails(input_feed, expectations);
}

template<>
template<>
void test_group<test_data>::object::test<7>() {
	set_test_name("Test parsing invalid missing end bracket");

	string input_feed = "(1234;sample";
	vector<pair_parser::pair_type> expectations;

	ensure_parsing_fails(input_feed, expectations);
}

template<>
template<>
void test_group<test_data>::object::test<8>() {
	set_test_name("Test parsing invalid missing end bracket multiple elements");

	string input_feed = "(1999;another sample)(7777;asdf   ";
	vector<pair_parser::pair_type> expectations;
	expectations.push_back(make_pair(1999, string("another sample")));

	ensure_parsing_fails(input_feed, expectations);
}

template<>
template<>
void test_group<test_data>::object::test<9>() {
	set_test_name("Test parsing empty string");

	string input_feed = "(1234;)";
	vector<pair_parser::pair_type> expectations;
	expectations.push_back(make_pair(1234, string("")));

	ensure_parsing_success(input_feed, expectations);
}

template<>
template<>
void test_group<test_data>::object::test<10>() {
	set_test_name("Test parsing invalid key format multiple elements");

	string input_feed = "(1999;another sample)(123a;sample)";
	vector<pair_parser::pair_type> expectations;
	expectations.push_back(make_pair(1999, string("another sample")));

	ensure_parsing_fails(input_feed, expectations);
}

template<>
template<>
void test_group<test_data>::object::test<11>() {
	set_test_name("Test parsing invalid element with '('");

	string input_feed = "(1999;another sample)(1999;another sample(1999;sample)";
	vector<pair_parser::pair_type> expectations;
	expectations.push_back(make_pair(1999, string("another sample")));

	ensure_parsing_fails(input_feed, expectations);
}

};

