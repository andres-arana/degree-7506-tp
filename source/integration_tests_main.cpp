/******************************************************************************
 * main.cpp
 * 		Punto de entrada al programa cuando se compila el ejecutable que corre
 * 		las pruebas unitarias
******************************************************************************/
#include <cstdlib>
#include "dependencies/tut/tut.hpp"
#include "dependencies/tut/tut_console_reporter.hpp"

using namespace tut;

int main(int argc, char **argv) {
	console_reporter reporter;

	runner.get().set_callback(&reporter);
	runner.get().run_tests_starting_with("integration::");

	return reporter.all_ok() ? EXIT_SUCCESS : EXIT_FAILURE;
}
