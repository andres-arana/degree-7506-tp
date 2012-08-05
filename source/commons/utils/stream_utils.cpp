/******************************************************************************
 * stream_utils.cpp
 * 		Definiciones de las funciones de commons::utils::streams
******************************************************************************/
#include "stream_utils.h"
#include "../io/ioexception.h"
#include <sstream>

using namespace std;

bool commons::utils::streams::is_size_less_than(fstream &file, int byte_size) {
	file.seekg(0, std::ios::end);
	int position = file.tellg();
	file.seekg(0);
	return byte_size >= position;
}

int commons::utils::streams::file_size(const string &filename) {
	ifstream s(filename.c_str());
	s.seekg(0, std::ios::end);
	return s.tellg();
}

void commons::utils::streams::rename(const string &current_filename, const string &new_filename) {
	int result = std::rename(current_filename.c_str(), new_filename.c_str());
	if (result) {
		stringstream error_description;
		error_description
			<< "Error renaming file '" << current_filename
			<< "' to '" << new_filename << "': std::rename returned " << result;
		throw commons::io::ioexception(error_description.str());
	}
}

string commons::utils::streams::extract_path(const std::string &filename) {
	string::size_type last_dir_position = filename.find_last_of('/');

	string result =  last_dir_position == string::npos ? "" : filename.substr(0, last_dir_position);
	if (result == "") result = ".";
	return result;
}

string commons::utils::streams::extract_filename(const std::string &filename) {
	string::size_type last_dir_position = filename.find_last_of('/');

	return last_dir_position == string::npos ? filename : filename.substr(last_dir_position + 1);
}
