#include "common.hpp"
#include "defines.hpp"

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
