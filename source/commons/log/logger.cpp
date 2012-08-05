/******************************************************************************
 * log_service.cpp
 * 		Definiciones de la clase commons::log::log_service
******************************************************************************/
#include "../../config/config.h"
#include "logger.h"
#include "../io/directory.h"
#include "../utils/stream_utils.h"
#include "../utils/string_utils.h"
#include <sstream>
#include <time.h>
#include <iostream>

using namespace std;
using namespace commons::utils;
using namespace commons::log;

namespace {

string get_date() {
	time_t rawtime;
	struct tm * timeinfo;

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

	char result[50];
	strftime (result,50,"%B %d %Y %X",timeinfo);

	return result;
}

};

void logger::do_log(const string &severity, const string &what, const string &file, int line) {
	log_file
		<< get_date() << "[" << severity << "] "
		<< "[at " << file << ":" << line << "]"
		<< " : " << what << endl;
	log_file.flush();

	if (!streams::is_size_less_than(log_file, max_file_size)) {
		++file_number;

		stringstream new_name;
		new_name << full_filename << "." << file_number;

		log_file.close();
		utils::streams::rename(full_filename, new_name.str());
		log_file.open(full_filename.c_str(), ios_base::in | ios_base::out | ios_base::app);
	}
}

logger logger::instance(LOG_FILE, LOG_FILE_SIZE);

logger::logger(const string &log_filename, int max_file_size)
: full_filename(log_filename), max_file_size(max_file_size) {
	string path = utils::streams::extract_path(full_filename);
	string filename = streams::extract_filename(full_filename);
	file_number = io::directory(path).get_list_filenames("", filename).size() - 1;
	log_file.open(full_filename.c_str(), ios_base::in | ios_base::out | ios_base::app);
}

void logger::critical(const string &what, const string &file, int line) {
	do_log("CRITICAL", what, file, line);
}

void logger::error(const string &what, const string &file, int line) {
	do_log("ERROR", what, file, line);
}

void logger::warn(const string &what, const string &file, int line) {
	do_log("WARNING", what, file, line);
}

void logger::info(const string &what, const string &file, int line) {
	do_log("INFO", what, file, line);
}

void logger::trace(const string &what, const string &file, int line) {
	do_log("TRACE", what, file, line);
}

void logger::debug(const string &what, const string &file, int line) {
	do_log("DEBUG", what, file, line);
}

void logger::debug_var(const string &variable_name, const std::string &value, const string &file, int line) {
	stringstream builder;
	builder << "Variable " << variable_name << " = '" << value << "'";
	do_log("DEBUG", builder.str(), file, line);
}

vector<string> logger::search(const string &key_words){
	vector<string> matches; // líneas del archivo a devolver
	vector<string> keys;

	string key_words_uppercase;
	string line_uppercase;

	strings::to_uppercase(key_words, key_words_uppercase);
	strings::split(key_words_uppercase, " ",keys);

	string path = utils::streams::extract_path(full_filename);
	commons::io::directory dir(path);
	vector<string> name_files = dir.get_list_filenames();
	ifstream file;

	for (unsigned int k = 0; k < name_files.size(); k++){
		file.open(name_files[k].c_str(), ios_base::in | ios_base::out | ios_base::app);
		file.seekg(0);
		if (file.is_open()){
			string line = "";
			while ( getline(file, line) ){
				vector<string> line_words;
				line_uppercase = "";
				strings::to_uppercase(line, line_uppercase);
				strings::split(line_uppercase, " ", line_words);
				unsigned int i = 0; // de las palabras claves
				// Itero palabra por palabra las palabras clave ingresadas
				while (i < keys.size()){
					unsigned int j = 0; // de las palabras de una linea del archivo
					while (j < line_words.size()){
						string word_from_file_line = line_words[j];
						if (word_from_file_line.compare(keys[i]) == 0){
							// Esta línea del logger contiene al menos una de las palabras clave
							// => lo devuelvo
							matches.push_back(line);
							// ya incluí esa linea, asi que voy a la siguiente, corto los ciclos
							j = line_words.size();
							i = keys.size();
						}
						j++;
					}
					i++;
				}
			}
			file.close();
		}
	}
	return matches;
}

void logger::dump_to_stream(ostream &output) {
	string path = utils::streams::extract_path(full_filename);
	commons::io::directory dir(path);
	vector<string> name_files = dir.get_list_filenames();

	for(vector<string>::iterator it = name_files.begin(); it != name_files.end(); it++) {
		ifstream file(it->c_str());
		string entry;
		while (getline(file, entry)) {
			output << entry << endl;
		}
	}
}
