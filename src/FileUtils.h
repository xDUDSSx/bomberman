#pragma once

#include <string>

/*!
 * Utility class for loading text files.
 */
class FileUtils {
public:
	/*!
	 * Loads all text from a text file and saves it into a string.
	 * 
	 * \param path Path to the file.
	 * \throws std::runtime_error
	 */
	static std::string loadStringFromFile(const char* path);

	/*!
	 * Writes string to file.
	 * 
	 * \param text String to write.
	 * \param path Path to the file.
	 * \throws std::runtime_error
	 */
	static void writeStringToFile(std::string& text, const char* path);
};
