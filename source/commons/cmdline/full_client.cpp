/******************************************************************************
 * full_client.cpp
 * 		Definición de la clase commons::cmdline::full_client
******************************************************************************/
#include "full_client.h"
#include "../input/pair_parser.h"

using namespace commons::cmdline;
using namespace commons::input;
using namespace std;

namespace {

const char *update_description = "Reads a key-value pair from standard input, and then searches the data "
		"structure for an element with that key. After finding the element, udpates the value of it. Outputs "
		"[NOT FOUND] if the element could not be found, otherwise outputs [OK]";

const char *delete_description = "Reads a key-value pair from standard input, and then searches the data "
		"structure for an element with that key. After finding the element, deletes the element. Outputs "
		"[NOT FOUND] if the element could not be found, otherwise outputs [OK]";

};

full_client::full_client(const string &title)
: read_client(title),
  update_switch("M", "update", update_description),
  delete_switch("Q", "delete", delete_description) {

}

void full_client::register_argument_switchs(vector<TCLAP::Arg *> &args) {
	read_client::register_argument_switchs(args);
	args.push_back(&update_switch);
	args.push_back(&delete_switch);
}

void full_client::do_operation_on_element(const pair<int, string> &element) {
	read_client::do_operation_on_element(element);
	if (update_switch.getValue()) {
		do_update(element);
	} else if (delete_switch.getValue()) {
		do_delete(element.first);
	}
}
