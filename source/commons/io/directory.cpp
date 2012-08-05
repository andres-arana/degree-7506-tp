/******************************************************************************
 * directory.cpp
 * 		Definiciones de la clase commons::io::directory
******************************************************************************/
#include "directory.h"
#include "ioexception.h"
#include <algorithm>
#include <sstream>
#include <cstdio>
#include <dirent.h>
#include <sys/stat.h>

using namespace std;
using namespace commons::io;


directory::directory(const string &path)
: path(path) {
	DIR *dir = opendir( path.c_str() );
	closedir(dir);
	if (dir == NULL){
		// creo el directorio
		if (mkdir( path.c_str(), 0777) == -1){
			stringstream error_description;
			error_description
				<< "Could not create directory '" << path << "'";
			throw ioexception(error_description.str());
		}
	}
}

const string &directory::get_path() const {
	return path;
}

vector<string> directory::get_list_filenames(const string &begin, const string &end) const {
	std::vector<std::string> result;

	DIR *dir = opendir( path.c_str() );
	if (dir == NULL){
		stringstream error_description;
		error_description
			<< "Could not list directory files for '" << path
			<< "': The directory does not exist anymore.";
		throw ioexception(error_description.str());
	}

	dirent *dir_entry = readdir(dir);

	while (dir_entry != NULL){
		string name = dir_entry->d_name;
		if (
				name != ".." &&
				name != "." &&
				(begin.empty() || name.find(begin) == 0) &&
				(end.empty() || name.find(end) == name.size() - end.size())) {
			result.push_back(path + "/" + name);
		}
		dir_entry = readdir(dir);
	}
	closedir(dir);

	std::sort(result.begin(), result.end());
	return result;
}

void directory::remove() {
	vector<string> files = get_list_filenames();
	for (vector<string>::iterator it = files.begin(); it != files.end(); it++)
		std::remove(it->c_str());

	int result = rmdir(path.c_str());
	if (result) {
		stringstream error_description;
		error_description
			<< "Could not remove the directory '" << path << "'";
		throw ioexception(error_description.str());
	}
}
