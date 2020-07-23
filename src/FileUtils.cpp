#include "FileUtils.h"

#include <fstream>

std::string FileUtils::loadStringFromFile(const char * path) {
	std::string output;
	
	std::ifstream file(path);
	std::string line;
	if (file.is_open()) {
		while (getline(file, line)) {
			output += line + "\n";
		}
		file.close();
		output = output.substr(0, output.size() - 1);
	} else {
		throw std::runtime_error("Failed to open file!");
	}
	if (file.bad()) {
		throw std::runtime_error("Error loading file!");
	}
	return output;
}

void FileUtils::writeStringToFile(std::string& text, const char* path) {
	std::ofstream out(path);
	out << text;
	out.close();
	if (out.bad()) {
		throw std::runtime_error("Error writing to file!");
	}
}

