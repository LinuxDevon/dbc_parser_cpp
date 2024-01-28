#include "common.hpp"
#include "defines.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>

bool create_tmp_dbc_with(const char* filename, const char* content) {
	auto* file = std::fopen(filename, "w");
	if (!file) {
		return false;
	}

	std::fputs(PRIMITIVE_DBC.c_str(), file);
	std::fputs(content, file);
	std::fclose(file);
	return true;
}

std::string create_temporary_dbc_with(const char* contents) {
	std::filesystem::path temp_dir = std::filesystem::temp_directory_path();

	// Generate a unique temporary file name
	std::filesystem::path temp_file = temp_dir / "temp_file_XXXXXX"; // "XXXXXX" is a placeholder for a unique name

	std::ofstream file(temp_file);
	if (file.is_open()) {
		file << contents << std::endl;
		file.close();
	}

	return temp_file;
}
