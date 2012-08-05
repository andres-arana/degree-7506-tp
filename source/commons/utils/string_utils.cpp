/******************************************************************************
 * string_utils.cpp
 * 		Definiciones de las funciones de commons::utils::strings
******************************************************************************/
#include "string_utils.h"
#include <algorithm>
#include <cctype>

using namespace std;

void commons::utils::strings::split(const string &str, const string &delim, vector<string> &results) {
	results.clear();

	// Busco el primer caracter que no sea el delimitador
	string::size_type last_pos = str.find_first_not_of(delim, 0);
	// Busco el primer delimitador
	string::size_type pos = str.find_first_of(delim, last_pos);

	while (string::npos != pos || string::npos != last_pos) {
		// Hay un token, que va desde lastPos hasta pos
		results.push_back(str.substr(last_pos, pos - last_pos));
		// Busco el siguiente caracter que no sea el delimitador
		last_pos = str.find_first_not_of(delim, pos);
		// Busco el siguiente delimitador
		pos = str.find_first_of(delim, last_pos);
	}
}

void commons::utils::strings::to_uppercase(const string &str, string &uppercase){
	uppercase.clear();
	transform(str.begin(), str.end(), back_inserter(uppercase), (int(*)(int))toupper);
}
