#include "common.hpp"
#include "defines.hpp"
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

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
	char filename[] = "temp_file_XXXXXX";
	if (std::tmpnam(filename) == nullptr) {
		throw std::runtime_error("Failed to generate a unique temporary filename.");
	}
	std::filesystem::path temp_file = temp_dir / filename;

	std::ofstream file(temp_file);
	if (!file.is_open()) {
		throw std::runtime_error("Failed to create temporary file.");
	}

	file << contents << std::endl;
	file.close();

	return temp_file;
}
